// DataSource.h: interface for the CDataSource class.
// - Created with DR&E AppWizard 1.0B10
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATASOURCE_H__A6592962_0240_11D3_808F_00A0C9A0CA5C__INCLUDED_)
#define AFX_DATASOURCE_H__A6592962_0240_11D3_808F_00A0C9A0CA5C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum SeekStart
{
	seek_current,
	seek_beginning,
	seek_end
};

//
// The CDataStream class is a pure virtual interface declaration for derived classes that will
// implement data loading from various sources.  See the CFileStream and CURLStream classes.
//
class CDataStream  
{
public:
	CDataStream() {};
	virtual ~CDataStream() {};

	//
	// The ReadBytes() method should read uiBytesToRead from the data stream of the class
	// and copy them into pBuffer.  The actual number of bytes copied should be returned in
	// uiActualBytesRead.
	//
	virtual HRESULT ReadBytes( void *pBuffer, UINT uiBytesToRead, UINT &uiActualBytesRead ) = 0;

	virtual HRESULT WriteBytes( void *pBuffer, UINT uiBytesToWrite, UINT &uiActualBytesWritten ) = 0;
	virtual HRESULT Seek( SeekStart pos, int iDistance, UINT &newPos ) = 0;
	virtual HRESULT GetSize( UINT &size ) = 0;
};

#endif // !defined(AFX_DATASOURCE_H__A6592962_0240_11D3_808F_00A0C9A0CA5C__INCLUDED_)
