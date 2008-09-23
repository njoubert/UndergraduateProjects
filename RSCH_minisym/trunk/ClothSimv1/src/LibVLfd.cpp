/*
	File:		LibVLfd.cpp
	
	Purpose:	Compiles mixed float/double code.
	
	Author:		Andrew Willmott

*/


#include "vl\VLd.h"

#include "vl\VLUndef.h"
#define VL_V_REAL Float
#define VL_V_SUFF(X) X ## f
#define VL_M_REAL Double
#define VL_M_SUFF(X) X ## d

#include "vl\Vec2.h"
#include "vl\Vec3.h"
#include "vl\Vec4.h"
#include "vl\Vec.h"
#include "vl\SparseVec.h"

#include "Mixed.cpp"
