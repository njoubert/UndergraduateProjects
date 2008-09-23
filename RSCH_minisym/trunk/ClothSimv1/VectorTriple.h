

#ifndef VECTORTRIPLE_H_
#define VECTORTRIPLE_H_

#include <math.h>
#include <iostream>
#include <fstream>
#include <cmath>

class VectorTriple {
    float x;
    float y;
    float z;

  public:
    VectorTriple(float x1, float y1, float z1) : x(x1), y(y1), z(z1) {}
	VectorTriple() : x(0), y(0), z(0) {}

	float valueX() {return x;};
	float valueY() {return y;};
	float valueZ() {return z;};

	void setX(float x1) {x = x1;};
	void setY(float y1) {y = y1;};
	void setZ(float z1) {z = z1;};

    VectorTriple operator + (const VectorTriple v2) const;
    VectorTriple operator - (const VectorTriple v2) const;
    VectorTriple operator * (const float c) const;
	VectorTriple operator / (const float c) const;
	bool operator == (const VectorTriple v2) const;
	VectorTriple normalize();

    friend VectorTriple cross(const VectorTriple v1, const VectorTriple v2);
	friend VectorTriple compMult(const VectorTriple v1, const VectorTriple v2);
    friend float dot(const VectorTriple v1, const VectorTriple v2);
    
	float length();

	void print();

	
};




#endif