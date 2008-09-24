/*
	File:		LibVLf.cpp
	
	Purpose:	Compiles all code necessary for VLf.h.
	
	Author:		Andrew Willmott
*/


#define VL_V_REAL Float
#define VL_V_SUFF(X) X ## f
#define VL_M_REAL Float
#define VL_M_SUFF(X) X ## f

#include "Vec2.cpp"
#include "Vec3.cpp"
#include "Vec4.cpp"
#include "Vec.cpp"
#include "SparseVec.cpp"
#include "SubVec.cpp"
#include "SubSVec.cpp"

#include "Mat2.cpp"
#include "Mat3.cpp"
#include "Mat4.cpp"
#include "Mat.cpp"
#include "SparseMat.cpp"

#include "SubMat.cpp"
#include "SubSMat.cpp"
