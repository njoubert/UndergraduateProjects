/*
 * intel_Matrix3x3.h
 *
 *  Created on: Oct 7, 2008
 *      Author: silversatin
 */

#ifndef INTEL_MATRIX3X3_H_
#define INTEL_MATRIX3X3_H_

// Physics_Matrix3x3.h: interface for the Physics_Matrix3x3 class.
//
//////////////////////////////////////////////////////////////////////
/*
#if !defined(AFX_PHYSICS_MATRIX3X3_H__4A42BD04_1D5E_47CA_BFCF_4ED42AF47944__INCLUDED_)
#define AFX_PHYSICS_MATRIX3X3_H__4A42BD04_1D5E_47CA_BFCF_4ED42AF47944__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//*/

#include "global.h"
#include "intel_Vector3.h"

class Physics_Matrix3x3
{
public:
	double m_Mx[9];

public:
	Physics_Matrix3x3();
	virtual ~Physics_Matrix3x3();

	void Zero();
	void SetIdentity();

	void Multiply( Physics_Matrix3x3 &M, Physics_Matrix3x3 &Dst );
	void Add( Physics_Matrix3x3 &M, Physics_Matrix3x3 &Dst );
	void Subtract( Physics_Matrix3x3 &M, Physics_Matrix3x3 &Dst );
	void PostMultiply( Physics_Vector3 &v, Physics_Vector3 &Dst );
	void PreMultiply( Physics_Vector3 &v, Physics_Vector3 &Dst );
	void Multiply( double scale, Physics_Matrix3x3 &Dst );
	void CrossProduct( Physics_Vector3 &v, Physics_Matrix3x3 &Dst );
	void Invert( Physics_Matrix3x3 &Dst );
	void Set( double a00, double a01, double a02,
			  double a10, double a11, double a12,
			  double a20, double a21, double a22 );
	void Set( Physics_Vector3 &row1, Physics_Vector3 &row2, Physics_Vector3 &row3 );
	void SetTilde( double x, double y, double z );
	void SetTilde( Physics_Vector3 &v );
	void Transpose( void );
	void Negate( void );
	inline Physics_Vector3 &Row(int iRow )
	{
		return *((Physics_Vector3 *)&m_Mx[iRow*3]);
	}

	Physics_Matrix3x3& operator=( Physics_Matrix3x3 &other );
	void SetFromOuterProduct( Physics_Vector3 &v1, Physics_Vector3 &v2 );
	bool IsSymmetric();
	bool IsTranspose( Physics_Matrix3x3& other );
	void Dump( char *szHeading = NULL );

};

//#endif // !defined(AFX_PHYSICS_MATRIX3X3_H__4A42BD04_1D5E_47CA_BFCF_4ED42AF47944__INCLUDED_)


#endif /* INTEL_MATRIX3X3_H_ */
