/*
 * intel_Vector3.cpp
 *
 *  Created on: Oct 7, 2008
 *      Author: silversatin
 */

// Physics_Vector3.cpp: implementation of the Physics_Vector3 class.
//
//////////////////////////////////////////////////////////////////////

//#include "DRGShell.h"
#include "intel_Vector3.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Physics_Vector3::Physics_Vector3()
{

}

Physics_Vector3::Physics_Vector3( double nx, double ny, double nz )
{
	x = nx;
	y = ny;
	z = nz;
}

Physics_Vector3::Physics_Vector3( double common )
{
	x = y = z = common;
}

inline Physics_Vector3& Physics_Vector3::operator = (const Physics_Vector3& v)
{ x = v.x; y = v.y; z = v.z; return *this; }

double Physics_Vector3::Length()
{
	return (double)sqrt( x*x+y*y+z*z );
}
