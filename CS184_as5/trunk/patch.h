#ifndef PATCH_H_
#define PATCH_H_

#include "as5.h"

class Patch {
public:
	Point* (cP[4][4]);
	vector<vector<Bezier*> > bezpoints;
	Bezier* bezcurveinterp(Point*, Point*, Point*, Point*, double);
	Bezier* bezsurfaceinterp(double, double);
	Point* uniformSubdividePatch(double);
	void adaptivelyGetTriangle(double, double, double, double, double, double, vector<Triangle*>);
	Point* adaptiveSubdividePatch(double, vector<Triangle*>);
	
//	class BezierTriangles {
//		public:
//		BezierTriangles();
//		BezierTriangles(double,double,double,double,double,double);
//		double a[2]; //(u,v)
//		double b[2]; //(u,v)
//		double c[2]; //(u,v)
//			
//	};
};


#endif /*PATCH_H_*/
