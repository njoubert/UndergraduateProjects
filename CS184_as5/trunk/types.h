#ifndef TYPES_H_
#define TYPES_H_

//****** Add libraries ******
#include "cml/cml.h"
#include <vector>
using namespace std;

//****** Add Types ******
typedef cml::vector3d Point;
typedef cml::vector3d Normal;
typedef cml::vector3d Deriv;
typedef Point* Curve[4];

struct Bezier {
	Point p;
	Normal n;
	Deriv d;
};


#endif /*TYPES_H_*/
