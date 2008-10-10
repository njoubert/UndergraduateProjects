// Physics_Vector3.h: interface for the Physics_Vector3 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PHYSICS_VECTOR3_H__D37019FA_845B_4E04_A05C_454A9B3755F2__INCLUDED_)
#define AFX_PHYSICS_VECTOR3_H__D37019FA_845B_4E04_A05C_454A9B3755F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Physics_Vector3  
{
public:
	Physics_t x;
	Physics_t y;
	Physics_t z;

public:
	Physics_Vector3();
	Physics_Vector3( Physics_t nx, Physics_t ny, Physics_t nz );
	Physics_Vector3( Physics_t common );
	inline Physics_t operator[](int i )
	{
		return (&x)[i];
	}

	inline void Normalize()
	{
		Physics_t l = (Physics_t)sqrt( x*x+y*y+z*z );
		x /= l;
		y /= l;
		z /= l;
	}

	Physics_t Length();
};

#define VECTOR3_ADD( v1,v2, dst ) do { (dst).x = (v1).x+(v2).x; (dst).y = (v1).y+(v2).y; (dst).z = (v1).z+(v2).z; } while (0)
#define VECTOR3_SUB( v1,v2, dst ) do { (dst).x = (v1).x-(v2).x; (dst).y = (v1).y-(v2).y; (dst).z = (v1).z-(v2).z; } while (0)
#define VECTOR3_SCALE( v1,scale,dst) do { (dst).x = (v1).x*(scale); (dst).y = (v1).y*(scale); (dst).z = (v1).z*(scale); } while (0)
#define VECTOR3_DP( v1, v2 ) ((v1).x*(v2).x+(v1).y*(v2).y+(v1).z*(v2).z)
#define VECTOR3_LENGTH( v ) (Physics_t)sqrt( VECTOR3_DP(v,v) );
#define VECTOR3_NORMALIZE(v,dst) do { Physics_t l = VECTOR3_LENGTH(v); (dst).x = (v).x/l; (dst).y = (v).y/l; (dst).z = (v).z/l; } while (0)
#define VECTOR3_CROSSPRODUCT( v1, v2, dst ) do { (dst).x = (v1).y*(v2).z - (v2).y*(v1).z; (dst).y = (v2).x*(v1).z-(v1).x*(v2).z; (dst).z = (v1).x*(v2).y-(v2).x*(v1).y; } while (0)

#endif // !defined(AFX_PHYSICS_VECTOR3_H__D37019FA_845B_4E04_A05C_454A9B3755F2__INCLUDED_)
