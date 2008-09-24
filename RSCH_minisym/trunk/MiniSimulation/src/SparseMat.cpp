/*
	File:			SparseMat.cpp

	Function:		Implements SparseMat.h

	Author(s):		Andrew Willmott

	Copyright:		(c) 1995-2000, Andrew Willmott

	Notes:			

*/


#include "vl\SparseMat.h"
#include <iomanip.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include "cl\Array.h"


// --- SparseMat Constructors & Destructors -----------------------------------


TSparseMat::TSparseMat() : rows(0), cols(0), row(0)
{
}

TSparseMat::TSparseMat(Int nrows, Int ncols) : row(0)
{
	SetSize(nrows, ncols);
	MakeZero();	// Adds in sentinels.
}

TSparseMat::TSparseMat(Int rows, Int cols, ZeroOrOne k) : row(0)
{
	SetSize(rows, cols);
	MakeDiag(k);
}

TSparseMat::TSparseMat(Int rows, Int cols, Block k) : row(0)
{
	SetSize(rows, cols);
	MakeBlock(k);
}

TSparseMat::TSparseMat(const TSparseMat &m) : row(0)
{
	Assert(m.row != 0, "(SparseMat) Can't construct from null matrix");
	
	Int i;
	
	SetSize(m.rows, m.cols);
	
	for (i = 0; i < rows; i++)
		row[i] = m.row[i];
}

TSparseMat::TSparseMat(const TSubSMat &m) : row(0)
{
	SELF = m;
}

TSparseMat::TSparseMat(const TMat &m) : row(0)
{
	Int i;
	
	SetSize(m.Rows(), m.Cols());

	for (i = 0; i < rows; i++)
		row[i] = m[i];
}

TSparseMat::~TSparseMat()
{
	delete[] row;
}

Void TSparseMat::SetSize(Int m, Int n)
{
	Int i;

	Assert(m > 0 && n > 0, "(SparseMat::SetSize) illegal matrix size");

	rows = m;
	cols = n;
	
	delete[] row;	
	row = new TMSparseVec[rows];
	
	for (i=0; i < rows; i++)
		row[i].SetSize(cols);
}


// --- SparseMat Assignment Operators -----------------------------------------


TSparseMat &TSparseMat::operator = (const TSparseMat &m)
{	
	Int i;
	
	if (rows == 0)
		SetSize(m.Rows(), m.Cols());
	else
		Assert(rows == m.Rows() && cols == m.Cols(),
			 "(SparseMat::=) matrices not the same size");
		
	for (i = 0; i < rows; i++)
		row[i] = m.row[i];
	
	return(SELF);
}
	  
TSparseMat &TSparseMat::operator = (const TMat &m)
{	
	Int i;
	
	SetSize(m.Rows(), m.Cols());

	for (i = 0; i < rows; i++)
		row[i] = m[i];

	return(SELF);
}

TSparseMat &TSparseMat::operator = (const TSubSMat &m)
{	
	Int i;
	
	SetSize(m.Rows(), m.Cols());

	for (i = 0; i < rows; i++)
		row[i] = m[i];

	return(SELF);
}

Void TSparseMat::MakeZero()
{
	Int		i;
	
	for (i = 0; i < rows; i++)
		row[i] = vl_zero;		
}

Void TSparseMat::MakeDiag(TMReal k)
{
	Int		i;
	
	for (i = 0; i < rows; i++)
		row[i].MakeUnit(i, k);		
}

Void TSparseMat::MakeBlock(TMReal k)
{
	Int		i;
	
	for (i = 0; i < rows; i++)
		row[i].MakeBlock(k);
}


// --- Mat Assignment Operators -----------------------------------------------


TSparseMat &operator += (TSparseMat &m, const TSparseMat &n)
{
	Assert(n.Rows() == m.Rows(), "(SparseMat::+=) matrix rows don't match");	
	
	Int		i;
	
	for (i = 0; i < m.Rows(); i++) 
		m[i] += n[i];
	
	return(m);
}

TSparseMat &operator -= (TSparseMat &m, const TSparseMat &n)
{
	Assert(n.Rows() == m.Rows(), "(SparseMat::-=) matrix rows don't match");	
	
	Int		i;
	
	for (i = 0; i < m.Rows(); i++) 
		m[i] -= n[i];
	
	return(m);
}

TSparseMat &operator *= (TSparseMat &m, const TSparseMat &n)
{
	Assert(m.Cols() == n.Cols(), "(SparseMat::*=) matrix columns don't match");	
	
	Int		i;
	
	for (i = 0; i < m.Rows(); i++) 
		m[i] *= (TSparseMat &) n;
	
	return(m);
}

TSparseMat &operator *= (TSparseMat &m, TMReal s)
{	
	Int		i;
	
	for (i = 0; i < m.Rows(); i++) 
		m[i] *= s;
	
	return(m);
}

TSparseMat &operator /= (TSparseMat &m, TMReal s)
{	
	Int		i;
	
	for (i = 0; i < m.Rows(); i++) 
		m[i] /= s;
	
	return(m);
}

// --- Mat Comparison Operators -----------------------------------------------

Bool operator == (const TSparseMat &m, const TSparseMat &n)
{
	Assert(n.Rows() == m.Rows(), "(SparseMat::==) matrix rows don't match");	
	
	Int		i;
	
	for (i = 0; i < m.Rows(); i++) 
		if (m[i] != n[i])
			return(0);

	return(1);
}

Bool operator != (const TSparseMat &m, const TSparseMat &n)
{
	Assert(n.Rows() == m.Rows(), "(SparseMat::!=) matrix rows don't match");	
	
	Int		i;
	
	for (i = 0; i < m.Rows(); i++) 
		if (m[i] != n[i])
			return(1);

	return(0);
}

// --- Mat Arithmetic Operators -----------------------------------------------

TSparseMat operator + (const TSparseMat &m, const TSparseMat &n)
{
	Assert(n.Rows() == m.Rows(), "(SparseMat::+) matrix rows don't match");	
	
	TSparseMat	result(m.Rows(), m.Cols());
	Int		i;
	
	for (i = 0; i < m.Rows(); i++) 
		result[i] = m[i] + n[i];
	
	return(result);
}

TSparseMat operator - (const TSparseMat &m, const TSparseMat &n)
{
	Assert(n.Rows() == m.Rows(), "(SparseMat::-) matrix rows don't match");	
	
	TSparseMat	result(m.Rows(), m.Cols());
	Int		i;
	
	for (i = 0; i < m.Rows(); i++) 
		result[i] = m[i] - n[i];
	
	return(result);
}

TSparseMat operator - (const TSparseMat &m)
{
	TSparseMat	result(m.Rows(), m.Cols());
	Int		i;
	
	for (i = 0; i < m.Rows(); i++) 
		result[i] = -m[i];
	
	return(result);
}

TSparseMat operator * (const TSparseMat &m, const TSparseMat &n)
{
	Assert(m.Cols() == n.Rows(), "(SparseMat::*m) matrix cols don't match");	
	
	TSparseMat	result(m.Rows(), n.Cols());
	Int		i;
	
	for (i = 0; i < m.Rows(); i++) 
		result[i] = m[i] * n;
	
	return(result);
}

TSparseVec operator * (const TSparseMat &m, const TSparseVec &v)
{
	Assert(m.Cols() == v.Elts(), "(SparseMat::m*v) matrix and vector sizes don't match");
	
	Int			i;
	TSparseVec	result(m.Rows());
	
	for (i = 0; i < m.Rows(); i++) 
		result.AddElt(i, dot(m[i], v));
	
	result.End();
		
	return(result);
}

TMVec operator * (const TSparseMat &m, const TMVec &v)
{
	Assert(m.Cols() == v.Elts(), "(SparseMat::*v) matrix and vector sizes don't match");
	
	Int			i;
	TMVec		result(m.Rows());
	
	for (i = 0; i < m.Rows(); i++) 
		result[i] = dot(m[i], v);
	
	return(result);
}

TSparseMat operator * (const TSparseMat &m, TMReal s)
{
	Int		i;
	TSparseMat	result(m.Rows(), m.Cols());
	
	for (i = 0; i < m.Rows(); i++) 
		result[i] = m[i] * s;
	
	return(result);
}

TSparseMat operator / (const TSparseMat &m, TMReal s)
{
	Int		i;
	TSparseMat	result(m.Rows(), m.Cols());
	
	for (i = 0; i < m.Rows(); i++) 
		result[i] = m[i] / s;
	
	return(result);
}


// --- Mat-Vec Functions ------------------------------------------------------


TMSparseVec operator * (const TSparseVec &v, const TSparseMat &m)			// v * m
{
	Assert(v.Elts() == m.Rows(), "(Mat::v*m) vector/matrix sizes don't match");
	
	TMSparseVec result(m.Cols());
	Int			i;
	TSVIter 	j(v);

	result = vl_zero;	
	
	// v * M = v[0] * m[0] + v[1] * m[1] + ...
	
	for (i = 0; i < m.Rows(); i++) 
	{		
		j.Inc(i);
		if (j.Exists(i))
			result += m[i] * j.Data();
	}
	
	return(result);
}

TMVec operator * (const TMVec &v, const TSparseMat &m)			// v * m
{
	Assert(v.Elts() == m.Rows(), "(Mat::v*m) vector/matrix sizes don't match");
	
	TMVec	 	result(m.Cols());
	Int			i, j;

	result = vl_zero;	

	// v * M = v[0] * m[0] + v[1] * m[1] + ...
	
	for (i = 0; i < m.Rows(); i++) 
		if (len(v[i]) > 0)
			for (j = 0; j < m[i].NumItems() - 1; j++)
				result[m[i][j].index] += v[i] * m[i][j].elt;
	
	return(result);
}

TSparseVec &operator *= (TSparseVec &v, const TSparseMat &m)		// v *= m
{
	TSparseVec t = v * m;
	v.Replace(t);
	return(v);
}

TMVec &operator *= (TMVec &v, const TSparseMat &m)					// v *= m
{
	v = v * m;
	return(v);
}


// --- Mat Special Functions --------------------------------------------------


TSparseMat trans(const TSparseMat &m)
{
	Int			i, j;
	TSparseMat	result(m.Cols(), m.Rows());
		
	for (i = 0; i < result.Rows(); i++)
		result[i].Begin();

	for (i = 0; i < m.Rows(); i++)
	{
		// For row i of 'm', add its elements to the ends of the
		// appropriate row of 'result'.
	
		for (j = 0; j < m[i].NumItems() - 1; j++)
			result[m[i][j].index].AddNZElt(i, m[i][j].elt);
	}
	
	for (i = 0; i < result.Rows(); i++)
		result[i].End();
			
	return(result);
}

TMReal trace(const TSparseMat &m)
{
	Int		i;
	TSVIter	j;
	TMReal 	result = vl_0;
	
	for (i = 0; i < m.Rows(); i++) 
	{
		j.Begin(m[i]);
		
		// Find element i of m[i], and if it exists,
		// add it to the result.
		
		if (j.IncTo(i))
			result += j.Data();
	}
	
	return(result);
}

TSparseMat oprod(const TSparseVec &a, const TSparseVec &b)
// returns outerproduct of a and b:  a * trans(b)
{
	TSparseMat	result;
	TSVIter		i;
	
	result.SetSize(a.Elts(), b.Elts());
	result = vl_0;
		
	for (i.Begin(a); !i.AtEnd(); i.Inc())
	{
		result[i.Index()] = b;
		result[i.Index()] *= i.Data();
	}
	
	return(result);
}

TSparseMat oprods(const TVec &a, const TVec &b)
// returns outerproduct of a and b:  a * trans(b)
{
	TSparseMat	result;
	Int			i;

	result.SetSize(a.Elts(), b.Elts());
	
	for (i = 0; i < a.Elts(); i++)
		if (TSparseVec::IsNonZero(a[i]))
		{
			result[i] = b;
			result[i] *= a[i];
		}
		else
			result[i] = vl_0;

	return(result);
}


// --- Mat Input & Output -----------------------------------------------------


ostream	&operator << (ostream &s, const TSparseMat &m)
{
	Int i, w = s.width();

	s << '[';
	for (i = 0; i < m.Rows() - 1; i++)
		s << setw(w) << m[i] << endl;
	s << setw(w) << m[i] << ']' << endl;
	
	return(s);
}

istream	&operator >> (istream &s, TSparseMat &m)
{
	Array< TMSparseVec > array;	
    Int		i;
	
	s >> array;						// Read input into array of SparseVecs
	
	m.SetSize(array.NumItems(), array[0].NumItems());
	
	for (i = 0; i < m.Rows(); i++)	// copy the result into m
	{
		Assert(m.Cols() == array[i].NumItems(), "(Mat/>>) different sized matrix rows");
		m[i] = array[i];
	}
	
    return(s);
}

TSparseMat inv(const TSparseMat &m, TMReal *determinant, TMReal pEps)
{
	Assert(m.IsSquare(), "(inv) Matrix not square"); 

	//	Note that the sparse version of inv() is actually simpler than
	//	the normal version.

    Int				i, j, k;
    Int				n = m.Rows();
    TMReal			t, det, pivot;
    Real			max;
    TSparseMat		A(m);
    TSparseMat		B(n, n, vl_I);		

    // ---------- Forward elimination ---------- ------------------------------
	
    det = vl_1;
    
    for (i = 0; i < n; i++)		
    {		    
		// Eliminate in column i, below diagonal
		
		max = -1.0;
		
		// Find a pivot for column i 
		// For the sparse rows, we take advantage of the fact that if A(i, k) exists,
		// it will be the first element in the sparse vector. (Previous elements will have
		// been zeroed by previous elimination steps.)
		
		for (k = i; k < n; k++)	
		    if ((A[k][0].index == i) && (len(A[k][0].elt) > max))
		    {
				pivot = A[k][0].elt;
				max = len(pivot);
				j = k;
		    }
		    
		// If no nonzero pivot exists, A must be singular...

		Assert(max > pEps, "(inv) Matrix is singular");
			
		// Swap rows i and j

		if (j != i)
		{	
			//	Sparse matrix rows are just arrays: we can swap the contents
			//	of the two arrays efficiently by using the array Swap function.

			A[i].SwapWith(A[j]);
			B[i].SwapWith(B[j]);
				
		    det = -det;
		}
		
		det *= pivot;
		
		A[i] /= pivot;
		B[i] /= pivot;    
		   
		for (j = i + 1; j < n; j++)
		{
			// Eliminate in rows below i 
			// Again, if A[j,i] exists, it will be the first non-zero element of the row.
			
			if (A[j][0].index == i)
			{
				t = A[j][0].elt;
				A[j] -= A[i] * t;
				B[j] -= B[i] * t;
			}
		}
    }

    // ---------- Backward elimination ---------- -----------------------------

    for (i = 1; i < n; i++)			// Eliminate in column i, above diag 
    {		
		for (j = 0; j < i; j++)			// Eliminate in rows above i 
		{		
			if (A[j][1].index == i)
			{
				t = A[j][1].elt;
				A[j] -= A[i] * t;
				B[j] -= B[i] * t;
			}
		}
    }
	if (determinant)
		*determinant = det;
    return(B);
}

