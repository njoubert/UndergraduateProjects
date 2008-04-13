#ifndef BEZIER_H_
#define BEZIER_H_

#include "as5.h"
#include "cml/cml.h"
typedef cml::vector3d Point;
typedef cml::vector3d Normal;
typedef cml::vector3d Deriv;

struct Bezier {
	Point p;
	Normal n;
	Deriv d;
};

#endif /*BEZIER_H_*/
