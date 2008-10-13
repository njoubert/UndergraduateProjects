// DRGMatrix.cpp - Created with DR&E AppWizard 1.0B10
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
//  Implementation of the DRGMatrix & DRGVector classes. Includes overloaded operators
//  and some utility functions (set view, rotate, etc)
//
// ------------------------------------------------------------------------------------
//
// Authors: Dean Macri, Kim Pallister - Intel Developer Relations & Engineering Group
//
///////////////////////////////////////////////////////////////////////////////////////

#include "..\DRGShell.h"

//////////////////////////////////////////////////////////////////////
// DRGVector class
//////////////////////////////////////////////////////////////////////

// Constructors

DRGVector::DRGVector(float f)							// this just inits x,y,z to f
{
	x = y = z = f;	
}							

DRGVector::DRGVector(float _x, float _y, float _z)		// also simple enough
{
	x = _x;
	y = _y;
	z = _z;
}

DRGVector::~DRGVector()
{
	;//nothing to destroy
}

void DRGVector::Init(float a, float b, float c)
{
	x = a;
	y = b;
	z = c;			
}


DRGVector DRGVector::operator= (const float f)
{
	x = f;
	y = f;
	z = f;
	return *this;	
}


DRGVector DRGVector::operator+= (const DRGVector v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;	
}

DRGVector DRGVector::operator-= (const DRGVector v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;	
}

DRGVector DRGVector::operator*= (const DRGVector v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
	return *this;	
}

DRGVector DRGVector::operator/= (const DRGVector v)
{
	x /= v.x;
	y /= v.y;
	z /= v.z;
	return *this;	
}

DRGVector DRGVector::operator*= (float s)
{
	x *= s;
	y *= s;
	z *= s;
	return *this;	
}

DRGVector DRGVector::operator/= (float s)
{
	x /= s; 
	y /= s;	
	z /= s;
	return *this;	
}


// Unary operators
DRGVector operator + (const DRGVector& v)
{
	DRGVector	temp = v;
	return temp;
}

DRGVector operator - (const DRGVector& v)
{
	DRGVector	temp = v;
	temp.x = -v.x;
	temp.y = -v.y;
	temp.z = -v.z;
	return temp;
}


DRGVector operator + (const DRGVector& v1, const DRGVector& v2)
{
	DRGVector vtemp;
	vtemp.x = v1.x + v2.x;
	vtemp.y = v1.y + v2.y;
	vtemp.z = v1.z + v2.z;
	return vtemp;
}

DRGVector operator - (const DRGVector& v1, const DRGVector& v2)
{ 
	DRGVector vtemp;
	vtemp.x = v1.x - v2.x;
	vtemp.y = v1.y - v2.y;
	vtemp.z = v1.z - v2.z;
	return vtemp;
}

DRGVector operator * (const DRGVector& v, float f)
{ 
	DRGVector vtemp;
	vtemp.x = v.x * f;
	vtemp.y = v.y * f;
	vtemp.z = v.z * f;
	return vtemp;
}

DRGVector operator * (float f, const DRGVector& v)
{ 
	DRGVector vtemp;
	vtemp.x = v.x * f;
	vtemp.y = v.y * f;
	vtemp.z = v.z * f;
	return vtemp;
}

DRGVector operator / (const DRGVector& v, float f)
{ 
	DRGVector vtemp;
	float invf = 1.0f / f; //THERE CAN BE ONLY ONE! (divide that is)	
	vtemp.x = v.x * invf;
	vtemp.y = v.y * invf;
	vtemp.z = v.z * invf;
	return vtemp;
}

//memberwise multiplication and division
DRGVector operator * (const DRGVector& v1, const DRGVector& v2)
{ 
	DRGVector vtemp;
	vtemp.x = v1.x * v2.x; 
	vtemp.y = v1.y * v2.y;
	vtemp.z = v1.z * v2.z; 
	return vtemp;
}

DRGVector operator / (const DRGVector& v1, const DRGVector& v2)
{ 
	DRGVector vtemp;
	vtemp.x = v1.x / v2.x; 
	vtemp.y = v1.y / v2.y;
	vtemp.z = v1.z / v2.z; 
	return vtemp;
}

float Magnitude( const DRGVector& v)
{
	return (float)sqrt( (v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

DRGVector Normalize (const DRGVector& v)
{
	DRGVector vtemp;
	float invmag = 1.0f/(float)sqrt( (v.x * v.x) + (v.y * v.y) + (v.z * v.z));
	vtemp.x = v.x * invmag;
	vtemp.y = v.y * invmag;
	vtemp.z = v.z * invmag; 
	return vtemp;
}

float DotProduct (const DRGVector& v1, const DRGVector& v2)
{
	return ( (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z));
}

DRGVector CrossProduct (const DRGVector& v1, const DRGVector& v2)
{
	DRGVector vtemp;
	
	vtemp.x = (v1.y * v2.z - v1.z * v2.y);
	vtemp.y = (v1.x * v2.z - v1.z * v2.x);
	vtemp.z = (v1.x * v2.y - v1.y * v2.x);
	return vtemp;
}



//////////////////////////////////////////////////////////////////////
// DRGMatrix class
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
// DRGMatrix::DRGMatrix( bool bSetIdentiy )
//
// -----------------------------------------------------------------------
DRGMatrix::DRGMatrix( bool bSetIdentiy )
{
	if( bSetIdentiy )
		SetIdentity();
}

// -----------------------------------------------------------------------
// DRGMatrix::~DRGMatrix()
//
// -----------------------------------------------------------------------
DRGMatrix::~DRGMatrix()
{
	// Nothing to destroy
}

// -----------------------------------------------------------------------
// void DRGMatrix::SetIdentity( void )
//
// -----------------------------------------------------------------------
void DRGMatrix::SetIdentity( void )
{
	for( int i=0; i<4; i++ )
		for( int j=0; j<4; j++)
			m_Mx(i,j) = ((i==j) ? 1.0f : 0.0f);
}

// can we get rid of the pre/post multiply, or at least make them private?
// -----------------------------------------------------------------------
// void DRGMatrix::PreMultiply( DRGMatrix &Mx )
//
// -----------------------------------------------------------------------
void DRGMatrix::PreMultiply( DRGMatrix &Mx ) 
{
	int i, j, k;
	DRGMatrix MxTemp = *this;

	memset( &m_Mx, 0, sizeof( float ) * 16 );

    for( i=0; i<4; i++ ) 
        for( j=0; j<4; j++ ) 
            for( k=0; k<4; k++ ) 
				m_Mx( i,j) += Mx.m_Mx(i,k) * MxTemp.m_Mx(k,j);
}

// -----------------------------------------------------------------------
// void DRGMatrix::PostMultiply( DRGMatrix &Mx )
//
// -----------------------------------------------------------------------
void DRGMatrix::PostMultiply( DRGMatrix &Mx )
{
	DRGMatrix MxTemp = *this;

	memset( &m_Mx, 0, sizeof( float ) * 16 );

    for( int i=0; i<4; i++ ) 
        for( int j=0; j<4; j++ ) 
            for( int k=0; k<4; k++ ) 
				m_Mx( i,j) += MxTemp.m_Mx(i,k) * Mx.m_Mx(k,j);
}


DRGMatrix DRGMatrix::operator= (DRGMatrix Mx)
{
	//DRGMatrix MxTemp = *this;

	for( int i=0; i<4; i++ ) 
	{
        for( int j=0; j<4; j++ )
		{
			m_Mx(i,j) = Mx.m_Mx(i,j);
		}
	}
	return *this;
	
}

DRGMatrix DRGMatrix::operator= (D3DMATRIX &mx1)
{
	m_Mx(0,0) = mx1._11;
	m_Mx(0,1) = mx1._12;
	m_Mx(0,2) = mx1._13;
	m_Mx(0,3) = mx1._14;

	m_Mx(1,0) = mx1._21;
	m_Mx(1,1) = mx1._22;
	m_Mx(1,2) = mx1._23;
	m_Mx(1,3) = mx1._24;

	m_Mx(2,0) = mx1._31;
	m_Mx(2,1) = mx1._32;
	m_Mx(2,2) = mx1._33;
	m_Mx(2,3) = mx1._34;

	m_Mx(3,0) = mx1._41;
	m_Mx(3,1) = mx1._42;
	m_Mx(3,2) = mx1._43;
	m_Mx(3,3) = mx1._44;

	return *this;
}


DRGMatrix operator*(const DRGMatrix &mx1, const DRGMatrix &mx2)
{
	int i, j, k;
	DRGMatrix MxTemp;// = *this;

	memset( &MxTemp.m_Mx, 0, sizeof( float ) * 16 );

    for( i=0; i<4; i++ ) 
        for( j=0; j<4; j++ ) 
            for( k=0; k<4; k++ ) 
				//MxTemp.m_Mx(i,j) += mx1.m_Mx(i,k) * mx2.m_Mx(k,j);
				MxTemp.m_Mx(i,j) += mx2.m_Mx(i,k) * mx1.m_Mx(k,j);

	return MxTemp;
}

DRGVector operator*(const DRGVector &V, const DRGMatrix &Mx)
{
	//int i, j, k;
	DRGVector VTemp;// = *this;

	VTemp = 0.0f; //overloading broadcasts this.


	VTemp.x = (V.x * Mx.m_Mx(0,0)) + (V.y * Mx.m_Mx(1,0)) + (V.z * Mx.m_Mx(2,0)) + (/*V.w * */ Mx.m_Mx(3,0));
	VTemp.y = (V.x * Mx.m_Mx(0,1)) + (V.y * Mx.m_Mx(1,1)) + (V.z * Mx.m_Mx(2,1)) + (/*V.w * */ Mx.m_Mx(3,1));
	VTemp.z = (V.x * Mx.m_Mx(0,2)) + (V.y * Mx.m_Mx(1,2)) + (V.z * Mx.m_Mx(2,2)) + (/*V.w * */ Mx.m_Mx(3,2));
	////VTemp.w = (V.x * Mx.m_Mx(3,0)) + (V.y * Mx.m_Mx(3,1)) + (V.z * Mx.m_Mx(3,2)) + (V.w * Mx.m_Mx(3,3));

	return VTemp;

}



// -----------------------------------------------------------------------
// void DRGMatrix::RotateX( float fAngle )
//
// -----------------------------------------------------------------------
void DRGMatrix::RotateX( float fAngle )
{
	DRGMatrix MxRotate( true );
	
	MxRotate.m_Mx(1,1) = MxRotate.m_Mx(2,2) = (float)cos( fAngle );
	MxRotate.m_Mx(1,2) = (float)sin( fAngle );
	MxRotate.m_Mx(2,1) = - MxRotate.m_Mx(1,2);

	PreMultiply( MxRotate );
}

// -----------------------------------------------------------------------
// void DRGMatrix::RotateY( float fAngle )
//
// -----------------------------------------------------------------------
void DRGMatrix::RotateY( float fAngle )
{
	DRGMatrix MxRotate( true );
	
	MxRotate.m_Mx(0,0) = MxRotate.m_Mx(2,2) = (float)cos( fAngle );
	MxRotate.m_Mx(0,2) = (float)sin( -fAngle );
	MxRotate.m_Mx(2,0) = - MxRotate.m_Mx(0,2);

	PreMultiply( MxRotate );

}

// -----------------------------------------------------------------------
// void DRGMatrix::RotateZ( float fAngle )
//
// -----------------------------------------------------------------------
void DRGMatrix::RotateZ( float fAngle )
{
	DRGMatrix MxRotate( true );
	
	MxRotate.m_Mx(0,0) = MxRotate.m_Mx(1,1) = (float)cos( fAngle );
	MxRotate.m_Mx(0,1) = (float)sin( fAngle );
	MxRotate.m_Mx(1,0) = - MxRotate.m_Mx(0,1);

	PreMultiply( MxRotate );
}

// -----------------------------------------------------------------------
// void DRGMatrix::Rotate( float fX, float fY, float fZ, float fAngle )
//
// -----------------------------------------------------------------------
void DRGMatrix::Rotate( float fX, float fY, float fZ, float fAngle )
{
	DRGVector dvAxis( fX, fY, fZ );

	Rotate( dvAxis, fAngle );
}

// -----------------------------------------------------------------------
// void DRGMatrix::Rotate( DRGVector dvAxis, float fAngle )
//
// -----------------------------------------------------------------------
void DRGMatrix::Rotate( DRGVector dvAxis, float fAngle )
{
    float fCos = (float)cos( fAngle );
    float fSin = (float)sin( fAngle );
	float fX, fY, fZ;
	DRGMatrix MxRotate;

	//
	// Normalize the vector
	//
	dvAxis = Normalize( dvAxis );
	fX = dvAxis.x;
	fY = dvAxis.y;
	fZ = dvAxis.z;

	MxRotate.m_Mx(0,0) = (fX * fX) * (1.0f - fCos) + fCos;
	MxRotate.m_Mx(0,1) = (fX * fY) * (1.0f - fCos) - (fZ * fSin);
	MxRotate.m_Mx(0,2) = (fX * fZ) * (1.0f - fCos) + (fY * fSin);

    MxRotate.m_Mx(1,0) = (fY * fX) * (1.0f - fCos) + (fZ * fSin);
    MxRotate.m_Mx(1,1) = (fY * fY) * (1.0f - fCos) + fCos ;
    MxRotate.m_Mx(1,2) = (fY * fZ) * (1.0f - fCos) - (fX * fSin);

    MxRotate.m_Mx(2,0) = (fZ * fX) * (1.0f - fCos) - (fY * fSin);
    MxRotate.m_Mx(2,1) = (fZ * fY) * (1.0f - fCos) + (fX * fSin);
    MxRotate.m_Mx(2,2) = (fZ * fZ) * (1.0f - fCos) + fCos;
    
    MxRotate.m_Mx(0,3) = MxRotate.m_Mx(1,3) = MxRotate.m_Mx(2,3) = 0.0f;
    MxRotate.m_Mx(3,0) = MxRotate.m_Mx(3,1) = MxRotate.m_Mx(3,2) = 0.0f;
    MxRotate.m_Mx(3,3) = 1.0f;

	PreMultiply( MxRotate );
}

// -----------------------------------------------------------------------
// void DRGMatrix::Translate( float fX, float fY, float fZ )
//
// -----------------------------------------------------------------------
void DRGMatrix::Translate( float fX, float fY, float fZ )
{
	DRGMatrix MxTranslate( true );

	MxTranslate.m_Mx(3,0) = fX;
	MxTranslate.m_Mx(3,1) = fY;
	MxTranslate.m_Mx(3,2) = fZ;

	PreMultiply( MxTranslate );
}

// -----------------------------------------------------------------------
// void DRGMatrix::Scale( float fSX, float fSY, float fSZ )
//
// -----------------------------------------------------------------------
void DRGMatrix::Scale( float fSX, float fSY, float fSZ )
{
	DRGMatrix MxScale( true );

	MxScale.m_Mx(0,0) = fSX;
	MxScale.m_Mx(1,1) = fSY;
	MxScale.m_Mx(2,2) = fSZ;

	PreMultiply( MxScale );
}

// -----------------------------------------------------------------------
// void DRGMatrix::SetView( D3DVECTOR dvFrom, D3DVECTOR dvTo, D3DVECTOR dvUp )
//
// -----------------------------------------------------------------------
void DRGMatrix::SetView( DRGVector dvFrom, DRGVector dvTo, DRGVector dvUp )
{
	// Create the View vector
    DRGVector dvView = dvTo - dvFrom;

    float fLength = Magnitude( dvView );
    if( fLength < 1e-6f )
        return;

    // Normalize the view vector
    dvView /= fLength;

    // Get the dot product, and calculate the projection of the view
    // vector onto the up vector. The projection is the y basis vector.
    float fDotProduct = DotProduct( dvUp, dvView );

    dvUp = dvUp - fDotProduct * dvView;

    // If this vector has near-zero length because the input specified a
    // bogus up vector, let's try a default up vector
    if( 1e-6f > ( fLength = Magnitude( dvUp ) ) )
    {
        dvUp = DRGVector( 0.0f, 1.0f, 0.0f ) - dvView.y * dvView;

        // If we still have near-zero length, resort to a different axis.
        if( 1e-6f > ( fLength = Magnitude( dvUp ) ) )
        {
            dvUp = DRGVector( 0.0f, 0.0f, 1.0f ) - dvView.z * dvView;

            if( 1e-6f > ( fLength = Magnitude( dvUp ) ) )
                return;
        }
    }

    // Normalize the y basis vector
    dvUp /= fLength;

    // The x basis vector is found simply with the cross product of the y
    // and z basis vectors
    DRGVector dvRight = CrossProduct( dvUp, dvView );

	SetIdentity();
	m_Mx(0,0) = dvRight.x;	m_Mx(0,1) = dvUp.x;	m_Mx(0,2) = dvView.x;
	m_Mx(1,0) = dvRight.y;	m_Mx(1,1) = dvUp.y;	m_Mx(1,2) = dvView.y;
	m_Mx(2,0) = dvRight.z;	m_Mx(2,1) = dvUp.z;	m_Mx(2,2) = dvView.z;

    // Do the translation values (rotations are still about the eyepoint)
	m_Mx(3,0) = - DotProduct( dvFrom, dvRight );
	m_Mx(3,1) = - DotProduct( dvFrom, dvUp );
	m_Mx(3,2) = - DotProduct( dvFrom, dvView );

	return;
}

// -----------------------------------------------------------------------
// void DRGMatrix::SetProjection( float fFOV, float fAspect, float fNearClip, float fFarClip )
//
// -----------------------------------------------------------------------
void DRGMatrix::SetProjection( float fFOV, float fAspect, float fNearClip, float fFarClip )
{
    if( fabs(fFarClip-fNearClip) < 0.01f )
        return;
    if( fabs(sin(fFOV/2)) < 0.01f )
        return;

	float w = fAspect * (float)( cos(fFOV/2)/sin(fFOV/2) );
	float h =   1.0f  * (float)( cos(fFOV/2)/sin(fFOV/2) );
    float Q = fFarClip / ( fFarClip - fNearClip );

	SetIdentity();
	m_Mx(0,0) = w;
	m_Mx(1,1) = h;
	m_Mx(2,2) = Q;
	m_Mx(2,3) = 1.0f;
	m_Mx(3,2) = -Q * fNearClip;
	m_Mx(3,3) = 0.0f;

	
}




