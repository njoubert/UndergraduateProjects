/*
	File:			SubVec.cpp

	Function:		Implements SubVec.h

	Author(s):		Andrew Willmott

	Copyright:		(c) 1995-2000, Andrew Willmott

	Notes:			

*/

#include "vl\SubVec.h"
#include "vl\Vec.h"


// --- Vector Memory Management -----------------------------------------------


TSubVec::TSubVec(Int n, Int span, TVReal data[]) : elts(n), span(span),
	data(data)
{
}

TSubVec::TSubVec(const TSubVec &v) : elts(v.elts), span(v.span), data(v.data)
{
}

TSubVec::TSubVec(const TVec &v) : elts(v.Elts()), span(1), data(v.Ref())
{
}

TSubVec &TSubVec::operator = (const TSubVec &v)
{
	Assert(Elts() == v.Elts(), "(SubVec::=) Vector sizes don't match");
	Int i;
	
	for (i = 0; i < Elts(); i++)
		SELF[i] = v[i];
		
	return(SELF);
}

TSubVec &TSubVec::operator = (const TVec &v)
{
	Assert(Elts() == v.Elts(), "(SubVec::=) Vector sizes don't match");
	Int i;
	
	for (i = 0; i < Elts(); i++)
		SELF[i] = v[i];
		
	return(SELF);
}


// --- SubVec In-Place operators ----------------------------------------------


TSubVec &TSubVec::operator += (const TSubVec &sv)
{
	Assert(Elts() == sv.Elts(), "(Vec::+=) vector sizes don't match");	

	Int		i;
	
	for (i = 0; i < Elts(); i++) 
		SELF[i] += sv[i];
	
	return(SELF);
}

TSubVec &TSubVec::operator -= (const TSubVec &sv)
{
	Assert(Elts() == sv.Elts(), "(Vec::-=) vector sizes don't match");	

	Int		i;
	
	for (i = 0; i < Elts(); i++) 
		SELF[i] -= sv[i];
		
	return(SELF);
}

TSubVec &TSubVec::operator *= (const TSubVec &sv)
{
	Assert(Elts() == sv.Elts(), "(Vec::*=) Vec sizes don't match");

	Int		i;
	
	for (i = 0; i < Elts(); i++) 
		SELF[i] *= sv[i];
	
	return(SELF);
}

TSubVec &TSubVec::operator *= (TVReal s)
{
	Int		i;
	
	for (i = 0; i < Elts(); i++) 
		SELF[i] *= s;
	
	return(SELF);
}

TSubVec &TSubVec::operator /= (const TSubVec &sv)
{
	Assert(Elts() == sv.Elts(), "(Vec::/=) Vec sizes don't match");

	Int		i;
	
	for (i = 0; i < Elts(); i++) 
		SELF[i] /= sv[i];
	
	return(SELF);
}

TSubVec &TSubVec::operator /= (TVReal s)
{
	Int		i;
	
	for (i = 0; i < Elts(); i++) 
		SELF[i] /= s;
	
	return(SELF);
}
