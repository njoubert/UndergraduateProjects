#ifndef VECTOR3d_C_
#define VECTOR3d_C_

#include "Debug.cpp"
#include "Position.cpp"
#include <cmath>

using namespace std;

class Vector3d;

class Vector3d {
public:
	
	Vector3d() {}
	Vector3d(float x, float y, float z) {setX(x); setY(y); setZ(z); }
	
	void calculateFromPositions(Position3d * start, Position3d * end) {
		setX(end->x - start->x);
		setY(end->y - start->y);
		setZ(end->z - start->z);
	}
	
//	void calculateReflective(Vector3d * in, Vector3d * normal) {
//		Vector3d temp = *normal;
//		float scale = 2.0f*(in->dot(normal));
//		*this = *in; //Copy this
//		temp = temp*scale;
//		*this += temp;
//	}
	
	float dot(Vector3d * other) {
		return x*other->x + y*other->y + z*other->z;
	}
	
	void normalize() {
		float l = sqrt(x*x + y*y + z*z);
		x = x/l;
		y = y/l;
		z = z/l;
		/* Vector3d result = *this; //Make a copy of the current class
		float l = sqrt(x*x + y*y + z*z);
		result.x = result.x/l;
		result.y = result.y/l;
		result.z = result.z/l;
		return result; */
	}
	Vector3d normalize(float length) {
		Vector3d result = *this; //Make a copy of the current class
		result.x = result.x/length;
		result.y = result.y/length;
		result.z = result.z/length;
		return result;
	}
	void debugMe() { printDebug("(" << x << "," << y << "," << z << ")"); }

	float getX() { return x; }
	float getY() { return y; }
	float getZ() { return z; }
	void setX(float x) { this->x = x; }
	void setY(float y) { this->y = y; }
	void setZ(float z) { this->z = z; }		
private:	
	float x, y, z; // width and height
};

#endif /*VECTOR3d_C_*/
