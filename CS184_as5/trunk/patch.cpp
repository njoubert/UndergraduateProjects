#include "patch.h"

Bezier Patch::bezcurveinterp(Point* p0, Point* p1, Point* p2, Point* p3, double u) {
	Bezier bez;
	
	Point a = (*p0)*(1-u) + (*p1)*u;
	Point b = (*p1)*(1-u) + (*p2)*u;
	Point c = (*p2)*(1-u) + (*p3)*u;
	
	Point d = a*(1-u) + b*u;
	Point e = b*(1-u) + c*u;
	
	bez.p = d*(1-u) + e*u;

	bez.d = 3 * d-e;
	return bez;
}

Bezier* Patch::bezsurfaceinterp(double u, double v) {
	Bezier* temp;
	Curve ucurve, vcurve;
	for (int i = 0; i<4; i++) {
		for (int j=0; j<4; j++) {
		Curve temp[4];
		ucurve[i] = (temp=bezcurveinterp(controlpoints[i], u))->p;
		temp[0] = &(controlpoints[0][i]);
		temp[1] = &(controlpoints[1][i]);
		temp[2] = controlpoints[2][i]);
		temp[3] = controlpoints[3][i]);

		vcurve[i] = (temp=bezcurveinterp(temp, v))->p;
		}
	}
	Bezier* vBez = bezcurveinterp(vcurve, v);
	Bezier* uBez = bezcurveinterp(ucurve, u);
	
	Normal n = cross(vBez->d, uBez->d);
	n = n / length(n);
	temp->p = vBez->p;
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
	for (double u= 0; u<1+step; u=u+step) {
		if (u>1)
			u=1;
		int j= 0;
		for (double v=0; v<1+step; v=v+step) {
			if (v>1)
				v=1;
			Bezier* bez = bezsurfaceinterp(u, v);
			bezpoints[i][j] = bez; //Store the Bezier point
			j++;
		}
		i++;
	}
}