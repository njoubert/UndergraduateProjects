// BMPLoader.h: interface for the CBMPLoader class.
// - Created with DR&E AppWizard 1.0B10
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BMPLOADER_H__359135A6_02C8_11D3_808F_00A0C9A0CA5C__INCLUDED_)
#define AFX_BMPLOADER_H__359135A6_02C8_11D3_808F_00A0C9A0CA5C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBMPLoader : public CImageLoader  
{
protected:
	CDataStream &m_DataSource;

public:
	CBMPLoader(	CDataStream &Source );
	virtual ~CBMPLoader();

	virtual HRESULT LoadImage( HBITMAP *hImage, HBITMAP *hAlpha);
};

#endif // !defined(AFX_BMPLOADER_H__359135A6_02C8_11D3_808F_00A0C9A0CA5C__INCLUDED_)
