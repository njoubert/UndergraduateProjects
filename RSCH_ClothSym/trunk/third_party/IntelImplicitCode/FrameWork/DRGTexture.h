// DRGTexture.h - Created with DR&E AppWizard 1.0B10
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
// DRGTexture.h: declaration of the CDRGTexture class.
// Originally adapted from some DX6 sample code. Added some robustness and extra 
// functionality (alpha bitmaps, creating inverse hieghtmaps for embossing, automatic
// resize for HW requirements, etc)
//
// ------------------------------------------------------------------------------------
//
// Authors: Dean Macri, Kim Pallister - Intel Developer Relations & Engineering Group
//
////////////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRGTEXTURE_H__424B8540_49CB_11D2_8AA0_00A0C91916A8__INCLUDED_)
#define AFX_DRGTEXTURE_H__424B8540_49CB_11D2_8AA0_00A0C91916A8__INCLUDED_


#define DRGTEXTR_TRANSPARENTWHITE	0x00000002
#define	DRGTEXTR_TRANSPARENTBLACK	0x00000004
#define DRGTEXTR_32BITSPERPIXEL		0x00000008
#define DRGTEXTR_RENDERTARGET		0x00000010 //create texture w. DDSCAPS_3DDEVICE, to render to it.  		
#define DRGTEXTR_FORCEINAGP			0x00000020 //Specifies to create the texture in agp memory
#define DRGTEXTR_FORCEINSYSMEM		0x00000040 //Specifies to create the texture in sys memory
#define DRGTEXTR_FORCEINVIDMEM		0x00000080 //Specifies to create the texture in sys memory
//some flags added for embossing and bump mapping
#define DRGTEXTR_CREATEINVHEIGHTMAP 0x00000100 //will create a texture which is (0.5 - src texture)
#define DRGTEXTR_CREATEBUMPMAP_DUDV 0x00000200 //will create with DUDV caps bits set
//this is just a hack for this demo to create a DDSurface without the Texture flag
#define DRGTEXTR_DDSURFACEONLY		0x00000400 



#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CDRGTexture  
{
public:
	//member vars
	HBITMAP              hbmBitmap;      // Windows bitmap containing texture image
	HBITMAP				 hbmAlphaBitmap; // Windows bitmap containing alpha image
	LPDIRECTDRAWSURFACE7 ptexTexture;    
    DWORD                dwStage;        // Texture stage (for multi-textures)

	bool                 bHasAlpha;		//used to signify 4 or 8 bit alpha plane.	
	
	CImageLoader		*m_pTextureLoader;
	CImageLoader		*m_pAlphaTextureLoader;
	DWORD                dwFlags;
	
	DDSURFACEDESC2       m_ddsd;

	TCHAR				 *g_strTexturePath;

	CDRGTexture();
	//create a texture with no alpha image source (alpha can come from DRGTEXTR_TRANSPARENTBLACK/WHITE flags
	HRESULT CreateTexture( CImageLoader &TextureLoader, 
						   DWORD dwTextureStage=0L, 
						   DWORD dwTextureFlags=0L );

	//create a texture with an alpha channel from another image source
	HRESULT CreateTexture( CImageLoader &TextureLoader,
						   CImageLoader &AlphaTextureLoader,							
						   DWORD dwTextureStage=0L, 
						   DWORD dwTextureFlags=0L );

	//create a blank texture, or one from a straight array of bits
	HRESULT CreateTexture( CDRGWrapper *pWrapper,
						   DWORD	iHres,
						   DWORD	iVres,
						   DWORD	dwBpp, 
						   DWORD dwTextureStage=0L, 
						   DWORD dwTextureFlags=0L,
						   DWORD	*pBits=NULL);

	// Restore() destroys the current device texture and creates a new one from the
	// hbitmap and flags member vars, and device info. (Be aware that even if the 
	// Restore worked once, it mail fail later. For example if you restore after 
	// switching to a device with different caps, or switch to a higher res and run 
	// out of video memory. ALWAYS CHECK RETURN VALUES!)
	HRESULT Restore(LPDIRECT3DDEVICE7 pd3dDevice );
	HRESULT Restore(LPDIRECT3DDEVICE7 pd3dDevice, DWORD dwNewStage );
	~CDRGTexture();

	void Cleanup();

private:
	//hidden member functions
	HRESULT			LoadTextureImage( );
	//if bitmap has no alpha, we just pass in null for hbmAlphaBitmap.
	HRESULT			CopyBitmapToSurface( LPDIRECTDRAWSURFACE7 pddsTarget, 
										 HBITMAP hbmBitmap,
										 HBITMAP hbmAlphaBitmap,
										 DWORD dwFlags );
	HRESULT			RestoreFromBitmap(LPDIRECT3DDEVICE7 pd3dDevice );
	LPDIRECTDRAW7	GetDirectDrawFromDevice( LPDIRECT3DDEVICE7 pd3dDevice );

};

#endif // !defined(AFX_DRGTEXTURE_H__424B8540_49CB_11D2_8AA0_00A0C91916A8__INCLUDED_)
