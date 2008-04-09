#ifndef BEZIER_H_
#define BEZIER_H_

#include "glut-3.7.6-bin/GL/glut.h"
#include "cml/cml.h
typedef glVertex3d Point
typedef glNormal3d Normal
typedef glVertex3d Deriv

struct Bezier {
	Point p;
	Normal n;
	Deriv d;
};

static Bezier* bezcurveinterp(Point[4], double);
static Bezier* bezsurfaceinterp(Point[4][4], double, double);

#endif /*BEZIER_H_*/
