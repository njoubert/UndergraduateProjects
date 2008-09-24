/*
	File:			SubSMat.cpp

	Function:		Implements SubSMat.h

	Author(s):		Andrew Willmott

	Copyright:		(c) 1995-2000, Andrew Willmott

	Notes:			

*/


#include "vl\SubSMat.h"
#include "vl\SparseMat.h"
#include "vl\SubSVec.h"


// --- SubSMat Constructors & Destructors -------------------------------------


TSubSMat::TSubSMat(Int m, Int n, Int start, TMSparseVec *target) : 
	rows(m), cols(n), start(start), target(target)
{
}

TSubSMat::TSubSMat(const TSubSMat &m) :
	rows(m.rows), cols(m.cols), start(m.start), target(m.target)
{
}


// --- SubSMat Assignment Operators -------------------------------------------

TSubSMat &TSubSMat::operator = (const TSubSMat &m)
{	
	Int	i;
	
	for (i = 0; i < m.Rows(); i++) 
		SELF[i] = m[i];
		
	return(SELF);
}
	  
TSubSMat &TSubSMat::operator = (const TSparseMat &m)
{	
	Int	i;
	
	for (i = 0; i < m.Rows(); i++) 
		SELF[i] = m[i];
		
	return(SELF);
}

TSubSMat &TSubSMat::operator = (const TMat &m)
{	
	Int	i;
	
	for (i = 0; i < m.Rows(); i++) 
		SELF[i] = m[i];
		
	return(SELF);
}

TSubSMat sub(const TSparseMat &m, Int top, Int left, Int nrows, Int ncols)
{
	Assert(left >= 0 && ncols > 0 && left + ncols <= m.Cols(), 
		   "(sub(SparseMat)) illegal subset of matrix");
	Assert(top >= 0 && nrows > 0 && top + nrows <= m.Rows(), 
		   "(sub(SparseMat)) illegal subset of matrix");

	return(TSubSMat(nrows, ncols, left, m.Ref() + top));
}

TSubSMat sub(const TSparseMat &m, Int nrows, Int ncols)
{
	Assert(ncols > 0 && ncols <= m.Cols(), 
		   "(sub(SparseMat)) illegal subset of matrix");
	Assert(nrows > 0 && nrows <= m.Rows(), 
		   "(sub(SparseMat)) illegal subset of matrix");

	return(TSubSMat(nrows, ncols, 0, m.Ref()));
}

TMSubSVec col(const TSparseMat &m, Int i)
{
	CheckRange(i, 0, m.Cols(), "(col(SparseMat)) illegal column index");

	return(TSubSVec(i, m.Rows(), m.Ref(), m.Cols()));
}

TMSubSVec diag(const TSparseMat &m, Int diagNum)
{
	if (diagNum == 0)
		return(TSubSVec(0, Min(m.Rows(), m.Cols()), m.Ref(), m.Cols() + 1));
	else if (diagNum < 0)
		return(TSubSVec(0, Min(m.Rows() + diagNum, m.Cols()), 
						m.Ref() - diagNum, m.Cols() + 1));
	else
		return(TSubSVec(diagNum, Min(m.Cols() - diagNum, m.Rows()), m.Ref(), 
						m.Cols() + 1));
}

