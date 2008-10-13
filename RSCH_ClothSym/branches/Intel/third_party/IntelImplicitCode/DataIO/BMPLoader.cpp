// BMPLoader.cpp: implementation of the CBMPLoader class.
// - Created with DR&E AppWizard 1.0B10
//////////////////////////////////////////////////////////////////////

#include "..\DRGShell.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// How wide, in bytes, would this many bits be, DWORD aligned? 
#define WIDTHBYTES(bits)      ((((bits) + 31)>>5)<<2)

CBMPLoader::CBMPLoader( CDataStream &Source ) :
	m_DataSource(Source)
{

}

CBMPLoader::~CBMPLoader()
{

}

//
// Implements the LoadImage() method of the CImageLoader parent class.  The hAlpha bitmap
// will never be set because .BMP files don't support alpha channels.
//
HRESULT CBMPLoader::LoadImage( HBITMAP *hImage, HBITMAP *hAlpha )
{
	HDC hdcDevice = NULL;  // Is this okay?  May need to test on a multi-monitor configuration
	BITMAPFILEHEADER bfh;
	struct 
	{
		BITMAPINFOHEADER bmih;
		RGBQUAD	pal[256];
	} bmi;
	void *pBits = NULL;
	UINT uiBytesRead;

	//
	// Set the bitmaps to NULL initially (And the BMP loader will NEVER load an Alpha) 
	//
	if( hImage )
		*hImage = NULL;
	if( hAlpha )
		*hAlpha = NULL;

	//
	// Read the BMP Header from the DataStream
	//
	memset( &bfh, 0, sizeof( bfh ) );
	if( FAILED( m_DataSource.ReadBytes( &bfh, sizeof( bfh ), uiBytesRead ) ) || 
		(uiBytesRead != sizeof( bfh ) ) )
	{
		return ERROR_INVALID_DATA;
	}     

	//
	// Check to see if this is a BMP file
	//
    if( ( bfh.bfType != 0x4d42 ) || (bfh.bfReserved1!=0) || (bfh.bfReserved2!=0) ) 
    {         
		//  It doesn't look familiar, so bail out
        return ERROR_INVALID_DATA;     
	}     

	//
	// Read in the BITMAPINFOHEADER
	//
	if( FAILED( m_DataSource.ReadBytes( &bmi.bmih, sizeof( BITMAPINFOHEADER ), uiBytesRead ) ) || 
		(uiBytesRead != sizeof( BITMAPINFOHEADER ) ) )
	{
		// Houston, we have a problem
        return ERROR_INVALID_DATA;     
	} 

    //
	// Verify that we understand this BMP format (not supporting OS/2 BMPs for now)
	//
	if( bmi.bmih.biSize != sizeof( BITMAPINFOHEADER ) ) 
    {         
		// This could be a corrupt file, an OS/2 format, or some new format we don't know about
		// Just bail
        return ERROR_INVALID_DATA;     
	}     
	
	//
	// Do we need to read in a palette?
	//
	DWORD dwPalSize = 0;
	if( bmi.bmih.biClrUsed )
		dwPalSize = bmi.bmih.biClrUsed;
	else 
	{
		switch( bmi.bmih.biBitCount )
		{
			case 1:
				dwPalSize = 2; break;
			case 4:
				dwPalSize = 16; break;
			case 8:
				dwPalSize = 256; break;
		}
	}

	//
	// We can't handle this
	//
	if( dwPalSize > 256 )
		return ERROR_INVALID_DATA;

	if( dwPalSize )
	{
		if( FAILED( m_DataSource.ReadBytes( bmi.pal, dwPalSize * sizeof( RGBQUAD ), uiBytesRead ) ) || 
			(uiBytesRead != dwPalSize * sizeof( RGBQUAD ) ) )
		{
			// Something's fishy
			return ERROR_INVALID_DATA;     
		} 
	}

	//
	// Okay, we should have everything necessary to create our DIB Section now, so do it
	//
	m_hImage = CreateDIBSection( hdcDevice,
								  (const struct tagBITMAPINFO *)&bmi,
								  DIB_RGB_COLORS,
								  &pBits,
								  NULL, 0 );
	if( m_hImage == NULL )
	{
		// Who knows why, but we couldn't create the DIB Section -- we must bail
		return ERROR_INVALID_DATA;
	}

	//
	// If the "File" header has an offset to the bits, we'll seek to that offset.  Otherwise,
	// we assume the bits of the image immediately follow the header/palette
	//
    if( bfh.bfOffBits != 0 )     
	{ 
		UINT uiNewPos;
		if( FAILED( m_DataSource.Seek( seek_beginning, bfh.bfOffBits, uiNewPos ) ) ||
			(uiNewPos != bfh.bfOffBits) )
		{
			//
			// The seek failed.  Destroy our DIB Section and get out of here
			//
			DeleteObject( m_hImage );
			return ERROR_INVALID_DATA;
		}     
	}     

	//
	// Calculate the number of bytes in a line of the image.  This needs to be DWORD aligned.
	//
	DWORD dwBytesPerLine = WIDTHBYTES( bmi.bmih.biWidth * bmi.bmih.biPlanes * bmi.bmih.biBitCount );
	//
	// How many total bytes will we need?
	//
	DWORD dwBitsSize = bmi.bmih.biHeight * dwBytesPerLine;

	//
	// We're coming in for a landing.  Read in the image bits and we're done
	// 
	if( FAILED( m_DataSource.ReadBytes( pBits, dwBitsSize, uiBytesRead ) ) ||
		(uiBytesRead != dwBitsSize) )
    {         
		//
		// Darn it, we were almost there.  Oh well, cleanup and get out
		//
		DeleteObject( m_hImage );
        return ERROR_INVALID_DATA;     
	}     

	//
	// We're done.  Return the DIB Section 
	//
	if( hImage )
		*hImage = m_hImage;

	return S_OK;
}
