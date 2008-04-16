#ifndef PATCH_H_
#define PATCH_H_

#include "types.h"

class Patch {
public:
	Point* (cP[4][4]);
	vector<vector<Bezier*> > bezpoints;
	Bezier bezcurveinterp(Point*, Point*, Point*, Point*, double);
	Bezier* bezsurfaceinterp(double, double);
	Point* subdividepatch(double);
};


#endif /*PATCH_H_*/
