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
	
	void calculateFromPositions(const Position3d * const start, const Position3d * const end) {
		setX(end->x - start->x);
		setY(end->y - start->y);
		setZ(end->z - start->z);
	}
	
	void calculateReflective(const Vector3d & in, const Vector3d & normal) {
		float scale = 2.0f*(in.dot(&normal));
		this->x = -in.x + scale*normal.x;
		this->y = -in.y + scale*normal.y;
		this->z = -in.z + scale*normal.z;
	}
	
	float dot(const Vector3d * other) const {
		return x*other->x + y*other->y + z*other->z;
	}
	Vector3d & operator*=(float v) {
		x *= v;
		y *= v;
		z *= v;
		return *this;
	}
	Vector3d & operator+=(const Vector3d &v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	void normalize() {
		float l = sqrt(x*x + y*y + z*z);
		x = x/l;
		y = y/l;
		z = z/l;
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
protected:	
	float x, y, z; // width and height
};

#endif /*VECTOR3d_C_*/
