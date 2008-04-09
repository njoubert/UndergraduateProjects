#ifndef BEZIER_H_
#define BEZIER_H_

#include "glut-3.7.6-bin/GL/glut.h"
#include "cml/cml.h
typedef glVertex3d Point;
typedef glNormal3d Normal;
typedef glVertex3d Deriv;
typedef Point Patch [4][4];
typedef Point Curve [4];

struct Bezier {
	Point p;
	Normal n;
	Deriv d;
};

static Bezier* bezcurveinterp(Curve, double);
static Bezier* bezsurfaceinterp(Patch, double, double);
static void subdividepatch(Patch, double);

#endif /*BEZIER_H_*/
