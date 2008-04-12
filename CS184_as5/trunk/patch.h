#ifndef PATCH_H_
#define PATCH_H_

#include "bezier.h"
typedef Point Curve [4];

class Patch {
public:
	Point[4][4] controlpoints;
	vector<vector<Bezier*>> bezpoints;
	Bezier* bezcurveinterp(Curve, double);
	Bezier* bezsurfaceinterp(double, double);
	void subdividepatch(double);
};


#endif /*PATCH_H_*/
