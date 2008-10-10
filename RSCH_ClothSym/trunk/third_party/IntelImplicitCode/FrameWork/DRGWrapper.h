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

#if !defined(AFX_DRGWRAPPER_H__CFF24424_47D6_11D2_B1A5_00A0C90D2C1B__INCLUDED_)
#define AFX_DRGWRAPPER_H__CFF24424_47D6_11D2_B1A5_00A0C90D2C1B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define		REQUIREBUMPMAPPING		0x00000001	//flags used when enumerating hardware
#define		REQUIRETARGETTEXTURE	0x00000002	


//Structure defining an individual display mode. A linked list of
//these is constructed from the selected device.
typedef struct tagD3DFSMode
{
	DWORD dwWidth;				// Width (in pixels, of course)	
	DWORD dwHeight;				// Height
	DWORD dwBpp;				// Color depth
	char szDescription[30];		// 
	DDSURFACEDESC2 SurfDesc;	// The DirectDraw Surface Descriptor describing the surface.
								//  the width and height are contained in the descriptor, but 
								//  have been placed in this structure.
	struct tagD3DFSMode *pNext;	// ptr to next mode in the linked list.
} D3DFSMode;

//Structure describing a Device found. A 2 linked lists of these is constructed during
// device enumeration, one for actual physical DDraw devices (1 per card), and then a 
// list of D3DDevices from those (N per card)
struct D3DDEVICEINFO
{
    // D3D Device info
    CHAR            strDesc[40];
    GUID*           pDeviceGUID;
    D3DDEVICEDESC7  ddDeviceDesc;
    BOOL            bHardware;

    // DDraw Driver info
    GUID*           pDriverGUID;
    DDCAPS          ddDriverCaps;
    DDCAPS          ddHELCaps;
	
    // DDraw Mode Info (one that will be used for FS by default
    D3DFSMode		*pDefaultFSMode;
    BOOL            bFullscreen;

    // For internal use (apps should not need these)
    GUID            guidDevice;
    GUID            guidDriver;
	D3DFSMode       *pFSModes;  //linked list of FS modes enumerated by the device
    DWORD           dwNumFSModes;
    DWORD           dwCurrentMode;
    BOOL			bDesktopCompatible;
	D3DDEVICEINFO	*pNext;
};

// The Wrapper class itself. 
class CDRGWrapper  
{
private:
	//don't know why, but had to make this public so the enum function would be able to access it, even though it was a friend
	int m_iCurrentDevice;		//The current one being used.
	int m_iCurrentMode;			//The current mode on that device being used.
	bool m_bInitialized;		// The device has been init'd
	DWORD m_dwBpp;				// Current Color depth
	DWORD m_dwFSRenderWidth;	// Current resolution
	DWORD m_dwFSRenderHeight;	//
	DWORD m_dwFSBpp;			// Color depth in Full Screen (remember, we may toggle)
	RECT m_rcWindow;			// Location of the window in windowed mode

	RECT m_rcScreen, m_rcView;	//

	D3DFSMode *m_pCurrentMode;	// pointer to the current mode being used on that device.

	D3DDEVICEINFO * m_pFirstHWDriver; //list of the physical devices (i.e. one per graphics card)
	D3DDEVICEINFO * m_pFirstD3DDevice; //list of all the D3D device (i.e. N per graphics card)

	//
	// Helper functions for creating the stuff required to initialize a mode
	//
	HRESULT CreateDirectDraw( void );
	HRESULT CreateD3D( void );
	HRESULT CreateSurfaces( DWORD dwBackBufferCount );
	HRESULT CreateZBuffer( void );
	HRESULT Create3DDevice( void );
	HRESULT CreateViewport( void );

	//
	// The following member variables are public for now, but they should
	// probably be replaced with private variables and public access methods
	//
public:

	IUnknown *m_pClientSite;
	int m_iDrivers;				//Total number of drivers that were enumerated
	int m_iDevices;				//Total number of devices that were enumerated

	

	//These two were changed from private to public since the app may need to check them. i.e. 
	//If I do an init mode, and then wonder if what I got was hardware...
	bool m_bFullscreen;			// We toggle between FS and windowed. This tracks it.
	bool m_bHardware;			// Is hardware rendering being used.

	HWND m_hWnd;				// Window handle
	DWORD m_dwRenderWidth;
	DWORD m_dwRenderHeight;
	bool m_bZBuffer;			// Is a Z buffer being used.
	DWORD m_dwBackBufferCount;	// How many back buffers are being used? (Not always one!)

	D3DDEVICEINFO *m_pCurrentDevice;// pointer to the device being used.	
	
	
	LPDIRECTDRAW7 m_pDD7;
	LPDIRECT3D7 m_pD3D7;
	LPDIRECTDRAWSURFACE7 m_pFrontBuffer;
	LPDIRECTDRAWSURFACE7 m_pBackBuffer;
	LPDIRECTDRAWSURFACE7 m_pZBuffer;
	LPDIRECT3DDEVICE7 m_pD3DDevice7;

	float m_fFrameTime;			//current frame time.
	
public:
    CDRGWrapper( HWND hWnd );
    virtual ~CDRGWrapper();

	//
	// Internal functions for dealing with the linked lists of drivers and modes
	//
	virtual D3DDEVICEINFO *GetDevice( int iDevice );
	virtual D3DFSMode *GetMode( int iDevice, int iMode );

	virtual bool Initialized( void ) { return m_bInitialized; }

	virtual int GetNumDevices( void );
	virtual int GetNumModes( int iDevice );

	
	virtual HRESULT GetBestDevice( int &iDevice,				//Helper function that lets the app
								   bool bHardware = true,		// get the best device for the job, 
								   bool bAGP = false,			// based on some common requests 
								   DWORD dwOtherFlags = 0 );    // (hardware? AGP? etc. Other flags
																// are for user required features 
																// (i.e. "my demo requires render-to-
																// texture")
	virtual HRESULT EnumModes( int iDevice, int *iModes, D3DFSMode **pModes );
	
	virtual HRESULT InitMode( int iDevice,						//Helper function to init the
							  int iMode,						//chosen device to the chose mode.
							  DWORD dwBackBufferCount = 1,		//Init mode is overloaded so you
							  bool bZBuffer = true );			//can call it with either a mode
	virtual HRESULT InitMode( int iDevice = 0,					//number (look for the one you want)
							  bool bFullScreen = true,			//or specify some parameters and
							  DWORD dwWidth = 640,				//have it look for the right mode.
							  DWORD dwHeight = 480, 
							  DWORD dwBpp = 16,
							  DWORD dwBackBufferCount = 1,
							  bool bZBuffer = true, 
							  bool bUseDefaultOnError = true );
	virtual HRESULT DestroyMode( void );
	virtual HRESULT Clear( DWORD dwFlags );
	virtual HRESULT StartFrame( void );
	virtual HRESULT EndFrame( void );

	virtual HRESULT MoveWindow( void );
	virtual HRESULT RefreshMode( bool bFullscreen );
	virtual HRESULT ChangeDevice( int iDevice );

	friend BOOL WINAPI EnumDDrawDeviceCallback( GUID FAR *lpGUID, 
												LPSTR lpDriverDescription,
												LPSTR lpDriverName,
												LPVOID lpContext, 
												HMONITOR hm );
	friend HRESULT CALLBACK EnumD3DDevicesCallback(  LPSTR lpDeviceDescription,             
													 LPSTR lpDeviceName,                  
													 LPD3DDEVICEDESC7 lpD3DDeviceDesc,     
													 LPVOID lpContext );
	
	friend HRESULT WINAPI EnumModesCallback(  LPDDSURFACEDESC2 lpDDSurfaceDesc,  
											  LPVOID lpContext );

};

#endif // !defined(AFX_DRGWRAPPER_H__CFF24424_47D6_11D2_B1A5_00A0C90D2C1B__INCLUDED_)
