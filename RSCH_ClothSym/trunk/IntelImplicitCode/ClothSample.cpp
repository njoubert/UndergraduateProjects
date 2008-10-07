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
// ClothSample.h: implementation the ClothSample class.
// This class is the "blank" class created by the DRG 3D Appwizard, where you will put your stuff
//
// ------------------------------------------------------------------------------------
//
// Author: Dean Macri- Intel Developer Relations & Engineering Group
//
////////////////////////////////////////////////////////////////////////////////////////

#include "DRGShell.h"


// -----------------------------------------------------------------------
// CClothSample::CClothSample() 
// The ClothSample class constructor initializes some of it's member vars (which
// are instances of the DRGMatrix class
// -----------------------------------------------------------------------
CClothSample::CClothSample( char *szConfigFile ) :	
	m_Config( szConfigFile )
{
	m_MxWorld.SetIdentity();
	m_MxView.SetView( DRGVector( 0, 0, -2 ), DRGVector( 0, 0, 0 ), DRGVector( 0, 1, 0 ) );
	//set the projection matrix to 1.57 field of view (radians), 1.33 aspect ratio 
	// (640x480), //the near clip plane to 1 and the far to clip plane 1000
	m_MxProjection.SetProjection( 1.57f, 1.333f, 0.2f, 100.0f );

	m_bWireframe = false;

	m_pFloorTexture = NULL;
	m_pClothTexture = NULL;

	m_pDefaultShader = NULL;
	m_pFloorShader = NULL;
	m_pClothShader = NULL;

	m_pSampleRootObj = NULL;
	m_pSampleFloorPlaneObj = NULL;


	
}

// -----------------------------------------------------------------------
// CClothSample::~CClothSample()
// destructor does nothing for now.
// -----------------------------------------------------------------------
CClothSample::~CClothSample()
{

}

// -----------------------------------------------------------------------
// HRESULT CClothSample::InitWorld( CDRGWrapper *pWrapper )
//
// -----------------------------------------------------------------------
HRESULT CClothSample::InitWorld( CDRGWrapper *pWrapper )
{
	//
	// Make local copy of the pointer to the wrapper class
	//
	m_pWrapper = pWrapper;




	//
	// Create a new texture, and init it, with an alpha channel. 
	// (try swapping the flag field in the line below for the remmed
	// one to compare the quality of a 4444 vs 8888 ARGB texture, if 
	// your device supports it (if the device doesn't, it will get 
	// converted to a supported format automatically))
	//
	m_pFloorTexture = new CDRGTexture;
	if( m_pFloorTexture->CreateTexture( CBMPLoader( CFileStream( "Images\\Floor.bmp" ) ),
						  (DWORD)0, (DWORD)0) == S_OK )
		OutputDebugString("Texture Object created. Surface to be created later /n");
	m_pClothTexture = new CDRGTexture;
	if( m_pClothTexture->CreateTexture( CBMPLoader( CFileStream( "Images\\TableCloth.bmp" ) ),
						  (DWORD)0, (DWORD)0) == S_OK )
		OutputDebugString("Texture Object created. Surface to be created later /n");


	m_pSampleRootObj		= new CDRGObject;
	m_pSampleRootObj->Init(pWrapper, NULL, NULL, 0, 0,						 
					DRGVector(0.0f,0.0f,0.0f), DRGVector(1.0f), DRGVector(0.0f,1.0f,0.0f), 0.0f,
					DRGVector(0.0f),  DRGVector(0.0f),  0.0f,
					0x0);

	m_pSampleFloorPlaneObj	= new CDRGObjectPlane;
	m_pSampleFloorPlaneObj->Init(pWrapper, 1,
					DRGVector(0.0f,-1.0f,0.0f), DRGVector(1.5f), DRGVector(0.0f,1.0f,0.0f), 0.0f,
					DRGVector(0.0f), DRGVector(0.0f), 0.0f,
					0x0);
	m_pSampleRootObj->AddChild(m_pSampleFloorPlaneObj);

	m_pClothObject = new CClothObject;
	m_pClothObject->Init( m_pWrapper, m_Config, DRGVector( 0.0f, 1.25f, 0.0f ) );
	m_pSampleRootObj->AddChild( m_pClothObject );


	//
	// TO DO: Your application should initialize it's world here. This may include 
	// loading geometry, textures, etc. remember that device specific stuff gets
	// called in TargetChanged
	//	

	//now we call target changed, which sets up device dependant items
	TargetChanged();
	
	return S_OK;
}

// -----------------------------------------------------------------------
// HRESULT CClothSample::DestroyWorld( void )
//
// -----------------------------------------------------------------------
HRESULT CClothSample::DestroyWorld( void )
{
	return S_OK;
}

// -----------------------------------------------------------------------
// HRESULT CClothSample::UpdateWorld( DWORD dwTime, int InputFlag )
//
// -----------------------------------------------------------------------
HRESULT CClothSample::UpdateWorld()
{

	//Call the Update method of the object at the root of our hierarchy,
	// specifying to propogate the update down the hierarchy. This will
	// update all the objects positions/orientations according to their
	// velocities. By specifying the frame time, our animation speed is
	// independant of frame rate
	if(m_pSampleRootObj)
		m_pSampleRootObj->Update(m_pWrapper->m_fFrameTime, true);

	//
	// TO DO: This is the function called once per frame where your application should
	// update it's objects, animations, etc. 
	//
	// Use the m_fFrameTime wrapper variable to scale the speed at which your 
	// application performs animation, if the goal is to maintain constant 
	// animation speed regardless of frame rate.
	//
	// For example:
	//
	// pMyWidget->vPosition += pMyWidget->vVelocity * m_pWrapper->m_fFrameTime;
	//

	return S_OK;
}

// -----------------------------------------------------------------------
// HRESULT CClothSample::RenderWorld( void )
//
// -----------------------------------------------------------------------
HRESULT CClothSample::RenderWorld( void )
{
	HRESULT hr;

	// if wireframe
	if (m_bWireframe)
		hr = m_pWrapper->m_pD3DDevice7->SetRenderState( D3DRENDERSTATE_FILLMODE, D3DFILL_WIREFRAME );

	



	
	hr = m_pWrapper->m_pD3DDevice7->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
	hr = m_pWrapper->m_pD3DDevice7->SetRenderState(D3DRENDERSTATE_SRCBLEND , D3DBLEND_SRCALPHA );
	hr = m_pWrapper->m_pD3DDevice7->SetRenderState(D3DRENDERSTATE_DESTBLEND , D3DBLEND_INVSRCALPHA );
	



	if(m_pSampleRootObj)
		hr = m_pSampleRootObj->Render(m_MxWorld,true);

	//
	// TO DO: This is the function called once per frame where your application should
	// draw it's objects.
	//
	// For something like...
	//
	//	hr = m_pWrapper->m_pD3DDevice7->DrawPrimitive( D3DPT_TRIANGLELIST, 
	//												  D3DFVF_LVERTEX,
	//												  pMyWidget->pVerts,
	//												  pMyWidget->iNumVerts,
	//												  D3DDP_DONOTLIGHT);//
	//
	// Remember, if you have included the DRGText class, and are drawing text, then it
	// is safe practice to restore whatever renderstates are required at the beginning
	// of each frame. Alternatively, you may store and restore the renderstate in the
	// DRGText::Draw() function.
	//	

	// if wireframe option is on, reset the fill mode, so that the text will still draw properly
	if (m_bWireframe)
		hr = m_pWrapper->m_pD3DDevice7->SetRenderState( D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID );
		
	return S_OK;
}


// -----------------------------------------------------------------------
// HRESULT CClothSample::TargetChanged( void )
//
// -----------------------------------------------------------------------
HRESULT CClothSample::TargetChanged( void )
{
	HRESULT hr;
	//
	// This indicates that the devices of the DRGWrapper class have changed.
	// If you have anything that depends on the current rendering device, you'll
	// have to re-initialize it here
	//
	m_pWrapper->m_pD3DDevice7->SetRenderState( D3DRENDERSTATE_DITHERENABLE, TRUE );

	m_pWrapper->m_pD3DDevice7->SetTransform( D3DTRANSFORMSTATE_WORLD, &m_MxWorld.m_Mx );
	m_pWrapper->m_pD3DDevice7->SetTransform( D3DTRANSFORMSTATE_VIEW, &m_MxView.m_Mx );
	m_pWrapper->m_pD3DDevice7->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &m_MxProjection.m_Mx );

	
	hr = SetupDeviceDependantItems();

	hr = SetupShaders();

	return hr;
}

HRESULT CClothSample::SetupDeviceDependantItems(void)
{
	HRESULT hr;


	//restore texture
	if (m_pFloorTexture)
		m_pFloorTexture->Restore(m_pWrapper->m_pD3DDevice7);
	if (m_pClothTexture)
		m_pClothTexture->Restore(m_pWrapper->m_pD3DDevice7);


	if(m_pSampleRootObj)
		hr = m_pSampleRootObj->TargetChanged(m_pWrapper, true);

	//create a default material
	D3DMATERIAL7	*ptempmat = new D3DMATERIAL7;
	ZeroMemory( ptempmat, sizeof(D3DMATERIAL7) );
  
	ptempmat->ambient.r = 0.7f;    ptempmat->ambient.g = 0.7f;    ptempmat->ambient.b = 0.7f;
	ptempmat->diffuse.r = 1.0f;    ptempmat->diffuse.g = 1.0f;    ptempmat->diffuse.b = 1.0f;
	ptempmat->specular.r = 1.0f;    ptempmat->specular.g = 1.0f;    ptempmat->specular.b = 1.0f;

	if( FAILED( hr = m_pWrapper->m_pD3DDevice7->SetMaterial(ptempmat) ) )
		return hr;

	// Set up a default light
    D3DLIGHT7 light;
    ZeroMemory( &light, sizeof(light) );
    light.dltType       = D3DLIGHT_DIRECTIONAL;
    light.dcvDiffuse.r  = 0.9f;
    light.dcvDiffuse.g  = 0.9f;
    light.dcvDiffuse.b  = 0.9f;
    light.dvRange       = D3DLIGHT_RANGE_MAX;
    
    light.dvDirection    = D3DVECTOR( 0.2f, -0.3f, 0.3f);
    
    // Set the light
    if( FAILED(m_pWrapper->m_pD3DDevice7->SetLight( 0, &light )))
		return hr;

	if( FAILED(m_pWrapper->m_pD3DDevice7->LightEnable( 0, TRUE )))
		return hr;

    if( FAILED(m_pWrapper->m_pD3DDevice7->SetRenderState( D3DRENDERSTATE_LIGHTING, TRUE )))
		return hr;

	if( FAILED(m_pWrapper->m_pD3DDevice7->SetRenderState( D3DRENDERSTATE_AMBIENT, 0xff333333 ) ) )
		return hr;

	// Some one time renderstate changes
	
	hr = m_pWrapper->m_pD3DDevice7->SetRenderState(D3DRENDERSTATE_COLORVERTEX, TRUE);

	hr = m_pWrapper->m_pD3DDevice7->SetRenderState( D3DRENDERSTATE_DITHERENABLE, TRUE );
	hr = m_pWrapper->m_pD3DDevice7->SetRenderState( D3DRENDERSTATE_SPECULARENABLE, FALSE );

	

	return S_OK;
}

///////////////
// SetupShaders
///////////////
HRESULT CClothSample::SetupShaders()
{
	HRESULT hr;

	/////////////////
	//this is the shader that gets used on the objects in the scene by default (just a simple texture)
	m_pDefaultShader	= new CDRGShader("Default Shader");
	hr = m_pWrapper->m_pD3DDevice7->BeginStateBlock();
	hr = m_pWrapper->m_pD3DDevice7->SetTextureStageState(0,D3DTSS_COLOROP, D3DTOP_DISABLE); //disable other stages
	hr = m_pWrapper->m_pD3DDevice7->SetTextureStageState(0,D3DTSS_ALPHAOP, D3DTOP_DISABLE); //disable other stages
	hr = m_pWrapper->m_pD3DDevice7->SetTexture(0,NULL);
	hr = m_pWrapper->m_pD3DDevice7->SetTextureStageState(1,D3DTSS_COLOROP, D3DTOP_DISABLE); //disable other stages
	hr = m_pWrapper->m_pD3DDevice7->SetTextureStageState(1,D3DTSS_ALPHAOP, D3DTOP_DISABLE); //disable other stages
	hr = m_pWrapper->m_pD3DDevice7->SetTexture(1,NULL);
	hr = m_pWrapper->m_pD3DDevice7->SetTextureStageState(2,D3DTSS_COLOROP, D3DTOP_DISABLE); //disable other stages
	hr = m_pWrapper->m_pD3DDevice7->SetTextureStageState(2,D3DTSS_ALPHAOP, D3DTOP_DISABLE); //disable other stages
	hr = m_pWrapper->m_pD3DDevice7->SetTexture(2,NULL);
	
	hr = m_pWrapper->m_pD3DDevice7->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
	hr = m_pWrapper->m_pD3DDevice7->EndStateBlock(&m_pDefaultShader->m_dwStateBlock);

	m_pSampleRootObj->SetShader(m_pDefaultShader);

	//
	// Shader for the floor
	//
	m_pFloorShader	= new CDRGShader("Floor Shader");
	hr = m_pWrapper->m_pD3DDevice7->BeginStateBlock();
	hr = m_pWrapper->m_pD3DDevice7->SetTexture(0,m_pFloorTexture->ptexTexture);
    hr = m_pWrapper->m_pD3DDevice7->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE );	
	hr = m_pWrapper->m_pD3DDevice7->SetTextureStageState(0,D3DTSS_COLORARG1, D3DTA_TEXTURE); //disable other stages
	hr = m_pWrapper->m_pD3DDevice7->SetTextureStageState(0,D3DTSS_COLORARG2, D3DTA_CURRENT); //disable other stages

	hr = m_pWrapper->m_pD3DDevice7->SetTextureStageState(1,D3DTSS_COLOROP, D3DTOP_DISABLE); //disable other stages
	hr = m_pWrapper->m_pD3DDevice7->SetTextureStageState(1,D3DTSS_ALPHAOP, D3DTOP_DISABLE); //disable other stages
	hr = m_pWrapper->m_pD3DDevice7->SetTexture(1,NULL);
	hr = m_pWrapper->m_pD3DDevice7->SetTextureStageState(2,D3DTSS_COLOROP, D3DTOP_DISABLE); //disable other stages
	hr = m_pWrapper->m_pD3DDevice7->SetTextureStageState(2,D3DTSS_ALPHAOP, D3DTOP_DISABLE); //disable other stages
	hr = m_pWrapper->m_pD3DDevice7->SetTexture(2,NULL);

	
	hr = m_pWrapper->m_pD3DDevice7->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
	hr = m_pWrapper->m_pD3DDevice7->EndStateBlock(&m_pFloorShader->m_dwStateBlock);

	m_pSampleFloorPlaneObj->SetShader( m_pFloorShader );

	//
	// Shader for the piece of cloth
	//
	m_pClothShader	= new CDRGShader("Cloth Shader");
	hr = m_pWrapper->m_pD3DDevice7->BeginStateBlock();
	hr = m_pWrapper->m_pD3DDevice7->SetTexture(0,m_pClothTexture->ptexTexture);
	hr = m_pWrapper->m_pD3DDevice7->SetTextureStageState(1,D3DTSS_COLOROP, D3DTOP_DISABLE); //disable other stages
	hr = m_pWrapper->m_pD3DDevice7->SetTextureStageState(1,D3DTSS_ALPHAOP, D3DTOP_DISABLE); //disable other stages
	hr = m_pWrapper->m_pD3DDevice7->SetTexture(1,NULL);
	hr = m_pWrapper->m_pD3DDevice7->SetTextureStageState(2,D3DTSS_COLOROP, D3DTOP_DISABLE); //disable other stages
	hr = m_pWrapper->m_pD3DDevice7->SetTextureStageState(2,D3DTSS_ALPHAOP, D3DTOP_DISABLE); //disable other stages
	hr = m_pWrapper->m_pD3DDevice7->SetTexture(2,NULL);
	
	hr = m_pWrapper->m_pD3DDevice7->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
	hr = m_pWrapper->m_pD3DDevice7->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE );
	hr = m_pWrapper->m_pD3DDevice7->EndStateBlock(&m_pClothShader->m_dwStateBlock);

	m_pClothObject->SetShader( m_pClothShader );

	// we are going to succeed here, regardless of whether hr failed. Chances
	// are that a ValidateDevice() failure is not going to crash the app, so we
	// probably shouldn't exit (which is what will happen if we return an error
	// from here
	return S_OK;

}
















// -----------------------------------------------------------------------
// void CClothSample::KeyUp(WPARAM wParam)
//
//   Keyboard keypress handler. 
// -----------------------------------------------------------------------
void CClothSample::KeyDown(WPARAM wParam)
{
	//we need a variable to modify our rotation speed with frame rate	
	float scaler = 1.0f;

	//if exiting, the wrapper could be invalid, but we don't care, since we will exit anyway
	if (wParam == 'X') 
		return;

	if(m_pWrapper) //we could get into this loop after the wrapper has been invalidates
	{
		if (m_pWrapper->m_fFrameTime >0.0f)
			scaler = m_pWrapper->m_fFrameTime;  
	}
		
	switch (wParam) 
	{
	case 'W':
		m_bWireframe = !m_bWireframe;
		break;
	case 'R':
		m_pClothObject->Reset();
		break;
	case '0':
	case '1':
	case '2':
	case '3':
		m_pClothObject->ToggleConstraint( wParam - '0' );
		break;

	case VK_UP:
		m_MxView.Rotate( m_MxView.m_Mx._11, m_MxView.m_Mx._21, m_MxView.m_Mx._31, -3.14159f*scaler );
		break;
	
	case VK_DOWN:
		m_MxView.Rotate( m_MxView.m_Mx._11, m_MxView.m_Mx._21, m_MxView.m_Mx._31, 3.14159f*scaler );
		break;
	
	case VK_LEFT:
		m_MxView.Rotate( m_MxView.m_Mx._12, m_MxView.m_Mx._22, m_MxView.m_Mx._32, -3.14159f*scaler );
		break;
	
	case VK_RIGHT:	
		m_MxView.Rotate( m_MxView.m_Mx._12, m_MxView.m_Mx._22, m_MxView.m_Mx._32, 3.14159f*scaler );
		break;
	}

	if (m_pWrapper)
	{
		m_pWrapper->m_pD3DDevice7->SetTransform( D3DTRANSFORMSTATE_WORLD, &m_MxWorld.m_Mx );
		m_pWrapper->m_pD3DDevice7->SetTransform( D3DTRANSFORMSTATE_VIEW, &m_MxView.m_Mx );
	}

	
	return;
}

void CClothSample::KeyUp(WPARAM wParam)
{		
	switch (wParam) 
	{
		//add code here to handle key releases, if you need them (as in remmed out example)
	case VK_UP:
		//OutputDebugString("The up arrow was released;");			
		break;
	}
	
	return;
}


void CClothSample::MouseLeftUp(WPARAM wParam, LPARAM lParam)
{	
	return;
}

void CClothSample::MouseLeftDown(WPARAM wParam, LPARAM lParam)
{		
	return;
}

void CClothSample::MouseRightUp(WPARAM wParam, LPARAM lParam)
{	
	return;
}

void CClothSample::MouseRightDown(WPARAM wParam, LPARAM lParam)
{		
	return;
}

void CClothSample::MouseMove(WPARAM wParam, LPARAM lParam)
{
	if ( wParam & MK_LBUTTON )	// Left button down plus mouse move == rotate
	{
		//we need a variable to modify our rotation speed with frame rate	
		float scaler = 1.0f;
		if(m_pWrapper) //we could get into this loop after the wrapper has been invalidates
		{
			scaler = 2.0f / max(m_pWrapper->m_dwRenderWidth, m_pWrapper->m_dwRenderHeight);  
		}

		//we need to compare the last position to the current position
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		
		if (x - m_iMousePrevX)
		{
			m_MxView.Rotate( m_MxView.m_Mx._12, m_MxView.m_Mx._22, m_MxView.m_Mx._32, 3.14159f*scaler*(float)(x - m_iMousePrevX) );
		}
			
		if (y - m_iMousePrevY)
		{
			m_MxView.Rotate( m_MxView.m_Mx._11, m_MxView.m_Mx._21, m_MxView.m_Mx._31, 3.14159f*scaler*(float)(y - m_iMousePrevY) );
		}

		m_pWrapper->m_pD3DDevice7->SetTransform( D3DTRANSFORMSTATE_WORLD, &m_MxWorld.m_Mx );
		m_pWrapper->m_pD3DDevice7->SetTransform( D3DTRANSFORMSTATE_VIEW, &m_MxView.m_Mx );

	
		m_iMousePrevX = x;
		m_iMousePrevY = y;

	}
	else if ( wParam & MK_RBUTTON )	// Right button down plus mouse move == Zoom camera in/out
	{
		//we need a variable to modify our rotation speed with frame rate	
		float scaler = 1.0f;
		if(m_pWrapper) //we could get into this loop after the wrapper has been invalidates
		{
			scaler = 5.0f / max(m_pWrapper->m_dwRenderWidth, m_pWrapper->m_dwRenderHeight);  
		}

		//we need to compare the last position to the current position
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		
		if (y - m_iMousePrevY)
		{
			DRGMatrix Temp(true);

			Temp.Translate( 0,0,scaler * (y - m_iMousePrevY) );
			m_MxView.PostMultiply( Temp );
		}

		m_pWrapper->m_pD3DDevice7->SetTransform( D3DTRANSFORMSTATE_WORLD, &m_MxWorld.m_Mx );
		m_pWrapper->m_pD3DDevice7->SetTransform( D3DTRANSFORMSTATE_VIEW, &m_MxView.m_Mx );

	
		m_iMousePrevX = x;
		m_iMousePrevY = y;

	}
	else
	{
		//need to update these, or the next time someone clicks, the delta is massive
		m_iMousePrevX = LOWORD(lParam);
		m_iMousePrevY = HIWORD(lParam);	
	}
	return;
}

