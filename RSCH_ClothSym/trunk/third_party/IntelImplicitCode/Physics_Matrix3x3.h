// Physics_Matrix3x3.h: interface for the Physics_Matrix3x3 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PHYSICS_MATRIX3X3_H__4A42BD04_1D5E_47CA_BFCF_4ED42AF47944__INCLUDED_)
#define AFX_PHYSICS_MATRIX3X3_H__4A42BD04_1D5E_47CA_BFCF_4ED42AF47944__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Physics_Matrix3x3  
{
public:
	Physics_t m_Mx[9];

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
	void Multiply( Physics_t scale, Physics_Matrix3x3 &Dst );
	void CrossProduct( Physics_Vector3 &v, Physics_Matrix3x3 &Dst );
	void Invert( Physics_Matrix3x3 &Dst );
	void Set( Physics_t a00, Physics_t a01, Physics_t a02,
			  Physics_t a10, Physics_t a11, Physics_t a12, 
			  Physics_t a20, Physics_t a21, Physics_t a22 );
	void Set( Physics_Vector3 &row1, Physics_Vector3 &row2, Physics_Vector3 &row3 );
	void SetTilde( Physics_t x, Physics_t y, Physics_t z );
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

#endif // !defined(AFX_PHYSICS_MATRIX3X3_H__4A42BD04_1D5E_47CA_BFCF_4ED42AF47944__INCLUDED_)
