#ifndef PATCH_H_
#define PATCH_H_

#include "bezier.h"
#include <vector>
typedef Point Curve [4];

using namespace std;

class Patch {
public:
	Point controlpoints [4][4];
	vector<vector<Bezier*>> bezpoints;
	Bezier* bezcurveinterp(Curve, double);
	Bezier* bezsurfaceinterp(double, double);
	void subdividepatch(double);
};


#endif /*PATCH_H_*/
