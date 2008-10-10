// DRGText.cpp - Created with DR&E AppWizard 1.0B10
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
//  Implementation of the DRGText. A class used to represent a string of ascii text as
//  a collection of triangles with a font texture in order to avoid using GDI (and to
//  do nifty visual tricks with them)
//
// ------------------------------------------------------------------------------------
//
// Authors: Dean Macri, Kim Pallister - Intel Developer Relations & Engineering Group
//
///////////////////////////////////////////////////////////////////////////////////////

						
// assumes also that the font texture will be arranged 16x16, with the 
// 256 chars then being in ascii character order, left to right, top to bottom
//
//////////////////////////////////////////////////////////////////////

#include "..\DRGShell.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDRGText::CDRGText()
{
	iNumStrings = 0;
	pFirstString = NULL;
	pDRGWrapper = NULL;
}

HRESULT CDRGText::Init(CDRGWrapper *pWrapper)
{
	pDRGWrapper = pWrapper;
	return S_OK;
}


HRESULT CDRGText::LoadFontTexture(char *filename)
{
	//Could place something in here that loads a default font texture?

	//This init'd to NULL for now. The app should use SetFontTexture
	LPDIRECT3DTEXTURE2 pFontTexture = NULL;
	return S_OK;
}

HRESULT CDRGText::CreateFontTexture()
{
	//this where we will eventually create the font texture on the fly. (So much code, so 
	// little time)
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////////////
// CreateString() looks at a string, allocates enough vertices to draw it, and sets 
// them up to the right color, character, etc. X,Y are floats into the screen space 
// where 0,0 is top left of the screen, and 1,1 is bottom left hsize and vsize are 
// the size of the characters in pixels. Color is the color of the text. piStringID
// is a pointer to an int that will be filled with the string id of the string created.
HRESULT CDRGText::CreateString(char *string, float x, float y, float hsize, float vsize, DWORD color, int *piStringID)
{

	//first, what if pFirstString points to NULL, i.e. no strings created yet?
	//we'll create a blank node, with which we do nothing 
	if (pFirstString == NULL) 
	{
		pFirstString = new DRGString;

		pFirstString->iStringLen = 0;
		pFirstString->fXPos = 0;
		pFirstString->fYPos = 0;
		pFirstString->pVerts = NULL;
		pFirstString->bVisible = false;
		pFirstString->iStringID = 0;
		pFirstString->pNext = NULL;
	}

	DRGString	*pCurrString = pFirstString;

	while (pCurrString->pNext != NULL)
		pCurrString = pCurrString->pNext; //skip through to end of string.	
	
	//CREATE AND FILL OUT A STRUCTURE, AND ADD IT TO THE LIST

	pCurrString->pNext = new DRGString; //create a new string structure, add to end of list.
	
	pCurrString->pNext->iStringLen = lstrlen(string); //set the length field in the structure;

	pCurrString->pNext->fXPos = x;
	pCurrString->pNext->fYPos = y;
		
	
	pCurrString->pNext->pVerts = new D3DTLVERTEX[pCurrString->pNext->iStringLen * 6]; //create array of TLverts, the length of string x 6 per char
	
	pCurrString->pNext->bVisible = true; //set the visible flage to true
	
	piStringID[0] = pCurrString->iStringID + 1; //set the ID number in the struct,
	pCurrString->pNext->iStringID = pCurrString->iStringID + 1; // and the one to be returned to the next in the sequence.

	pCurrString->pNext->pNext = NULL;
	
	//increase our count of how many strings there are
	iNumStrings++;	

	dwLastScreenHRes=pDRGWrapper->m_dwRenderWidth;
	dwLastScreenVRes=pDRGWrapper->m_dwRenderHeight;


	//2-FILL OUT THE D3DTLVERTS
	float fXStart = x * (float)pDRGWrapper->m_dwRenderWidth;
	float fYStart = y * (float)pDRGWrapper->m_dwRenderHeight;
	float fCharWidth = hsize * (float)pDRGWrapper->m_dwRenderWidth;
	float fCharHeight = vsize * (float)pDRGWrapper->m_dwRenderHeight;

	for (int i = 0; i< pCurrString->pNext->iStringLen; i++) 
	{
		//set x,y positions 
		//NEED TO CHECK SCREEN EXTENTS!!! Assumes D3D will clip!
		pCurrString->pNext->pVerts[(i*6)].sx = (float)fXStart+(i*fCharWidth);				//top left
		pCurrString->pNext->pVerts[(i*6)].sy = (float)fYStart;
		
		pCurrString->pNext->pVerts[(i*6)+1].sx = (float)fXStart+(i*fCharWidth)+fCharWidth;	//top right	
		pCurrString->pNext->pVerts[(i*6)+1].sy = (float)fYStart;//+fCharHeight;
		
		pCurrString->pNext->pVerts[(i*6)+2].sx = (float)fXStart+(i*fCharWidth);				//bottom left
		pCurrString->pNext->pVerts[(i*6)+2].sy = (float)fYStart+fCharHeight;
		//these are good.
		pCurrString->pNext->pVerts[(i*6)+3].sx = (float)fXStart+(i*fCharWidth)+fCharWidth;	//top right
		pCurrString->pNext->pVerts[(i*6)+3].sy = (float)fYStart;
		
		pCurrString->pNext->pVerts[(i*6)+4].sx = (float)fXStart+(i*fCharWidth)+fCharWidth;	//bottom right
		pCurrString->pNext->pVerts[(i*6)+4].sy = (float)fYStart+fCharHeight;
		
		pCurrString->pNext->pVerts[(i*6)+5].sx = (float)fXStart+(i*fCharWidth);				//bottom left
		pCurrString->pNext->pVerts[(i*6)+5].sy = (float)fYStart+fCharHeight;
		
		
		// set some of the redundant stuff
		for (int j=0; j<6;j++)
		{
			//note, the Z value may need to change if you operate out of this range
			pCurrString->pNext->pVerts[(i*6)+j].sz = 0.05f ; //set Z to the front.
			pCurrString->pNext->pVerts[(i*6)+j].rhw = 1.0 ; 
			pCurrString->pNext->pVerts[(i*6)+j].specular = 0 ; 

			
			pCurrString->pNext->pVerts[(i*6)+0].color = TOPCOLOR;
			pCurrString->pNext->pVerts[(i*6)+1].color = TOPCOLOR;
			pCurrString->pNext->pVerts[(i*6)+2].color = BOTTOMCOLOR;
			pCurrString->pNext->pVerts[(i*6)+3].color = TOPCOLOR;
			pCurrString->pNext->pVerts[(i*6)+4].color = BOTTOMCOLOR;
			pCurrString->pNext->pVerts[(i*6)+5].color = BOTTOMCOLOR;
			
		}

		//set the UV's into the font texture.
		float fCharOffset = 1.0/(float)FONTTEXTCHARSPERROW; //the fraction between 0&1 per char
		float fUStart = (float)((int)string[i] % FONTTEXTCHARSPERROW)*fCharOffset;
		float fVStart = (float)((int)string[i] / FONTTEXTCHARSPERROW)*fCharOffset;

		pCurrString->pNext->pVerts[(i*6)].tu = fUStart;					//top left
		pCurrString->pNext->pVerts[(i*6)].tv = fVStart;
		
		pCurrString->pNext->pVerts[(i*6)+1].tu = fUStart+fCharOffset;	//top right	
		pCurrString->pNext->pVerts[(i*6)+1].tv = fVStart;
		
		pCurrString->pNext->pVerts[(i*6)+2].tu = fUStart;				//bottom left	
		pCurrString->pNext->pVerts[(i*6)+2].tv = fVStart+fCharOffset;
		
		pCurrString->pNext->pVerts[(i*6)+3].tu = fUStart+fCharOffset;	//top right	
		pCurrString->pNext->pVerts[(i*6)+3].tv = fVStart;
		
		pCurrString->pNext->pVerts[(i*6)+4].tu = fUStart+fCharOffset;	//bottom right
		pCurrString->pNext->pVerts[(i*6)+4].tv = fVStart+fCharOffset;
		
		pCurrString->pNext->pVerts[(i*6)+5].tu = fUStart;				//bottom left
		pCurrString->pNext->pVerts[(i*6)+5].tv = fVStart+fCharOffset;
		
	}
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////////////
//ModifyStringContents is comprised of two main sections: one for when the string passed
//to the function is the same length as the old one (just changes the UV offsets of the
//string tris); and one section for when the string has changed length (frees the buffer
//of verts, allocates a new buffer and sets it up. WHENEVER POSSIBLE, MODIFY STRINGS TO 
//THE SAME LENGTH!
HRESULT CDRGText::ModifyStringContent(int iStringID, char *string)
{
	DRGString	*pCurrString = pFirstString;

	while ((pCurrString->iStringID != iStringID) &&	(pCurrString != NULL))
	{
		pCurrString = pCurrString->pNext;
	}

	//now if pointing at NULL, we never found the string
	if (pCurrString == NULL)
		return E_FAIL;
		
	//otherwise, we have found the right string

	if (pCurrString->iStringLen == (int)strlen(string))
	{
		//then all we need to do is modify contents
		for (int i = 0; i< pCurrString->iStringLen; i++) 
		{
			//set the UV's into the font texture.
			float fCharOffset = 1.0/(float)FONTTEXTCHARSPERROW; //the fraction between 0&1 per char
			float fUStart = (float)((int)string[i] % FONTTEXTCHARSPERROW)*fCharOffset;
			float fVStart = (float)((int)string[i] / FONTTEXTCHARSPERROW)*fCharOffset;

			pCurrString->pVerts[(i*6)].tu = fUStart;					//top left
			pCurrString->pVerts[(i*6)].tv = fVStart;
		
			pCurrString->pVerts[(i*6)+1].tu = fUStart+fCharOffset;	//top right	
			pCurrString->pVerts[(i*6)+1].tv = fVStart;
		
			pCurrString->pVerts[(i*6)+2].tu = fUStart;				//bottom left	
			pCurrString->pVerts[(i*6)+2].tv = fVStart+fCharOffset;
		
			pCurrString->pVerts[(i*6)+3].tu = fUStart+fCharOffset;	//top right	
			pCurrString->pVerts[(i*6)+3].tv = fVStart;
		
			pCurrString->pVerts[(i*6)+4].tu = fUStart+fCharOffset;	//bottom right
			pCurrString->pVerts[(i*6)+4].tv = fVStart+fCharOffset;
		
			pCurrString->pVerts[(i*6)+5].tu = fUStart;				//bottom left
			pCurrString->pVerts[(i*6)+5].tv = fVStart+fCharOffset;
		
		}

		return S_OK;
	}
	else
	{
		//init vars
		pCurrString->iStringLen = (int)strlen(string);
		//Xpos,Ypos,Visible,ID,Next, all stay the same

		float fXStart = pCurrString->fXPos * (float)pDRGWrapper->m_dwRenderWidth;
		float fYStart = pCurrString->fYPos * (float)pDRGWrapper->m_dwRenderHeight;
		//we didn't track these, so I'll grab them from the old verts.
		float fCharWidth = pCurrString->pVerts[4].sx - fXStart;
		float fCharHeight = pCurrString->pVerts[4].sy - fYStart;

		//delete old buffer of verts
		delete (pCurrString->pVerts);
		
		//create a new buffer of verts, and fill in the vertex info
		pCurrString->pVerts = new D3DTLVERTEX[pCurrString->iStringLen * 6]; //create array of TLverts, the length of string x 6 per char
		//init all the verts in the array.
		
		for (int i = 0; i< pCurrString->iStringLen; i++) 
		{
			//set x,y positions 
			//NEED TO CHECK SCREEN EXTENTS!!! Assumes D3D will clip!
			pCurrString->pVerts[(i*6)].sx = (float)fXStart+(i*fCharWidth);				//top left
			pCurrString->pVerts[(i*6)].sy = (float)fYStart;
		
			pCurrString->pVerts[(i*6)+1].sx = (float)fXStart+(i*fCharWidth)+fCharWidth;	//top right	
			pCurrString->pVerts[(i*6)+1].sy = (float)fYStart;//+fCharHeight;
		
			pCurrString->pVerts[(i*6)+2].sx = (float)fXStart+(i*fCharWidth);				//bottom left
			pCurrString->pVerts[(i*6)+2].sy = (float)fYStart+fCharHeight;
			//these are good.
			pCurrString->pVerts[(i*6)+3].sx = (float)fXStart+(i*fCharWidth)+fCharWidth;	//top right
			pCurrString->pVerts[(i*6)+3].sy = (float)fYStart;
		
			pCurrString->pVerts[(i*6)+4].sx = (float)fXStart+(i*fCharWidth)+fCharWidth;	//bottom right
			pCurrString->pVerts[(i*6)+4].sy = (float)fYStart+fCharHeight;
		
			pCurrString->pVerts[(i*6)+5].sx = (float)fXStart+(i*fCharWidth);				//bottom left
			pCurrString->pVerts[(i*6)+5].sy = (float)fYStart+fCharHeight;
		
		
			// set some of the redundant stuff
			for (int j=0; j<6;j++)
			{
				pCurrString->pVerts[(i*6)+j].sz = 0.1f ; //set Z to the front.
				pCurrString->pVerts[(i*6)+j].rhw = 1.0 ; 
				pCurrString->pVerts[(i*6)+j].specular = 0 ; 				
			}

			pCurrString->pNext->pVerts[(i*6)+0].color = TOPCOLOR;
			pCurrString->pNext->pVerts[(i*6)+1].color = TOPCOLOR;
			pCurrString->pNext->pVerts[(i*6)+2].color = BOTTOMCOLOR;
			pCurrString->pNext->pVerts[(i*6)+3].color = TOPCOLOR;
			pCurrString->pNext->pVerts[(i*6)+4].color = BOTTOMCOLOR;
			pCurrString->pNext->pVerts[(i*6)+5].color = BOTTOMCOLOR;

		
			//set the UV's into the font texture.
			float fCharOffset = 1.0/(float)FONTTEXTCHARSPERROW; //the fraction between 0&1 per char
			float fUStart = (float)((int)string[i] % FONTTEXTCHARSPERROW)*fCharOffset;
			float fVStart = (float)((int)string[i] / FONTTEXTCHARSPERROW)*fCharOffset;

			pCurrString->pVerts[(i*6)].tu = fUStart;					//top left
			pCurrString->pVerts[(i*6)].tv = fVStart;
		
			pCurrString->pVerts[(i*6)+1].tu = fUStart+fCharOffset;	//top right	
			pCurrString->pVerts[(i*6)+1].tv = fVStart;
		
			pCurrString->pVerts[(i*6)+2].tu = fUStart;				//bottom left	
			pCurrString->pVerts[(i*6)+2].tv = fVStart+fCharOffset;
		
			pCurrString->pVerts[(i*6)+3].tu = fUStart+fCharOffset;	//top right	
			pCurrString->pVerts[(i*6)+3].tv = fVStart;
		
			pCurrString->pVerts[(i*6)+4].tu = fUStart+fCharOffset;	//bottom right
			pCurrString->pVerts[(i*6)+4].tv = fVStart+fCharOffset;
		
			pCurrString->pVerts[(i*6)+5].tu = fUStart;				//bottom left
			pCurrString->pVerts[(i*6)+5].tv = fVStart+fCharOffset;
		
		}		
		return S_OK;
	}
}

HRESULT CDRGText::SetFontTexture(CDRGTexture *TClass)
{
	pDRGTexture = TClass;
	return (S_OK);
}

HRESULT CDRGText::DestroyString(int iStringNum)
{
//	HRESULT r;
	DRGString	*pCurrString = pFirstString;
	DRGString	*pRemoved;

	//first, handle case of it being the first string.
	if (pCurrString->iStringID == iStringNum)
	{
		pFirstString = pCurrString->pNext;
		delete (pCurrString->pVerts);
		delete (pCurrString);
	}

	while (pCurrString->pNext->iStringID != iStringNum)
	{
		if (pCurrString->pNext->pNext == NULL)
			return E_FAIL; //couldn't find string.
		else
			pCurrString=pCurrString->pNext;
	}

	pRemoved = pCurrString->pNext; 
	pCurrString->pNext = pCurrString->pNext->pNext; 

	delete (pRemoved->pVerts);
	delete (pRemoved);

	//decrement the counter of the strings.
	iNumStrings--;

	return S_OK;
	
}

HRESULT CDRGText::ToggleStringOnOff(int iStringNum)
{
//	HRESULT r;
	DRGString	*pCurrString = pFirstString;

	while (pCurrString->iStringID != iStringNum)
	{
		if (pCurrString->pNext == NULL)
			return E_FAIL; //couldn't find string. //What is the HRESULT I should return here?
		else
			pCurrString=pCurrString->pNext;
	}
	pCurrString->bVisible = !pCurrString->bVisible;

	return S_OK;
}

// DrawText()
// this steps through the linked list of strings that have been created, and draws them.
// it assumes that it is being stuck between two begin/endscene pairs.
HRESULT CDRGText::DrawText()
{
	HRESULT		r;
	DRGString	*pCurrString = pFirstString;
	//LPDIRECT3DTEXTURE2 pOldTexture;
	LPDIRECTDRAWSURFACE7 pOldTexture;

	//I think I need to do a renderstate call to point to my texture.
	if (pDRGTexture == NULL)
	{
		r = pDRGWrapper->m_pD3DDevice7->GetTexture( 0, &pOldTexture);
		r = pDRGWrapper->m_pD3DDevice7->SetTexture( 0, pDRGTexture->ptexTexture);		
		
	}
	else
	{
		
		r = pDRGWrapper->m_pD3DDevice7->GetTexture(   0,  &pOldTexture);
		r = pDRGWrapper->m_pD3DDevice7->SetTexture(   0,  pDRGTexture->ptexTexture);
		
		// make the font background transparent, and the vert colors modify the text
		
		r = pDRGWrapper->m_pD3DDevice7->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		r = pDRGWrapper->m_pD3DDevice7->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		r = pDRGWrapper->m_pD3DDevice7->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		r = pDRGWrapper->m_pD3DDevice7->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

		r = pDRGWrapper->m_pD3DDevice7->SetTexture( 1, NULL );
		r = pDRGWrapper->m_pD3DDevice7->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
		r = pDRGWrapper->m_pD3DDevice7->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		
		r = pDRGWrapper->m_pD3DDevice7->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE,   TRUE );
		
		
	    // Set blending for simple color blend. (we could opt to use alpha, since I've created
		// the font texture with an alpha channel. Depends what you are doing in your shaders 
		// outside this class in the main app
		r = pDRGWrapper->m_pD3DDevice7->SetRenderState( D3DRENDERSTATE_SRCBLEND,  D3DBLEND_SRCCOLOR );
		r = pDRGWrapper->m_pD3DDevice7->SetRenderState( D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCCOLOR );

		r = pDRGWrapper->m_pD3DDevice7->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR );
		r = pDRGWrapper->m_pD3DDevice7->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFN_LINEAR );
	

	}

	//if the user resized the window or went to a different fullscreen res since last time...
	if ((dwLastScreenHRes != pDRGWrapper->m_dwRenderWidth) || (dwLastScreenVRes != pDRGWrapper->m_dwRenderHeight))
	{
		ResizeAllStrings();
		pDRGTexture->Restore(pDRGWrapper->m_pD3DDevice7);
	}

	while (pCurrString !=NULL) 
	{
		if (pCurrString->bVisible) //skip ones that have been hidden.
		{
			r = pDRGWrapper->m_pD3DDevice7->DrawPrimitive(D3DPT_TRIANGLELIST,
													     D3DFVF_TLVERTEX,
													     pCurrString->pVerts,
													     (pCurrString->iStringLen * 6),
													     NULL);//D3DDP_DONOTLIGHT);
			if FAILED(r) return r; //fail out if drawprim didn't work.			
		}
		pCurrString = pCurrString->pNext;
	}

	r = pDRGWrapper->m_pD3DDevice7->SetTexture(   0,  pOldTexture);

	return S_OK; //return the last DrawPrim result.
}

/////////////////////////////////////////////////////////////////
// ResizeAllStrings() steps through the linked list of strings
// multiplyinging each vertex x and y positions by the change from
// the old resolution to the new one. For example, if going from 
// 800x600 to 1600x1200, it would multiply all positions by 2.
// It should be noted that the possibility exists for some error
// here if strings are resized many times. 
/////////////////////////////////////////////////////////////////
HRESULT CDRGText::ResizeAllStrings()
{
	DRGString	*pCurrString = pFirstString;

	float Hdifference = (float)pDRGWrapper->m_dwRenderWidth / (float)dwLastScreenHRes;
	float Vdifference = (float)pDRGWrapper->m_dwRenderHeight / (float)dwLastScreenVRes;


	while (pCurrString !=NULL) //while not null
	{
		for (int i = 0; i<pCurrString->iStringLen*6; i++)
		{
			//offset verts by change in resolution X current position
			pCurrString->pVerts[i].sx = (Hdifference * pCurrString->pVerts[i].sx);
			pCurrString->pVerts[i].sy = (Vdifference * pCurrString->pVerts[i].sy);
		}
		pCurrString = pCurrString->pNext;
	}

	dwLastScreenHRes = pDRGWrapper->m_dwRenderWidth;
	dwLastScreenVRes = pDRGWrapper->m_dwRenderHeight;
	
	return S_OK; //return the last DrawPrim result.	
}


CDRGText::~CDRGText()
{
	//should release all allocated for the linked list
	DRGString	*pTempString = pFirstString;
	
	while (pFirstString) //!=null
	{
		pTempString = pFirstString;
		pFirstString = pFirstString->pNext;

		delete (pTempString->pVerts);
		delete (pTempString);
	}
}
