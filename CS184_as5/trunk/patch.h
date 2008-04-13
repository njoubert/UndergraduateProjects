#ifndef PATCH_H_
#define PATCH_H_

#include "as5.h"

using namespace std;

class Patch {
public:
	Point* (cP[4][4]);
	vector<vector<Bezier*> > bezpoints;
	Bezier bezcurveinterp(Point*, Point*, Point*, Point*, double);
	Bezier* bezsurfaceinterp(double, double);
	void subdividepatch(double);
};


#endif /*PATCH_H_*/
