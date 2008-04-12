	#ifndef BEZIER_H_
#define BEZIER_H_

#include "cml/cml.h"
typedef glVertex3d Point;
typedef glNormal3d Normal;
typedef vector3d Deriv;

struct Bezier {
	Point p;
	Normal n;
	Deriv d;
};

#endif /*BEZIER_H_*/
