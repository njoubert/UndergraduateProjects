// Physics_Vector3.cpp: implementation of the Physics_Vector3 class.
//
//////////////////////////////////////////////////////////////////////

#include "DRGShell.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Physics_Vector3::Physics_Vector3()
{

}

Physics_Vector3::Physics_Vector3( Physics_t nx, Physics_t ny, Physics_t nz )
{
	x = nx;
	y = ny;
	z = nz;
}

Physics_Vector3::Physics_Vector3( Physics_t common )
{
	x = y = z = common;
}


Physics_t Physics_Vector3::Length()
{
	return (Physics_t)sqrt( x*x+y*y+z*z );
}
