/*
	File:			Vec2.cpp

	Function:		Implements Vec2.h

	Author(s):		Andrew Willmott

	Copyright:		(c) 1995-2000, Andrew Willmott

	Notes:			

*/


#include "vl\Vec2.h"
#include <ctype.h>
#include <iomanip.h>


ostream &operator << (ostream &s, const TVec2 &v)
{
	Int w = s.width();

	return(s << '[' << v[0] << ' ' << setw(w) << v[1] << ']');
}

istream &operator >> (istream &s, TVec2 &v)
{
    TVec2	result;
    Char	c;
	
	// Expected format: [1 2]
	
    while (s >> c && isspace(c))		
		;
		
    if (c == '[')						
    {
		s >> result[0] >> result[1];	

		if (!s)
		{
			cerr << "Error: Expected number while reading vector\n";
			return(s);
		}
			
		while (s >> c && isspace(c))
			;
			
		if (c != ']')
    	{
    		s.clear(ios::failbit);
	    	cerr << "Error: Expected ']' while reading vector\n";
	    	return(s);
    	}
	}
    else
	{
	    s.clear(ios::failbit);
	    cerr << "Error: Expected '[' while reading vector\n";
	    return(s);
	}
	
	v = result;
    return(s);
}

