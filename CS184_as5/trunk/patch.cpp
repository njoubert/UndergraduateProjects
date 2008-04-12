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

// You'll notice that I placed 1+step as the terminating condition.
// Uniform subdivision, in this case, isn't quite so uniform. E.g.,
// a subdivision factor of 0.3 will yield 0.3, 0.6, 0.9 and 1.0 - 
// the last chunk is smaller than the other chunks. That's why I 
// used 1+step instead of simply 1.
void Patch::subdividepatch(double step) {
	int i = 0;
	for (double u= 0; u<1+step; u+step) {
		if (u>1)
			u=1;
		int j= 0;
		for (double v=0; v<1+step; v+step) {
			if (v>1)
				v=1;
			Bezier* bez = bezsurfaceinterp(u, v);
			bezpoints[i][j] = bez //Store the Bezier point
			j++
		}
		i++;
	}
}