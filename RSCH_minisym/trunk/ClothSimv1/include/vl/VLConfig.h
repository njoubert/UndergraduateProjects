/*
	File:			VLConfig.h

	Function:		Contains configuration options for compiling the VL 
					library.
					
	Author(s):		Andrew Willmott

	Copyright:		(c) 1995-2000, Andrew Willmott
*/

//
//  Current options are as follows:
//
//  VL_HAS_ABSF    - has the absf() call. 
//  VL_HAS_IEEEFP  - ieeefp.h exists.
//  VL_ROW_ORIENT  - Use row-oriented transforms, so you can swap 'em with 
//                   OpenGL. If this is defined, transformations are 
//                   v = v * Rot3x(u, 0.5), rather than v = Rot3x(u, 0.5) * v.
//                   This is off by default.
//  VL_USE_MEMCPY  - Use memcpy and friends for generic vector operations.
//                   Generally memcpy is only faster than loop-based copies
//                   for n > n0, so this should be enabled if the average
//                   vector size is larger than n0. Some values of n0:
//                       SGI R4400: 30
//                       i686, 750MHz: 5
//

// --- Configuration ----------------------------------------------------------

#define VL_CONFIG msvc

