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

/*
 * Converts a UV point to a World point on this patch.
 */
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
Point* Patch::uniformSubdividePatch(double step) {
	double x=0.0f,y=0.0f,z=0.0f;
	int i = 0;
	bool doneA=false, doneB=false;
	for (double u= 0; u<1+step; u=u+step) {
		if (u>1) {
			u=1;
			doneA = true;
		}
		bezpoints.push_back(vector<Bezier*>());
		for (double v=0; v<1+step; v=v+step) {
			if (v>1) {
				v=1;
				doneB = true;
			}
			Bezier* bez = bezsurfaceinterp(u, v);
			bezpoints[i].push_back(bez); //Store the Bezier point
			if (abs(bez->p[0]) > x)
				x = abs(bez->p[0]);
			if (abs(bez->p[1]) > y)
				y = abs(bez->p[1]);
			if (abs(bez->p[2]) > z)
				z = abs(bez->p[2]);
		
			if (doneB)
				break;		
		}
		doneB = false;
		if (doneA)
			break;
		i++;
	}
	Point* ret = new Point(x,y,z);
	return ret;
}

//Given three endpoints in UV-space
void Patch::adaptivelyGetTriangle(UVPoint uvA, UVPoint uvB, UVPoint uvC, vector<Triangle*> * output) {
	Bezier *wA, *wB, *wC;
	
	//endpoints in world space:
	wA = bezsurfaceinterp(uvA.u, uvA.v);
	wB = bezsurfaceinterp(uvB.u, uvB.v);
	wC = bezsurfaceinterp(uvC.u, uvC.v);
	
	//Midpoints in UV Space
	UVPoint uvAB, uvBC, uvCA, uvCentr;
	getMidpoint(&uvA, &uvB, &uvAB);
	getMidpoint(&uvB, &uvC, &uvBC);
	getMidpoint(&uvC, &uvA, &uvCA);
	getCentroid(&uvA, &uvB, &uvC, &uvCentr);
		
	//Approximated Midpoints in World Space
	Point fwAB, fwBC, fwCA, fwCentr;	//flat World Points
	getMidpoint(&(wA->p), &(wB->p), &fwAB);
	getMidpoint(&(wB->p), &(wC->p), &fwBC);
	getMidpoint(&(wC->p), &(wA->p), &fwCA);
	getCentroid(&(wA->p), &(wB->p), &(wC->p), &fwCentr);
	
	//Actual Midpoints in World Space
	Bezier *wAB, *wBC, *wCA, *wCentr;    //World Points
	wAB = bezsurfaceinterp(uvAB.u, uvAB.v);
	wBC = bezsurfaceinterp(uvBC.u, uvBC.v);
	wCA = bezsurfaceinterp(uvCA.u, uvCA.v);
	wCentr = bezsurfaceinterp(uvCentr.u, uvCentr.v);

	//Compute distances between approximated midpoints and actual midpoints:
	Point dA, dB, dC, dCentr;
	double deltaA, deltaB, deltaC, deltaCentr;
	
	dA.calculateFromPositions(&fwAB, &(wAB->p));
	dB.calculateFromPositions(&fwBC, &(wBC->p));
	dC.calculateFromPositions(&fwCA, &(wCA->p));
	dCentr.calculateFromPositions(&fwCentr, &(wCentr->p));
	deltaA = dA.length();
	deltaB = dB.length();
	deltaC = dC.length();
	deltaCentr = dCentr.length();
	
	printDebug(4, "Calculated flatness of triangle edges: a=" << deltaA << ", b=" << deltaB << ", c=" << deltaC << ", centr=" << deltaCentr);
	
	bool isFlat = false;
	//Here we check each case
	if (deltaCentr < epsilon) {
	
		if ((deltaA < epsilon) && (deltaB < epsilon) && (deltaC < epsilon)) {
			
			isFlat = true;
			
		} else if ((deltaA >= epsilon) && (deltaB >= epsilon) && (deltaC >= epsilon)) {
			
			adaptivelyGetTriangle(uvAB, uvCA, uvA, output);
			adaptivelyGetTriangle(uvBC, uvAB, uvB, output);
			adaptivelyGetTriangle(uvCA, uvBC, uvC, output);
			adaptivelyGetTriangle(uvBC, uvCA, uvAB, output);
			
		} else if ((deltaA >= epsilon) && (deltaB >= epsilon)) {
			
			adaptivelyGetTriangle(uvAB, uvC, uvA, output);
			adaptivelyGetTriangle(uvAB, uvBC, uvC, output);
			adaptivelyGetTriangle(uvBC, uvAB, uvB, output);
			
		} else if ((deltaA >= epsilon) && (deltaC >= epsilon)) {

			adaptivelyGetTriangle(uvAB, uvCA, uvA, output);
			adaptivelyGetTriangle(uvCA, uvAB, uvB, output);
			adaptivelyGetTriangle(uvCA, uvB, uvC, output);
			
		} else if ((deltaB >= epsilon) && (deltaC >= epsilon)) {
			
			adaptivelyGetTriangle(uvB, uvBC, uvA, output);
			adaptivelyGetTriangle(uvBC, uvCA, uvA, output);
			adaptivelyGetTriangle(uvCA, uvBC, uvC, output);
			
		} else if ((deltaA >= epsilon)) {
			
			adaptivelyGetTriangle(uvAB, uvC, uvA, output);
			adaptivelyGetTriangle(uvC, uvAB, uvB, output);
			
		} else if ((deltaB >= epsilon)) {
			
			adaptivelyGetTriangle(uvA, uvBC, uvC, output);
			adaptivelyGetTriangle(uvA, uvB, uvBC, output);
			
		} else if ((deltaC >= epsilon)) {
			
			adaptivelyGetTriangle(uvB, uvCA, uvA, output);
			adaptivelyGetTriangle(uvB, uvC, uvCA, output);
			
		} else {
			isFlat = true;
			printError("Something went TERRIBLY WRONG in calculating sides to subdivide!");
		}
		
		
	
	} else {
	
		
		
	}

	if (isFlat) {
		//Make a new triangle in world space.
		Triangle* flat = new Triangle;
		flat->v1 = wA->p;
		flat->v2 = wB->p;
		flat->v3 = wC->p;
		flat->n1 = wA->n;
		flat->n2 = wB->n;
		flat->n3 = wC->n;
		flat->d1 = wA->d;
		flat->d2 = wA->d2;
		
		printDebug(4, "Saving Triangle");
		
		//Add it to output.	
		output->push_back(flat);
	}
	
}

//Subdivides the patch adaptively, storing completed triangles in output vector.
Point* Patch::adaptiveSubdividePatch(double epsilon, vector<Triangle*> * output) {
	double x=0.0f,y=0.0f,z=0.0f;
	
	this->epsilon = epsilon;
	
	UVPoint a, b, c, d;
	a.u = 0; //0,0
	a.v = 0;
	b.u = 1; //1, 0
	b.v = 0;
	c.u = 0; //0, 1
	c.v = 1;
	d.u = 1; //1, 1
	d.v = 1;
	
	adaptivelyGetTriangle(a, b, d, output);
	adaptivelyGetTriangle(c, a, d, output);

	
	//Find the biggest point in the world.
	printDebug(1, "Finding largest point in the world.");
	
	for (unsigned int t=0; t < output->size(); t++) {
		
		if (abs((*output)[t]->v1[0]) > x)
			x = abs((*output)[t]->v1[0]);
		if (abs((*output)[t]->v1[1]) > y)
			y = abs((*output)[t]->v1[1]);
		if (abs((*output)[t]->v1[2]) > z)
			z = abs((*output)[t]->v1[2]);
			
		if (abs((*output)[t]->v2[0]) > x)
			x = abs((*output)[t]->v2[0]);
		if (abs((*output)[t]->v2[1]) > y)
			y = abs((*output)[t]->v2[1]);
		if (abs((*output)[t]->v2[2]) > z)
			z = abs((*output)[t]->v2[2]);
			
		if (abs((*output)[t]->v3[0]) > x)
			x = abs((*output)[t]->v3[0]);
		if (abs((*output)[t]->v3[1]) > y)
			y = abs((*output)[t]->v3[1]);
		if (abs((*output)[t]->v3[2]) > z)
			z = abs((*output)[t]->v3[2]);
		
	}
				
	Point* ret = new Point(x,y,z);
	return ret;
}

inline void Patch::getMidpoint(UVPoint* start, UVPoint* end, UVPoint* midpoint) {
	midpoint->u = 0.5*(start->u + end->u);
	midpoint->v = 0.5*(start->v + end->v);
}

inline void Patch::getCentroid(UVPoint* a, UVPoint* b, UVPoint* c, UVPoint* centroid) {
	centroid->u = (a->u + b->u + c->u)/3.0;	
	centroid->v = (a->v + b->v + c->v)/3.0;
}

inline void Patch::getMidpoint(Point* start, Point* end, Point* midpoint) {
	midpoint->setPos(
		(start->getX() + end->getX()) / 2.0,
		(start->getY() + end->getY()) / 2.0,
		(start->getZ() + end->getZ()) / 2.0);
}

inline void Patch::getCentroid(Point* a, Point* b, Point* c, Point* centroid) {
	centroid->setPos(
		(a->getX() + b->getX() + c->getX()) / 3.0,
		(a->getY() + b->getY() + c->getY()) / 3.0,
		(a->getZ() + b->getZ() + c->getZ()) / 3.0);
}

//Patch::BezierTriangles::BezierTriangles() { }
//
//
//Patch::BezierTriangles::BezierTriangles(double u1,double v1,double u2,double v2,double u3,double v3) {
//	a[0] = u1;
//	a[1] = v1;
//	b[0] = u2;
//	b[1] = v2;
//	c[0] = u3;
//	c[1] = v3;
//	
//}
