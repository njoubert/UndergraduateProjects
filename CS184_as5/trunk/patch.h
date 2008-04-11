#ifndef PATCH_H_
#define PATCH_H_

#include "bezier.h"
typedef Point Curve [4];

struct Patch {
	Point[4][4] controlpoints;
	Bezier* bezcurveinterp(Curve, double);
	Bezier* bezsurfaceinterp(double, double);
	void subdividepatch(double);
};


#endif /*PATCH_H_*/
