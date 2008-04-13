#ifndef PATCH_H_
#define PATCH_H_

#include "types.h"
#include "bezier.h"
#include <vector>
typedef vector<Point> Curve;
using namespace std;

class Patch {
public:
	vector<Point> controlpoints;
	vector<vector<Bezier*> > bezpoints;
	Bezier* bezcurveinterp(Curve, double);
	Bezier* bezsurfaceinterp(double, double);
	void subdividepatch(double);
};


#endif /*PATCH_H_*/
