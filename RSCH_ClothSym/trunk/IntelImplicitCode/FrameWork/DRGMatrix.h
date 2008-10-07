// DRGMatrix.h - Created with DR&E AppWizard 1.0B10
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
//  Declatation of the DRGMatrix & DRGVector classes. Includes overloaded operators
//  and some utility functions (set view, rotate, etc)
//
// ------------------------------------------------------------------------------------
//
// Authors: Dean Macri, Kim Pallister - Intel Developer Relations & Engineering Group
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRGMATRIX_H__1EFA6526_5939_11D2_B1A5_00A0C90D2C1B__INCLUDED_)
#define AFX_DRGMATRIX_H__1EFA6526_5939_11D2_B1A5_00A0C90D2C1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class DRGVector  
{
public:

	//member vars, not too many of 'em
	float	x,y,z;	

	// Constructors
	DRGVector() { };							
	DRGVector(float f);							//all the same
	DRGVector(float _x, float _y, float _z);	//each itited
	~DRGVector(); //destructor (dont know if we'll need it...)

	//another method for initing (should get reid of this entirely
	void Init(float a, float b, float c);

	// Assignment operators
	//DRGVector operator= (DRGVector v);
	DRGVector operator= (const float f);

	DRGVector operator += (const DRGVector v);
	DRGVector operator -= (const DRGVector v);
	DRGVector operator *= (const DRGVector v);
	DRGVector operator /= (const DRGVector v);
	DRGVector operator *= (float s);
	DRGVector operator /= (float s);

	// Unary operators
	friend DRGVector operator + (const DRGVector& v);
	friend DRGVector operator - (const DRGVector& v);


	// ---- Binary operators ----

	// Addition and subtraction
	friend DRGVector operator + (const DRGVector& v1, const DRGVector& v2); 
	friend DRGVector operator - (const DRGVector& v1, const DRGVector& v2);
	// Scalar multiplication and division
	friend DRGVector operator * (const DRGVector& v, float s);
	friend DRGVector operator * (float s, const DRGVector& v);
	friend DRGVector operator / (const DRGVector& v, float s);
	// Memberwise multiplication and division
	friend DRGVector operator * (const DRGVector& v1, const DRGVector& v2);
	friend DRGVector operator / (const DRGVector& v1, const DRGVector& v2);

	// Length-related functions
	//    friend float SquareMagnitude (const DRGVector& v);
	friend float Magnitude (const DRGVector& v);

	// Returns vector with same direction and unit length
	friend DRGVector Normalize (const DRGVector& v);

	// Dot and cross product
	friend float DotProduct (const DRGVector& v1, const DRGVector& v2);
	friend DRGVector CrossProduct (const DRGVector& v1, const DRGVector& v2);

}; //end DRG Vector class declaration




class DRGMatrix  
{
public:
	D3DMATRIX m_Mx;

	DRGMatrix( bool bSetIdentity = false );
	virtual ~DRGMatrix();

	void SetIdentity( void );
	void PreMultiply( DRGMatrix &Mx );
	void PostMultiply( DRGMatrix &Mx );

	// Overload the math operators we need (mult, equals)
	DRGMatrix operator= (DRGMatrix mx1);
	DRGMatrix operator= (D3DMATRIX &mx1);
	
	friend DRGMatrix operator *(const DRGMatrix &Mx1, const DRGMatrix &Mx2); 
	friend DRGVector operator *(const DRGVector &V, const DRGMatrix &Mx); 

	void RotateX( float fAngle );
	void RotateY( float fAngle );
	void RotateZ( float fAngle );
	void Rotate( float fX, float fY, float fZ, float fAngle );
	void Rotate( DRGVector dvAxis, float fAngle );
	void Translate( float fX, float fY, float fZ );

	void Scale( float fSX, float fSY, float fSZ );
	void SetView( DRGVector dvFrom, DRGVector dvTo, DRGVector dvUp );
	void SetProjection( float fFOV, float fAspect, float fNearClip, float fFarClip );
};



#endif // !defined(AFX_DRGMATRIX_H__1EFA6526_5939_11D2_B1A5_00A0C90D2C1B__INCLUDED_)
