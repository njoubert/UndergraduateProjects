// ImageLoader.h: interface for the CImageLoader class.
// - Created with DR&E AppWizard 1.0B10
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGELOADER_H__359135A5_02C8_11D3_808F_00A0C9A0CA5C__INCLUDED_)
#define AFX_IMAGELOADER_H__359135A5_02C8_11D3_808F_00A0C9A0CA5C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//
// The CImageLoader class is passed to the CDRGTexture class for loading image data.  Classes
// will be derived from CImageLoader to accomodate different image formats.  E.g. see the
// CBMPLoader class.
//
class CImageLoader  
{
protected:
	HBITMAP m_hImage, m_hImageAlpha;

public:
	CImageLoader();
	virtual ~CImageLoader();

	//
	// Derived classes must implement the LoadImage() method.  When this method is called,
	// the class should load its image data and create the hImage to be passec back.  If
	// an alpha channel is available in the image format, it should be returned in hAlpha.
	//
	virtual HRESULT LoadImage( HBITMAP *hImage, HBITMAP *hAlpha) = 0;
};

#endif // !defined(AFX_IMAGELOADER_H__359135A5_02C8_11D3_808F_00A0C9A0CA5C__INCLUDED_)
