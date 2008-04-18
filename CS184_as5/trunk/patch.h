#ifndef PATCH_H_
#define PATCH_H_

#include "as5.h"

class Patch {
public:
	double epsilon; 
	Point* (cP[4][4]);
	vector<vector<Bezier*> > bezpoints;
	Bezier* bezcurveinterp(Point*, Point*, Point*, Point*, double);
	Bezier* bezsurfaceinterp(double, double);
	Point* uniformSubdividePatch(double);
	void adaptivelyGetTriangle(UVPoint, UVPoint, UVPoint, vector<Triangle*> * );
	Point* adaptiveSubdividePatch(double, vector<Triangle*> * );
	
	void getMidpoint(UVPoint*, UVPoint*, UVPoint*);  
	void getCentroid(UVPoint*, UVPoint*, UVPoint*, UVPoint*); 
	
	void getMidpoint(Point*, Point*, Point*);  
	void getCentroid(Point*, Point*, Point*, Point*); 
	
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
