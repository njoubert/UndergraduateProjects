// DRGText.h - Created with DR&E AppWizard 1.0B10
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
//  Declaration of the DRGText. A class used to represent a string of ascii text as
//  a collection of triangles with a font texture in order to avoid using GDI (and to
//  do nifty visual tricks with them)
//
//  It makes sense for apps to have an instance of this class for each group
//  of strings they will have (for example, a help screen, a debug screen,
//  could each have their own instance). This way, an app can easily choose 
//  to draw/not draw a group of strings.
//
// ------------------------------------------------------------------------------------
//
// Authors: Dean Macri, Kim Pallister - Intel Developer Relations & Engineering Group
//
///////////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRGTEXT_H__801448C1_488C_11D2_914B_0080C7A02BD6__INCLUDED_)
#define AFX_DRGTEXT_H__801448C1_488C_11D2_914B_0080C7A02BD6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


//defines
#define	FONTTEXTCHARSPERROW	16
//file used for fonts. It may make sense to define this as part of main.h, as it is used there, but
//other instances of the class may use different font textures. Anyhow, it is here for now.
#define FONTFILENAME		"Images/font512.bmp"
#define FONTALPHAFILENAME		"Images/font512_a.bmp"
#define	BOTTOMCOLOR			0xFF3333FF //blue
#define	TOPCOLOR			0xFFFFFFFF //white


typedef struct	tagDRGString 
{
	int				iStringID;		//each string contained within the class contains it's own ID
	int				iStringLen;		//the length of the string
	float			fXPos;			//fraction between 0 and 1 representing the position on screen
	float			fYPos;			//  where 0,0 is top left and 1,1 is bottom right
	bool			bVisible;		//the draw method skips strings where this is false
	D3DTLVERTEX		*pVerts;		//the array of verts that make up the characters on screen
	struct tagDRGString	*pNext;		//the strings are stored in a linked list, so we need a ptr
} DRGString; 


class CDRGText  
{
public:
	//
	//member vars
	//
	int iNumStrings;				//number of strings currently held by the class
	
	DRGString	*pFirstString;		//ptr to the start of the linked list
	CDRGWrapper *pDRGWrapper;		//ptr to the DRGWrapper that handles the D3D Stuff
	CDRGTexture *pDRGTexture;		//ptr to the instance of the DRG texture with the text texture

private:
	DWORD dwLastScreenHRes;			//the screen res, last we checked (used to check whether 
	DWORD dwLastScreenVRes;			//resizing is necesary)

	//
	//methods
	//
public:
	CDRGText();											//constructor - sets up some member vars
	HRESULT Init(CDRGWrapper *pWrapper);				//sets up the ptr to the DRGWrapper 
	HRESULT LoadFontTexture(char *filename);			//currently not implemented
	HRESULT SetFontTexture(CDRGTexture *pDRGTexture);	//sets a ptr to a DRGTexture class instance
	HRESULT CreateFontTexture();						//currently not implemented
	HRESULT CreateString(char *string, float x, float y, float hsize, 
						 float vsize, DWORD color, int *piStringID); //create a string and add to list
	HRESULT ModifyStringContent(int piStringID, char *string); //change a strings contents
	HRESULT DestroyString(int iStringNum);				//remove string from list
	HRESULT DrawText();									//draw all the visible strings in the linklist
	HRESULT ToggleStringOnOff(int iStringNum);			//make a string visible/invisible
	//the following are some ideas of stuff to add
	//HRESULT SetStringPosition(int iStringNum, float x,float y);
	//HRESULT SetStringColor(int iStringNum, DWORD color);
	//HRESULT SetStringSize(int iStringNum, int hsize, int vsize);
	~CDRGText();										//destructor - deletes all the items in the
														// items in the linked list
private:
	HRESULT ResizeAllStrings();							//when screen res has changed, resize. 

};

#endif // !defined(AFX_DRGTEXT_H__801448C1_488C_11D2_914B_0080C7A02BD6__INCLUDED_)
