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
	Bezier* temp = new Bezier;
	Curve ucurve, vcurve;
	ucurve[0] = &(bezcurveinterp(cP[0][0], cP[0][1], cP[0][2], cP[0][3], u).p);
	ucurve[1] = &(bezcurveinterp(cP[1][0], cP[1][1], cP[1][2], cP[1][3], u).p);
	ucurve[2] = &(bezcurveinterp(cP[2][0], cP[2][1], cP[2][2], cP[2][3], u).p);
	ucurve[3] = &(bezcurveinterp(cP[3][0], cP[3][1], cP[3][2], cP[3][3], u).p);
	
	vcurve[0] = &(bezcurveinterp(cP[0][0], cP[1][0], cP[2][0], cP[3][0], u).p);
	vcurve[1] = &(bezcurveinterp(cP[0][1], cP[1][1], cP[2][1], cP[3][1], u).p);
	vcurve[2] = &(bezcurveinterp(cP[0][2], cP[1][2], cP[2][2], cP[3][2], u).p);
	vcurve[3] = &(bezcurveinterp(cP[0][3], cP[1][3], cP[2][3], cP[3][3], u).p);
	
	Bezier vBez = bezcurveinterp(vcurve[0], vcurve[1],vcurve[2], vcurve[3], v);
	Bezier uBez = bezcurveinterp(ucurve[0], ucurve[1], ucurve[2], ucurve[3], u);
	
	Normal n = cross(vBez.d, uBez.d);
	n = n / length(n);
	temp->p = vBez.p;
	temp->n = n;
	return temp;
}

// You'll notice that I placed 1+step as the terminating condition.
// Uniform subdivision, in this case, isn't quite so uniform. E.g.,
// a subdivision factor of 0.3 will yield 0.3, 0.6, 0.9 and 1.0 - 
// the last chunk is smaller than the other chunks. That's why I 
// used 1+step instead of simply 1.
// We return the BIGGEST point we found.
Point* Patch::subdividepatch(double step) {
	double x = -99999.999;
	double y = -99999.999; 
	double z = -99999.999;
	int i = 0;
	for (double u= 0; u<1+step; u=u+step) {
		if (u>1)
			u=1;
		bezpoints.push_back(vector<Bezier*>());
		for (double v=0; v<1+step; v=v+step) {
			if (v>1)
				v=1;
			Bezier* bez = bezsurfaceinterp(u, v);
			bezpoints[i].push_back(bez); //Store the Bezier point
			if (bez->p[0] > x)
				x = bez->p[0];
			if (bez->p[1] > y)
				y = bez->p[1];
			if (bez->p[2] > z)
				z = bez->p[2];
		}
		i++;
	}
	Point* ret = new Point(x,y,z);
	return ret;
}
