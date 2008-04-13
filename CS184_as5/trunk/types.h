#ifndef TYPES_H_
#define TYPES_H_

//****** Add libraries ******
#include "cml/cml.h"
#include <vector>

//****** Add Types ******
typedef cml::vector3d Point;
typedef cml::vector3d Normal;
typedef cml::vector3d Deriv;
typedef Point* Curve[4];

#endif /*TYPES_H_*/
