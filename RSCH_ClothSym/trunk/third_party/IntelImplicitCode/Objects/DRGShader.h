// DRGShader.h - Created with DR&E AppWizard 1.0B10
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
// DRGShader.h: declaration of the CDRGShader class.
// a work in progress
// ------------------------------------------------------------------------------------
//
// Author: Kim Pallister. 
// 
////////////////////////////////////////////////////////////////////////////////////////

class CDRGShader {

public:

	//public functions
	CDRGShader(char*	strname = "Unnamed");
	virtual ~CDRGShader();

	void	SetStateBlock(DWORD	dwSB);

	//HRESULT	Activate(CDRGWrapper	*pWrapper);

	HRESULT AddChild(CDRGShader *pShader);

	DWORD			m_dwStateBlock;
	char		m_strShaderName[255];

	//public member vars
	CDRGShader		*m_pNext;
	

protected:

	HRESULT	CheckAgainstChildren(CDRGShader	*pPotentialParent, CDRGShader *pPotentialChild);


	

};
