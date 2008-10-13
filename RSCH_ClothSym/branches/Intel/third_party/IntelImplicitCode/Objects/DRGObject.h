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
// The intent is to use this as a base class for deriving other objects, like procedural
// ones, or ones loaded from specific file types
//
// ------------------------------------------------------------------------------------
//
// Author: Kim Pallister. 
// 
////////////////////////////////////////////////////////////////////////////////////////

#define OBJECT_LOAD_FILE	1
#define OBJECT_LOAD_URL		2


class CDRGObject {

public:
	CDRGObject();
	virtual ~CDRGObject();
	
	
	// Initialize an object class instance, if the pVerts and pIndices are not null
	// it will copy the verts and indices from these location
	HRESULT Init(	CDRGWrapper *pDRGWrapper,
					D3DVERTEX	*pVerts			= NULL,
					WORD		*pIndices		= NULL,	
					DWORD		dwNumVerts		= 0,
					DWORD		dwNumIndices	= 0,					
					DRGVector	pos				= DRGVector(0.0f),
					DRGVector	scale			= DRGVector(1.0f),
					DRGVector	rotationaxis	= DRGVector(0.0f,1.0f,0.0f),
					float		rotationangle	= 0.0f,
					DRGVector	vel				= DRGVector(0.0f),
					DRGVector	scalevel		= DRGVector(0.0f),
					float		rotationalvel	= 0.0f,
					DWORD		dwFlags			= 0 );
	
	// overloaded version of the above, to load the verts and indices from a .X file, and build
	// a hierarchy of DRGObjects from the file info
	HRESULT Init(	CDRGWrapper *pDRGWrapper,
					char		*strFileName,
					DRGVector	pos				= DRGVector(0.0f),
					DRGVector	scale			= DRGVector(1.0f),
					DRGVector	rotationaxis	= DRGVector(0.0f,1.0f,0.0f),
					float		rotationangle	= 0.0f,
					DRGVector	vel				= DRGVector(0.0f),
					DRGVector	scalevel		= DRGVector(0.0f),
					float		rotationalvel	= 0.0f,
					DWORD		dwFlags			= 0 );
	
	// This is called (usually) once per frame to update positions according to velocities, etc
	virtual HRESULT	Update(float	fTime, bool bUpdateChildren = true); //1.0 is the target frame rate
	
	// This is called to actually draw the primitives to the render target
	virtual HRESULT Render(DRGMatrix& MxWorld, bool bRenderChildren = true );

	// This needs to be called anytime there is a change to the D3DDevice (like when switching to full screen)
	virtual HRESULT TargetChanged(CDRGWrapper *pDRGWrapper, bool bUpdateChildren = true);

	// Used to build a hierarch
	HRESULT	AddChild(CDRGObject	*pChildObject);

	// Used to set shader
	HRESULT	SetShader(CDRGShader	*pShader);
	
	// Use this to get the number of triangles
	virtual int GetTriangleCount();

	// Some member vars that it didn't make sense to not give access to.

	// Beginning offset of any geometry before applying transforms. This is init'd to an identity matrix
	// but might be different when loading a .x file (if the default in the file was other than that
	DRGMatrix	m_MxLocalBase;

	// current position, rotation, scale
	DRGVector	m_vRotationAxis;
	float		m_fRotationAngle;
	DRGVector	m_vScale;
	DRGVector	m_vPosition;	
	
	// current velocity, rotational velocity, scale velociy
	float		m_fRotationalVelocity;
	DRGVector	m_vScalingVelocity;
	DRGVector	m_vVelocity;

	//variable to allow making the object visible without removing from scene
	bool		m_bVisible;
	
	//pointer to siblings and children, if this object is part of a hierarchy 
	CDRGObject	*m_pNext;
	CDRGObject	*m_pFirstChild;
	CDRGShader *m_pShader; //material for this object


protected:

	// Sets up a D3D Vertex Buffer using the current device.
	virtual HRESULT SetupIndexedVertexBuffer();

	// This is a safety check to make sure someone doesn't try to add an object as it's own child
	// (prevent a circular hierarchy)
	HRESULT	CheckAgainstChildren(CDRGObject	*pPotentialParent, CDRGObject *pPotentialChild);


	// Local transformation matrix, concatenation of the translation, scale, rotation
	DRGMatrix	m_MxLocalToParent;
	DRGMatrix	m_MxLocalToWorld;	//this gets updated at each frame, as the objects current tX,Rt * the parent's world MX

	//VB created by the above function
	LPDIRECT3DVERTEXBUFFER7 m_pIndexedVertexBuffer;
	
	// Pointer to sysmem copy of verts
	D3DVERTEX	*m_pVertices;
	WORD		*m_pIndices;
	int			m_iNumVertices;	    // number of indexed vertices
	int			m_iNumIndices;		// number of indexed vertex indices
	D3DPRIMITIVETYPE	m_d3dPrimType;

	// Local copy of the CDRGWrapper
	CDRGWrapper *m_pWrapper;


private:
	
	//PRIVATE METHODS

	//copy the verts and indices to the member vars for them
	virtual HRESULT SetupVertices( D3DVERTEX *pVertices );
	virtual HRESULT SetupIndices( WORD *pVertexIndices );

	//added to attempt .x file loading
	HRESULT LoadObject( char *szSource, CDRGObject *pRoot, DWORD dwFlags = OBJECT_LOAD_FILE );
	
	// these are all used by the LoadObject function
	HRESULT ParseFrame( LPDIRECTXFILEDATA pFileData, CDRGObject* pParentFrame );
	HRESULT ParseMesh( LPDIRECTXFILEDATA pFileData, CDRGObject* pParentFrame );
	HRESULT ParseMeshMaterialList( LPDIRECTXFILEDATA pFileData, CDRGObject* pObject );
	HRESULT ParseMaterial( LPDIRECTXFILEDATA pFileData, CDRGObject* pObject, DWORD dwMaterial );
};
