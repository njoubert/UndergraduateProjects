/*
	File:			SubSVec.cpp

	Function:		Implements SubSVec.h

	Author(s):		Andrew Willmott

	Copyright:		(c) 1995-2000, Andrew Willmott

	Notes:			
					
*/


#include "vl\SubSVec.h"
#include "vl\SparseVec.h"


// --- Vector Memory Management -----------------------------------------------


TSubSVec::TSubSVec(Int start, Int length, TSparseVec *target, Int span) :
	start(start), elts(length), target(target)
{
	if (span == 1)
	{
		colSpan = 1;
		rowSpan = 0;
	}		
	else
	{
		rowSpan = span / target->Elts();
		colSpan = span % target->Elts();
	}
}

TSubSVec::TSubSVec(const TSubSVec &v) : elts(v.elts), start(v.start),
	colSpan(v.colSpan), rowSpan(v.rowSpan), target(v.target)
{
}

TSubSVec &TSubSVec::operator = (const TSparseVec &v)
{
	if (rowSpan)
	{
		TSVIter	j;
		
		for (j.Begin(v); !j.AtEnd(); j.Inc())
			target[j.Index() * rowSpan]
				.Set(start + j.Index() * colSpan, j.Data());
	}
	else
	{
		Int			i, srcSize, dstSize, cstart, k;
		TSVIter		j(target[0]);

		// find indices of the first & last ...
				
		srcSize = v.NumItems() - 1;
		j.IncTo(start);
		cstart = j.PairIdx();
		j.IncTo(start + colSpan * v.Elts());
		dstSize = j.PairIdx() - cstart;

		// resize the hole in the index/elt array

		if (srcSize > dstSize)
			target->Insert(cstart, srcSize - dstSize);
		else if (dstSize > srcSize)
			target->Delete(cstart, dstSize - srcSize);
		
		// copy over the source vector pairs
		
		for (i = 0, k = cstart; i < srcSize; i++, k++)
		{
			target->Item(k).elt = v.Item(i).elt;
			target->Item(k).index = v.Item(i).index + start;
		}
	}
	
	return(SELF);
}

TSubSVec &TSubSVec::operator = (const TSubSVec &v)
{
	// for the moment, use a cast... inefficient! XXX
	
	return(SELF = TSparseVec(v));
}

Void TSubSVec::Store(TSparseVec &sv) const
{
	sv.SetNumElts(Elts());
	sv.Begin();
	
	if (rowSpan)
	{
		Int		i, ccol = start;
		
		for (i = 0; i < Elts(); i++)
		{
			// We only want one element from each SparseVec, so
			// use Get()...
			sv.AddElt(i, target[i].Get(ccol));
			ccol += colSpan;
		}
	}
	else
	{
		Int		endIndex = start + elts * colSpan;
		TSVIter	j(target[0]);

		for (j.IncTo(start); j.PairIdx() < endIndex; j.Inc())
			sv.AddNZElt(j.Index() - start, j.Data());
	}
	
	sv.End();
}

// --- Sub-vector function ----------------------------------------------------

TSubSVec sub(const TSparseVec &v, Int start, Int length)
{
	Assert(start >= 0 && length > 0 && start + length <= v.Elts(),
		"(sub(SparseVec)) illegal subset of vector");

	return(TSubSVec(start, length, (TSparseVec*) &v));
}

