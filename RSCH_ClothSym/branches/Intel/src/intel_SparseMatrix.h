/*
 * SparseMatrix.h
 *
 *  Created on: Oct 7, 2008
 *      Author: silversatin
 */

#ifndef SPARSEMATRIX_H_
#define SPARSEMATRIX_H_

// Physics_SparseSymmetricMatrix.h: interface for the Physics_SparseSymmetricMatrix class.
//
//////////////////////////////////////////////////////////////////////
/*
#if !defined(AFX_PHYSICS_SPARSESYMMETRICMATRIX_H__437FC57A_AF43_40F1_8889_7D0E36ECE7F3__INCLUDED_)
#define AFX_PHYSICS_SPARSESYMMETRICMATRIX_H__437FC57A_AF43_40F1_8889_7D0E36ECE7F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//*/

#include "global.h"
#include "intel_LargeVector.h"

class SparseMatrix
{
public:
	int m_iRows, m_iColumns;
	int m_iValues;
	int *m_pRows;
	int *m_pColumns;

	int m_dwCheckSum;

	Physics_Matrix3x3 *m_pData;

public:
	SparseMatrix( int iRows, int iColumns );
	virtual ~SparseMatrix();

	void Empty();
	void Zero();

    Physics_Matrix3x3 & operator() (int i, int j);

	bool PostMultiply( Physics_LargeVector &V, Physics_LargeVector &Dst );
	bool PreMultiply( Physics_LargeVector &V, Physics_LargeVector &Dst );
	bool Add( SparseMatrix &M, SparseMatrix &Dst );
	bool Subtract( SparseMatrix &M, SparseMatrix &Dst );
	bool Scale( double scale, SparseMatrix &Dst );
	void Dump( char *szTitle = NULL );
	bool IsSymmetric();
	bool IsPositiveDefinite();
	void Copy( SparseMatrix &M );
};

//#endif // !defined(AFX_SparseMatrix_H__437FC57A_AF43_40F1_8889_7D0E36ECE7F3__INCLUDED_)

#endif /* SPARSEMATRIX_H_ */

