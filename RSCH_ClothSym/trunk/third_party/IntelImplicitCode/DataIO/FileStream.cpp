// FileStream.cpp: implementation of the CFileStream class.
// - Created with DR&E AppWizard 1.0B10
//////////////////////////////////////////////////////////////////////

#include "..\DRGShell.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileStream::CFileStream( char *szFile, DWORD dwFlags )
{
	DWORD dwAccess = 0;
	DWORD dwCreationDisposition = 0;

	if( dwFlags & OPEN_READ )
		dwAccess |= GENERIC_READ;
	if( dwFlags & OPEN_WRITE )
		dwAccess |= GENERIC_WRITE;

	if( dwFlags & OPEN_CREATE )
		if( dwFlags & OPEN_TRUNCATE )
			dwCreationDisposition = CREATE_ALWAYS;
		else
			dwCreationDisposition = OPEN_ALWAYS;
	else if( dwFlags & OPEN_TRUNCATE )
		dwCreationDisposition = TRUNCATE_EXISTING;
	else
		dwCreationDisposition = OPEN_EXISTING;

	m_hFile = CreateFile( (LPCTSTR)szFile, dwAccess, 0, NULL, dwCreationDisposition, 0, NULL );

	if( m_hFile == INVALID_HANDLE_VALUE )
	{
		int error = GetLastError();
//		throw HRESULT_FROM_WIN32( error );
	}

	m_dwFlags = dwFlags;
}

CFileStream::~CFileStream()
{
	CloseHandle( m_hFile );
}

HRESULT CFileStream::ReadBytes( void *pBuffer, UINT uiBytesToRead, UINT &uiActualBytesRead )
{
	if( m_hFile == INVALID_HANDLE_VALUE )
		return ERROR_FILE_NOT_FOUND;
	if( ReadFile( m_hFile, pBuffer, uiBytesToRead, (DWORD *)&uiActualBytesRead, NULL ) )
		return S_OK;

	return HRESULT_FROM_WIN32( GetLastError() );
}

HRESULT CFileStream::WriteBytes( void *pBuffer, UINT uiBytesToWrite, UINT &uiActualBytesWritten )
{
	if( m_hFile == INVALID_HANDLE_VALUE )
		return ERROR_FILE_NOT_FOUND;
 	if( WriteFile( m_hFile, pBuffer, uiBytesToWrite, (DWORD *)&uiActualBytesWritten, NULL ) )
		return S_OK;

	return HRESULT_FROM_WIN32( GetLastError() );
}

HRESULT CFileStream::Seek( SeekStart pos, int iDistance, UINT &newPos )
{
	if( m_hFile == INVALID_HANDLE_VALUE )
		return ERROR_FILE_NOT_FOUND;

	DWORD dwMoveType;

	dwMoveType = (pos == seek_current) ? FILE_CURRENT : ((pos == seek_beginning) ? FILE_BEGIN : FILE_END);

	SetLastError( NO_ERROR );
	newPos = SetFilePointer( m_hFile, iDistance, NULL, dwMoveType );

	if( GetLastError() == NO_ERROR )
		return S_OK;

	int iError = GetLastError();
	return HRESULT_FROM_WIN32( GetLastError() );
}

HRESULT CFileStream::GetSize( UINT &size )
{
	if( m_hFile == INVALID_HANDLE_VALUE )
		return ERROR_FILE_NOT_FOUND;

	size = GetFileSize( m_hFile, NULL );

	if( GetLastError() == NO_ERROR )
		return S_OK;

	return HRESULT_FROM_WIN32( GetLastError() );
}
