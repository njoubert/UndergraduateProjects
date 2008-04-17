#include "patch.h"

Bezier* Patch::bezcurveinterp(Point* p0, Point* p1, Point* p2, Point* p3, double u) {
	Bezier* bez = new Bezier;
	Point a,b,c,d,e;
	
	double minu = 1.0 - u;
	
	a.setX(p0->getX()*minu + p1->getX()*u);
	a.setY(p0->getY()*minu + p1->getY()*u);
	a.setZ(p0->getZ()*minu + p1->getZ()*u);
	
	b.setX(p1->getX()*minu + p2->getX()*u);
	b.setY(p1->getY()*minu + p2->getY()*u);
	b.setZ(p1->getZ()*minu + p2->getZ()*u);
	
	c.setX(p2->getX()*minu + p3->getX()*u);
	c.setY(p2->getY()*minu + p3->getY()*u);
	c.setZ(p2->getZ()*minu + p3->getZ()*u);
	
	d.setX(a[0]*minu + b[0]*u);
	d.setY(a[1]*minu + b[1]*u);
	d.setZ(a[2]*minu + b[2]*u);
	
	e.setX(b[0]*minu + c[0]*u);
	e.setY(b[1]*minu + c[1]*u);
	e.setZ(b[2]*minu + c[2]*u);
	
	bez->p.setX(d[0]*minu + e[0]*u);
	bez->p.setY(d[1]*minu + e[1]*u);
	bez->p.setZ(d[2]*minu + e[2]*u);

	bez->d.setX(3*(e[0]-d[0]));
	bez->d.setY(3*(e[1]-d[1]));
	bez->d.setZ(3*(e[2]-d[2]));
	
	printDebug(5, "Address of returned bezier - p:" << (int) &(bez->p) << " d:" << (int) &(bez->d)); 
	return bez;
}


Bezier* Patch::bezsurfaceinterp(double u, double v) {
	Bezier* temp = new Bezier;
	Curve ucurve, vcurve;
	Normal n;
	
	vcurve[0] = &(bezcurveinterp(cP[0][0], cP[0][1], cP[0][2], cP[0][3], u)->p);
	vcurve[1] = &(bezcurveinterp(cP[1][0], cP[1][1], cP[1][2], cP[1][3], u)->p);
	vcurve[2] = &(bezcurveinterp(cP[2][0], cP[2][1], cP[2][2], cP[2][3], u)->p);
	vcurve[3] = &(bezcurveinterp(cP[3][0], cP[3][1], cP[3][2], cP[3][3], u)->p);
	
	ucurve[0] = &(bezcurveinterp(cP[0][0], cP[1][0], cP[2][0], cP[3][0], v)->p);
	ucurve[1] = &(bezcurveinterp(cP[0][1], cP[1][1], cP[2][1], cP[3][1], v)->p);
	ucurve[2] = &(bezcurveinterp(cP[0][2], cP[1][2], cP[2][2], cP[3][2], v)->p);
	ucurve[3] = &(bezcurveinterp(cP[0][3], cP[1][3], cP[2][3], cP[3][3], v)->p);
	
	Bezier* vBez = bezcurveinterp(vcurve[0], vcurve[1],vcurve[2], vcurve[3], v);
	Bezier* uBez = bezcurveinterp(ucurve[0], ucurve[1], ucurve[2], ucurve[3], u);
	
	n.setX(uBez->d[1]*vBez->d[2] - uBez->d[2]*vBez->d[1]);
	n.setY(uBez->d[2]*vBez->d[0] - uBez->d[0]*vBez->d[2]);
	n.setZ(uBez->d[0]*vBez->d[1] - uBez->d[1]*vBez->d[0]);
	n.normalize();
	
	//n.makeSameDirection(vBez->p); //This makes all the normals point "outward"...
	
	vBez->d.normalize();
	uBez->d.normalize();
	
	temp->p = vBez->p;
	temp->n = n;
	temp->d = vBez->d;
	temp->d2 = uBez->d;
	
	printDebug(5, "Address of returned bezier - p:" << (int) &(temp->p) << " n:" << (int) &(temp->n));
	
	return temp;
}

// You'll notice that I placed 1+step as the terminating condition.
// Uniform subdivision, in this case, isn't quite so uniform. E.g.,
// a subdivision factor of 0.3 will yield 0.3, 0.6, 0.9 and 1.0 - 
// the last chunk is smaller than the other chunks. That's why I 
// used 1+step instead of simply 1.
// We return the BIGGEST point we found.
Point* Patch::subdividepatch(double step) {
	double x=0.0f,y=0.0f,z=0.0f;
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
			if (abs(bez->p[0]) > x)
				x = abs(bez->p[0]);
			if (abs(bez->p[1]) > y)
				y = abs(bez->p[1]);
			if (abs(bez->p[2]) > z)
				z = abs(bez->p[2]);
		}
		i++;
	}
	Point* ret = new Point(x,y,z);
	return ret;
}
