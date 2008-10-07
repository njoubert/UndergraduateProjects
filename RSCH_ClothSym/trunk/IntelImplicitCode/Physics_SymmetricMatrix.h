// Physics_SymmetricMatrix.h: interface for the Physics_SymmetricMatrix class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PHYSICS_SYMMETRICMATRIX_H__07B77C16_F7DC_42CB_AFE9_16663FE2305B__INCLUDED_)
#define AFX_PHYSICS_SYMMETRICMATRIX_H__07B77C16_F7DC_42CB_AFE9_16663FE2305B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Physics_SymmetricMatrix  
{
public:
	int m_iRows;

	Physics_t *m_pData;

public:
	Physics_SymmetricMatrix( int iRows );
	virtual ~Physics_SymmetricMatrix();

	void Zero();

    Physics_t &	operator() (int i, int j);

	bool PostMultiply( Physics_LargeVector &V, Physics_LargeVector &Dst );
	bool PreMultiply( Physics_LargeVector &V, Physics_LargeVector &Dst );
	bool Add( Physics_SymmetricMatrix &M, Physics_SymmetricMatrix &Dst );
	bool Subtract( Physics_SymmetricMatrix &M, Physics_SymmetricMatrix &Dst );
	bool Scale( Physics_t scale, Physics_SymmetricMatrix &Dst );
	void Dump( char *szTitle = NULL );
	void Copy( Physics_SymmetricMatrix &M );
	void SetIdentity();
	bool Invert();
};

#endif // !defined(AFX_PHYSICS_SYMMETRICMATRIX_H__07B77C16_F7DC_42CB_AFE9_16663FE2305B__INCLUDED_)
