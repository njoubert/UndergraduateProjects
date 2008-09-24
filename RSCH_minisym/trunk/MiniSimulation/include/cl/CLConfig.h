/*
	File:			CLConfig.h

	Function:		Contains configuration options for compiling the CL 
					library.
					
	Author(s):		Andrew Willmott

	Copyright:		(c) 1995-2000, Andrew Willmott
*/

//
//	Current options are as follows:
//
//	CL_CHECKING			- Do range checking on array accesses
//	CL_FLOAT			- Preferred math type is float, rather than double
//	CL_NO_BOOL			- Compiler has no bool type.
//	CL_NO_TF			- true and false are not predefined.
//  CL_TMPL_INST    	- use ANSI syntax to explicitly instantiate Array templates
//

// --- Configuration ----------------------------------------------------------

#define CL_CONFIG msvc

#pragma warning (disable: 4244) // this is a silly warning.
#pragma warning (disable: 4305) // this is a silly warning.
#pragma warning (disable: 4800) // but these should be fixed
#pragma warning (disable: 4018)
