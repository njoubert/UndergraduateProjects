// DRGTexture.cpp - Created with DR&E AppWizard 1.0B10
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
// DRGTexture.cpp: implementation of the CDRGTexture class.
// Originally adapted from some DX6 sample code. Added some robustness and extra 
// functionality (alpha bitmaps, creating inverse heightmaps for embossing, automatic
// resize for HW requirements, support for envbump,  support for rendering to textures, 
// etc)
//
// ------------------------------------------------------------------------------------
//
// Authors: Dean Macri, Kim Pallister - Intel Developer Relations & Engineering Group
//
////////////////////////////////////////////////////////////////////////////////////////

#include <tchar.h>

#include "..\DRGShell.h"

struct TEXTURESEARCHINFO
{
	DWORD dwDesiredBPP; // Input for texture format search
	BOOL  bUseAlpha;
	DWORD dwDesiredAlphaBPP;
	BOOL  bUsePalette;
	BOOL  bUseFourCC;
	BOOL  bUseBumpDUDV;
	BOOL  bFoundGoodFormat;

	DDPIXELFORMAT* pddpf; // Result of texture format search
};


//-----------------------------------------------------------------------------
// Name: TextureSearchCallback()
// Desc: Enumeration callback routine to find a best-matching texture format. 
//       The param data is the DDPIXELFORMAT of the best-so-far matching
//       texture. Note: the desired BPP is passed in the dwSize field, and the
//       default BPP is passed in the dwFlags field.
// Return: If the current texture format being looked at is a good match, we set
//       the ptsi->bFoundGoodFormat to true. Otherwise, it remains false.
//		 either way, returning DDENUMRET_OK indicates to continue the enumeration
//       process (which we do when we realize the current format is no good). 
//		 Returning DDENUMRET_CANCEL indicates to stop enumerations (which we want
//		 to do if we have found a good texture format).
//-----------------------------------------------------------------------------
static HRESULT CALLBACK TextureSearchCallback( DDPIXELFORMAT* pddpf, VOID* param )
{
	//if bad data in...
    if( NULL==pddpf || NULL==param )
        return DDENUMRET_OK;

	TEXTURESEARCHINFO* ptsi = (TEXTURESEARCHINFO*)param;

    if (ptsi->bUseBumpDUDV)
	{
		//for now, I haven't supported the luminance flags, that's next
		if ( pddpf->dwFlags & (DDPF_BUMPLUMINANCE | DDPF_LUMINANCE))
		{
			//this tex format is not the bump map we want
			return DDENUMRET_OK;
		}
		// make sure we get a BUMPDUDV mode;		
		if (!( pddpf->dwFlags & DDPF_BUMPDUDV ))
		{
			//this tex format is not a bump map
			return DDENUMRET_OK;
		}
		else
		{
			OutputDebugString("desired bump map format found \n");
		}
	}
	else
	{
		// Skip any bump mapping and/or luminance modes, since they werem't requested 
		if( pddpf->dwFlags & (DDPF_LUMINANCE|DDPF_BUMPLUMINANCE|DDPF_BUMPDUDV) )
			return DDENUMRET_OK;	
	}

	// Check for palettized formats
	if( ptsi->bUsePalette )
	{
		if( !( pddpf->dwFlags & DDPF_PALETTEINDEXED8 ) )
			return DDENUMRET_OK;

		// Accept the first 8-bit palettized format we get
        memcpy( ptsi->pddpf, pddpf, sizeof(DDPIXELFORMAT) );
		ptsi->bFoundGoodFormat = TRUE;
        return DDENUMRET_CANCEL;
    }

	// Else, skip any paletized formats (all modes under 16bpp)
	// NOTE, THAT MEANS WE ARE NOT CURRENTLY HANDLING PALETTIZED TEXTURES
	if( pddpf->dwRGBBitCount < 16 )
		return DDENUMRET_OK;

	// Check for FourCC formats
    if( ptsi->bUseFourCC )
	{
		if( pddpf->dwFourCC == 0 )
		    return DDENUMRET_OK;

		return DDENUMRET_CANCEL;
	}

	// Else, skip any FourCC formats
	if( pddpf->dwFourCC != 0 )
		return DDENUMRET_OK;

	// Make sure current alpha format agrees with requested format type
	// (i.e. If the user asks for alpha, and the surface doesn't have an
	// alpha channel, or vice versa, then we want to get out)
	if( (ptsi->bUseAlpha==TRUE) && !(pddpf->dwFlags&DDPF_ALPHAPIXELS) )
		return DDENUMRET_OK;
	if( (ptsi->bUseAlpha==FALSE) && (pddpf->dwFlags&DDPF_ALPHAPIXELS) )
		return DDENUMRET_OK;

    // Check if we found a good match
    if( pddpf->dwRGBBitCount == ptsi->dwDesiredBPP )
    {
		//if requesting to use alpha, then format must have required alpha
		if ((ptsi->bUseAlpha) || (ptsi->dwDesiredAlphaBPP == 1))
		{
			// dwAlphaBitDepth member reflects the bit depth of an alpha-only 
			// pixel format. For pixel formats with RGBA, we have to 
			// do the following
			
			if ((pddpf->dwFlags && DDPF_ALPHAPIXELS)==0)				
			{
				//rejecting, format had no alpha
				return	DDENUMRET_OK;		
			}
			else
			{
				if	((ptsi->dwDesiredAlphaBPP == 4) && (pddpf->dwRGBAlphaBitMask != 0xf000))
				{
					//rejecting. Format had alpha, but not 4 bits
					return	DDENUMRET_OK;		
				}
				if	((ptsi->dwDesiredAlphaBPP == 8) && (pddpf->dwRGBAlphaBitMask != 0xff000000))
				{
					//rejecting. Format had alpha, but not 8 bits
					return	DDENUMRET_OK;		
				}
			}					
			
			//else successful, so fall through
		}

		//if we fell through to here, then we found what we wanted
        memcpy( ptsi->pddpf, pddpf, sizeof(DDPIXELFORMAT) );
		ptsi->bFoundGoodFormat = TRUE;
        return DDENUMRET_CANCEL;
    }
	//if we fell through to here, then the bpp didn't match what we wanted
    return DDENUMRET_OK;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDRGTexture::CDRGTexture()
{
	//init the member variables.
	hbmBitmap = NULL;
	hbmAlphaBitmap = NULL;
//    pddsSurface = NULL;
    ptexTexture = NULL;
    dwStage = 0;
    bHasAlpha = false;
	dwFlags = 0;	
}

void CDRGTexture::Cleanup()
{
	//
	// Release our stored bitmaps
	//
	if (hbmBitmap)
	{
		DeleteObject( hbmBitmap );
		hbmBitmap = NULL;
	}
	
	if (hbmAlphaBitmap)		
	{
		DeleteObject( hbmAlphaBitmap );
		hbmAlphaBitmap = NULL;
	}
    
	if (ptexTexture)
	{
		ptexTexture->Release();
		ptexTexture = NULL;
	}
	
    dwStage = 0;
	bHasAlpha = false;
	dwFlags = 0;
}

CDRGTexture::~CDRGTexture()
{
	Cleanup();
}


//-----------------------------------------------------------------------------
// Name: D3DTextr_CreateTexture()
// Desc: Loads an image using the passed in CImageLoader and creates a texture from it.
//       The texture can not be used until it is restored, however.
//-----------------------------------------------------------------------------
HRESULT CDRGTexture::CreateTexture( CImageLoader &TextureLoader, DWORD dwTextureStage, DWORD dwTextureFlags )
{
	Cleanup();

	//copy parameters into member vars
	m_pTextureLoader = &TextureLoader;
	m_pAlphaTextureLoader = NULL;	// No alpha in this case
    dwStage = dwTextureStage;
    dwFlags = dwTextureFlags;

    // Create a bitmap and load the texture file into it,
    if( FAILED( LoadTextureImage() ) )
	{
		m_pTextureLoader = NULL;
        return E_FAIL;
	}

	//
	// This may not be usable after returning from this function so set it to NULL
	//
	m_pTextureLoader = NULL;

    return S_OK;
}

HRESULT CDRGTexture::CreateTexture( CImageLoader &TextureLoader, CImageLoader &AlphaTextureLoader, DWORD dwTextureStage, DWORD dwTextureFlags )
{
	Cleanup();

	//copy parameters into member vars
	m_pTextureLoader = &TextureLoader;
	m_pAlphaTextureLoader = &AlphaTextureLoader;
    dwStage = dwTextureStage;
    dwFlags = dwTextureFlags;

	//because this function got called, we are trying to load an alpha texture as well
	bHasAlpha = true;

    // Create a bitmap and load the texture file into it,
    if( FAILED( LoadTextureImage() ) )
	{
		m_pTextureLoader = NULL;
		m_pAlphaTextureLoader = NULL;
        return E_FAIL;
	}

	//
	// These may not be usable after returning from this function so set them to NULL
	//
	m_pTextureLoader = NULL;
	m_pAlphaTextureLoader = NULL;

    return S_OK;
}

//creates a blank texture
HRESULT CDRGTexture::CreateTexture(  CDRGWrapper *pWrapper, DWORD dwHres, DWORD dwVres,
									DWORD dwBpp, DWORD dwTextureStage, DWORD dwTextureFlags, DWORD	*pBits)
{
	dwStage = dwTextureStage;
    dwFlags = dwTextureFlags;

	// CREATE A TEMP SURFACE IN SYSMEM WITH SIMILAR FORMAT AS THE DEVICE
	
	// Setup the new surface desc
    DDSURFACEDESC2 ddsd;
	ZeroMemory( &ddsd, sizeof(DDSURFACEDESC2) );
    ddsd.dwSize                 = sizeof(DDSURFACEDESC2);
    ddsd.dwFlags                = 0;
    ddsd.ddsCaps.dwCaps         = 0;
    ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	
	// Get info on the target surface, so we can create one similar, but in sys mem
    pWrapper->m_pBackBuffer->GetSurfaceDesc( &ddsd );
    ddsd.dwFlags          = DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT|
		                    DDSD_TEXTURESTAGE;
    ddsd.ddsCaps.dwCaps   = DDSCAPS_TEXTURE|DDSCAPS_SYSTEMMEMORY;
    ddsd.ddsCaps.dwCaps2  = 0L;
    ddsd.dwWidth          = 16; // Since we are only using this to get a compatible DC, 
    ddsd.dwHeight         = 16; // there is no need to create anything very large

    // Create the temp sysmem surface
    LPDIRECTDRAWSURFACE7 pddsTempSurface;
	HRESULT hr;
    if( FAILED( hr = pWrapper->m_pDD7->CreateSurface( &ddsd, &pddsTempSurface, NULL ) ) )
	{
        OutputDebugString("Couldn't create a temp surface \n");
		return E_FAIL;
	}

	HDC hdcBitmap; 

	//get a DC from the temp surface
	hr = pddsTempSurface->GetDC(&hdcBitmap);
	if (hr == DD_OK)
	{
		hbmBitmap = CreateCompatibleBitmap(  hdcBitmap, dwHres,  dwVres);
		hr = pWrapper->m_pFrontBuffer->ReleaseDC(hdcBitmap);
		pddsTempSurface->Release();
	}

	// If array of bits was supplied to fill bitmap, copy them over
	if(pBits)
	{
		if (SetBitmapBits( hbmBitmap, (4* dwHres * dwVres), pBits) == 0)
			return E_FAIL;
	}  
		
	return S_OK;    
}



//-----------------------------------------------------------------------------
// Name: LoadTextureImage()
// Desc: Loads a texture map file (or resource) into a BITMAP surface.
//-----------------------------------------------------------------------------
HRESULT CDRGTexture::LoadTextureImage()
{

	//
	// Load the Texture
	//
	if( m_pTextureLoader )
	{
		HBITMAP *pAlpha = NULL;
		if( !m_pAlphaTextureLoader )
			pAlpha = &hbmAlphaBitmap;
		if( FAILED( m_pTextureLoader->LoadImage( &hbmBitmap, pAlpha ) ) )
			return DDERR_NOTFOUND;
		if( hbmBitmap == NULL )
			return DDERR_NOTFOUND;
		if( hbmAlphaBitmap )
			bHasAlpha = TRUE;
	}
	
	if( bHasAlpha && m_pAlphaTextureLoader && !hbmAlphaBitmap )
	{
		if( FAILED( m_pAlphaTextureLoader->LoadImage( &hbmAlphaBitmap, NULL ) ) )
			return DDERR_NOTFOUND;
		if( hbmAlphaBitmap == NULL )
			return DDERR_NOTFOUND;
	}

	//last sanity check.
	if (hbmBitmap)
	{
		if (bHasAlpha)
		{
			if (hbmAlphaBitmap)
				return DD_OK; // alpha and bitmap found
			else
			{
				//
				// Free the loaded Bitmap
				//
				DeleteObject( hbmBitmap );
				hbmBitmap = NULL;
				return DDERR_NOTFOUND; //asked for alpha, but alpha not found
			}
		}
		else
			return DD_OK; //no alpha, bitmap found
	}
	else if( hbmAlphaBitmap )
	{
		DeleteObject( hbmAlphaBitmap );
		hbmAlphaBitmap = NULL;
	}

	return DDERR_NOTFOUND; //bitmap couldn't load
}



//-----------------------------------------------------------------------------
// Name: DRGTexturer:Restore()
// Desc: Invalidates the current texture object and rebuild it with the 
//       given device
//-----------------------------------------------------------------------------
HRESULT CDRGTexture::Restore(LPDIRECT3DDEVICE7 pd3dDevice )
{
    // Check params
    if( NULL == pd3dDevice )
        return DDERR_INVALIDPARAMS;

	// Release previously created object
    SAFE_RELEASE( ptexTexture );
    
	// Restore the texture surface from the bitmap image. At this point, code
	// can be added to handle other texture formats, such as those created from
	// .dds files, .jpg files, or whatever else.
	return RestoreFromBitmap( pd3dDevice );
}

//-----------------------------------------------------------------------------
// Name: DRGTexturer:Restore(dev, stage)
// Desc: same as above, but changes the stage that the texture was originally 
// created for. This is used when falling back from a multi-texture approach
// to a multi-pass approach, or similar
//-----------------------------------------------------------------------------
HRESULT CDRGTexture::Restore(LPDIRECT3DDEVICE7 pd3dDevice, DWORD dwNewStage )
{
	dwStage = dwNewStage;
	return Restore( pd3dDevice );
}


//-----------------------------------------------------------------------------
// Name: RestoreFromBitmap()
// Desc: Invalidates the current texture objects and rebuilds new ones
//       using the new device.
//-----------------------------------------------------------------------------
HRESULT CDRGTexture::RestoreFromBitmap( LPDIRECT3DDEVICE7 pd3dDevice )
{
    // Get the DirectDraw interface for creating surfaces
    LPDIRECTDRAW7 pDD;
    if( NULL == ( pDD = GetDirectDrawFromDevice( pd3dDevice ) ) )
        return E_FAIL;
    
    // Get the device caps
	D3DDEVICEDESC7 devDesc;
    DWORD         dwDeviceCaps;
    
	if( FAILED( pd3dDevice->GetCaps( &devDesc ) ) )
        return E_FAIL;
    
	bool bHardware = false;

	if( devDesc.dwDevCaps & D3DDEVCAPS_HWRASTERIZATION) 
		bHardware = true;

	dwDeviceCaps = devDesc.dpcTriCaps.dwTextureCaps;

	// Get the bitmap structure (to extract width, height, and bpp)
    BITMAP bm;
    GetObject( hbmBitmap, sizeof(BITMAP), &bm );
    DWORD dwWidth  = (DWORD)bm.bmWidth;
    DWORD dwHeight = (DWORD)bm.bmHeight;

	//Currently, we fail if the alpha bitmap is a different size.
	// at some point, we should fix this to scale the smaller to fit the larger
	if (hbmAlphaBitmap)
	{
		BITMAP bmtemp;
		GetObject( hbmAlphaBitmap, sizeof(BITMAP), &bmtemp );
		if ((dwWidth  != (DWORD)bmtemp.bmWidth) || (dwHeight != (DWORD)bmtemp.bmHeight))
		{
			OutputDebugString ("Alpha and Color bitmaps are different sizes! Failing...\n");
			return E_FAIL;
		}
	}
	
    // Setup the new surface desc
    ZeroMemory( &m_ddsd, sizeof(DDSURFACEDESC2) );
    m_ddsd.dwSize                 = sizeof(DDSURFACEDESC2);
    m_ddsd.dwFlags                = 0;
    m_ddsd.ddsCaps.dwCaps         = 0;
    m_ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);


    m_ddsd.dwFlags         = DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|
                           DDSD_PIXELFORMAT|DDSD_TEXTURESTAGE;//|DDSD_ALPHABITDEPTH;
    
	//we can't use DX texture management if we are doing target textures, or trying to force in agp
    if (dwFlags & (DRGTEXTR_RENDERTARGET | DRGTEXTR_FORCEINAGP ))
	{
		//if trying to create a texture you can render to on a 3Dfx (should handle this better at some point)
		if ((devDesc.dwDevCaps & D3DDEVCAPS_SEPARATETEXTUREMEMORIES) && (dwFlags & DRGTEXTR_RENDERTARGET))
			OutputDebugString("WARNING! DEV HAS SEPARATE TEXTURE MEMORIES. CREATING TARGET TEXTURE LIKELY WONT WORK\n");
		
		if (!(devDesc.dwDevCaps & D3DDEVCAPS_TEXTURENONLOCALVIDMEM) && (dwFlags & DRGTEXTR_FORCEINAGP))
			OutputDebugString("WARNING! TRYING TO FORCE A TEXTURE INTO AGP ON A DEVICE THAT CANT TEXTURE FROM THERE. WATCH OUT\n");

		if (bHardware)
			m_ddsd.ddsCaps.dwCaps  = (DDSCAPS_TEXTURE | DDSCAPS_VIDEOMEMORY);
		else
			m_ddsd.ddsCaps.dwCaps  = (DDSCAPS_TEXTURE | DDSCAPS_SYSTEMMEMORY);

		if (dwFlags & DRGTEXTR_RENDERTARGET)
			m_ddsd.ddsCaps.dwCaps |= DDSCAPS_3DDEVICE;

		if (dwFlags & DRGTEXTR_FORCEINAGP)
			m_ddsd.ddsCaps.dwCaps |= DDSCAPS_NONLOCALVIDMEM;				
	}
	else
	{
		//default case, use DX texture management, unless running in SW
		m_ddsd.ddsCaps.dwCaps  = DDSCAPS_TEXTURE;
		m_ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;

		if (bHardware == false)
		{
			m_ddsd.ddsCaps.dwCaps  = (DDSCAPS_TEXTURE | DDSCAPS_SYSTEMMEMORY);
			m_ddsd.ddsCaps.dwCaps2 = 0;
		}
	}

	if (dwFlags & (DRGTEXTR_FORCEINSYSMEM | DRGTEXTR_FORCEINVIDMEM))
	{
		m_ddsd.ddsCaps.dwCaps  = (DDSCAPS_TEXTURE | DDSCAPS_SYSTEMMEMORY);
		m_ddsd.ddsCaps.dwCaps2 = 0;
		if ((dwFlags & DRGTEXTR_FORCEINVIDMEM) && (bHardware == true))
			m_ddsd.ddsCaps.dwCaps  = (DDSCAPS_TEXTURE | DDSCAPS_VIDEOMEMORY);
	}

	if (dwFlags & DRGTEXTR_DDSURFACEONLY)
	{
		m_ddsd.ddsCaps.dwCaps  = DDSCAPS_VIDEOMEMORY;
		if (bHardware == false)
			m_ddsd.ddsCaps.dwCaps  = DDSCAPS_SYSTEMMEMORY;

		m_ddsd.ddsCaps.dwCaps2 = 0;
	}
			
	//now handle creation of a bump map
	BOOL bUseBumpDUDV = false;
	if (dwFlags & DRGTEXTR_CREATEBUMPMAP_DUDV)
	{
		bUseBumpDUDV = true;
		m_ddsd.ddpfPixelFormat.dwFlags = (m_ddsd.ddpfPixelFormat.dwFlags | DDPF_BUMPDUDV);
		m_ddsd.ddpfPixelFormat.dwBumpBitCount          = 16;
		m_ddsd.ddpfPixelFormat.dwBumpDuBitMask         = 0x000000ff;
		m_ddsd.ddpfPixelFormat.dwBumpDvBitMask         = 0x0000ff00;
		m_ddsd.ddpfPixelFormat.dwBumpLuminanceBitMask  = 0x00000000;

		if (bHardware)
			m_ddsd.ddsCaps.dwCaps  |= DDSCAPS_VIDEOMEMORY;
		else
			m_ddsd.ddsCaps.dwCaps  |= DDSCAPS_SYSTEMMEMORY;

		m_ddsd.ddsCaps.dwCaps2 = 0;
	}

	//caution: The dwStage member doesn't matter on a lot of HW, but needs to be set explicitly
	// on HW where textures are stored in separate memory spaces depending on stage (3Dfx 
	// Voodoo 2)for example)
    m_ddsd.dwTextureStage  = dwStage;
    m_ddsd.dwWidth         = dwWidth;
    m_ddsd.dwHeight        = dwHeight;
	
    // Adjust width and height, if the driver requires it...
	
	//   for 'requires power of 2'
    if( dwDeviceCaps & D3DPTEXTURECAPS_POW2 )
    {
        for( m_ddsd.dwWidth=1;  dwWidth>m_ddsd.dwWidth;   m_ddsd.dwWidth<<=1 );
        for( m_ddsd.dwHeight=1; dwHeight>m_ddsd.dwHeight; m_ddsd.dwHeight<<=1 );
    }
	
	//   for 'requires square texture'
    if( dwDeviceCaps & D3DPTEXTURECAPS_SQUAREONLY )
    {
        if( m_ddsd.dwWidth > m_ddsd.dwHeight ) 
			m_ddsd.dwHeight = m_ddsd.dwWidth;
        else
			m_ddsd.dwWidth  = m_ddsd.dwHeight;
    }
	
	//   for 'I tried to create a texture bigger than the HW supports and I can't get up'
	if ((devDesc.dwMaxTextureWidth < dwWidth) || (devDesc.dwMaxTextureHeight < dwHeight))
	{	
		while(devDesc.dwMaxTextureWidth < dwWidth)
			dwWidth>>=1;
		while(devDesc.dwMaxTextureHeight < dwHeight)
			dwHeight>>=1;
		m_ddsd.dwWidth         = dwWidth;
		m_ddsd.dwHeight        = dwHeight;
	}
	
	BOOL bUsePalette = ( bm.bmBitsPixel <= 8 );
	BOOL bUseAlpha   = FALSE;
		
	if( bHasAlpha )
		bUseAlpha = bHasAlpha;
	
	if( dwFlags & (DRGTEXTR_TRANSPARENTWHITE|DRGTEXTR_TRANSPARENTBLACK) )
	{
		if( bUsePalette )
		{
			if( dwDeviceCaps & D3DPTEXTURECAPS_ALPHAPALETTE )
			{
				bUseAlpha   = TRUE; 
				bUsePalette = TRUE; 
			}
		}
		else
		{
			bUseAlpha   = TRUE; 
			bUsePalette = FALSE;
		}
	}
	
	// Setup the structure to be used for texture enumeration.
	TEXTURESEARCHINFO tsi;
	tsi.pddpf            = &m_ddsd.ddpfPixelFormat;
	tsi.bUseAlpha        = bUseAlpha;
	tsi.bUsePalette      = bUsePalette;
	tsi.bUseFourCC       = ( m_ddsd.ddpfPixelFormat.dwFlags & DDPF_FOURCC );
	tsi.bUseBumpDUDV	 = bUseBumpDUDV;
	tsi.dwDesiredBPP     = 16;
	tsi.bFoundGoodFormat = FALSE;
	if (bHasAlpha)
		tsi.dwDesiredAlphaBPP = 4;
	else
		tsi.dwDesiredAlphaBPP = 0;

	if( dwFlags & DRGTEXTR_32BITSPERPIXEL )
	{
		m_ddsd.dwAlphaBitDepth = 8;
		tsi.dwDesiredBPP = 32;
		tsi.dwDesiredAlphaBPP = 8;
	}
	
	// Enumerate the texture formats, and find the closest device-supported
	// texture pixel format
    pd3dDevice->EnumTextureFormats( TextureSearchCallback, &tsi );

	//if we tried to create a 32bpp and it didn't work, try 16... (3Dfx, PowerVR PCX2)
	if (( dwFlags & DRGTEXTR_32BITSPERPIXEL ) && (tsi.bFoundGoodFormat == false))
	{
		tsi.dwDesiredBPP = 16;
		if(tsi.dwDesiredAlphaBPP == 8)
			tsi.dwDesiredAlphaBPP = 4;
		dwFlags -= DRGTEXTR_32BITSPERPIXEL;			
		pd3dDevice->EnumTextureFormats( TextureSearchCallback, &tsi );
	}

	// If a palettized format was requested, but not found, default to a
	// 16-bit texture format
	if( FALSE == tsi.bFoundGoodFormat && bUsePalette )
	{
		tsi.bUsePalette  = FALSE;
		tsi.dwDesiredBPP = 16;
	    pd3dDevice->EnumTextureFormats( TextureSearchCallback, &tsi );
		if( FALSE == tsi.bFoundGoodFormat )
			return E_FAIL;
	}
	
	// Create a new surface for the texture
    HRESULT hr;
    if( FAILED( hr = pDD->CreateSurface( &m_ddsd, &ptexTexture, NULL ) ) )
	{
	    if (dwFlags & DRGTEXTR_RENDERTARGET)
		{
			OutputDebugString("CreateSurface failed with RenderTarget flags\n");
			OutputDebugString("likely not supported on this device\n");
		}
        return E_FAIL;
	}
	
	//If we are rendering to a texture, and we are using a Z buffer with the main render 
	// target, we need to attach a Zbuffer to the texture
	if (dwFlags & DRGTEXTR_RENDERTARGET)
	{
		LPDIRECTDRAWSURFACE7	pBackBuffer = NULL;	// ptr to back buffer
		LPDIRECTDRAWSURFACE7	pZBuffer = NULL;		// ptr to Z buffer
		LPDIRECTDRAWSURFACE7	pTextureZBuffer = NULL; // surface that will be created and attached
		DDSCAPS2				SurfCaps;		
		DDSURFACEDESC2			SurfDesc;		//the desc of the Z buffer we will create
		DDSURFACEDESC2			ZbuffDesc;		//the desc of the Z buffer of the back buffer
		
		SurfCaps.dwCaps = DDSCAPS_ZBUFFER; 
		SurfCaps.dwCaps2 = SurfCaps.dwCaps3 = SurfCaps.dwCaps4 = 0; 

		hr = pd3dDevice->GetRenderTarget(&pBackBuffer);

		hr = pBackBuffer->GetAttachedSurface(&SurfCaps,&pZBuffer);

		if (pZBuffer != NULL)
		{
			memset( &ZbuffDesc, 0, sizeof( DDSURFACEDESC2 ) );
			ZbuffDesc.dwSize = sizeof( DDSURFACEDESC2 );

			hr = pZBuffer->GetSurfaceDesc(&ZbuffDesc);

			memset( &SurfDesc, 0, sizeof( DDSURFACEDESC2 ) );
			SurfDesc.dwSize = sizeof( DDSURFACEDESC2 );

			//set the flags
			SurfDesc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_PIXELFORMAT;

			//get the device info and z info, and pixel format from the current z buffer
			SurfDesc.ddsCaps.dwCaps = ZbuffDesc.ddsCaps.dwCaps; //this will specify Z buffer, and sysmem or vidmem
			memcpy( &SurfDesc.ddpfPixelFormat, &ZbuffDesc.ddpfPixelFormat, sizeof( DDPIXELFORMAT ) );

			//get the size from the texture surface desc
			SurfDesc.dwWidth = m_ddsd.dwWidth;
			SurfDesc.dwHeight = m_ddsd.dwHeight;

			//now we should have a z buffer description of the right size and format
			// and we can create the surface and attach it.

			hr = pDD->CreateSurface( &SurfDesc, &pTextureZBuffer, NULL );

			hr = ptexTexture->AddAttachedSurface( pTextureZBuffer );
			OutputDebugString("Zbuffer attached to render target surface");
		}		

	}

	//dont need this anymore
	pDD->Release();

    
	// Copy the bitmap to the texture surface
	if (hbmAlphaBitmap)
		return CopyBitmapToSurface( ptexTexture, hbmBitmap, hbmAlphaBitmap, dwFlags );
	else
		return CopyBitmapToSurface( ptexTexture, hbmBitmap, NULL, dwFlags );

}

#define MyGetPixel( bits, x, y, width, height ) \
	(COLORREF)((*(bits + (height-y-1) * ((width*3+3)&~3) + x*3) ) << 16) | \
	((*(bits + (height-y-1) * ((width*3+3)&~3) + x*3+1) ) <<8) | \
	((*(bits + (height-y-1) * ((width*3+3)&~3) + x*3+2) ) )


//-----------------------------------------------------------------------------
// Name: CopyBitmapToSurface()
// Desc: Copies the image of a bitmap into a surface
// for bitmaps with no alpha, just pass in NULL for hbmAlphaBitmap
//-----------------------------------------------------------------------------
HRESULT CDRGTexture::CopyBitmapToSurface( LPDIRECTDRAWSURFACE7 pddsTarget,
                                    HBITMAP hbmBitmap,
									HBITMAP hbmAlphaBitmap,									
									DWORD dwFlags )
{
	LPDIRECTDRAW7 pDD;
    pddsTarget->GetDDInterface( (VOID**)&pDD );
    

    // Get the bitmap structure (to extract width, height, and bpp)
    BITMAP bm;
    GetObject( hbmBitmap, sizeof(BITMAP), &bm );
	
	//do the same for the alpha bitmap, if there is one
	BITMAP bmAlpha;
	if (hbmAlphaBitmap)
	{
		GetObject( hbmAlphaBitmap, sizeof(BITMAP), &bmAlpha );
		if ((bmAlpha.bmWidth != bm.bmWidth) || (bmAlpha.bmHeight != bm.bmHeight))
		{
			OutputDebugString ("color and alpha bitmaps not the same size (this isn't supported yet \n");
			return NULL;
		} //end if not same size
	} //end if Alpha bitmap exists
    
			
    // Setup the new surface desc
    DDSURFACEDESC2 ddsd;
	ZeroMemory( &ddsd, sizeof(DDSURFACEDESC2) );
    ddsd.dwSize                 = sizeof(DDSURFACEDESC2);
	
	// get info on the target surface, so we can create one similar, but in sys mem
    pddsTarget->GetSurfaceDesc( &ddsd );
    ddsd.dwFlags          = DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT|
		                    DDSD_TEXTURESTAGE;
    ddsd.ddsCaps.dwCaps   = DDSCAPS_TEXTURE|DDSCAPS_SYSTEMMEMORY;
    ddsd.ddsCaps.dwCaps2  = 0L;
    ddsd.dwWidth          = bm.bmWidth;
    ddsd.dwHeight         = bm.bmHeight;

    // Create a new surface for the texture
    LPDIRECTDRAWSURFACE7 pddsTempSurface;
	HRESULT hr;
    if( FAILED( hr = pDD->CreateSurface( &ddsd, &pddsTempSurface, NULL ) ) )
        return NULL;

	// Get a Device Context for the bitmap. We'll need it for blitting
    HDC hdcBitmap = CreateCompatibleDC( NULL );
	if( NULL == hdcBitmap )
    {
        pddsTempSurface->Release();
        return NULL;
    }
    //KP SelectObject( hdcBitmap, hbmBitmap );
	
	HDC hdcAlphaBitmap = CreateCompatibleDC( NULL );
	if (hbmAlphaBitmap)
	{
		// and get a DC for the alpha bitmap
		if( NULL == hdcAlphaBitmap )
		{
			pddsTempSurface->Release();
			return NULL;
		}
		//KP SelectObject( hdcAlphaBitmap, hbmAlphaBitmap );
	}
	
	// Handle palettized textures. Need to attach a palette
    if( ddsd.ddpfPixelFormat.dwRGBBitCount == 8 )
    {
        // Create the color table and parse in the palette
        DWORD pe[256];
        WORD  wNumColors = GetDIBColorTable( hdcBitmap, 0, 256,
                                             (RGBQUAD*)pe );
        for( WORD i=0; i<wNumColors; i++ )
        {
            pe[i] = 0xff000000 + RGB( GetBValue(pe[i]), GetGValue(pe[i]),
                                      GetRValue(pe[i]) );

            // Set alpha for transparent pixels
            if( dwFlags & DRGTEXTR_TRANSPARENTBLACK )
            {
                if( (pe[i]&0x00ffffff) == 0x00000000 )
                    pe[i] &= 0x00ffffff;
            }
            else if( dwFlags & DRGTEXTR_TRANSPARENTWHITE )
            {
                if( (pe[i]&0x00ffffff) == 0x00ffffff )
                    pe[i] &= 0x00ffffff;
            }
        }
        // Create & attach a palette with the bitmap's colors
        LPDIRECTDRAWPALETTE  pPalette;
        if( dwFlags & (DRGTEXTR_TRANSPARENTWHITE|DRGTEXTR_TRANSPARENTBLACK) )
            pDD->CreatePalette( DDPCAPS_8BIT|DDPCAPS_ALPHA, (PALETTEENTRY*)pe, &pPalette, NULL );
        else
            pDD->CreatePalette( DDPCAPS_8BIT, (PALETTEENTRY*)pe, &pPalette, NULL );
        pddsTempSurface->SetPalette( pPalette );
        pddsTarget->SetPalette( pPalette );
        SAFE_RELEASE( pPalette );
    }

	//don't need this anymore
	pDD->Release();


    //For some reason, I can't ge a DC to the surface to do a GDI blit,
	// so I am going to copy the bits over. Right now I am using a 
	// GDI GetPixel() to get the RGB's from the bitmap, but I should 
	// replace that for obvious performance reasons
		
	DDSURFACEDESC2 lpSurfDesc;
	ZeroMemory( &lpSurfDesc, sizeof(DDSURFACEDESC2) );
	lpSurfDesc.dwSize                 = sizeof(DDSURFACEDESC2);
	lpSurfDesc.dwFlags                = 0;
	lpSurfDesc.ddsCaps.dwCaps         = 0;
	lpSurfDesc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
		
	//lock the surface so we can start writing bits to it
	while( pddsTempSurface->Lock( NULL, &lpSurfDesc, 0, NULL ) ==
			   DDERR_WASSTILLDRAWING );

	if ((lpSurfDesc.ddpfPixelFormat.dwRGBBitCount != 16 ) &&
		(lpSurfDesc.ddpfPixelFormat.dwRGBBitCount != 32 ))
	{
		if ((lpSurfDesc.ddpfPixelFormat.dwFlags & DDPF_BUMPLUMINANCE) &&
			(lpSurfDesc.ddpfPixelFormat.dwRGBBitCount != 24 ))
		// Yikes! not 16 or 32, or 24bit DuDvL, we shouldn't be here!
		OutputDebugString("Unknown format. proceed with caution");
	}

	//calculate shift increments (clever shift stuff courtesy of the MS dudes
	DWORD dwRMask = lpSurfDesc.ddpfPixelFormat.dwRBitMask;
    DWORD dwGMask = lpSurfDesc.ddpfPixelFormat.dwGBitMask;
    DWORD dwBMask = lpSurfDesc.ddpfPixelFormat.dwBBitMask;
    DWORD dwAMask = lpSurfDesc.ddpfPixelFormat.dwRGBAlphaBitMask;

	//we'll need these if this is a bump map
	DWORD dwDuMask = lpSurfDesc.ddpfPixelFormat.dwBumpDuBitMask;
	DWORD dwDvMask = lpSurfDesc.ddpfPixelFormat.dwBumpDvBitMask;
	DWORD dwLumMask = lpSurfDesc.ddpfPixelFormat.dwBumpLuminanceBitMask;


    DWORD dwRShiftL = 8, dwRShiftR = 0;
    DWORD dwGShiftL = 8, dwGShiftR = 0;
    DWORD dwBShiftL = 8, dwBShiftR = 0;
    DWORD dwAShiftL = 8, dwAShiftR = 0;

	//again, these are only needed for bump mapping
	DWORD dwDuShiftL = 8, dwDuShiftR = 0;
	DWORD dwDvShiftL = 8, dwDvShiftR = 0;
	DWORD dwLumShiftL = 8, dwLumShiftR = 0;

    DWORD dwMask;
    for( dwMask=dwRMask; dwMask && !(dwMask&0x1); dwMask>>=1 ) dwRShiftR++;
    for( ; dwMask; dwMask>>=1 ) dwRShiftL--;

    for( dwMask=dwGMask; dwMask && !(dwMask&0x1); dwMask>>=1 ) dwGShiftR++;
    for( ; dwMask; dwMask>>=1 ) dwGShiftL--;

    for( dwMask=dwBMask; dwMask && !(dwMask&0x1); dwMask>>=1 ) dwBShiftR++;
    for( ; dwMask; dwMask>>=1 ) dwBShiftL--;

    for( dwMask=dwAMask; dwMask && !(dwMask&0x1); dwMask>>=1 ) dwAShiftR++;
    for( ; dwMask; dwMask>>=1 ) dwAShiftL--;

	//again, some stuff for bump mapping
	DWORD dwBumpMask;
	for( dwBumpMask=dwDuMask; dwBumpMask && !(dwBumpMask&0x1); dwBumpMask>>=1 ) dwDuShiftR++;
    for( ; dwBumpMask; dwBumpMask>>=1 ) dwDuShiftL--;

    for( dwBumpMask=dwDvMask; dwBumpMask && !(dwBumpMask&0x1); dwBumpMask>>=1 ) dwDvShiftR++;
    for( ; dwBumpMask; dwBumpMask>>=1 ) dwDvShiftL--;

    for( dwBumpMask=dwLumMask; dwBumpMask && !(dwBumpMask&0x1); dwBumpMask>>=1 ) dwLumShiftR++;
    for( ; dwBumpMask; dwBumpMask>>=1 ) dwLumShiftL--;

	unsigned char *pBits, *pAlphaBits;
	BITMAPINFO bi;
	memset( &bi, 0, sizeof(  BITMAPINFO ) );
	bi.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
	bi.bmiHeader.biWidth = lpSurfDesc.dwWidth;
	bi.bmiHeader.biHeight = lpSurfDesc.dwHeight;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 24;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biSizeImage = ((bi.bmiHeader.biWidth * bi.bmiHeader.biBitCount/8 + 3) & ~3) * bi.bmiHeader.biHeight;
	pBits = new unsigned char[ ((bi.bmiHeader.biWidth * bi.bmiHeader.biBitCount/8 + 3) & ~3) * bi.bmiHeader.biHeight ];
	GetDIBits( hdcBitmap, hbmBitmap, 0, lpSurfDesc.dwHeight, pBits, &bi, DIB_RGB_COLORS );
	if( hdcAlphaBitmap )
	{
		pAlphaBits = new unsigned char[ ((bi.bmiHeader.biWidth * bi.bmiHeader.biBitCount/8 + 3) & ~3) * bi.bmiHeader.biHeight ];
		GetDIBits( hdcAlphaBitmap, hbmAlphaBitmap, 0, lpSurfDesc.dwHeight, pAlphaBits, &bi, DIB_RGB_COLORS );
	}
 

	for( DWORD y=0; y<lpSurfDesc.dwHeight; y++ )
	{
		WORD*  p16 =  (WORD*)((BYTE*)lpSurfDesc.lpSurface + y*lpSurfDesc.lPitch);
		DWORD* p32 = (DWORD*)((BYTE*)lpSurfDesc.lpSurface + y*lpSurfDesc.lPitch);
       
		for( DWORD x=0; x<lpSurfDesc.dwWidth; x++ )
		{
			//Get the color info from the bitmap
			COLORREF tempcolor = MyGetPixel(  pBits, x, y, lpSurfDesc.dwWidth, lpSurfDesc.dwHeight);
			DWORD r = tempcolor       & 0x000000ff;
			DWORD g = (tempcolor>>8)  & 0x000000ff; 
			DWORD b = (tempcolor>>16) & 0x000000ff;
			DWORD a = 0x000000ff;

			if (lpSurfDesc.ddpfPixelFormat.dwRGBAlphaBitMask) //if alpha
			{
				if( dwFlags & (DRGTEXTR_TRANSPARENTWHITE|DRGTEXTR_TRANSPARENTBLACK) )
				{
					if ((dwFlags & DRGTEXTR_TRANSPARENTWHITE) &&
						(tempcolor == 0x00ffffff))
							a = 0x00000000;
					else
					{
						if ((dwFlags & DRGTEXTR_TRANSPARENTBLACK) &&
						(tempcolor == 0x00000000))
							a = 0x00000000;
						else
							a = 0x000000ff;
					}
				} //end if white/black, which means there must have been
				  // an alpha bitmap supplied, or only format avail. had alpha
				else
				{
					if (hdcAlphaBitmap)
					{
						COLORREF tempalpha = MyGetPixel(  pAlphaBits, x, y, lpSurfDesc.dwWidth, lpSurfDesc.dwHeight);
						a = tempalpha & 0x000000ff;
					}
					// else a remains 0x000000ff						
				}
			} //end if alpha
			
			if (dwFlags & DRGTEXTR_CREATEINVHEIGHTMAP) //if doing embossing
			{
				r = 0x00000080 - r;
				g = 0x00000080 - g;
				b = 0x00000080 - b;
			}

			DWORD dr = ((r>>(dwRShiftL))<<dwRShiftR)&dwRMask;
            DWORD dg = ((g>>(dwGShiftL))<<dwGShiftR)&dwGMask;
            DWORD db = ((b>>(dwBShiftL))<<dwBShiftR)&dwBMask;
            DWORD da = ((a>>(dwAShiftL))<<dwAShiftR)&dwAMask;

			if( 32 == lpSurfDesc.ddpfPixelFormat.dwRGBBitCount )
                p32[x] = (DWORD)(dr+dg+db+da);
            else
                p16[x] = (WORD)(dr+dg+db+da);
			
			//have to override the above for any 'special' formats we want to support
			if (lpSurfDesc.ddpfPixelFormat.dwFlags & DDPF_BUMPDUDV)
			{
				DWORD du = ((r>>(dwDuShiftL))<<dwDuShiftR)&dwDuMask;
				DWORD dv = ((g>>(dwDvShiftL))<<dwDvShiftR)&dwDvMask;
				DWORD lum = ((b>>(dwLumShiftL))<<dwLumShiftR)&dwLumMask;
				if( 32 == lpSurfDesc.ddpfPixelFormat.dwRGBBitCount )
					p32[x] = (DWORD)(du+dv+lum);
				else
					p16[x] = (WORD)(du+dv+lum);
			} //end of override for bump mapping
		}//end for x=width
		p16++;
		p32++;
	} //end for y=height

	pddsTempSurface->Unlock( NULL );

	//now blit the temp surface to the vidmem surface
	pddsTarget->Blt( NULL, pddsTempSurface, NULL, DDBLT_WAIT, NULL );

	//be sure to clean up before you leave
	delete pBits;
	if (hdcAlphaBitmap)
		delete pAlphaBits;

	DeleteDC( hdcBitmap );
	DeleteDC( hdcAlphaBitmap );
		
	return DD_OK;
}
	

//-----------------------------------------------------------------------------
// Name: D3DUtil_GetDirectDrawFromDevice()
// Desc: Get the DDraw interface from a D3DDevice.
//-----------------------------------------------------------------------------
LPDIRECTDRAW7 CDRGTexture::GetDirectDrawFromDevice( LPDIRECT3DDEVICE7 pd3dDevice )
{
	LPDIRECTDRAW7        pDD = NULL;
	LPDIRECTDRAWSURFACE7 pddsRender;

    if( pd3dDevice )
	{
	    // Get the current render target
		if( SUCCEEDED( pd3dDevice->GetRenderTarget( &pddsRender ) ) )
		{
		    // Get the DDraw7 interface from the render target
			pddsRender->GetDDInterface( (VOID**)&pDD );
			pddsRender->Release();
		}
	}
	return pDD;
}


