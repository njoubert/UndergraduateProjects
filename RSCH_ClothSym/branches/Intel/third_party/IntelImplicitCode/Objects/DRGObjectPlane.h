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
// Author: Kim Pallister. Originally modified (much) from Mike Rohman's sphere class
// 
////////////////////////////////////////////////////////////////////////////////////////

class CDRGObjectPlane : public CDRGObject {

public:
	CDRGObjectPlane();
	virtual ~CDRGObjectPlane();
	
	
	// Initialize a sphere object (center, radius, tesselation, rotation)
	// default parameters are for object having no velocity, and centered
	// also, note that default for the base class has no vertices. 
	HRESULT Init(	CDRGWrapper *pDRGWrapper,
					int			divisions		= 1,
					DRGVector	pos				= DRGVector(0.0f),
					DRGVector	scale			= DRGVector(1.0f),
					DRGVector	rotationaxis	= DRGVector(0.0f,1.0f,0.0f),
					float		rotationangle	= 0.0f,
					DRGVector	vel				= DRGVector(0.0f),
					DRGVector	scalevel		= DRGVector(0.0f),
					float		rotationalvel	= 0.0f,
					DWORD		dwFlags			= 0 );	
		
private:
	
	//setup the verts and indices
	HRESULT SetupVertices( );
	HRESULT SetupIndices( );
};
