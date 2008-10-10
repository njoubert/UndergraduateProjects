// FileStream.h: interface for the CFileStream class.
// - Created with DR&E AppWizard 1.0B10
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILESTREAM_H__A6592963_0240_11D3_808F_00A0C9A0CA5C__INCLUDED_)
#define AFX_FILESTREAM_H__A6592963_0240_11D3_808F_00A0C9A0CA5C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define OPEN_READ		0x01	// Open the file for reading
#define OPEN_WRITE		0x02	// Open the file for writing
#define OPEN_CREATE		0x04	// Create the file if it doesn't already exist
#define OPEN_TRUNCATE	0x08	// Truncate the file after opening it

class CFileStream : public CDataStream  
{
protected:
	HANDLE m_hFile;
	DWORD m_dwFlags;

public:
	CFileStream( char *szFile, DWORD dwFlags = OPEN_READ );
	virtual ~CFileStream();

	virtual HRESULT ReadBytes( void *pBuffer, UINT uiBytesToRead, UINT &uiActualBytesRead );
	virtual HRESULT WriteBytes( void *pBuffer, UINT uiBytesToWrite, UINT &uiActualBytesWritten );
	virtual HRESULT Seek( SeekStart pos, int iDistance, UINT &newPos );
	virtual HRESULT GetSize( UINT &size );
};

#endif // !defined(AFX_FILESTREAM_H__A6592963_0240_11D3_808F_00A0C9A0CA5C__INCLUDED_)
