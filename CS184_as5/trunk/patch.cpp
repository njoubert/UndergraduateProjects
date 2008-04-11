#include "patch.h"

Bezier* Patch::bezcurveinterp(Curve curve, double u) {
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

Bezier* Patch::bezsurfaceinterp(double u, double v) {
	Bezier* temp;
	Curve ucurve, vcurve;
	for (int i = 0; i<controlpoints.length; i++) {
		ucurve[i] = (temp=bezcurveinterp(controlpoints[i][0:3], u))->p;
		vcurve[i] = (temp=bezcurveinterp(controlpoints[0:3][i], v))->p;
	}
	Bezier* vBez = bezcurveinterp(vcurve, v);
	Bezier* uBez = bezcurveinterp(ucurve, u);
	
	n = cross(vBez->d, uBez->d);
	n = n / length(n);
	temp->p = vBez->point;
	temp->n = n;
	return temp;
}

void Patch::subdividepatch(double step) {
	int numberDivs = (1+epsilon / step);
	for (int i= 0; i<numberDivs; i++) {
		u = i*step;
		for (int j=0; j<numberDivs; j++) {
			v= i*step;
			Bezier* bez = bezsurfaceinterp(u, v);
			save bez somewhere; //:P need to write point storage
		}
	}
}