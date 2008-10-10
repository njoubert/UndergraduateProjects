// DRGWrapper.cpp - Created with DR&E AppWizard 1.0B10
// ------------------------------------------------------------------------------------
// Copyright © 1999 Intel Corporation
// All Rights Reserved
// 
// Permission is granted to use, copy, distribute and prepare derivative works of this 
// software for any purpose and without fee, provided, that the above copyright notice
// and this statement appear in all copies.  Intel makes no representations about the
// suitability of this software for any purpose.  This software is provided "AS IS." 
//
// Intel specifically disclaims all warranties, express or implied, and all liability,
// including consequential and other indirect damages, for the use of this software, 
// including liability for infringement of any proprietary rights, and including the 
// warranties of merchantability and fitness for a particular purpose.  Intel does not
// assume any responsibility for any errors which may appear in this software nor any
// responsibility to update it.
// ------------------------------------------------------------------------------------
//
//  PURPOSE:
//    
// DRGWrapper.cpp: implementation of the CDRGWrapper class.
// This class handles all device & mode enumeration, surface creation, viewport setup, etc.
//
// ------------------------------------------------------------------------------------
//
// Authors: Dean Macri, Kim Pallister - Intel Developer Relations & Engineering Group
//
////////////////////////////////////////////////////////////////////////////////////////

#include "..\DRGShell.h"

//-----------------------------------------------------------------------------
// Name: struct D3DDEVICEINFO
// Desc: Structure to hold info about the enumerated Direct3D devices.
//-----------------------------------------------------------------------------



//
// The following three file globals are used during the enumeration
// process because we're combining the DDraw+D3D driver pairs into
// one driver for each pair.
//
static GUID g_guidDDraw;	
static GUID *g_pguidDDraw;
static LPSTR g_pDDrawDescription;
static LPSTR g_pDDrawName;
static LPDIRECTDRAW7	g_pDirectDraw7;

BOOL WINAPI EnumDDrawDeviceCallback( GUID FAR *lpGUID, 
											LPSTR lpDriverDescription,
											LPSTR lpDriverName,
											LPVOID lpContext, 
											HMONITOR hm );
HRESULT CALLBACK EnumD3DDevicesCallback(  GUID FAR* lpGuid,                    
												 LPSTR lpDeviceDescription,             
												 LPSTR lpDeviceName,                  
												 LPD3DDEVICEDESC7 lpD3DDeviceDesc,     
												 LPVOID lpContext );
HRESULT WINAPI EnumModesCallback(  LPDDSURFACEDESC2 lpDDSurfaceDesc,  
										  LPVOID lpContext );
HRESULT WINAPI EnumZBufferFormatsCallback( DDPIXELFORMAT* pddpf, void* pddpfDesired );

// -----------------------------------------------------------------------
// CDRGWrapper::CDRGWrapper( HWND hWnd)
//
// -----------------------------------------------------------------------
CDRGWrapper::CDRGWrapper( HWND hWnd )
{
	m_iDevices = 0;
	m_iCurrentDevice = -1;
	m_iCurrentMode = -1;
	m_pFirstHWDriver = NULL;
	m_pFirstD3DDevice = NULL;
	m_pCurrentDevice = NULL;
	m_pCurrentMode = NULL;
	m_bInitialized = false;
	m_bFullscreen = false;
	m_bHardware = false;
	m_bZBuffer = false;
	m_dwBackBufferCount = 0;
	m_dwFSRenderWidth = 640;
	m_dwFSRenderHeight = 480;
	m_dwFSBpp = 16;

	m_pClientSite = NULL;

	m_fFrameTime = 0.0f;

	m_pDD7 = NULL;
	m_pD3D7 = NULL;
	m_pFrontBuffer = NULL;
	m_pBackBuffer = NULL;
	m_pZBuffer = NULL;
	m_pD3DDevice7 = NULL;

	m_iDrivers = 0;
	m_iDevices = 0;


	m_hWnd = hWnd;
	//
	// Enumerate the available DirectDraw devices and in the process
	// enumerate the available D3D Devices for each DirectDraw device
	//
	HRESULT hr = DirectDrawEnumerateEx( EnumDDrawDeviceCallback, this, 
						DDENUM_ATTACHEDSECONDARYDEVICES |
						DDENUM_DETACHEDSECONDARYDEVICES |
						DDENUM_NONDISPLAYDEVICES );
	//
	// Should throw an exception here if we couldn't get any devices
	//
	if( FAILED( hr ) )
	{
		OutputDebugString ("no devices enumerated");
	}

}

// -----------------------------------------------------------------------
// CDRGWrapper::~CDRGWrapper()
//
// -----------------------------------------------------------------------
CDRGWrapper::~CDRGWrapper()
{
	if( m_iCurrentDevice >= 0 )
		DestroyMode();

	//
	// Delete the drivers
	//
	
	D3DDEVICEINFO *pDriver = m_pFirstHWDriver;
	D3DDEVICEINFO *pDriverTemp = pDriver;
	
	while( pDriver )
	{
		//
		// Delete the modes for the driver
		//
		D3DFSMode *pMode = pDriver->pFSModes;
		D3DFSMode *pModeTemp = pMode;
		while( pMode )
		{
			pMode = pMode->pNext;
			delete pModeTemp;
			pModeTemp = pMode;
		}

		pDriver = pDriver->pNext;
		delete pDriverTemp;
		pDriverTemp = pDriver;
	}

}

// -----------------------------------------------------------------------
// D3DDriver *CDRGWrapper::GetDevice( int iDevice )
//
// -----------------------------------------------------------------------
D3DDEVICEINFO *CDRGWrapper::GetDevice( int iDevice )
{
	if( (iDevice < 0) || (iDevice >= m_iDevices) )
		return NULL;

	D3DDEVICEINFO *pDevice = m_pFirstD3DDevice;

	while( iDevice )
	{
		iDevice--;
		pDevice = pDevice->pNext;
	}
	return pDevice;
}

// -----------------------------------------------------------------------
// D3DFSMode *CDRGWrapper::GetMode( int iDevice, int iMode )
//
// -----------------------------------------------------------------------
D3DFSMode *CDRGWrapper::GetMode( int iDevice, int iMode )
{
	D3DDEVICEINFO *pDevice = GetDevice( iDevice );
	if( !pDevice )
		return NULL;

	if( (iMode < 0) || (iMode >= (int)pDevice->dwNumFSModes ) )
		return NULL;

	D3DFSMode *pMode = pDevice->pFSModes;
	while( iMode )
	{
		iMode--;
		pMode = pMode->pNext;
	}

	return pMode;
}


// -----------------------------------------------------------------------
// int CDRGWrapper::GetNumDevices( void )
//
// -----------------------------------------------------------------------
int CDRGWrapper::GetNumDevices( void )
{
	return m_iDevices;
}

// -----------------------------------------------------------------------
// int CDRGWrapper::GetNumModes( int iDevice )
//
// -----------------------------------------------------------------------
int CDRGWrapper::GetNumModes( int iDevice )
{
	D3DDEVICEINFO *pDevice = GetDevice( iDevice );

	if( pDevice )
	{
		if( !pDevice->dwNumFSModes )
		{
			// Enumerate the modes
			//
			EnumModes( iDevice, NULL, NULL );
		}
		return pDevice->dwNumFSModes;
	}

	return 0;
}

// -----------------------------------------------------------------------
// HRESULT CDRGWrapper::GetBestDevice( int &iDevice, bool bHardware, bool bAGP, DWORD dwOtherFlags )
//
// -----------------------------------------------------------------------
HRESULT CDRGWrapper::GetBestDevice( int &iDevice, bool bHardware, bool bAGP, DWORD dwOtherFlags )
{
	D3DDEVICEINFO *pDevice = m_pFirstD3DDevice;
	int iCurrentDevice = 0;

	iDevice = -1;

	bool bDeviceGood = false;

	//This first nested check is for options the appwizard can rem out or not. 
	while( pDevice )
	{
		if( bHardware )
		{
			//if( pDevice->HALDesc.dwFlags )
			if( pDevice->ddDeviceDesc.dwDevCaps & D3DDEVCAPS_HWRASTERIZATION )
			{
				if( bAGP )
				{
					//if( pDevice->HALDesc.dwDevCaps & D3DDEVCAPS_TEXTURENONLOCALVIDMEM )
					if( pDevice->ddDeviceDesc.dwDevCaps & D3DDEVCAPS_TEXTURENONLOCALVIDMEM )
					{
						// Could check dwOtherFlags here
						//
						bDeviceGood = true;
					}
				}
				else
				{
					//
					// Could check dwOtherFlags here
					//
					bDeviceGood = true;
				}
			}
		}
		else
		{
			// Could check dwOtherFlags here
			//

			// For now, if you don't ask for hardware, you get the first
			// device in the list
			//
			bDeviceGood = true;
		}

		//if no need to check for other 'special stuff'
		if ((bDeviceGood) && (dwOtherFlags == 0))
		{
			//make sure this next line gets put back into appwizard source
			m_pCurrentDevice = pDevice;
			iDevice = iCurrentDevice;
			return S_OK;
		}

		//if the device supports the caps bits requested so far, we need to check the other 
		//required ones, that were specified in the dwOtherFlags. This is where
		if (bDeviceGood)
		{
			//note: I am using the separate texture memories flag, as this is a sure sign that
			// the HW cannot render to a texture surface (you could get around it by blitting,
			// but I haven't done this. Some devices are improperly succeeding to a call to
			// CreateSurface when the D3DDevice and D3DTexture flags are both specified. The
			// HW should fail thew call to create the surface, but does not.

			//HW - bump - target
			if ( (bHardware) && (dwOtherFlags & REQUIREBUMPMAPPING) && 
				                (dwOtherFlags & REQUIRETARGETTEXTURE) )
			{
				if (((pDevice->ddDeviceDesc.dwDevCaps & D3DDEVCAPS_SEPARATETEXTUREMEMORIES) == 0) &&
					(pDevice->ddDeviceDesc.dwTextureOpCaps & D3DTEXOPCAPS_BUMPENVMAP))
				{					
					iDevice = iCurrentDevice;
					return S_OK;			
				}				
			}
			else
			{
				// HW - no bump - target
   				if ((bHardware) && (dwOtherFlags & REQUIRETARGETTEXTURE))
				{
					if ((bHardware) && 
						//This is a sub-optimal hack that only excludes 1 card of many. I really need
						//to do this in the Texture creation, unless there is a specific DX7 caps bit.
						((pDevice->ddDeviceDesc.dwDevCaps & D3DDEVCAPS_SEPARATETEXTUREMEMORIES) == 0))
					{
						iDevice = iCurrentDevice;
						return S_OK;			
					}				
				}
				// HW - bump - no target
				if ((bHardware) && (dwOtherFlags & REQUIREBUMPMAPPING))
				{
					if ((bHardware) && 
						//(pDevice->HALDesc.dwTextureOpCaps & D3DTEXOPCAPS_BUMPENVMAP))
						(pDevice->ddDeviceDesc.dwTextureOpCaps & D3DTEXOPCAPS_BUMPENVMAP))
					{
						iDevice = iCurrentDevice;
						return S_OK;			
					}				
				}		

			}

			//SW - bump (no need to check for target texture support, SW devices support them)
			if ( (!bHardware) && (dwOtherFlags & REQUIREBUMPMAPPING) )
			{
				//if (pDevice->HELDesc.dwTextureOpCaps & D3DTEXOPCAPS_BUMPENVMAP)					
				if (pDevice->ddDeviceDesc.dwTextureOpCaps & D3DTEXOPCAPS_BUMPENVMAP)					
				{					
					iDevice = iCurrentDevice;
					return S_OK;			
				}
			}

			//if we have gotten to here, it means we did not find the device we wanted, so we will fall through to
			// the next device on the list			
		}
		
		pDevice = pDevice->pNext;
		m_pCurrentDevice = pDevice;
		iCurrentDevice++;
	}
	//if we get to here it means we have gone through the list of devices and have not found one
	// with the requirements requested
	return D3DERR_INVALID_DEVICE;
}

// -----------------------------------------------------------------------
// HRESULT CDRGWrapper::EnumModes( int iDevice, int *iModes, D3DFSMode **pModes )
//
// -----------------------------------------------------------------------
HRESULT CDRGWrapper::EnumModes( int iDevice, int *iModes, D3DFSMode **pModes )
{
	D3DDEVICEINFO *pDevice = GetDevice( iDevice );
	D3DDEVICEINFO *pHoldDevice = m_pCurrentDevice;
	LPDIRECTDRAW7 pDD7 = NULL;
	HRESULT hr;

	if( !pDevice )
		return D3DERR_INVALID_DEVICE;


	if( iDevice != m_iCurrentDevice )
	{
		hr = DirectDrawCreateEx( pDevice->pDriverGUID, (void **)&pDD7, IID_IDirectDraw7, NULL );
		HANDLE_ERROR( hr, TRUE );
	}
	else
	{
		pDD7 = m_pDD7;
	}

	m_pCurrentDevice = pDevice;

	//
	// Delete any existing modes
	//
	D3DFSMode *pMode = pDevice->pFSModes;
	D3DFSMode *pModeTemp = pMode;
	while( pMode )
	{
		pMode = pMode->pNext;
		delete pModeTemp;
		pModeTemp = pMode;
	}
	pDevice->dwNumFSModes = 0;

	hr = pDD7->EnumDisplayModes( 0, NULL, this, EnumModesCallback );
	m_pCurrentDevice = pHoldDevice;

	HANDLE_ERROR( hr, hr );

	if( iDevice != m_iCurrentDevice )
		SAFE_RELEASE( pDD7 );

	if( pModes )
		*pModes = pDevice->pFSModes;
	if( iModes )
		*iModes = pDevice->dwNumFSModes;

	return S_OK;
}

// -----------------------------------------------------------------------
// HRESULT CDRGWrapper::InitMode( int iDevice, int iMode, DWORD dwBackBufferCount, bool bZBuffer )
//
// -----------------------------------------------------------------------
HRESULT CDRGWrapper::InitMode( int iDevice, int iMode, DWORD dwBackBufferCount, bool bZBuffer )
{
	m_pCurrentDevice = GetDevice( iDevice );
	m_pCurrentMode = GetMode( iDevice, iMode );

	return InitMode( iDevice, true, m_pCurrentMode->dwWidth, m_pCurrentMode->dwHeight,
			  m_pCurrentMode->dwBpp, dwBackBufferCount, bZBuffer, false );
}

// -----------------------------------------------------------------------
// HRESULT CDRGWrapper::InitMode( int iDevice, bool bFullScreen,
//							  DWORD dwWidth, DWORD dwHeight, DWORD dwBpp, DWORD dwBackBufferCount,
//							  bool bZBuffer, bool bUseDefaultOnError )
//
// -----------------------------------------------------------------------
HRESULT CDRGWrapper::InitMode( int iDevice, bool bFullScreen, 
							   DWORD dwWidth, DWORD dwHeight, DWORD dwBpp, DWORD dwBackBufferCount,
							   bool bZBuffer, bool bUseDefaultOnError )
{
	DWORD dwFlags = 0;
	HRESULT hr;

	//
	// If we've already initialized a mode, shut it down
	//
	if( m_iCurrentDevice >= 0 )
		DestroyMode();

	//
	// Do some parameter checking
	//
	if( dwBackBufferCount < 1 )
		return ERROR_INVALID_PARAMETER;

	m_pCurrentDevice = GetDevice( iDevice );
	if( !m_pCurrentDevice )
		return ERROR_INVALID_PARAMETER;

	//
	// Save the window position
	//
	GetWindowRect( m_hWnd, &m_rcWindow );


	m_bHardware = false;
	if (m_pCurrentDevice->ddDeviceDesc.dwDevCaps & D3DDEVCAPS_HWRASTERIZATION )
		m_bHardware = true;

	m_bFullscreen = (bFullScreen || !(m_pCurrentDevice->bDesktopCompatible)); // FS if requested or mandatory
	m_dwBpp = dwBpp;
	if( !m_bFullscreen )
	{
		RECT rcView;

		GetClientRect( m_hWnd, &rcView );
		m_dwRenderWidth = rcView.right;
		m_dwRenderHeight = rcView.bottom;
	}
	else
	{
		m_dwRenderWidth = dwWidth;
		m_dwRenderHeight = dwHeight;
		//
		// Keep these around for toggling between fullscreen and non-fullscreen
		//
		m_dwFSRenderWidth = dwWidth;
		m_dwFSRenderHeight = dwHeight;
		m_dwFSBpp = dwBpp;
	}
	m_bZBuffer = bZBuffer;
	m_dwBackBufferCount = dwBackBufferCount;

	hr = CreateDirectDraw();
	ON_ERROR_GOTO( hr, InitCleanup );

	hr = CreateD3D();
	ON_ERROR_GOTO( hr, InitCleanup );

	hr = CreateSurfaces( dwBackBufferCount );
	ON_ERROR_GOTO( hr, InitCleanup );

	//
	// Create and attach the z-buffer if requested
	//
	if( bZBuffer )
	{
		hr = CreateZBuffer();
		ON_ERROR_GOTO( hr, InitCleanup );
	}

	//
	// Create the 3D device
	//
	if( FAILED( hr = Create3DDevice() ) )
		return hr;

	//
	// Create and set the 3D viewport
	//
	if( FAILED( hr = CreateViewport() ) )
		return hr;
  
	m_iCurrentDevice = iDevice;

	m_bInitialized = true;

	return S_OK;

InitCleanup:
	DestroyMode();

	return hr;
}

// -----------------------------------------------------------------------
// HRESULT CDRGWrapper::DestroyMode( void )
//
// -----------------------------------------------------------------------
HRESULT CDRGWrapper::DestroyMode( void )
{
	HRESULT hr;

	if( !m_bInitialized )
		return S_OK;

	m_bInitialized = false;

	SAFE_RELEASE( m_pD3DDevice7 );

	SAFE_RELEASE( m_pZBuffer );
	SAFE_RELEASE( m_pBackBuffer );
	SAFE_RELEASE( m_pFrontBuffer );
	SAFE_RELEASE( m_pD3D7 );

	if( m_pDD7 )
	{
		char szTemp[256];
		hr = m_pDD7->SetCooperativeLevel( m_hWnd, DDSCL_NORMAL );
		if( FAILED( hr ) )
		{
			sprintf( szTemp, "SetCooperativeLevel failed: %8lx\r\n", hr );
			OutputDebugString( szTemp );
		}
	}
	SAFE_RELEASE( m_pDD7 );
	if( m_bFullscreen )
		SetWindowPos( m_hWnd, HWND_NOTOPMOST, m_rcWindow.left, m_rcWindow.top,
							m_rcWindow.right - m_rcWindow.left,
							m_rcWindow.bottom - m_rcWindow.top,
							SWP_SHOWWINDOW );


	m_pCurrentDevice = NULL;
	m_pCurrentMode = NULL;
	m_iCurrentDevice = -1;
	m_iCurrentMode = -1;
	m_dwRenderWidth = 0;
	m_dwRenderHeight = 0;
	m_bFullscreen = false;

	return S_OK;
}

// -----------------------------------------------------------------------
// HRESULT CDRGWrapper::Clear( DWORD dwFlags )
//
// -----------------------------------------------------------------------
HRESULT CDRGWrapper::Clear( DWORD dwFlags )
{
	if( !m_bInitialized )
		return S_OK;

	DWORD dwClearColor = 0;
	D3DVALUE dvZClearValue = 1.0f;
	DWORD dwStencilClearValue = 0;
	D3DRECT rcClear;
	HRESULT hr;

	rcClear.x1 = 0;
	rcClear.y1 = 0;
	rcClear.x2 = m_dwRenderWidth;
	rcClear.y2 = m_dwRenderHeight;

	hr = m_pD3DDevice7->Clear( 1, &rcClear, dwFlags,
							dwClearColor, dvZClearValue, dwStencilClearValue );


	return hr;
}

// -----------------------------------------------------------------------
// HRESULT CDRGWrapper::StartFrame( void )
//
// -----------------------------------------------------------------------
HRESULT CDRGWrapper::StartFrame( void )
{
	if( !m_bInitialized )
		return S_OK;

	return m_pD3DDevice7->BeginScene();
}

// -----------------------------------------------------------------------
// HRESULT CDRGWrapper::EndFrame( void )
//
// -----------------------------------------------------------------------
HRESULT CDRGWrapper::EndFrame( void )
{
	if( !m_bInitialized )
		return S_OK;

	HRESULT hr;

	hr = m_pD3DDevice7->EndScene();
	HANDLE_ERROR( hr, hr );

	if( m_bFullscreen )
	{
		//
		// Full screen mode, just flip
		//
		hr = m_pFrontBuffer->Flip( NULL, DDFLIP_WAIT );
		HANDLE_ERROR( hr, hr );
	}
	else
	{
		//
		// Windowed mode, do a Blt
		//
		hr = m_pFrontBuffer->Blt( &m_rcScreen, m_pBackBuffer, 
								  &m_rcView, DDBLT_WAIT, NULL );
		HANDLE_ERROR( hr, hr );
	}

	return S_OK;
}

// -----------------------------------------------------------------------
// HRESULT CDRGWrapper::MoveWindow( void )
//
// -----------------------------------------------------------------------
HRESULT CDRGWrapper::MoveWindow( void )
{
	if( !m_bInitialized )
		return S_OK;

	if( !m_bFullscreen )
	{
		GetWindowRect( m_hWnd, &m_rcWindow );

		GetClientRect( m_hWnd, &m_rcView );
		GetClientRect( m_hWnd, &m_rcScreen );
		ClientToScreen( m_hWnd, (POINT*)&m_rcScreen.left );
		ClientToScreen( m_hWnd, (POINT*)&m_rcScreen.right );
	}

	return S_OK;
}

// -----------------------------------------------------------------------
// HRESULT CDRGWrapper::RefreshMode( bool bFullscreen )
//
// -----------------------------------------------------------------------
HRESULT CDRGWrapper::RefreshMode( bool bFullscreen )
{
	if( !m_bInitialized )
		return S_OK;

	int iDevice = m_iCurrentDevice;
	DWORD dwRenderWidth = m_dwRenderWidth;
	DWORD dwRenderHeight = m_dwRenderHeight;
	DWORD dwBpp = m_dwBpp;
	DWORD dwBackBufferCount = m_dwBackBufferCount;
	bool bZBuffer = m_bZBuffer;
	HRESULT hr;

	if( bFullscreen )
	{
		dwRenderWidth = m_dwFSRenderWidth;
		dwRenderHeight = m_dwFSRenderHeight;
		dwBpp = m_dwFSBpp;
	}

	//
	// The mode has changed (typically the result of a window resizing)
	// Destroy the current mode and re-initialize it.
	//
	hr = DestroyMode();
	HANDLE_ERROR( hr, hr );

	hr = InitMode( iDevice, bFullscreen, dwRenderWidth,
				   dwRenderHeight, dwBpp, dwBackBufferCount,
				   bZBuffer, false );
	HANDLE_ERROR( hr, hr );

	return S_OK;
}

// -----------------------------------------------------------------------
// HRESULT CDRGWrapper::ChangeDevice( int iDevice )
//
// -----------------------------------------------------------------------
HRESULT CDRGWrapper::ChangeDevice( int iDevice )
{
	if( !m_bInitialized )
		return S_OK;

	DWORD dwRenderWidth = m_dwRenderWidth;
	DWORD dwRenderHeight = m_dwRenderHeight;
	DWORD dwBpp = m_dwBpp;
	DWORD dwBackBufferCount = m_dwBackBufferCount;
	bool bZBuffer = m_bZBuffer;
	bool bFullscreen = m_bFullscreen;
	HRESULT hr;

	//need to check the device being selected, to see if it is FS only
	D3DDEVICEINFO	*pDrv = m_pFirstD3DDevice;
	int iDrvNum = 0;
		
	while ((iDrvNum++ != iDevice) && (pDrv->pNext !=NULL))
		pDrv = pDrv->pNext;
	//we should now be pointed at the device that the user selected
	if (pDrv->bDesktopCompatible == FALSE)		
	{
		m_bFullscreen = true;
		bFullscreen = true;
	}

	if( bFullscreen )
	{
		dwRenderWidth = m_dwFSRenderWidth;
		dwRenderHeight = m_dwFSRenderHeight;
		dwBpp = m_dwFSBpp;
	}

	//
	// Destroy the current mode and re-initialize it with the new device
	//
	hr = DestroyMode();
	HANDLE_ERROR( hr, hr );

	hr = InitMode( iDevice, bFullscreen, dwRenderWidth,
				   dwRenderHeight, dwBpp, dwBackBufferCount,
				   bZBuffer, false );
	HANDLE_ERROR( hr, hr );

	return S_OK;
}


// -----------------------------------------------------------------------
// HRESULT CALLBACK EnumD3DDevicesCallback(  GUID FAR* lpGuid,
//												 LPSTR lpDeviceDescription,             
//												 LPSTR lpDeviceName,                  
//												 LPD3DDEVICEDESC7 lpD3DHWDeviceDesc,     
//												 LPD3DDEVICEDESC7 lpD3DHELDeviceDesc,  
//												 LPVOID lpContext )
//
// -----------------------------------------------------------------------
HRESULT CALLBACK EnumD3DDevicesCallback(  		 LPSTR lpDeviceDescription,             
												 LPSTR lpDeviceName,                  
												 LPD3DDEVICEDESC7 lpD3DDevDesc,     
												 LPVOID lpContext )
{
    CDRGWrapper *pWrapper = (CDRGWrapper *)lpContext;
		//point to last driver on list
	D3DDEVICEINFO* pDriverInfo = pWrapper->m_pFirstHWDriver;
	while (pDriverInfo->pNext != NULL)
		pDriverInfo = pDriverInfo->pNext;
	
	//skip to end of already enum'd D3D devices
	//  with an exception if this is the first time through
	D3DDEVICEINFO* pDeviceInfo;

	if (pWrapper->m_pFirstD3DDevice == NULL)
	{
		pWrapper->m_pFirstD3DDevice = new D3DDEVICEINFO;
		pDeviceInfo = pWrapper->m_pFirstD3DDevice;
		pDeviceInfo->pNext = NULL;

	}
	else
	{
		pDeviceInfo = pWrapper->m_pFirstD3DDevice;
		while (pDeviceInfo->pNext)
			pDeviceInfo = pDeviceInfo->pNext;
		//now at end of list
		pDeviceInfo->pNext = new D3DDEVICEINFO;
		pDeviceInfo = pDeviceInfo->pNext;
		pDeviceInfo->pNext = NULL;
	}
	
	memcpy( &pDeviceInfo->ddDeviceDesc, lpD3DDevDesc, sizeof(D3DDEVICEDESC7) );
    pDeviceInfo->guidDevice   = lpD3DDevDesc->deviceGUID;
    pDeviceInfo->pDeviceGUID  = &pDeviceInfo->guidDevice;
    pDeviceInfo->bHardware    = lpD3DDevDesc->dwDevCaps & D3DDEVCAPS_HWRASTERIZATION;
    pDeviceInfo->dwNumFSModes   = 0;
	pDeviceInfo->pFSModes = NULL;
    pDeviceInfo->ddDriverCaps = pDriverInfo->ddDriverCaps;
    pDeviceInfo->ddHELCaps    = pDriverInfo->ddHELCaps;

    // Copy the driver GUID and description for the device
    if( pDriverInfo->pDriverGUID )
    {
        pDeviceInfo->guidDriver  = pDriverInfo->guidDriver;
        pDeviceInfo->pDriverGUID = &pDeviceInfo->guidDriver;

        strncpy( pDeviceInfo->strDesc, lpDeviceName, 39 );

		char *tempstring = " on ";
		strncat( pDeviceInfo->strDesc, tempstring, 4 );
		strncat( pDeviceInfo->strDesc, pDriverInfo->strDesc, 39 );
    }
    else
    {
        pDeviceInfo->pDriverGUID = NULL;
        strncpy( pDeviceInfo->strDesc, lpDeviceName, 39 );
		char *tempstring = " on Primary Display Device";
		strncat(pDeviceInfo->strDesc, tempstring, 39);
    }

    // Avoid duplicates: only enum HW devices for secondary DDraw drivers.
	// KP: REmmed out to debug menus
//  if( NULL != pDeviceInfo->pDriverGUID && FALSE == pDeviceInfo->bHardware )
//        return D3DENUMRET_OK;

    // This is a good place to give the app a chance to accept or reject this
    // device via a callback function. This simple tutorial does not do this,
    // though.
	D3DFSMode *pTempMode;
	pTempMode = pDriverInfo->pFSModes;

	while (pTempMode != NULL)
	{
		DWORD dwModeDepth = pTempMode->SurfDesc.ddpfPixelFormat.dwRGBBitCount;
        DWORD dwBitDepth  = pDeviceInfo->ddDeviceDesc.dwDeviceRenderBitDepth;
        BOOL  bCompatible = FALSE;

        // Check mode for compatability with device. Skip 8-bit modes.
        if( (32==dwModeDepth) && (dwBitDepth&DDBD_32) ) bCompatible = TRUE;
        if( (24==dwModeDepth) && (dwBitDepth&DDBD_24) ) bCompatible = TRUE;
        if( (16==dwModeDepth) && (dwBitDepth&DDBD_16) ) bCompatible = TRUE;

        // Copy compatible modes to the list of device-supported modes
        if( bCompatible )
		{
			//allocate a new FSMode struct
			D3DFSMode *pCompatMode = new D3DFSMode;
			//copy the info over from the one in the DDraw device list
			memcpy( pCompatMode, pTempMode, sizeof(D3DFSMode));
			//point it to nothing
			pCompatMode->pNext = NULL;

			//if no modes are in the list, start the list with this one
			if (pDeviceInfo->pFSModes == NULL)
			{
				pDeviceInfo->pFSModes = pCompatMode;
			}
			else // else proceeed to end of list and add it there
			{
				D3DFSMode *pTemp = pDeviceInfo->pFSModes;
				while (pTemp->pNext != NULL)
					pTemp = pTemp->pNext;
				pTemp->pNext = pCompatMode;
			}			
			
			pDeviceInfo->dwNumFSModes++;
		}

		//go to next mode in the DD FS mode list
		pTempMode = pTempMode->pNext;
	}
            

/*
    // Build list of supported modes for the device
    for( DWORD i=0; i<pDriverInfo->dwNumModes; i++ )
    {
        DDSURFACEDESC2 ddsdMode = pDriverInfo->ddsdModes[i];
        DWORD dwModeDepth = ddsdMode.ddpfPixelFormat.dwRGBBitCount;
        DWORD dwBitDepth  = pDeviceInfo->ddDeviceDesc.dwDeviceRenderBitDepth;
        BOOL  bCompatible = FALSE;

        // Check mode for compatability with device. Skip 8-bit modes.
        if( (32==dwModeDepth) && (dwBitDepth&DDBD_32) ) bCompatible = TRUE;
        if( (24==dwModeDepth) && (dwBitDepth&DDBD_24) ) bCompatible = TRUE;
        if( (16==dwModeDepth) && (dwBitDepth&DDBD_16) ) bCompatible = TRUE;

        // Copy compatible modes to the list of device-supported modes
        if( bCompatible )
            pDeviceInfo->ddsdModes[pDeviceInfo->dwNumModes++] = ddsdMode;
    }
	*/

    // Make sure the device has supported modes
    if( 0 == pDeviceInfo->dwNumFSModes )
        return D3DENUMRET_OK;

    // Mark whether the device can render into a desktop window
    if( 0 == pDeviceInfo->pFSModes->dwWidth )
    {
        pDeviceInfo->bDesktopCompatible = TRUE;
        pDeviceInfo->pDefaultFSMode = pDeviceInfo->pFSModes->pNext;
    }
    else
    {
        pDeviceInfo->bDesktopCompatible = FALSE;
        pDeviceInfo->pDefaultFSMode = pDeviceInfo->pFSModes;
    }

    // Accept the device and return
	//pWrapper->m_iDrivers++;

    pWrapper->m_iDevices++;
    return D3DENUMRET_OK;

}

// -----------------------------------------------------------------------
// BOOL WINAPI EnumDDrawDeviceCallback( GUID FAR *lpGUID, 
// 									 LPSTR lpDriverDescription,
//									 LPSTR lpDriverName,
//									 LPVOID lpContext, 
//									 HMONITOR hm )
//
// Callback for enumerating DirectDraw devices.  Each DirectDraw
// device could have multiple D3D Devices.
// -----------------------------------------------------------------------
BOOL WINAPI EnumDDrawDeviceCallback( GUID FAR *lpGUID, 
									 LPSTR lpDriverDescription,
									 LPSTR lpDriverName,
									 LPVOID lpContext, 
									 HMONITOR hm )
{
    CDRGWrapper *pWrapper = (CDRGWrapper *)lpContext;
	D3DDEVICEINFO *pd3dDeviceInfo;
	
	LPDIRECTDRAW7 pDD7 = NULL;
	LPDIRECT3D7 pD3D7 = NULL;
	HRESULT hr;

	//skip to end of already enum'd devices
	//  with an exception if this is the first time through
	if (pWrapper->m_pFirstHWDriver == NULL)
	{
		pWrapper->m_pFirstHWDriver = new D3DDEVICEINFO;
		pd3dDeviceInfo = pWrapper->m_pFirstHWDriver;
		pd3dDeviceInfo->pNext = NULL;

	}
	else
	{
		pd3dDeviceInfo = pWrapper->m_pFirstHWDriver;
		while (pd3dDeviceInfo->pNext)
			pd3dDeviceInfo = pd3dDeviceInfo->pNext;
		//now at end of list
		pd3dDeviceInfo->pNext = new D3DDEVICEINFO;
		pd3dDeviceInfo = pd3dDeviceInfo->pNext;
		pd3dDeviceInfo->pNext = NULL;
	}


	g_pguidDDraw = lpGUID;

	if( g_pguidDDraw )
		memcpy( &g_guidDDraw, lpGUID, sizeof( GUID ) );
	g_pDDrawDescription = lpDriverDescription;
	g_pDDrawName = lpDriverName;

	hr = DirectDrawCreateEx( lpGUID, (void **)&pDD7, IID_IDirectDraw7, NULL );
	HANDLE_ERROR( hr, TRUE ); 

	
	//
	// Query for access to Direct3D
	//
	hr = pDD7->QueryInterface( IID_IDirect3D7, (void **)&pD3D7 );
	if( FAILED( hr ) )
	{
		SAFE_RELEASE( pDD7 );
		return TRUE;
	}
	g_pDirectDraw7 = pDD7; //point the global over so that we can use it in the d3ddevice enum

	/////////
	//added from DX7 framework, we will enum modes here
	
    // Copy data to a device info structure
    ZeroMemory( pd3dDeviceInfo, sizeof(D3DDEVICEINFO) );
    strncpy( pd3dDeviceInfo->strDesc, lpDriverDescription, 39 );
    pd3dDeviceInfo->ddDriverCaps.dwSize = sizeof(DDCAPS);
    pd3dDeviceInfo->ddHELCaps.dwSize    = sizeof(DDCAPS);
    pDD7->GetCaps( &pd3dDeviceInfo->ddDriverCaps, &pd3dDeviceInfo->ddHELCaps );
    pd3dDeviceInfo->pDriverGUID = lpGUID;
    if( lpGUID )
        pd3dDeviceInfo->guidDriver = (*lpGUID);


    // Add a display mode for rendering into a desktop window
    if( ( NULL == pd3dDeviceInfo->pDriverGUID ) &&
        ( pd3dDeviceInfo->ddDriverCaps.dwCaps2 & DDCAPS2_CANRENDERWINDOWED ) )
    {
        // Get the current display depth
        DEVMODE devmode;
        devmode.dmSize = sizeof(DEVMODE);
        EnumDisplaySettings( NULL, ENUM_CURRENT_SETTINGS, &devmode );

		D3DFSMode	*pMode;
        // Set up the mode info
		if(pd3dDeviceInfo->pFSModes == NULL)
		{
			pd3dDeviceInfo->pFSModes = new D3DFSMode;
			pMode = pd3dDeviceInfo->pFSModes;
		}
		else
		{
			pMode =  pd3dDeviceInfo->pFSModes;
			while (pMode->pNext != NULL)
				pMode = pMode->pNext;
			pMode->pNext = new D3DFSMode;
			pMode = pMode->pNext;
		}
		
        pMode->SurfDesc.ddpfPixelFormat.dwRGBBitCount = devmode.dmBitsPerPel;
        pMode->dwWidth  = 0;
        pMode->dwHeight = 0;
		strncpy(pMode->szDescription, "Windowed", 10);
		pMode->pNext = NULL;

        pd3dDeviceInfo->dwNumFSModes = 1;
    }
	
	//enum all the display modes for this current device
	pDD7->EnumDisplayModes( 0, NULL, pd3dDeviceInfo, EnumModesCallback );

	/////////// end of dx7 add-ons
    
	// Enumerate the various D3D devices (Hardware, Software, etc.) for this 
	// DirectDraw device
	//
	pD3D7->EnumDevices( EnumD3DDevicesCallback, pWrapper );

	pWrapper->m_iDrivers++;

	SAFE_RELEASE( pD3D7 );
	SAFE_RELEASE( pDD7 );

	return TRUE;
}

// -----------------------------------------------------------------------
// HRESULT WINAPI EnumModesCallback(  LPDDSURFACEDESC2 lpDDSurfaceDesc,
//									  LPVOID lpContext )
//
// -----------------------------------------------------------------------
HRESULT WINAPI EnumModesCallback(  LPDDSURFACEDESC2 lpDDSurfaceDesc,  
								   LPVOID lpContext )
{
    //CDRGWrapper *pWrapper = (CDRGWrapper *)lpContext;
	D3DDEVICEINFO *pd3dDeviceInfo = (D3DDEVICEINFO *)lpContext;
	D3DFSMode *pNewMode;

	if( !lpDDSurfaceDesc )
		return DDENUMRET_OK;
	//
	// Ignore modes that are not 16 or 32 bit
	//
//	if(( lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount != 16 ) &&
//		( lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount != 32 ) )
//		return DDENUMRET_OK;

	//skip through already-enumerated modes
    if( pd3dDeviceInfo->pFSModes )
	{
        D3DFSMode *pTemp = pd3dDeviceInfo->pFSModes;
		while( pTemp->pNext )
			pTemp = pTemp->pNext;
		pTemp->pNext = new D3DFSMode;
		pNewMode = pTemp->pNext;
	}
	else
	{
        pNewMode = pd3dDeviceInfo->pFSModes = new D3DFSMode;
	}

	memcpy( &pNewMode->SurfDesc, lpDDSurfaceDesc, sizeof( DDSURFACEDESC2 ) );
	pNewMode->dwWidth = lpDDSurfaceDesc->dwWidth;
	pNewMode->dwHeight = lpDDSurfaceDesc->dwHeight;
	pNewMode->dwBpp = lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount;
	wsprintf( pNewMode->szDescription, "%d x %d - %d Bit", 
		pNewMode->dwWidth,
		pNewMode->dwHeight,
		pNewMode->dwBpp );
	pNewMode->pNext = NULL;
	pd3dDeviceInfo->dwNumFSModes++;

	return DDENUMRET_OK;
}



// -----------------------------------------------------------------------
// HRESULT WINAPI EnumZBufferFormatsCallback( DDPIXELFORMAT* pddpf, void* pddpfDesired )
//
// -----------------------------------------------------------------------
HRESULT WINAPI EnumZBufferFormatsCallback( DDPIXELFORMAT* pddpf, void* pddpfDesired )
{
    if( !pddpf || !pddpfDesired )
        return D3DENUMRET_CANCEL;

	//
	// Check if the supported pixel format matches the desired format
	//
    if( pddpf->dwFlags == ((DDPIXELFORMAT*)pddpfDesired)->dwFlags )
    {
        memcpy( pddpfDesired, pddpf, sizeof(DDPIXELFORMAT) );

		//
		// We'll be happy with 16-bit or higher
		//
		if( pddpf->dwZBufferBitDepth >= 16 )
			return D3DENUMRET_CANCEL;
    }

    return D3DENUMRET_OK;
}



// -----------------------------------------------------------------------
// HRESULT CDRGWrapper::CreateDirectDraw( void )
//
// -----------------------------------------------------------------------
HRESULT CDRGWrapper::CreateDirectDraw( void )
{
	LPDIRECTDRAW7 pDD7 = NULL;
	DWORD dwFlags;
	HRESULT hr;

	//
	// Create the IDirectDraw7 interface. 
	//
	hr = DirectDrawCreateEx( m_pCurrentDevice->pDriverGUID, (void **)&pDD7, IID_IDirectDraw7, NULL );
	HANDLE_ERROR( hr, TRUE );


	m_pDD7 = pDD7;
	//
	// Set the DirectDraw cooperative level
	//
	if( m_bFullscreen )
		dwFlags = DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN; // | DDSCL_FPUSETUP;
	else
		dwFlags = DDSCL_NORMAL; // | DDSCL_FPUSETUP;

	hr = m_pDD7->SetCooperativeLevel( m_hWnd, dwFlags );
	HANDLE_ERROR( hr, hr );

	return S_OK;
}

// -----------------------------------------------------------------------
// HRESULT CDRGWrapper::CreateD3D( void )
//
// -----------------------------------------------------------------------
HRESULT CDRGWrapper::CreateD3D( void )
{
	HRESULT hr;

	//
	// Create the IDirect3D3 interface.
	//
	hr = m_pDD7->QueryInterface( IID_IDirect3D7, (void **)&m_pD3D7 );
	HANDLE_ERROR( hr, hr );

	//
	// Switch the mode if necessary
	//
	if( m_bFullscreen )
	{
		DWORD dwModeFlags = 0;

		hr = m_pDD7->SetDisplayMode( m_dwRenderWidth, m_dwRenderHeight,
									m_dwBpp, 0, dwModeFlags );
		HANDLE_ERROR( hr, hr );
	}

	return S_OK;
}

// -----------------------------------------------------------------------
// HRESULT CDRGWrapper::CreateSurfaces( DWORD dwBackBufferCount )
//
// -----------------------------------------------------------------------
HRESULT CDRGWrapper::CreateSurfaces( DWORD dwBackBufferCount )
{
	HRESULT hr;
	DDSURFACEDESC2 ddsd;
	DDSCAPS2 ddscaps;

	//
	// Create the primary surface
	//
	memset( &ddsd, 0, sizeof( DDSURFACEDESC2 ) );
	ddsd.dwSize = sizeof( DDSURFACEDESC2 );
	ddsd.ddpfPixelFormat.dwSize = sizeof( DDPIXELFORMAT );
	if( m_bFullscreen )
	{
		ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
		ddsd.dwBackBufferCount = dwBackBufferCount;
	}
	else
	{
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	}

	hr = m_pDD7->CreateSurface( &ddsd, &m_pFrontBuffer, NULL );
	HANDLE_ERROR( hr, hr );

	if( !m_bFullscreen )
	{
		//
		// Create a clipper if we're rendering to a window
		//
		LPDIRECTDRAWCLIPPER pClipper;
		hr = m_pDD7->CreateClipper( 0, &pClipper, NULL );
		HANDLE_ERROR( hr, hr );

		//
		// Attach the clipper to the front buffer
		//
		pClipper->SetHWnd( 0, m_hWnd );
		m_pFrontBuffer->SetClipper( pClipper );
		SAFE_RELEASE( pClipper );
	}


	//
	// Create the back buffers
	if( m_bFullscreen )
	{
		ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
		ddscaps.dwCaps2 = 0;
		ddscaps.dwCaps3 = 0;
		ddscaps.dwCaps4 = 0;
		
		hr = m_pFrontBuffer->GetAttachedSurface( &ddscaps, &m_pBackBuffer );
		switch (hr){
		case DDERR_INVALIDOBJECT:
			OutputDebugString("help");
			break;
		case DDERR_INVALIDPARAMS:
			OutputDebugString("help");
			break;
		case DDERR_SURFACELOST:
			OutputDebugString("help");
			break;
		case DDERR_NOTFOUND:
			OutputDebugString("help");
			break;
		}




		HANDLE_ERROR( hr, hr );

		//
		// Setup the rectangles for blting
		//
		SetRect( &m_rcView, 0, 0, m_dwRenderWidth, m_dwRenderHeight );
		memcpy( &m_rcScreen, &m_rcView, sizeof(RECT) );
	}
	else
	{
		ddsd.dwFlags        = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
		ddsd.dwWidth        = m_dwRenderWidth;
		ddsd.dwHeight       = m_dwRenderHeight;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;

		hr = m_pDD7->CreateSurface( &ddsd, &m_pBackBuffer, NULL );
		HANDLE_ERROR( hr, hr );

		//
		// Setup the blt rectangles
		//
		GetClientRect( m_hWnd, &m_rcView );
		GetClientRect( m_hWnd, &m_rcScreen );
		ClientToScreen( m_hWnd, (POINT*)&m_rcScreen.left );
		ClientToScreen( m_hWnd, (POINT*)&m_rcScreen.right );
	}

	return S_OK;
}

// -----------------------------------------------------------------------
// HRESULT CDRGWrapper::CreateZBuffer( void )
//
// -----------------------------------------------------------------------
HRESULT CDRGWrapper::CreateZBuffer( void )
{
	DDSURFACEDESC2 ddsd;
	DDPIXELFORMAT ddpfZBuffer;
	HRESULT hr;

	//
	// Enumerate the supported z-buffer formats to find one we like
	//
	memset( &ddpfZBuffer, 0, sizeof(DDPIXELFORMAT) );
	ddpfZBuffer.dwFlags = DDPF_ZBUFFER;	// EFTU: Can add DDPF_STENCILBUFFER to 
										//       get a stencil buffer

    hr = m_pD3D7->EnumZBufferFormats( m_pCurrentDevice->guidDevice, EnumZBufferFormatsCallback,
                                (void*)&ddpfZBuffer );
	HANDLE_ERROR( hr, hr );

    if( sizeof(DDPIXELFORMAT) != ddpfZBuffer.dwSize )
        return D3DERR_ZBUFFER_NOTPRESENT;

	//
	// Request a z-buffer in the appropriate format
	//
	memset( &ddsd, 0, sizeof( DDSURFACEDESC2 ) );
	ddsd.dwSize = sizeof( DDSURFACEDESC2 );
	ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_PIXELFORMAT;
	if( m_bHardware )
		ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | DDSCAPS_VIDEOMEMORY;
	else
		ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | DDSCAPS_SYSTEMMEMORY;
	ddsd.dwWidth = m_dwRenderWidth;
	ddsd.dwHeight = m_dwRenderHeight;
	memcpy( &ddsd.ddpfPixelFormat, &ddpfZBuffer, sizeof( DDPIXELFORMAT ) );

	//
	// EFTU:  Could check for the D3DPRASTERCAPS_ZBUFFERLESSHSR capability
	//		  and not allocate a z-buffer if the bit is set
	//

	//
	// Create the z-buffer
	//
	hr = m_pDD7->CreateSurface( &ddsd, &m_pZBuffer, NULL );
	HANDLE_ERROR( hr, hr );

	//
	// Attach the z-buffer to the back buffer (ideally you would attach the
	// z-buffer to the render target but the way we've implemented things here,
	// the back buffer is always the render target).
	// 
	hr = m_pBackBuffer->AddAttachedSurface( m_pZBuffer );
	HANDLE_ERROR( hr, hr );

    return S_OK;

}

// -----------------------------------------------------------------------
// HRESULT CDRGWrapper::Create3DDevice( void )
//
// -----------------------------------------------------------------------
HRESULT CDRGWrapper::Create3DDevice( void )
{
	DDSURFACEDESC2 ddsd;
	HRESULT hr;

	//
	// We aren't going to support palettes, so make sure we're not
	// in a palettized mode.  EFTU:  Support palettes if you want to (Ha Ha)
	//
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	m_pDD7->GetDisplayMode( &ddsd );
	if( ddsd.ddpfPixelFormat.dwRGBBitCount <= 8 )
		return D3DERR_INVALID_DEVICE;

	//
	// Create the device
	//
	hr = m_pD3D7->CreateDevice( m_pCurrentDevice->guidDevice, m_pBackBuffer,
								&m_pD3DDevice7);
	
	HANDLE_ERROR( hr, hr );

    return S_OK;

}

// -----------------------------------------------------------------------
// HRESULT CDRGWrapper::CreateViewport( void )
//
// -----------------------------------------------------------------------
HRESULT CDRGWrapper::CreateViewport( void )
{
    HRESULT      hr;
    D3DVIEWPORT7 viewData;

	//
	// Initialize the parameters for the viewport
	//
	memset( &viewData, 0, sizeof( D3DVIEWPORT7 ) );

	viewData.dwWidth = m_dwRenderWidth;
	viewData.dwHeight = m_dwRenderHeight;

	viewData.dvMaxZ = 1.0f;

	//
	// Set this viewport as the current viewport for the 3D device
	//
	hr = m_pD3DDevice7->SetViewport(  &viewData );
	HANDLE_ERROR( hr, hr );

    return S_OK;
}


