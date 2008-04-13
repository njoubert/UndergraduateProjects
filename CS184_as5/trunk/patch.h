#ifndef PATCH_H_
#define PATCH_H_

#include "types.h"
#include "bezier.h"

#include <vector>
typedef Point Curve [4];

using namespace std;

class Patch {
public:
	Point controlpoints [4][4];
	vector<vector<Bezier*> > bezpoints; //C++ does not like >>, you have to use > >
	Bezier* bezcurveinterp(Curve, double);
	Bezier* bezsurfaceinterp(double, double);
	void subdividepatch(double);
};


#endif /*PATCH_H_*/
