#ifndef PATCH_H_
#define PATCH_H_

#include "types.h"
#include "bezier.h"

using namespace std;

class Patch {
public:
	Point * cP[4][4];
	vector<vector<Bezier*> > bezpoints;
	Bezier* bezcurveinterp(Point * p0, Point * p1, Point * p2, Point * p3, double u) {
	Bezier* bezsurfaceinterp(double, double);
	void subdividepatch(double);
};


#endif /*PATCH_H_*/
