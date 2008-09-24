

#ifndef VECTORTRIPLE_H_
#define VECTORTRIPLE_H_

#include <math.h>
#include <iostream>
#include <fstream>
#include <cmath>

class VectorTriple {
    double x;
    double y;
    double z;

  public:
    VectorTriple(double x1, double y1, double z1) : x(x1), y(y1), z(z1) {}
	VectorTriple() : x(0), y(0), z(0) {}

	double valueX() {return x;};
	double valueY() {return y;};
	double valueZ() {return z;};

	void setX(double x1) {x = x1;};
	void setY(double y1) {y = y1;};
	void setZ(double z1) {z = z1;};

    VectorTriple operator + (const VectorTriple v2) const;
    VectorTriple operator - (const VectorTriple v2) const;
    VectorTriple operator * (const double c) const;
	VectorTriple operator / (const double c) const;
	bool operator == (const VectorTriple v2) const;
	VectorTriple normalize();

    friend VectorTriple cross(const VectorTriple v1, const VectorTriple v2);
	friend VectorTriple compMult(const VectorTriple v1, const VectorTriple v2);
    friend double dot(const VectorTriple v1, const VectorTriple v2);
    
	double length();

	void print();

	
};




#endif