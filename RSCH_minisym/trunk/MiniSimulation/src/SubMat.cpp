/*
	File:			SubMat.cpp

	Function:		Implements SubMat.h

	Author(s):		Andrew Willmott

	Copyright:		(c) 1995-2000, Andrew Willmott

	Notes:			

*/


#include "vl\SubMat.h"
#include "vl\Mat.h"


// --- SubMat Constructors & Destructors --------------------------------------


TSubMat::TSubMat(Int m, Int n, Int span, TMReal data[]) :
		rows(m), cols(n),
		span(span), data(data)
{
}

TSubMat::TSubMat(const TSubMat &m) :
		rows(m.rows), cols(m.cols),
		span(m.span), data(m.data)
{
}


// --- SubMat Assignment Operators --------------------------------------------


TSubMat &TSubMat::operator = (const TSubMat &m)
{	
	Assert(Rows() == m.Rows(), "(Mat::=) Matrix rows don't match");
	for (Int i = 0; i < Rows(); i++) 
		SELF[i] = m[i];

    return(SELF);
}
	  
TSubMat &TSubMat::operator = (const TMat &m)
{
	Assert(Rows() == m.Rows(), "(Mat::=) Matrix rows don't match");
	for (Int i = 0; i < Rows(); i++) 
		SELF[i] = m[i];

    return(SELF);
}


// --- Sub functions: Mat ------------------------------------------------------


TSubMat sub(const TMat &m, Int top, Int left, Int height, Int width)
{
	Assert(left >= 0 && width > 0 && left + width <= m.Cols(), "(sub(Mat)) illegal subset of matrix");
	Assert(top >= 0 && height > 0 && top + height <= m.Rows(), "(sub(Mat)) illegal subset of matrix");

	TSubMat result(height, width, m.Cols(), m.Ref() + top * m.Cols() + left);

	return(result);
}

TSubMat sub(const TMat &m, Int nrows, Int ncols)
{
	Assert(ncols > 0 && nrows > 0 && nrows <= m.Rows() && ncols <= m.Cols(), 
		"(sub(Mat)) illegal subset of matrix");

	TSubMat result(nrows, ncols, m.Cols(), m.Ref());

	return(result);
}

TMSubVec col(const TMat &m, Int i)
{
	CheckRange(i, 0, m.Cols(), "(col(Mat)) illegal column index");

	return(TMSubVec(m.Rows(), m.Cols(), m.Ref() + i));
}

TMSubVec row(const TMat &m, Int i)
{
	CheckRange(i, 0, m.Rows(), "(row(Mat)) illegal row index");

	return(TMSubVec(m.Cols(), 1, m[i].Ref()));
}

TMSubVec diag(const TMat &m, Int diagNum)
{
	CheckRange(diagNum, 1 - m.Rows(), m.Cols(), "(row(Mat)) illegal row index");

	if (diagNum == 0)
		return(TMSubVec(Min(m.Rows(), m.Cols()), m.Cols() + 1, m.Ref()));
	else if (diagNum < 0)
		return(TMSubVec(Min(m.Rows() + diagNum, m.Cols()), m.Cols() + 1,
			m.Ref() - diagNum * m.Cols()));
	else
		return(TMSubVec(Min(m.Cols() - diagNum, m.Rows()), m.Cols() + 1,
			m.Ref() + diagNum));
}

// --- Sub functions: SubMat ---------------------------------------------------


TSubMat sub(const TSubMat &m, Int top, Int left, Int height, Int width)
{
	Assert(left >= 0 && width > 0 && left + width <= m.Cols(),
		"(sub(SubMat)) illegal subset of matrix");
	Assert(top >= 0 && height > 0 && top + height <= m.Rows(), 
		"(sub(SubMat)) illegal subset of matrix");

	TSubMat result(height, width, m.span, m.data + top * m.span + left);

	return(result);
}

TSubMat sub(const TSubMat &m, Int nrows, Int ncols)
{
	Assert(ncols > 0 && nrows > 0 && nrows <= m.Rows() && ncols <= m.Cols(), 
		"(sub(SubMat)) illegal subset of matrix");

	TSubMat result(nrows, ncols, m.span, m.data);

	return(result);
}

TMSubVec col(const TSubMat &m, Int i)
{
	CheckRange(i, 0, m.Cols(), "(col(SubMat)) illegal column index");

	return(TMSubVec(m.rows, m.span, m.data + i));
}

TMSubVec row(const TSubMat &m, Int i)
{
	CheckRange(i, 0, m.Rows(), "(row(SubMat)) illegal row index");

	return(TMSubVec(m.cols, 1, m.data + i * m.span));
}

TMSubVec diag(const TSubMat &m, Int diagNum)
{
	CheckRange(diagNum, 1 - m.Rows(), m.Cols(), "(row(Mat)) illegal row index");

	if (diagNum == 0)
		return(TMSubVec(Min(m.rows, m.cols), m.span + 1, m.data));
	else if (diagNum < 0)
		return(TMSubVec(Min(m.rows + diagNum, m.cols), m.span + 1,
			m.data - diagNum * m.span));
	else
		return(TMSubVec(Min(m.cols - diagNum, m.rows), m.span + 1,
			m.data + diagNum));
}
