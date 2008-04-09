#include "bezier.h"

static Bezier* bezcurveinterp(Point[4] curve, double u) {
	Bezier* b;
	Point[curve.length-1] subcurve;
	while (subcurve.length > 1) {
		for (int i=0; i<subcurve.size(); i++) {
			subcurve[i] = curve[i] * (1.0-u) + curve[i+1] * u;
		}
		curve = subcurve;
		subcurve = new Point[curve.length-1];
	}
	b->p = subcurve[0];
	b->d = 3 * (curve[1]-curve[0]);
	return b;
}

static Bezier* bezsurfaceinterp(Point[4][4] patch, double u, double v) {
	Bezier* temp;
	Point[4] ucurve, vcurve;
	for (int i = 0; i<patch.length; i++) {
		ucurve[i] = (temp=bezcurveinterp(patch[i][0:3], u))->p;
		vcurve[i] = (temp=bezcurveinterp(patch[0:3][i], v))->p;
	}
	Bezier* vBez = bezcurveinterp(vcurve, v);
	Bezier* uBez = bezcurveinterp(ucurve, u);
	
	n = cross(vBez->d, uBez->d);
	n = n / length(n);
	temp->p = vBez->point;
	temp->n = n;
	return temp;
}