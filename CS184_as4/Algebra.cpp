#ifndef ALGEBRA_C_
#define ALGEBRA_C_

#include <cmath>
#include "Debug.cpp"

class Vector3d;
class Color;
class Ray;

class Vector3d {
public:
	Vector3d() {}
	Vector3d(float x, float y, float z) {setX(x); setY(y); setZ(z); }
	
	inline void calculateReflective(const Vector3d & in, const Vector3d & normal) {
		float scale = 2.0f*(in.dot(&normal));
		this->x = -in.x + scale*normal.x;
		this->y = -in.y + scale*normal.y;
		this->z = -in.z + scale*normal.z;
	}
	inline float dot(const Vector3d * other) const {
		return x*other->x + y*other->y + z*other->z;
	}
	inline void normalize() {
		float l = sqrt(x*x + y*y + z*z);
		x = x/l;
		y = y/l;
		z = z/l;
	}
	inline void setPos(float x, float y, float z) {
	    this->x = x;
	    this->y = y;
	    this->z = z;
	}
	inline float getX() { return x; }
	inline float getY() { return y; }
	inline float getZ() { return z; }
	inline void setX(float x) { this->x = x; }
	inline void setY(float y) { this->y = y; }
	inline void setZ(float z) { this->z = z; }
	inline void debugMe() {
		printDebug(5, "Vector3d=(" << x << "," << y << "," << z << ")");
	}
	float x, y, z;
};

class Point : public Vector3d {
public:
    double u;
    double v;
};

class Color {
public:
	Color() {r=0;g=0;b=0;}
	Color(float r, float g, float b) { setColor(r,g,b); }
	
	inline void setColor(float r, float g, float b) { this->r = r; this->g = g; this->b = b; }
	inline unsigned char getBMPR(int min, int max) { return (unsigned char) linearScale(min, max, 0, 255, r); }
	inline unsigned char getBMPG(int min, int max) { return (unsigned char) linearScale(min, max, 0, 255, g); }
	inline unsigned char getBMPB(int min, int max) { return (unsigned char) linearScale(min, max, 0, 255, b); }
	
	inline void debugMe() {
		printDebug(5, "Color=(" << r << "," << g << "," << b << ")");
	}
	
protected:
	float r, g, b;
	inline float linearScale(float in_low, float in_high, float out_low, float out_high, float x) {
		return (x - in_low)/(in_high-in_low)*(out_high-out_low) + out_low;
	}
	
};

class Ray {
public:
	Ray() {
	}
	
	static Ray getRay(Vector3d & start, Vector3d & end) {
		Ray r;
		r.zero = start;
		r.one = end;
		return r;
	}
	Vector3d zero;
	Vector3d one;
};

#endif /*ALGEBRA_C_*/
