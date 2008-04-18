#ifndef TYPES_H_
#define TYPES_H_

//****** Add libraries ******
//#include "cml/cml.h"
#include "as5.h"
#include <vector>
using namespace std;

//****** Add Types ******
typedef Vector3d Point;
typedef Vector3d Normal;
typedef Vector3d Deriv;
typedef Point* Curve[4];

struct Bezier {
	Point p;
	Normal n;
	Deriv d;
	Deriv d2;
};

struct Triangle {
	Point v1;
	Point v2;
	Point v3;
	Normal n1;
	Normal n2;
	Normal n3;	
	Deriv d1;
	Deriv d2;
};

#endif /*TYPES_H_*/
