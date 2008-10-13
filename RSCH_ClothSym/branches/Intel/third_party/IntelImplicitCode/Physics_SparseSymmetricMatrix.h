// Physics_SparseSymmetricMatrix.h: interface for the Physics_SparseSymmetricMatrix class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PHYSICS_SPARSESYMMETRICMATRIX_H__437FC57A_AF43_40F1_8889_7D0E36ECE7F3__INCLUDED_)
#define AFX_PHYSICS_SPARSESYMMETRICMATRIX_H__437FC57A_AF43_40F1_8889_7D0E36ECE7F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Physics_SparseSymmetricMatrix  
{
public:
	int m_iRows, m_iColumns;
	int m_iValues;
	int *m_pRows;
	int *m_pColumns;

	int m_dwCheckSum;

	Physics_Matrix3x3 *m_pData;

public:
	Physics_SparseSymmetricMatrix( int iRows, int iColumns );
	virtual ~Physics_SparseSymmetricMatrix();

	void Empty();
	void Zero();

    Physics_Matrix3x3 & operator() (int i, int j);

	bool PostMultiply( Physics_LargeVector &V, Physics_LargeVector &Dst );
	bool PreMultiply( Physics_LargeVector &V, Physics_LargeVector &Dst );
	bool Add( Physics_SparseSymmetricMatrix &M, Physics_SparseSymmetricMatrix &Dst );
	bool Subtract( Physics_SparseSymmetricMatrix &M, Physics_SparseSymmetricMatrix &Dst );
	bool Scale( Physics_t scale, Physics_SparseSymmetricMatrix &Dst );
	void Dump( char *szTitle = NULL );
	bool IsSymmetric();
	bool IsPositiveDefinite();
	void Copy( Physics_SparseSymmetricMatrix &M );
};

#endif // !defined(AFX_PHYSICS_SPARSESYMMETRICMATRIX_H__437FC57A_AF43_40F1_8889_7D0E36ECE7F3__INCLUDED_)
