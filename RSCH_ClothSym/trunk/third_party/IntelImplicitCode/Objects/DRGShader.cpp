// DRGShader.cpp - Created with DR&E AppWizard 1.0B10
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
// DRGShader.h: implementation of the CDRGShader class.
//
// ------------------------------------------------------------------------------------
//
// Author: Kim Pallister. 
// 
////////////////////////////////////////////////////////////////////////////////////////
#include "..\DRGShell.h"


CDRGShader::CDRGShader(char*	strname)
{
	m_dwStateBlock	= 0;

	strcpy(m_strShaderName,strname);

	m_pNext = NULL;

}

CDRGShader::~CDRGShader()
{
	// currently nothing here, but I'm not sure if I have to invalidate the state block
}
	
void CDRGShader::SetStateBlock(DWORD sb)
{
	m_dwStateBlock = sb;
	//return S_OK;
}

HRESULT	CDRGShader::AddChild(CDRGShader	*pChildShader)
{
	
	//need to make sure no one tried something nasty like a circular loop!
	if (CheckAgainstChildren(this,pChildShader) != S_OK)
	{
		OutputDebugString("failed to add shader as child. Likely due to circular link");
		return E_FAIL;
	}

	if (m_pNext == NULL)
	{
		m_pNext = pChildShader;
	}
	else
	{
		CDRGShader	*pTempShader = m_pNext;
		//skip to one before end of list
		while (pTempShader->m_pNext != NULL)
		{			
			pTempShader = pTempShader->m_pNext;
		}
		//now add shader to end of linked list
		pTempShader->m_pNext = pChildShader;
	}
	return S_OK;

}

// -----------------------------------------------------------------------
// HRESULT CDRGObject::CheckAgainstChildren(CDRGObject	*pPotentialParent, 
//											CDRGObject *pPotentialChild)
//
//  Checks to make sure the object you want to make a parent of another, 
// isn't already one of that objects children. Thou shalt not murder the stack!
// -----------------------------------------------------------------------
HRESULT	CDRGShader::CheckAgainstChildren(CDRGShader	*pPotentialParent, CDRGShader *pPotentialChild)
{
	CDRGShader	*pTempShader = pPotentialChild;

	//skip to end of list, comparing against parent Shader
	while (pTempShader != NULL)
	{
		if(pPotentialChild == pPotentialParent)
			return E_FAIL;

		pTempShader = pTempShader->m_pNext;
	}

	//if we got here, none of the children was the potential parent, no circular list
	if (pPotentialChild->m_pNext == NULL)
		return S_OK;
	else
		return CheckAgainstChildren(pPotentialParent, pPotentialChild->m_pNext);
}

