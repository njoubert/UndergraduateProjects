/*
	File:			Mixed.cpp
	
	Function:		Implements Mixed.h

	Author(s):		Andrew Willmott

	Copyright:		(c) 1997-2000, Andrew Willmott
*/

#include "vl\Mixed.h"


// --- Vector dot products ----------------------------------------------------


TMReal dot(const TMVec &a, const TVec &b) 
{
	Assert(a.Elts() == b.Elts(), "(Vec::dot) Vec sizes don't match");

	TMReal	sum = vl_zero;
	Int		i;
		
	for (i = 0; i < a.Elts(); i++) 
		sum += a[i] * b[i];
	
	return(sum);
}


#ifdef __SparseVec__

TMReal dot(const TMSparseVec &a, const TSparseVec &b) 
{
	Assert(a.Elts() == b.Elts(), "(SparseVec::dot) Vec sizes don't match");

	TMReal		sum = vl_zero;
	TMSVIter	j(a);
	Int			i;
	
	for (i = 0; i < b.NumItems() - 1; i++)
	{
		if (j.IncTo(b[i].index))
			sum += j.Data() * b[i].elt;
	}
	
	return(sum);
}

TMReal dot(const TMSparseVec &a, const TVec &b)
{
	Assert(a.Elts() == b.Elts(), "(SparseVec::dot) Vec sizes don't match");

	TMReal		sum = vl_zero;
	Int			i;
	
	for (i = 0; i < a.NumItems() - 1; i++)
		sum += a[i].elt * b[a[i].index];
	
	return(sum);
}

#endif


// --- Matrix-vector multiply -------------------------------------------------


TVec4 operator * (const TMat4 &m, const TVec4 &v)			// m * v
{
	TVec4 result;
	
	result[0] = v[0] * m[0][0] + v[1] * m[0][1] + v[2] * m[0][2] + v[3] * m[0][3];
	result[1] = v[0] * m[1][0] + v[1] * m[1][1] + v[2] * m[1][2] + v[3] * m[1][3];
	result[2] = v[0] * m[2][0] + v[1] * m[2][1] + v[2] * m[2][2] + v[3] * m[2][3];
	result[3] = v[0] * m[3][0] + v[1] * m[3][1] + v[2] * m[3][2] + v[3] * m[3][3];
	
	return(result);
}

TVec4 operator * (const TVec4 &v, const TMat4 &m)			// v * m
{
	TVec4 result;
	
	result[0] = v[0] * m[0][0] + v[1] * m[1][0] + v[2] * m[2][0] + v[3] * m[3][0];
	result[1] = v[0] * m[0][1] + v[1] * m[1][1] + v[2] * m[2][1] + v[3] * m[3][1];
	result[2] = v[0] * m[0][2] + v[1] * m[1][2] + v[2] * m[2][2] + v[3] * m[3][2];
	result[3] = v[0] * m[0][3] + v[1] * m[1][3] + v[2] * m[2][3] + v[3] * m[3][3];
	
	return(result);
}

TVec4 &operator *= (TVec4 &v, const TMat4 &m)				// v *= m
{
	TVReal	t0, t1, t2;

	t0   = v[0] * m[0][0] + v[1] * m[1][0] + v[2] * m[2][0] + v[3] * m[3][0];
	t1   = v[0] * m[0][1] + v[1] * m[1][1] + v[2] * m[2][1] + v[3] * m[3][1];
	t2   = v[0] * m[0][2] + v[1] * m[1][2] + v[2] * m[2][2] + v[3] * m[3][2];
	v[3] = v[0] * m[0][3] + v[1] * m[1][3] + v[2] * m[2][3] + v[3] * m[3][3];
	v[0] = t0;
	v[1] = t1;
	v[2] = t2;
	
	return(v);
}

TVec operator * (const TMat &m, const TVec &v)
{
	Assert(m.Cols() == v.Elts(), "(Mat::*v) matrix and vector sizes don't match");
	
	Int		i;
	TVec	result(m.Rows());
	
	for (i = 0; i < m.Rows(); i++) 
		result[i] = dot(m[i], v);
	
	return(result);
}

TVec operator * (const TVec &v, const TMat &m)			// v * m
{
	Assert(v.Elts() == m.Rows(), "(Mat::v*m) vector/matrix sizes don't match");
	
	TMVec 	temp(m.Cols(), vl_zero);	// accumulate in high precision
	TVec 	result(m.Cols());			// return low precision.
	Int		i;
	
	for (i = 0; i < m.Rows(); i++) 
		temp += m[i] * v[i];
	
	for (i = 0; i < temp.Elts(); i++)
		result[i] = temp[i];
		
	return(result);
}

TVec &operator *= (TVec &v, const TMat &m)				// v *= m
{
	v = v * m;		// Can't optimise much here...
	
	return(v);
}

#ifdef __SparseVec__

TSparseVec operator * (const TSparseMat &m, const TSparseVec &v)
{
	Assert(m.Cols() == v.Elts(), "(SparseMat::m*v) matrix and vector sizes don't match");
	
	Int			i;
	TSparseVec	result(m.Rows());
	
	result.Begin();

	for (i = 0; i < m.Rows(); i++) 
		result.AddElt(i, dot(m[i], v));
	
	result.End();
		
	return(result);
}

TSparseVec operator * (const TSparseVec &v, const TSparseMat &m)			// v * m
{
	Assert(v.Elts() == m.Rows(), "(Mat::v*m) vector/matrix sizes don't match");
	
	TMSparseVec	temp(m.Cols());
	TSparseVec	result(m.Cols());
	Int			i;
	TSVIter 	j(v);
	TMSVIter	k;
	
	temp = vl_zero;	
	
	// v * M = v[0] * m[0] + v[1] * m[1] + ...
	
	for (i = 0; i < m.Rows(); i++) 
	{		
		j.Inc(i);
		if (j.Exists(i))
			temp += m[i] * j.Data();
	}
	
	result.SetNumElts(temp.Elts());
	result.Begin();
		
	for (k.Begin(temp.Elts()); !k.AtEnd(); k.Inc())
		result.AddNZElt(k.Index(), k.Data());

	result.End();

	return(result);
}

TSparseVec &operator *= (TSparseVec &v, const TSparseMat &m)		// v *= m
{
	TSparseVec t = v * m;
	v.Replace(t);
	return(v);
}

TVec operator * (const TSparseMat &m, const TVec &v)
{
	Assert(m.Cols() == v.Elts(), "(SparseMat::*v) matrix and vector sizes don't match");
	
	Int			i;
	TVec		result(m.Rows());
	
	for (i = 0; i < m.Rows(); i++) 
		result[i] = dot(m[i], v);
	
	return(result);
}

TVec operator * (const TVec &v, const TSparseMat &m)			// v * m
{
	Assert(v.Elts() == m.Rows(), "(Mat::v*m) vector/matrix sizes don't match");
	
	TVec	 	result(m.Cols());
	Int			i, j;

	result = vl_zero;	

	// v * M = v[0] * m[0] + v[1] * m[1] + ...
	
	for (i = 0; i < m.Rows(); i++) 
		if (len(v[i]) > 0)
			for (j = 0; j < m[i].NumItems() - 1; j++)
				result[m[i][j].index] += v[i] * m[i][j].elt;
	
	return(result);
}

TVec &operator *= (TVec &v, const TSparseMat &m)					// v *= m
{
	v = v * m;
	return(v);
}

#endif
