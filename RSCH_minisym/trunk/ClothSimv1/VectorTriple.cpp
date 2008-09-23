#include "VectorTriple.h"

using namespace std;
VectorTriple VectorTriple::operator + (const VectorTriple v2) const {
  VectorTriple result;

  result.x = x + v2.x;
  result.y = y + v2.y;
  result.z = z + v2.z;

  return result;
}    

bool VectorTriple::operator == (const VectorTriple v2) const {
	if (x == v2.x && y == v2.y && z == v2.z) {
		return true;
	} else {
		return false;
	}
} 

VectorTriple VectorTriple::operator - (const VectorTriple v2) const {
  VectorTriple result;

  result.x = x - v2.x;
  result.y = y - v2.y;
  result.z = z - v2.z;

  return result;
}    

VectorTriple VectorTriple::operator * (const float c) const {
  VectorTriple result;

  result.x = c*x;
  result.y = c*y;
  result.z = c*z;

  return result;
}

VectorTriple VectorTriple::operator / (const float c) const {
  VectorTriple result;

  result.x = x/c;
  result.y = y/c;
  result.z = z/c;

  return result;
}   


float dot(const VectorTriple v1, const VectorTriple v2) {
  return ( (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z) );
}


VectorTriple cross(const VectorTriple v1, const VectorTriple v2) {
  VectorTriple result;

  result.x = (v1.y * v2.z) - (v2.y * v1.z);
  result.y = (v2.x * v1.z) - (v1.x * v2.z);
  result.z = (v1.x * v2.y) - (v2.x * v1.y);

  return result;
}

VectorTriple compMult(const VectorTriple v1, const VectorTriple v2) {
	VectorTriple result;

	result.x = v1.x * v2.x;
	result.y = v1.y * v2.y;
	result.z = v1.z * v2.z;

	return result;
}

VectorTriple VectorTriple::normalize() {

	float magnitude = sqrt( x*x + y*y + z*z );
	if (magnitude != 0) {
		x = x/magnitude;
		y = y/magnitude;
		z = z/magnitude;
	}

	return (*this);
	
}


float VectorTriple::length() {

	return (float) sqrt( x*x + y*y + z*z );
}

void VectorTriple::print() {
	cout << x << " " << y << " " << z << " " << endl;
}