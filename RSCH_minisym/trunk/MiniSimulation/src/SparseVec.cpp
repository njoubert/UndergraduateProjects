/*
	File:			SparseVec.cpp

	Function:		Implements SparseVec.h

	Author(s):		Andrew Willmott

	Copyright:		(c) 1995-2000, Andrew Willmott

	Notes:			

*/


#include "cl\Array.cpp"
#include "vl\SparseVec.h"
#include <stdarg.h>
#include <iomanip.h>
#include "vl\SparseVec.h"
#include "vl\SubSVec.h"


TSparseVec::TSparseVec() : TSVList(), elts(0)
{
	End();
}

TSparseVec::TSparseVec(Int n) : TSVList(), elts(n)
{
	Assert(n > 0,"(SparseVec) illegal vector size");
}

TSparseVec::TSparseVec(const TSparseVec &v) : TSVList(v), elts(v.elts)
{
}

TSparseVec::TSparseVec(const TSubSVec &v) : TSVList()
{
	SELF = v;
}

TSparseVec::TSparseVec(const TVec &v) : TSVList()
{
	SELF = v;
}

TSparseVec::TSparseVec(Int n, Int indices[], TVReal elts[]) : TSVList(), elts(n)
{
	Assert(n > 0,"(SparseVec) illegal vector size");
	Int	i = 0;
	
	while (1)
	{
		if (indices[i] < 0)
			break;
		AddElt(indices[i], elts[i]);
		i++;
	}
	
	End();
}

TSparseVec::TSparseVec(Int n, Int idx0, double elt0, ...) : TSVList(), elts(n)
{
	Assert(n > 0,"(SparseVec) illegal vector size");
	va_list		ap;
	Int 		idx;
	TVReal		elt;
	
	va_start(ap, elt0);
	Assert(idx0 >= 0 && idx0 < elts, "(SparseVec) illegal first index");
	SetReal(elt, elt0);
	AddElt(idx0, elt);
		
	while (1)
	{
		idx = va_arg(ap, Int);
		if (idx < 0)
			break;
		Assert(idx < elts, "(SparseVec) illegal index");
		SetReal(elt, va_arg(ap, double));
		AddElt(idx, elt);
	}
	
	End();
	va_end(ap);
}

TSparseVec::TSparseVec(Int n, ZeroOrOne k) : elts(n)
{
	Assert(n > 0,"(SparseVec) illegal vector size");
	MakeBlock(k);
}

TSparseVec::TSparseVec(Int n, Axis a) : elts(n)
{
	Assert(n > 0,"(SparseVec) illegal vector size");
	MakeUnit(a);
}


TSparseVec::~TSparseVec()
{
}

TSparseVec &TSparseVec::operator = (const TSparseVec &v)
{
	Int i;
	
	SetNumElts(v.Elts());
	Begin();
	
	for (i = 0; i < v.NumItems() - 1; i++)
		Append(v[i]);
	
	End();
	
	return(SELF);
}

TSparseVec &TSparseVec::operator = (const TVec &v)
{
	Int i;

	SetNumElts(v.Elts());
	Begin();
	
	for (i = 0; i < v.Elts(); i++)	
		AddElt(i, v[i]);
	
	End();
	
	return(SELF);
}

TSparseVec &TSparseVec::operator = (const TSubSVec &v)
{
	v.Store(SELF);
	return(SELF);
}

Void TSparseVec::SetSize(Int n)
{
	SetNumElts(n);
	Begin();	
	End();
}

Void TSparseVec::MakeZero()
{
	Begin();
	End();
}

Void TSparseVec::MakeUnit(Int i, TVReal k)
{
	Begin();
	AddElt(i, k);
	End();
}

Void TSparseVec::MakeBlock(TVReal k)
{
	if (len(k) == 0.0)
		MakeZero();
	else
		_Error("(SparseVec::MakeBlock) Calling this method is a really dumb idea.");
	// What's the point of setting every member of a sparse vector to
	// something non-zero? 
}


Bool TSparseVec::compactPrint = false;
TVReal TSparseVec::fuzz = 1e-10;

Void TSparseVec::SetCompactPrint(Bool on)
{ compactPrint = on; }

Void TSparseVec::SetFuzz(TVReal theFuzz)
{ fuzz = theFuzz; }


// --- Vector operations ------------------------------------------------------


#include <ctype.h>

/*	[Note]

	When operating on sparse vectors in place, it is faster to create a 
	new vector on the fly, and swap it in at the end, than to perform
	inserts/deletes on the original. (O(n) vs O(n^2).)
	
	Supporting Analysis:
	
	There will be O(n) new/deleted elts. 
	Copying will involve O(n) copies in creating the new array, plus 
	a temporary memory overhead the size of the old vector. Operating
	in place will require O(n) insert/deletes, each of which require
	O(n) copies, and thus requires O(n^2) copies overall.
*/

TSparseVec &operator += (TSparseVec &a, const TSparseVec &b)
{
	TSparseVec	t = a + b;	
	a.Replace(t);
	return(a);
}

TSparseVec &operator -= (TSparseVec &a, const TSparseVec &b)
{
	TSparseVec	t = a - b;
	a.Replace(t);
	return(a);
}

TSparseVec &operator *= (TSparseVec &a, const TSparseVec &b)
{
	TSparseVec	t = a * b;
	a.Replace(t);
	return(a);
}

TSparseVec &operator *= (TSparseVec &v, TVReal s)
{
	TSparseVec	t = v * s;
	v.Replace(t);
	return(v);
}

TSparseVec &operator /= (TSparseVec &a, const TSparseVec &b)
{
	TSparseVec	t = a / b;
	a.Replace(t);
	return(a);
}

TSparseVec &operator /= (TSparseVec &v, TVReal s)
{
	TSparseVec t = v / s;
	v.Replace(t);
	return(v);
}


// --- Vec Comparison Operators -----------------------------------------------


Bool operator == (const TSparseVec &a, const TSparseVec &b)
{
	Assert(a.Elts() == b.Elts(), "(SparseVec::==) Vec sizes don't match");

	if (a.NumItems() != b.NumItems())
		return(false);
	else
	{
		Int i;
		
		for (i = 0; i < a.NumItems(); i++)
			if (a[i] != b[i])
				return(false);
		
		return(true);
	}
}

Bool operator != (const TSparseVec &a, const TSparseVec &b)
{
	return(!(a == b));
}


// --- SparseVec Methods ------------------------------------------------------


TSparseVec TSparseVec::Overlay(const TSparseVec &b) const
{
	TSparseVec	result(Elts());
	Int			i, j;

	for (i = 0, j = 0; ; )
		if (item[i].index == b[j].index)
		{
			if (item[i].index == VL_SV_MAX_INDEX)
				break;
			
			result.AddNZElt(b[j]);
			i++;
			j++;
		}
		else if (item[i].index < b[j].index)
		{	result.AddNZElt(item[i]); i++;	}
		else
		{	result.AddNZElt(b[j]); j++;	}
			
	result.End();	
	return(result);
}

Void TSparseVec::SetElts(Int idx0, double elt0, ...)
{
	va_list		ap;
	Int 		idx;
	TVReal		elt;
	
	va_start(ap, elt0);
	Begin();
	Assert(idx0 >= 0 && idx0 < elts, "(SparseVec::SetElts) illegal first index");
	SetReal(elt, elt0);
	AddElt(idx0, elt);
		
	while (1)
	{
		idx = va_arg(ap, Int);
		if (idx < 0)
			break;
		Assert(idx < elts, "(SparseVec::SetElts) illegal index");
		SetReal(elt, va_arg(ap, double));
		AddElt(idx, elt);
	}
	
	End();
	va_end(ap);
}

Void TSparseVec::Set(Int index, TVReal elt)
{
	TSVIter		j(SELF);
	
	if (len(elt) <= fuzz)
		return;
					
	if (!j.IncTo(index))
	{
		Insert(j.PairIdx());
		Item(j.PairIdx()).index = index;
	}

	Item(j.PairIdx()).elt = elt;
}

TVReal TSparseVec::Get(Int index) const
{
	TSVIter		j(SELF);
	
	if (j.IncTo(index))
		return(j.Data());
	else
		return(vl_zero);
}

// --- Vec Arithmetic Operators -----------------------------------------------

TSparseVec operator + (const TSparseVec &a, const TSparseVec &b)
{
	Assert(a.Elts() == b.Elts(), "(SparseVec::+) Vec sizes don't match");

	TSparseVec	result(a.Elts());
	Int			i, j;

	// Step through a and b in parallel

	for (i = 0, j = 0; ; )
		if (a[i].index == b[j].index)
		{
			// We have two elements at the same index. 
			// Are we at the end of both arrays?
			if (a[i].index == VL_SV_MAX_INDEX)
				break;
			
			// If not, add the result
			
			result.AddElt(a[i].index, a[i].elt + b[j].elt);	// +
			i++;
			j++;
		}
		else if (a[i].index < b[j].index)
		// result[x] = a[i] + 0		
		{	result.AddNZElt(a[i]); i++;	}
		else
		// result[x] = b[j] + 0		
		{	result.AddNZElt(b[j]); j++;	}
			
	result.End();	
	return(result);
}

TSparseVec operator - (const TSparseVec &a, const TSparseVec &b) 
{
	Assert(a.Elts() == b.Elts(), "(SparseVec::-) Vec sizes don't match");

	TSparseVec	result(a.Elts());
	Int			i, j;

	for (i = 0, j = 0; ; )
		if (a[i].index == b[j].index)
		{
			if (a[i].index == VL_SV_MAX_INDEX)
				break;
			
			result.AddElt(a[i].index, a[i].elt - b[j].elt); // -
			i++;
			j++;
		}
		else if (a[i].index < b[j].index)
		{	result.AddNZElt(a[i]); i++;	}
		else
		{	result.AddNZElt(b[j].index, -b[j].elt); j++;	}
	
	result.End();	
	return(result);
}

TSparseVec operator - (const TSparseVec &v)
{
	TSparseVec	result(v.Elts());
	Int			i;
	
	for (i = 0; i < v.NumItems() - 1; i++)
		result.AddNZElt(v[i].index, -v[i].elt);
	
	result.End();	
	return(result);
}

TSparseVec operator * (const TSparseVec &a, const TSparseVec &b)			
{
	Assert(a.Elts() == b.Elts(), "(SparseVec::*) Vec sizes don't match");

	TSparseVec	result(a.Elts());
	TSVIter		j(a);
	Int			i;
	
	for (i = 0; i < b.NumItems() - 1; i++)
		if (j.IncTo(b[i].index))
			result.AddElt(b[i].index, j.Data() * b[i].elt);
	
	result.End();	
	return(result);
}

TSparseVec operator * (const TSparseVec &v, TVReal s) 
{
	TSparseVec	result(v.Elts());
	Int			i;
		
	for (i = 0; i < v.NumItems() - 1; i++)
		result.AddElt(v[i].index, s * v[i].elt);
	
	result.End();	
	return(result);
}

TSparseVec operator / (const TSparseVec &a, const TSparseVec &b)			
{
	Assert(a.Elts() == b.Elts(), "(SparseVec::/) Vec sizes don't match");
	
	TSparseVec	result(a.Elts());
	TSVIter		j(a);
	Int			i;
	
	for (i = 0; i < b.NumItems() - 1; i++)
		if (j.IncTo(b[i].index))
			result.AddElt(b[i].index, j.Data() / b[i].elt);
	
	result.End();	
	return(result);
}

TSparseVec operator / (const TSparseVec &v, TVReal s) 
{
	TSparseVec	result(v.Elts());
	Int			i;
	TVReal		t = vl_1 / s;
			
	for (i = 0; i < v.NumItems() - 1; i++)
		result.AddElt(v[i].index, v[i].elt * t);
	
	result.End();	
	return(result);
}

TVReal dot(const TSparseVec &a, const TSparseVec &b) 
{
	Assert(a.Elts() == b.Elts(), "(SparseVec::dot) Vec sizes don't match");

	TMReal		sum = vl_zero;
	TSVIter		j(a);
	Int			i;
	
	for (i = 0; i < b.NumItems() - 1; i++)
		if (j.IncTo(b[i].index))
			sum += j.Data() * b[i].elt;
	
	return(sum);
}

TVReal dot(const TSparseVec &a, const TVec &b)
{
	Assert(a.Elts() == b.Elts(), "(SparseVec::dot) Vec sizes don't match");

	TMReal		sum = vl_zero;
	Int			i;
	
	for (i = 0; i < a.NumItems() - 1; i++)
		sum += a[i].elt * b[a[i].index];
	
	return(sum);
}

TSparseVec operator * (TVReal s, const TSparseVec &v)
{
	return(v * s);
}

TVReal sqrlen(const TSparseVec &v)
{
	TVReal		sum = vl_zero;
	Int			i;
	
	for (i = 0; i < v.NumItems() - 1; i++)
		sum += sqrlen(v[i].elt);
	
	return(sum);
}

TVReal len(const TSparseVec &v)
{
	return(sqrt(sqrlen(v)));
}

TSparseVec norm(const TSparseVec &v)	
{
	return(v / len(v));
}


// --- Vec Input & Output -----------------------------------------------------


ostream &operator << (ostream &s, const TSparsePair &sp)
{
	s << sp.index << ':' << sp.elt;

	return(s);
}

ostream &operator << (ostream &s, const TSparseVec &v)
{
	if (TSparseVec::IsCompact())
	{
		Int i;

		s << '[';

		for (i = 0; i < v.NumItems() - 2; i++)
			s << v[i] << ' ';

		s << v[i] << ']';
	}
	else
	{
    	Int i, j;

    	s << '[';
    	
    	for (i = 0, j = 0; i < v.Elts() - 1; i++)
    		if (i < v[j].index)
    			s << "0 ";
    		else
    			s << v[j++].elt << ' ';

		if (i < v[j].index)
			s << "0]";
		else
			s << v[j].elt << ']';
	}

    return(s);
}

istream &operator >> (istream &s, TSparseVec &v)
{
    Char	c;
	Int		i = 0;
	TVReal 	elt;

	//	Expected format: [a b c d ...]
	
    while (isspace(s.peek()))			// 	chomp white space
		s.get(c);
		
    if (s.peek() == '[')						
    {
     	v.Begin();

	   	s.get(c);
    	
	    while (isspace(s.peek()))		// 	chomp white space
			s.get(c);
    	
		while (s.peek() != ']')
		{			
			s >> elt;
			
			if (!s)
			{
				_Warning("Couldn't read array component");
				return(s);
			}
			else
			{
    			v.AddElt(i, elt);
				i++;
			}
				
		    while (isspace(s.peek()))	// 	chomp white space
				s.get(c);
		}
		s.get(c);
	}
    else
	{
	    s.clear(ios::failbit);
	    _Warning("Error: Expected '[' while reading array");
	    return(s);
	}
	
	v.End();
	v.SetNumElts(i);
	
    return(s);
}


// --- SparseVec iterator -----------------------------------------------------


Void TSVIter::OrdFindLinear(Int i)
{
	// Linear search for the right pair
		
	while (!AtEnd() && (i > Index()))
		pairIdx++;
}

#define SV_MIXED_SEARCH

#ifndef __SV_CONST__
#define __SV_CONST__
static const Int kLinearSearchRange = 10;	//	Linear search over intervals smaller than this...
static const Int kSparsenessEst = 5; 		// estimated elts per non-zero elt.
static const Int kLSRSparse = kSparsenessEst * kLinearSearchRange;
#endif

Void TSVIter::OrdFindBinary(Int i)
//	Move index to point to the pair that corresponds to i, or if no such pair exists,
//	the pair with the next index after i that does exist.
{
#ifdef SV_MIXED_SEARCH	
	// Mixture of linear & binary, parameterised by kLinearSearchRange.
	// If the item we're looking for is farther away from the current 
	// pair than kLSRSparse, we binary search.

	if ((i - Index()) > kLSRSparse)
	{
#endif

    // --- Binary search on the pairs list-------------------------------------
	
	// A really nice thing to do here would be to back out
	// hierarchically from the current index instead of just
	// giving up on it. Similar to storing the current octree
	// node in RT, and doing the same.

		Int j = 0, k = list->NumItems() - 1, m;
		
		//	Test for trivial cases: i lies before or after the pairs list
		
		if (k < 0 || i <= list->Item(j).index)
		{	pairIdx = 0; return; }
		if (list->Item(k).index < i)
		{	pairIdx = k + 1; return; }
		if (list->Item(k).index == i)
		{	pairIdx = k; return; }
		
		while(k > j + 1 + kLinearSearchRange)
		{
			// precondition: j.index < i < k.index
			Assert(list->Item(j).index < i && i < list->Item(k).index, "Precondition failed.");
			
			// Naive midpoint picking
			m = (j + k) / 2;	// m ~ [j+1..k-1]

			// Linear midpoint picking
			// m = j + 1 +  ((k - j - 1) * (i - list->Item(j))) / list->Item(k) - list->Item(j));
						
			if (i > list->Item(m).index)
				j = m;
			else
			{
				k = m;	

				if (i >= list->Item(k).index)
				{	
					pairIdx = k;
					return; 
				}					
			}
		}

		pairIdx = j + 1;

#ifdef SV_MIXED_SEARCH
	}

	while (i > Index())		// Linear search, assuming sentinel
		pairIdx++;
#endif
}	

