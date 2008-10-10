// DRGObjectPlane.cpp - Created with DR&E AppWizard 1.0B10
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
// DRGObjectPlane.cpp: definition of hte the Plane Object, derived from the DRGObject base class.
// This class is used to contain a Plane object, it's vertices, matrices, etc.
// The class stores two copies of the vertices, an array of D3DVerts, and a Vertex
// buffer used for rendering. At the time this class was authored, it was unclear
// whether the array was needed in case of a lost vertex buffer (DX7 beta docs unclear)
// so there may be some memory savings to be made there.
//
// ------------------------------------------------------------------------------------
//
// Author: Kim Pallister
// 
////////////////////////////////////////////////////////////////////////////////////////

#include "..\DRGShell.h"


// -----------------------------------------------------------------------
// DRGObject::DRGObject()
//
// Initialize the necesary member vars.
// -----------------------------------------------------------------------
CDRGObjectPlane::CDRGObjectPlane()
{
	
	// current position, rotation, scale
	//m_vRotationAxis = DRGVector(0.0f,1.0f,0.0f);
	//m_fRotationAngle = 0.0f;
	//m_vScale = DRGVector(1.0f);
	//m_vPosition = DRGVector(0.0f);
		
	// current velocity, rotational velocity, scale velociy
	//m_fRotationalVelocity = 0.0f;
	//m_vScalingVelocity = DRGVector(0.0f);
	//m_vVelocity = DRGVector(0.0f);
	
	
	// Local transformation matrix, concatenation of the above
	//m_MxLocalToWorld.SetIdentity();
		
	
	// Vertex buffers
	//m_pIndexedVertexBuffer = NULL;
	
	// Pointer to sysmem copy of verts
	//m_pVertices = NULL;
	//m_pIndices = NULL;
	//m_iNumVertices = 0;	
	//m_iNumIndices = 0;	

	//m_d3dPrimType = D3DPT_TRIANGLELIST; 

	//m_pWrapper = NULL;

	//pointer to siblings and children, if this object is part of a hierarchy (this is still TBD)
	//m_pNext = NULL;
	//m_pFirstChild = NULL;
}

// -----------------------------------------------------------------------
// DRGObject::~DRGObject()
//
// El Destructor
// -----------------------------------------------------------------------
CDRGObjectPlane::~CDRGObjectPlane()
{
	
	// Get rid of the VB
	if (m_pIndexedVertexBuffer)
			SAFE_RELEASE( m_pIndexedVertexBuffer );

	m_pWrapper = NULL;

	//pointer to siblings and children, if this object is part of a hierarchy (this is still TBD)
	m_pNext = NULL;

	if (m_pFirstChild)
		delete m_pFirstChild;	

}

// -----------------------------------------------------------------------
// HRESULT CDRGObject::Init( *bunch o stuff* )
//	
//  This function is called to set up all the member vars with specific 
//  values and/or to feed in all the verts from a specific source known
//	to the app. 
//
//  derived versions of this class will call this member function to 
//  init all the verts for procedural objects, or ojbects coming from files.

// -----------------------------------------------------------------------

HRESULT CDRGObjectPlane::Init(	CDRGWrapper *pDRGWrapper,
					int			divisions,
					DRGVector	pos				,
					DRGVector	scale			,
					DRGVector	rotationaxis	,
					float		rotationangle	,
					DRGVector	vel				,
					DRGVector	scalevel		,
					float		rotationalvel	,
					DWORD		dwFlags			 )
{
	int idiv = divisions;
	if (divisions < 1) 
		idiv = 1;
	
	m_vRotationAxis			= rotationaxis;
	m_fRotationAngle = rotationangle;
	m_vScale			= scale;
	m_vPosition				= pos;	
	
	// current velocity, rotational velocity, scale velociy
	m_fRotationalVelocity	= rotationalvel;
	m_vScalingVelocity		= scalevel;
	m_vVelocity				= vel;	

	// vertex and index counters
	m_iNumVertices = (idiv+1)*(idiv+1); //1 division would give 4 verts
	m_iNumIndices = (idiv)*(idiv) * 2*3; //1 division would give 6 indices for 2 tris

	m_pVertices = new D3DVERTEX[m_iNumVertices];
	m_pIndices = new WORD[m_iNumIndices];


	//ptr to the wrapper
	m_pWrapper = pDRGWrapper;


	//if we are being fed a bunch of verts, then we need to set them up
	if (SetupVertices() != S_OK)
	{
		OutputDebugString("unable to set up verts");
		return E_FAIL;
	}
		
	if (SetupIndices() != S_OK)
	{
		OutputDebugString("unable to set up indices");
		return E_FAIL;
	}

	if (SetupIndexedVertexBuffer() != S_OK)
	{
		OutputDebugString("unable to set up VB");
		return E_FAIL;
	}

	return S_OK;
}
	
		
HRESULT CDRGObjectPlane::SetupVertices( )
{
	//since we don't store the number of divisions in the plane, we can just get it back
	int	iNumDivisions = (int)sqrt(m_iNumIndices/6);
	float fDelta = 2.0f/(float)iNumDivisions;
	int count = 0;
	
	for (int i=0;i<=iNumDivisions;i++)
	{
		for (int j=0;j<=iNumDivisions;j++)
		{
			m_pVertices[count].x = -1.0f + ((float)j*fDelta); 
			m_pVertices[count].y =  0.0f; 
			m_pVertices[count].z = -1.0f + ((float)i*fDelta);

			m_pVertices[count].nx = 0.0f; 
			m_pVertices[count].ny = 1.0f; 
			m_pVertices[count].nz = 0.0f;

			m_pVertices[count].tu = (float)j * fDelta*0.5f;
			m_pVertices[count].tv = 1.0f - (float)i * fDelta*0.5f; 

			count++;
		}
	}	
	
	return S_OK;
}

	// Set up indexed vertex indices
	// for using DrawIndexedPrimitive(VB/Strided)
HRESULT CDRGObjectPlane::SetupIndices( )
{
	int	iNumDivisions = (int)sqrt(m_iNumIndices/6);
	int index = 0;

	for (int i=0;i<iNumDivisions;i++)
	{
		for (int j=0;j<iNumDivisions;j++)
		{
			int	startvert = (i*(iNumDivisions+1) + j);  

			// tri 1	
			m_pIndices[index++] = startvert; 
			m_pIndices[index++] = startvert+iNumDivisions+1; 
			m_pIndices[index++] = startvert+1; 

			// tri 2
			m_pIndices[index++] = startvert+1; 
			m_pIndices[index++] = startvert+iNumDivisions+1; 
			m_pIndices[index++] = startvert+iNumDivisions+2; 
		}
	}

	return S_OK;
}
