// Physics_LargeVector.h: interface for the Physics_LargeVector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PHYSICS_LARGEVECTOR_H__FDF65582_D36F_4061_A343_BDDA44E2DFDE__INCLUDED_)
#define AFX_PHYSICS_LARGEVECTOR_H__FDF65582_D36F_4061_A343_BDDA44E2DFDE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Physics_Matrix3x3;
class Physics_LargeVector  
{
public:
	int m_iElements;
	Physics_Vector3 *m_pData;

public:
	Physics_LargeVector( int iElements=1 );
	virtual ~Physics_LargeVector();

	void Zero();
	void Resize( int iNewElements );
	bool Add( Physics_LargeVector &v, Physics_LargeVector &dst );
	bool Subtract( Physics_LargeVector &v, Physics_LargeVector &dst );
	Physics_t DotProduct( Physics_LargeVector &v );
	bool ElementMultiply( Physics_LargeVector &v, Physics_LargeVector &dst );
	bool ElementMultiply( Physics_Matrix3x3 S[], Physics_LargeVector &dst );
	bool Scale( Physics_t scale, Physics_LargeVector &dst );
	bool Invert( Physics_LargeVector &dst );
	Physics_LargeVector& operator=( Physics_LargeVector &copy );
	void Dump( char *szTitle = NULL );
};

#endif // !defined(AFX_PHYSICS_LARGEVECTOR_H__FDF65582_D36F_4061_A343_BDDA44E2DFDE__INCLUDED_)
