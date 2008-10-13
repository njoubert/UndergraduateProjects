// DRGObject.h - Created with DR&E AppWizard 1.0B10
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
// DRGObject.h: declaration of the CDRGObject class.
// This class is used to contain an object, it's vertices, matrices, etc.
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
#include <rmxfguid.h>		//These last two header files included because we use some stuff 
#include <rmxftmpl.h>       // here for loading .X files

// -----------------------------------------------------------------------
// DRGObject::DRGObject()
//
// Initialize the necesary member vars.
// -----------------------------------------------------------------------
CDRGObject::CDRGObject()
{
	
	// current position, rotation, scale
	m_vRotationAxis = DRGVector(0.0f,1.0f,0.0f);
	m_fRotationAngle = 0.0f;
	m_vScale = DRGVector(1.0f);
	m_vPosition = DRGVector(0.0f);
		
	// current velocity, rotational velocity, scale velociy
	m_fRotationalVelocity = 0.0f;
	m_vScalingVelocity = DRGVector(0.0f);
	m_vVelocity = DRGVector(0.0f);
	
	// Local transformation matrix, concatenation of the above
	m_MxLocalToParent.SetIdentity();
	m_MxLocalToWorld.SetIdentity();
	m_MxLocalBase.SetIdentity();
	
	// Vertex buffers
	m_pIndexedVertexBuffer = NULL;
	
	// Pointer to sysmem copy of verts
	m_pVertices = NULL;
	m_pIndices = NULL;
	m_iNumVertices = 0;	
	m_iNumIndices = 0;	

	m_d3dPrimType = D3DPT_TRIANGLELIST; 

	m_pWrapper = NULL;
	m_pShader = NULL;

	m_bVisible = true;

	//pointer to siblings and children, if this object is part of a hierarchy (this is still TBD)
	m_pNext = NULL;
	m_pFirstChild = NULL;
}

// -----------------------------------------------------------------------
// DRGObject::~DRGObject()
//
// El Destructor
// -----------------------------------------------------------------------
CDRGObject::~CDRGObject()
{
	
	// Get rid of the VB
	if (m_pIndexedVertexBuffer)
			SAFE_RELEASE( m_pIndexedVertexBuffer );

	m_pWrapper = NULL;

	//pointer to siblings and children, if this object is part of a hierarchy (this is still TBD)

	if (m_pNext)
		delete m_pNext;
	
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
//  init all the verts for procedural objects

// -----------------------------------------------------------------------

HRESULT CDRGObject::Init(	CDRGWrapper *pDRGWrapper,
							D3DVERTEX	*pVerts			,
							WORD		*pIndices		,	
							DWORD		dwNumVerts		,
							DWORD		dwNumIndices	,					
							DRGVector	pos				,
							DRGVector	scale			,
							DRGVector	rotationaxis	,
							float		rotationangle	,
							DRGVector	vel				,
							DRGVector	scalevel		,
							float		rotationalvel	,
							DWORD		dwFlags			 )
{
	m_vRotationAxis			= rotationaxis;
	m_fRotationAngle		= rotationangle;
	m_vScale				= scale;
	m_vPosition				= pos;	
	
	// current velocity, rotational velocity, scale velociy
	m_fRotationalVelocity	= rotationalvel;
	m_vScalingVelocity		= scalevel;
	m_vVelocity				= vel;	

	// vertex and index counters
	m_iNumVertices = dwNumVerts;
	m_iNumIndices = dwNumIndices;

	//ptr to the wrapper
	m_pWrapper = pDRGWrapper;
//	m_pMaterial = NULL;
//	m_pShader = NULL;

	
	//if we are being fed a bunch of verts, then we need to set them up
	if (dwNumVerts)
	{
		if (SetupVertices(pVerts) != S_OK)
		{
			OutputDebugString("unable to set up verts");
			return E_FAIL;
		}
		
		if (SetupIndices(pIndices) != S_OK)
		{
			OutputDebugString("unable to set up indices");
			return E_FAIL;
		}

		if (SetupIndexedVertexBuffer() != S_OK)
		{
			OutputDebugString("unable to set up VB");
			return E_FAIL;
		}
	}

	return S_OK;
}



// -----------------------------------------------------------------------
// HRESULT CDRGObject::Init(wrapper, file, *bunch o stuff* )
//	
//  This function is called to set up all the member vars with specific 
//  values and/or to feed in all the verts from a specific source known
//	to the app. 
//
//  derived versions of this class will call this member function to 
//  init all the verts for procedural objects, or ojbects coming from files.

// -----------------------------------------------------------------------

HRESULT CDRGObject::Init(	CDRGWrapper *pDRGWrapper,
							char		*strFileName,
							DRGVector	pos				,
							DRGVector	scale			,
							DRGVector	rotationaxis	,
							float		rotationangle	,
							DRGVector	vel				,
							DRGVector	scalevel		,
							float		rotationalvel	,
							DWORD		dwFlags			 )
{
	m_vRotationAxis			= rotationaxis;
	m_fRotationAngle		= rotationangle;
	m_vScale				= scale;
	m_vPosition				= pos;	
	
	// current velocity, rotational velocity, scale velociy
	m_fRotationalVelocity	= rotationalvel;
	m_vScalingVelocity		= scalevel;
	m_vVelocity				= vel;	

	// vertex and index counters
	m_iNumVertices = 0;//dwNumVerts;
	m_iNumIndices = 0;//dwNumIndices;

	//ptr to the wrapper
	m_pWrapper = pDRGWrapper;

	//this will load the file for us
	LoadObject( strFileName, NULL );


	if (m_iNumVertices)
	{
		if (SetupIndexedVertexBuffer() != S_OK)
		{
			OutputDebugString("unable to set up VB");
			return E_FAIL;
		}
	}
	return S_OK;
}

	
// Return the number of triangles
// the sphere is made from
int CDRGObject::GetTriangleCount()
{
	return m_iNumIndices /3;
}


	
HRESULT CDRGObject::SetupVertices( D3DVERTEX *pVertices )
{
	m_pVertices = new D3DVERTEX[m_iNumVertices];
	memcpy(m_pVertices,pVertices,m_iNumVertices * sizeof(D3DVERTEX));

	return S_OK;
}

HRESULT CDRGObject::SetupIndices( WORD *pVertexIndices )
{
	m_pIndices = new WORD[m_iNumIndices];
	memcpy(m_pIndices,pVertexIndices,m_iNumIndices * sizeof(WORD));

	return S_OK;
}


// Set up a vertex buffer for indexed vertices
HRESULT CDRGObject::SetupIndexedVertexBuffer()
{
	HRESULT hr;

	if (m_pIndexedVertexBuffer != NULL)
		SAFE_RELEASE(m_pIndexedVertexBuffer);
	
	// Define the vertex buffer descriptor for creating the
	// vertex buffer for DrawIndexedPrimitiveVB
    D3DVERTEXBUFFERDESC vbIndexedDesc;
    ZeroMemory(&vbIndexedDesc, sizeof(D3DVERTEXBUFFERDESC));
    vbIndexedDesc.dwSize        = sizeof(D3DVERTEXBUFFERDESC);
	
	// need to specify SysMem if on the RGB software renderer or regular HAL
	if ((m_pWrapper->m_bHardware) && 
		(m_pWrapper->m_pCurrentDevice->ddDeviceDesc.dwDevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ))
	{
		//for TnL HAL, need to let the driver decide where to put it
		vbIndexedDesc.dwCaps        = 0l;
	}
	else
	{
		//for regular hal, or rgb sw, need to specify sysmem
		vbIndexedDesc.dwCaps        = D3DVBCAPS_SYSTEMMEMORY ;
	}
	
    vbIndexedDesc.dwFVF         = D3DFVF_VERTEX;

    vbIndexedDesc.dwNumVertices = m_iNumVertices;
    
    // Create a vertex buffer using D3D7 interface
	if(m_pWrapper)
	{
		if (m_pWrapper->m_pD3D7)
		{
		if ( FAILED( hr = m_pWrapper->m_pD3D7->CreateVertexBuffer(&vbIndexedDesc, 
		                                                    &m_pIndexedVertexBuffer, 0)))
			return hr;
 
		D3DVERTEX	*pTemp;

		if ( FAILED( hr = m_pIndexedVertexBuffer->Lock( DDLOCK_WAIT, (VOID **)&pTemp, NULL ) ) )
			return hr;

		memcpy(pTemp,m_pVertices,m_iNumVertices * sizeof(D3DVERTEX));

		if ( FAILED( hr = m_pIndexedVertexBuffer->Unlock() ) )
			return hr;
		}
	}

	return S_OK;
}

// -----------------------------------------------------------------------
// HRESULT DRGObject::Update( float fTimeStep, bool bUpdateChildren = true)
//	
//  This updates all the vectors, and the local to world matrix according 
//	to a timestep, where 1.0 is the number of seconds elapsed since the 
//	last frame.  For example, if a velocity of '6' means that the object
//	should move 6 world units in one second,  when the app is running at 
//	50fps, the timestep value will be 1/50 or 0.02. This is then taken into
//	account when updating the matrices.	
// -----------------------------------------------------------------------

HRESULT	CDRGObject::Update(float	fTimeStep, bool bUpdateChildren)
{
	//update the object's current rotation angle scale and position
	m_fRotationAngle += m_fRotationalVelocity * fTimeStep;
	m_vScale += m_vScalingVelocity * fTimeStep;
	m_vPosition += m_vVelocity * fTimeStep;	

	//update the local to world matrix (start with the base matrix)
	m_MxLocalToParent = m_MxLocalBase;
		
	m_MxLocalToParent.Translate(m_vPosition.x, m_vPosition.y, m_vPosition.z );
	m_MxLocalToParent.Scale(m_vScale.x, m_vScale.y, m_vScale.z);
	m_MxLocalToParent.Rotate(m_vRotationAxis, m_fRotationAngle);

	if (bUpdateChildren)
	{
		CDRGObject	*pTempObject = m_pFirstChild;
		while (pTempObject != NULL)
		{
			pTempObject->Update(fTimeStep, true);
			pTempObject = pTempObject->m_pNext;
		}
	}
		
	return S_OK;
}


// -----------------------------------------------------------------------
// HRESULT DRGObject::Render( DRGMatrix& drgMxMatrix, bool bRenderChilren = true )
//
//  This draws the object to the screen using DrawIndexedPrimitiveVB. 
// -----------------------------------------------------------------------
HRESULT CDRGObject::Render( DRGMatrix& drgMxWorld, bool bRenderChildren)
{
	
	HRESULT hr = S_OK;

	//Setting the material and matrix here is not the most efficient way to do it (redundant calls
	// while stepping down hierarchy) but gets around some issues with the .x file hierarchy 
	// we build right now. It's at least functional, and will be improved in a later version of 
	// the appwizard
	
	// Set the world matrix to the local matrix
	m_MxLocalToWorld =  drgMxWorld * m_MxLocalToParent ;

	int	renderpasses = 0;

	CDRGShader	*pTempShader = m_pShader;

	do
	{		
		//we don't do a apply state block if there is no shader at all at the first level
		if (m_pShader != NULL) 
		{
			m_pWrapper->m_pD3DDevice7->ApplyStateBlock(pTempShader->m_dwStateBlock);
			//You can use these for debugging			
			//OutputDebugString(pTempShader->m_strShaderName);
			//OutputDebugString(" - shader activated\n");
		}

		
		if (m_iNumVertices && m_pWrapper && m_pWrapper->m_pD3DDevice7 && m_bVisible)
		{
			m_pWrapper->m_pD3DDevice7->SetTransform( D3DTRANSFORMSTATE_WORLD, &m_MxLocalToWorld.m_Mx );
	
			// Draw the object
			// the first 0 is for where to start in the VB. For now, we are assuming one VB per object
			hr = m_pWrapper->m_pD3DDevice7->DrawIndexedPrimitiveVB( m_d3dPrimType, 
																	 m_pIndexedVertexBuffer,
																	 0, 
																	 m_iNumVertices,
																	 m_pIndices, 
																	 m_iNumIndices, 
																	 0 );
			renderpasses++;
		}
		
	
		//if the object has children, and we want to render them, now is the time
		if (bRenderChildren)
		{
			CDRGObject	*pTempObject = m_pFirstChild;
			while (pTempObject != NULL)
			{
				hr |= pTempObject->Render(m_MxLocalToWorld, true);

				// if the child we just rendered had a shader, and the next child does not,
				// then we have to reapply the parent's shader (if the parent had one)
				if ((pTempObject->m_pShader) && 
					(pTempObject->m_pNext) &&
					(pTempObject->m_pNext->m_pShader == NULL) && 
					(m_pShader != NULL) )
				{
					m_pWrapper->m_pD3DDevice7->ApplyStateBlock(m_pShader->m_dwStateBlock);
					//can be used for debugging			
					//OutputDebugString(pTempShader->m_strShaderName);
					//OutputDebugString(" - shader re-activated\n");
				}

				pTempObject = pTempObject->m_pNext;
			}
		}

		//restore world matrix
		m_pWrapper->m_pD3DDevice7->SetTransform( D3DTRANSFORMSTATE_WORLD, &drgMxWorld.m_Mx );

		//again, the if is there to ignore this the 1st time through if there is no shader
		if (m_pShader)
			pTempShader=pTempShader->m_pNext;
	}
	while (pTempShader != NULL);

	//more stuff usefull for debugging
	//char	str[255];
	//sprintf(str,"Object rendered in %d passes\n",renderpasses);
	//OutputDebugString(str);

	return hr;
}

HRESULT CDRGObject::TargetChanged(CDRGWrapper *pDRGWrapper, bool bUpdateChildren )
{

	m_pWrapper = pDRGWrapper;

	// setup VB's
	if(m_iNumVertices)
	{
		if (SetupIndexedVertexBuffer() != S_OK)
		{
			OutputDebugString("unable to set up VB");
			return E_FAIL;
		}
	}

	//recursively refresh children
	if (bUpdateChildren)
	{
		CDRGObject	*pTempObject = m_pFirstChild;
		while (pTempObject != NULL)
		{
			pTempObject->TargetChanged(pDRGWrapper, bUpdateChildren);
			pTempObject = pTempObject->m_pNext;
		}
	}

	return S_OK;
}


HRESULT	CDRGObject::AddChild(CDRGObject	*pChildObject)
{
	
	//need to make sure no one tried something nasty like a circular loop!
	if (CheckAgainstChildren(this,pChildObject) != S_OK)
	{
		OutputDebugString("failed to add as child. Likely due to circular link");
		return E_FAIL;
	}

	if (m_pFirstChild == NULL)
	{
		m_pFirstChild = pChildObject;
	}
	else
	{
		CDRGObject	*pTempObject = m_pFirstChild;
		//skip to one before end of list
		while (pTempObject->m_pNext != NULL)
		{			
			pTempObject = pTempObject->m_pNext;
		}
		//now add child
		pTempObject->m_pNext = pChildObject;
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
HRESULT	CDRGObject::CheckAgainstChildren(CDRGObject	*pPotentialParent, CDRGObject *pPotentialChild)
{
	CDRGObject	*pTempObject = pPotentialChild;

	//skip to end of list, comparing against parent object
	while (pTempObject != NULL)
	{
		if(pPotentialChild == pPotentialParent)
			return E_FAIL;

		pTempObject = pTempObject->m_pNext;
	}

	//if we got here, none of the siblings was the potential parent
	if (pPotentialChild->m_pFirstChild == NULL)
		return S_OK;
	else
		return CheckAgainstChildren(pPotentialParent, pPotentialChild->m_pFirstChild);
}

HRESULT	CDRGObject::SetShader(CDRGShader	*pShader)
{
	m_pShader = pShader;
	//pShader->TargetChanged(m_pWrapper);
	return S_OK;
}

////////////////
// The following function and the ones after it are for loading .x files
//  note that we currently don't use the materials from the X file. This will
//  follow in a future version of the appwizard
////////////////

HRESULT CDRGObject::LoadObject( char *szSource, CDRGObject *pRoot, DWORD dwFlags )
{
	HRESULT hr = S_OK;
    LPDIRECTXFILE           pDXFile;
    LPDIRECTXFILEENUMOBJECT pEnumObj;
    LPDIRECTXFILEDATA       pFileData;
    const GUID*             pGUID;

    // Create the file object, and register the D3DRM templates for .X files
    if( FAILED( DirectXFileCreate( &pDXFile ) ) )
        return E_FAIL;
    if( FAILED( pDXFile->RegisterTemplates( (VOID*)D3DRM_XTEMPLATES,
                                            D3DRM_XTEMPLATE_BYTES ) ) )
    {
        pDXFile->Release();
        return E_FAIL;
    }

	//
	// Open the source file or url
	//
	if( dwFlags & OBJECT_LOAD_FILE )
	{
		// Create an enumerator object, to enumerate through the .X file objects.
		// This will open the file in the current directory.
		hr = pDXFile->CreateEnumObject( szSource, DXFILELOAD_FROMFILE, &pEnumObj );
	}
	else if( dwFlags & OBJECT_LOAD_URL )
	{
		// Create an enumerator object from an URL
		hr = pDXFile->CreateEnumObject( szSource, DXFILELOAD_FROMURL, &pEnumObj );
	}
	else
		hr = E_FAIL;
	if( FAILED(hr) )
	{
		pDXFile->Release();
		return hr;
	}

    // Cycle through each object. Parse meshes and frames as appropriate
    while( SUCCEEDED( hr = pEnumObj->GetNextDataObject( &pFileData ) ) )
    {
        pFileData->GetType( &pGUID );

        if( *pGUID == TID_D3DRMFrame )
            ParseFrame( pFileData, pRoot );

        if( *pGUID == TID_D3DRMMesh )
            ParseMesh( pFileData, pRoot );

        pFileData->Release();
    }

    // Success will result in hr == DXFILEERR_NOMOREOBJECTS
    if( DXFILEERR_NOMOREOBJECTS == hr ) 
        hr = S_OK;

    pEnumObj->Release();
    pDXFile->Release();

	return hr;
}

HRESULT CDRGObject::ParseFrame( LPDIRECTXFILEDATA pFileData, CDRGObject* pParentFrame )
{
    DWORD dwNameLen=80;
    TCHAR strName[80];
    if( FAILED( pFileData->GetName( strName, &dwNameLen ) ) )
        return E_FAIL;

	CDRGObject* pFrame;

	if (pParentFrame == NULL)
		pFrame = this;
	else
		pFrame = new CDRGObject();

    // Enumerate child objects.
    LPDIRECTXFILEOBJECT pChildObj;
    while( SUCCEEDED( pFileData->GetNextObject( &pChildObj ) ) )
    {
        LPDIRECTXFILEDATA pChildData;
        if( SUCCEEDED( pChildObj->QueryInterface( IID_IDirectXFileData,
                            (VOID**)&pChildData ) ) )
        {
            const GUID* pGUID;
            pChildData->GetType( &pGUID );

            if( TID_D3DRMFrame == *pGUID )
                ParseFrame( pChildData, pFrame );

            if( TID_D3DRMMesh == *pGUID )
                ParseMesh( pChildData, pFrame );

            if( TID_D3DRMFrameTransformMatrix == *pGUID )
            {
                DWORD dwSize;
                VOID* pData;
                if( FAILED( pChildData->GetData( NULL, &dwSize, &pData ) ) )
                {
                    delete pFrame;
                    return E_FAIL;
                }

                if( dwSize == sizeof(D3DMATRIX) )
                {
                    // Convert from a left- to a right-handed cordinate system
                    D3DMATRIX* pmatFrame = (D3DMATRIX*)pData;
                    pmatFrame->_13 *= -1.0f;
                    pmatFrame->_31 *= -1.0f;
                    pmatFrame->_23 *= -1.0f;
                    pmatFrame->_32 *= -1.0f;
                    pmatFrame->_43 *= -1.0f;
					pFrame->m_MxLocalBase = *pmatFrame;
                }
            }

            pChildData->Release();
        }

        pChildObj->Release();
    }

	if( pFrame != this)
	{
		pParentFrame->AddChild( pFrame );
	//	pFrame->m_pMaterial = pParentFrame->m_pMaterial;
	}

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: GetFace
// Desc: Get the nth face
//-----------------------------------------------------------------------------
DWORD* GetFace( DWORD* pFaceData, DWORD dwFace )
{
    for( DWORD i=0; i<dwFace; i++ )
        pFaceData += (*pFaceData) + 1;

    return pFaceData;
}




//-----------------------------------------------------------------------------
// Name: GetNumIndices
// Desc: Get number of indices from face data
//-----------------------------------------------------------------------------
DWORD GetNumIndices( DWORD* pFaceData, DWORD dwNumFaces )
{
    DWORD dwNumIndices = 0;
    while( dwNumFaces-- > 0 )
    {
        dwNumIndices += (*pFaceData-2)*3;
        pFaceData += *pFaceData + 1;
    }

    return dwNumIndices;
}

HRESULT CDRGObject::ParseMesh( LPDIRECTXFILEDATA pFileData, CDRGObject* pParentFrame )
{
    DWORD dwNameLen=80;
    TCHAR strName[80];
	CDRGObject *pMesh;

    if( FAILED( pFileData->GetName( strName, &dwNameLen ) ) )
        return E_FAIL;

    // Read the Mesh data from the file
    LONG  pData;
    DWORD dwSize;
    if( FAILED( pFileData->GetData( NULL, &dwSize, (VOID**)&pData ) ) )
        return E_FAIL;

    DWORD      dwNumVertices =    *((DWORD*)pData); pData += 4;
    D3DVECTOR* pVertices     = ((D3DVECTOR*)pData); pData += 12*dwNumVertices;
    DWORD      dwNumFaces    =    *((DWORD*)pData); pData += 4;
    DWORD*     pFaceData     =      (DWORD*)pData;

    // Create the Vertex Data
	//
    D3DVERTEX *pVerts     = new D3DVERTEX[dwNumVertices];
	
    if( NULL == pVertices )
        return E_FAIL;
    ZeroMemory( pVerts, dwNumVertices * sizeof(D3DVERTEX) );
    for( DWORD i=0; i< dwNumVertices; i++ )
    {
        pVerts[i].x = pVertices[i].x;
        pVerts[i].y = pVertices[i].y;
        pVerts[i].z = pVertices[i].z;
    }
    // Count the number of indices (converting n-sided faces to triangles)
    DWORD dwNumIndices = GetNumIndices( pFaceData, dwNumFaces );
    // Allocate memory for the indices
    WORD *pIndices = new WORD[dwNumIndices];
    if( NULL == pIndices )
        return E_FAIL;

	//
    // Assign the indices (build a triangle fan for high-order polygons)
	//
	DWORD *pFaceData2 = pFaceData;
	WORD *pIndices2 = pIndices;
	for( DWORD j=0; j<dwNumFaces; j++ )
    {
        DWORD dwNumVerticesPerFace = *pFaceData2++;

        for( DWORD i=2; i<dwNumVerticesPerFace; i++ )
        {
            *pIndices2++ = (WORD)pFaceData2[0];
            *pIndices2++ = (WORD)pFaceData2[i-1];
            *pIndices2++ = (WORD)pFaceData2[i];
        }

        pFaceData2 += dwNumVerticesPerFace;
    }


    BOOL bHasNormals = FALSE;
    BOOL bHasMaterials = FALSE;

    // Enumerate child objects.
    LPDIRECTXFILEOBJECT pChildObj;
    while( SUCCEEDED( pFileData->GetNextObject( &pChildObj ) ) )
    {
        LPDIRECTXFILEDATA pChildData;

        if( SUCCEEDED( pChildObj->QueryInterface( IID_IDirectXFileData,
                                                  (VOID**)&pChildData ) ) )
        {
            const GUID* pGUID;
            LONG        pData;
            DWORD       dwSize;

            pChildData->GetType( &pGUID );
            if( FAILED( pChildData->GetData( NULL, &dwSize, (VOID**)&pData ) ) )
            {
                return NULL;
            }

            if( TID_D3DRMMeshMaterialList == *pGUID )
            {
                DWORD  dwNumMaterials = *((DWORD*)pData);   pData += 4;
                DWORD  dwNumMatFaces  = *((DWORD*)pData);   pData += 4;
                DWORD* pMatFace       =   (DWORD*)pData;

                if( dwNumMaterials == 1 || dwNumMatFaces != dwNumFaces )
                {
				    pMesh = new CDRGObject();
					
					pMesh->Init( m_pWrapper, pVerts, pIndices, dwNumVertices, dwNumIndices,
								 DRGVector(0), DRGVector(1), DRGVector(0,1,0), 0, 
								 DRGVector(0), DRGVector(0), 0  );
                    // Only one material add all faces at once
					if (pParentFrame == NULL)
					{
						AddChild(pMesh);
					//	pMesh->m_pMaterial = m_pMaterial;
					}
					else
					{
						pParentFrame->AddChild( pMesh );
					//	pMesh->m_pMaterial = pParentFrame->m_pMaterial;
					}
                }
                else
                {
					DWORD dwIndices = 0, dwFaceIdx = 0;
                    // Multiple materials, add in sorted order
                    for( DWORD mat=0; mat<dwNumMaterials; mat++ )
                    {
                        for( DWORD face=0; face<dwNumMatFaces; face++ )
                        {
                            if( pMatFace[face] != mat )
							{
								if( dwIndices )
								{
									pMesh = new CDRGObject();
									
									pMesh->Init( m_pWrapper, pVerts, &pIndices[dwFaceIdx], dwNumVertices, dwIndices,
												 DRGVector(0), DRGVector(1), DRGVector(0,1,0), 0, 
												 DRGVector(0),  DRGVector(0), 0 );
									// Only one material add all faces at once
									if (pParentFrame == NULL)
									{
										AddChild(pMesh);
								//		pMesh->m_pMaterial = m_pMaterial;
									}
									else
									{
										pParentFrame->AddChild( pMesh );
									//	pMesh->m_pMaterial = pParentFrame->m_pMaterial;
									}
                
									
								}
								dwIndices = 0;
								dwFaceIdx = face;
							}
							else
								dwIndices++;
                        }
						if( dwIndices )
						{
							pMesh = new CDRGObject();
							
							pMesh->Init( m_pWrapper, pVerts, &pIndices[dwFaceIdx], dwNumVertices, dwIndices,
										 DRGVector(0), DRGVector(1), DRGVector(0,1,0), 0, DRGVector(0), 
										 DRGVector(0), 0 );
							// Only one material add all faces at once
							if (pParentFrame == NULL)
							{
								AddChild(pMesh);
							//	pMesh->m_pMaterial = m_pMaterial;
							}
							else
							{
								pParentFrame->AddChild( pMesh );
							//	pMesh->m_pMaterial = pParentFrame->m_pMaterial;
							}
                
						}
                    }
                }

                //ParseMeshMaterialList( pChildData, pMesh );
                bHasMaterials = TRUE;
            }

            if( TID_D3DRMMeshNormals == *pGUID )
            {
                DWORD      dwNumNormals = *((DWORD*)pData);
                D3DVECTOR* pNormals     = (D3DVECTOR*)(pData+4);

                if( dwNumNormals == dwNumVertices )
                {
					for( DWORD i=0; i<dwNumVertices; i++ )
					{
						pMesh->m_pVertices[i].nx = pNormals[i].x;
						pMesh->m_pVertices[i].ny = pNormals[i].y;
						pMesh->m_pVertices[i].nz = pNormals[i].z;
					}
                    bHasNormals = TRUE;
                }
            }

            if( TID_D3DRMMeshTextureCoords == *pGUID )
            {
                // Copy the texture coords into the mesh's vertices
                DWORD  dwNumTexCoords = *((DWORD*)pData);
                FLOAT* pTexCoords     = (FLOAT*)(pData+4);

                if( dwNumTexCoords == dwNumVertices )
				{
					for( DWORD i=0; i<dwNumVertices; i++ )
					{
						pMesh->m_pVertices[i].tu = pTexCoords[i*2];
						pMesh->m_pVertices[i].tv = pTexCoords[i*2+1];
					}
				}
            }

            pChildData->Release();
        }

        pChildObj->Release();
    }

    if( !bHasMaterials )
	{
		pMesh = new CDRGObject();
		
		pMesh->Init( m_pWrapper, pVerts, pIndices, dwNumVertices, dwNumIndices, 
					 DRGVector(0), DRGVector(1), DRGVector(0,1,0), 0, 
					 DRGVector(0), DRGVector(0), 0 );
        // Only one material add all faces at once
		if (pParentFrame == NULL)
		{
			AddChild(pMesh);
		//	pMesh->m_pMaterial = m_pMaterial;
		}
		else
		{
			pParentFrame->AddChild( pMesh );
		//	pMesh->m_pMaterial = pParentFrame->m_pMaterial;
		}
                
	}

//    if( !bHasNormals )
//        pMesh->ComputeNormals();

//    pParentFrame->AddChild( pMesh );
    return S_OK;
}

HRESULT CDRGObject::ParseMeshMaterialList( LPDIRECTXFILEDATA pFileData, CDRGObject* pObject )
{
	/*
    LPDIRECTXFILEOBJECT        pChildObj;
    LPDIRECTXFILEDATA          pChildData;
    LPDIRECTXFILEDATAREFERENCE pChildDataRef;
    DWORD                      dwMaterial = 0;

    while( SUCCEEDED( pFileData->GetNextObject( &pChildObj ) ) )
    {
        if( SUCCEEDED( pChildObj->QueryInterface( IID_IDirectXFileData,
                                                    (VOID**)&pChildData) ) )
        {
            const GUID* pguid;
            pChildData->GetType( &pguid );

            if( TID_D3DRMMaterial == *pguid )
            {
                ParseMaterial(pChildData, pObject, dwMaterial++);
            }

            pChildData->Release();
        }

        if( SUCCEEDED( pChildObj->QueryInterface( IID_IDirectXFileDataReference,
                                                    (VOID**)&pChildDataRef) ) )
        {
            if( SUCCEEDED( pChildDataRef->Resolve( &pChildData ) ) )
            {
                const GUID* pguid;
                pChildData->GetType( &pguid );

                if( TID_D3DRMMaterial == *pguid )
                {
                    ParseMaterial( pChildData, pObject, dwMaterial++ );
                }

                pChildData->Release();
            }
            pChildDataRef->Release();
        }

        pChildObj->Release();
    }
*/
  return S_OK;
}

HRESULT CDRGObject::ParseMaterial( LPDIRECTXFILEDATA pFileData, CDRGObject* pObject, DWORD dwMaterial )
{
    // Read data from the file
	/*
    LONG  pData;
    DWORD dwSize;
    TCHAR strTexture[128];

    if( FAILED( pFileData->GetData( NULL, &dwSize, (VOID**)&pData ) ) )
        return NULL;

    // Set the material properties for the mesh
    D3DMATERIAL7 mtrl;
    ZeroMemory( &mtrl, sizeof(mtrl) );
    memcpy( &mtrl.diffuse,  (VOID*)(pData+0),  sizeof(FLOAT)*4 );
    memcpy( &mtrl.ambient,  (VOID*)(pData+0),  sizeof(FLOAT)*4 );
    memcpy( &mtrl.power,    (VOID*)(pData+16), sizeof(FLOAT)*1 );
    memcpy( &mtrl.specular, (VOID*)(pData+20), sizeof(FLOAT)*3 );
    memcpy( &mtrl.emissive, (VOID*)(pData+32), sizeof(FLOAT)*3 );
    strTexture[0] = 0;

    LPDIRECTXFILEOBJECT pChildObj;
    if( SUCCEEDED( pFileData->GetNextObject(&pChildObj) ) )
    {
        LPDIRECTXFILEDATA pChildData;

        if( SUCCEEDED( pChildObj->QueryInterface( IID_IDirectXFileData,
                                                    (VOID**)&pChildData) ) )
        {
            const GUID* pguid;
            pChildData->GetType( &pguid );

            if( TID_D3DRMTextureFilename == *pguid )
            {
                TCHAR** string;

                if( FAILED( pChildData->GetData( NULL, &dwSize, (VOID**)&string ) ) )
                    return NULL;

                D3DTextr_CreateTextureFromFile( *string );
                lstrcpyn( strTexture, *string, 128 );
            }

            pChildData->Release();
        }

        pChildObj->Release();
    }

    pObject->SetMaterialData( dwMaterial, &mtrl, strTexture );
	*/

    return S_OK;
}

