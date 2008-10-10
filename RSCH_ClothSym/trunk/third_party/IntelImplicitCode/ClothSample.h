// ClothSample.h - Created with DR&E AppWizard 1.0B10
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
// ClothSample.h: declaration of the ClothSample class.
// This class is the "blank" class created by the DRG 3D Appwizard, where you will put your stuff
//
// ------------------------------------------------------------------------------------
//
// Author: Dean Macri - Intel Developer Relations & Engineering Group
//
////////////////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_DRGAPP_H__CFF24425_47D6_11D2_B1A5_00A0C90D2C1B__INCLUDED_)
#define AFX_DRGAPP_H__CFF24425_47D6_11D2_B1A5_00A0C90D2C1B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000



class CClothSample  
{
    CDRGWrapper *m_pWrapper;
	bool	m_bWireframe;

	
	DRGMatrix m_MxWorld;
	DRGMatrix m_MxProjection;
	DRGMatrix m_MxView;

	
	CDRGTexture *m_pFloorTexture, *m_pClothTexture;

	CDRGShader	*m_pDefaultShader, *m_pFloorShader, *m_pClothShader;

	CDRGObject *m_pSampleRootObj;
	CDRGObjectPlane *m_pSampleFloorPlaneObj;
	CClothObject *m_pClothObject;
	


	int  m_iMousePrevX;
	int  m_iMousePrevY;

	CConfiguration m_Config;

public:
    CClothSample( char *szConfigFile );
    virtual ~CClothSample();
	
    virtual HRESULT InitWorld( CDRGWrapper *pWrapper );
	virtual HRESULT SetupDeviceDependantItems(void);
	virtual HRESULT SetupShaders(void);
	virtual HRESULT DestroyWorld( void );

	virtual HRESULT UpdateWorld();
	virtual HRESULT RenderWorld( void );

	virtual HRESULT TargetChanged( void );

	void KeyDown(WPARAM wParam);
	void KeyUp(WPARAM wParam);

	void MouseLeftUp(WPARAM wParam, LPARAM lParam);
	void MouseLeftDown(WPARAM wParam, LPARAM lParam);
	void MouseRightUp(WPARAM wParam, LPARAM lParam);
	void MouseRightDown(WPARAM wParam, LPARAM lParam);
	void MouseMove(WPARAM wParam, LPARAM lParam);


private:


	
};

#endif // !defined(AFX_DRGAPP_H__CFF24425_47D6_11D2_B1A5_00A0C90D2C1B__INCLUDED_)
