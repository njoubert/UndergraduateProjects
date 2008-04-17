#ifndef ALGEBRA_C_
#define ALGEBRA_C_

#include <cmath>
#include "NielsDebug.h"

class Vector3d;
class Color;
class Ray;

class Vector3d {
public:
	Vector3d() {/*setPos(0,0,0);*/}
	Vector3d(double x, double y, double z) { setPos(x,y,z); }
	
	inline double* data() {
		return nrs;	
	}
	
	inline void calculateFromPositions(const Vector3d * const start, const Vector3d * const end) {
	    setX(end->nrs[0] - start->nrs[0]);
	    setY(end->nrs[1] - start->nrs[1]);
	    setZ(end->nrs[2] - start->nrs[2]);
	}
	
	inline void calculateReflective(const Vector3d & in, const Vector3d & normal) {
		float scale = 2.0f*(in.dot(&normal));
		this->nrs[0] = -in.nrs[0] + scale*normal.nrs[0];
		this->nrs[1] = -in.nrs[1] + scale*normal.nrs[1];
		this->nrs[2] = -in.nrs[2] + scale*normal.nrs[2];
	}

	inline double dot(const Vector3d * other) const {
		return nrs[0]*other->nrs[0] + nrs[1]*other->nrs[1] + nrs[2]*other->nrs[2];
	}
	inline void flip() {
	    nrs[0] *= -1.0;
	    nrs[1] *= -1.0;
	    nrs[2] *= -1.0;
	}
	inline double length() {
	    return sqrt(nrs[0]*nrs[0] + nrs[1]*nrs[1] + nrs[2]*nrs[2]);
	}
	inline void normalize() {
	    double l = length();
		nrs[0] = nrs[0]/l;
		nrs[1] = nrs[1]/l;
		nrs[2] = nrs[2]/l;
	}
	inline void setPos(double x, double y, double z) {
	    this->nrs[0] = x;
	    this->nrs[1] = y;
	    this->nrs[2] = z;
	}
    inline Vector3d & operator-=(Vector3d & v) {
        nrs[0] -= v.nrs[0];
        nrs[1] -= v.nrs[1];
        nrs[2] -= v.nrs[2];
        return *this;
    }
    inline Vector3d operator-(Vector3d & v) {
        Vector3d result = *this;
        result -= v;
        return result;
    }
    inline Vector3d & operator+=(Vector3d & v) {
        nrs[0] += v.nrs[0];
        nrs[1] += v.nrs[1];
        nrs[2] += v.nrs[2];
        return *this;
    }
    inline Vector3d operator+(Vector3d & v) {
        Vector3d result = *this;
        result += v;
        return result;
    }
    inline Vector3d & operator*=(double t) {
        nrs[0] *= t;
        nrs[1] *= t;
        nrs[2] *= t;
        return *this;
    }
    inline Vector3d operator*(double t) {
        Vector3d result = *this;
        result *= t;
        return result;
    }
    inline void makeSameDirection(Vector3d & other) {
        if (this->dot(&other) < 0)
            flip();
    }
    
    inline double operator[](int t) {
    	return nrs[t];
    }
    
	inline double getX() { return nrs[0]; }
	inline double getY() { return nrs[1]; }
	inline double getZ() { return nrs[2]; }
	inline void setX(double x) { nrs[0] = x; }
	inline void setY(double y) { nrs[1] = y; }
	inline void setZ(double z) { nrs[2] = z; }
	inline void debugMe() {
		printDebug(5, "Vector3d=(" << nrs[0] << "," << nrs[1] << "," << nrs[2] << ")");
	}
	double nrs[3];
};

class Color {
public:
	Color() {r=0;g=0;b=0;}
	Color(float r, float g, float b) { setColor(r,g,b); }
	
	inline void setColor(float r, float g, float b) { this->r = r; this->g = g; this->b = b; }
	inline unsigned char getBMPR(int min, int max) { return (unsigned char) linearScale(min, max, 0, 255, r); }
	inline unsigned char getBMPG(int min, int max) { return (unsigned char) linearScale(min, max, 0, 255, g); }
	inline unsigned char getBMPB(int min, int max) { return (unsigned char) linearScale(min, max, 0, 255, b); }
	
	inline void clip() {
	    if (r > 1)
	        r = 1;
	    if (g > 1)
	        g = 1;
	    if (b > 1)
	        b = 1;
	}
	
	inline void debugMe() {
		printDebug(5, "Color=(" << r << "," << g << "," << b << ")");
	}

    Color & operator*=(const Color& other) {
        r *= other.r;
        g *= other.g;
        b *= other.b;
        return *this;
    }
    Color & operator*=(const Color* other) {
        r *= other->r;
        g *= other->g;
        b *= other->b;
        return *this;
    }
    Color & operator *=(float v) {
        r *= v;
        g *= v;
        b *= v;
        return *this;
    }   
    Color operator *(const Color & other) const {
        Color result = *this; //Make a copy of the current class
        result *= other;
        return result;
    }
    Color operator *(float v) const {
        Color result = *this; //Make a copy of the current class
        result *= v;
        return result;
    }
    Color operator +=(const Color & other) {
        r += other.r;
        g += other.g;
        b += other.b;
        return *this;
    }
    
    Color operator +(const Color& other) const {
        Color result = *this;
        result += other;
        return result;
    }
	float r, g, b;
	inline float linearScale(float in_low, float in_high, float out_low, float out_high, float x) {
		return (x - in_low)/(in_high-in_low)*(out_high-out_low) + out_low;
	}
	
};

class Ray {
public:
    //p(t) = e + td
    Vector3d e;
    Vector3d d;
    float min_t;
        
	Ray() {min_t = 0;}
	
	Vector3d getPos(float t) {
	    if (t < min_t)
	        printError("Request for a ray position with t < t_min: "<<t<<"<"<<min_t);
	    Vector3d dt = d*t;
		return (e + dt);
	}
	
	static Ray getRay(Vector3d & start, Vector3d & end, float min_t) {
		Ray r;
		r.e = start;
		r.d = (end - start);
		r.min_t = min_t;
		return r;
	}
};


class Matrix {
public:
    float a,b,c,d,e,f,g,h,i;
    Matrix() {b=c=d=f=g=h=0; a=e=i=1.0;}
    void makeScale(float sx, float sy, float sz) {
        a = sx;
        e = sy;
        i = sz;
    }
    void makeRotate(float rx, float ry, float rz) {
        
    }
    
    void applyAsInverseScale(Ray & ray) {
        ray.e.nrs[0] /= a;
        ray.e.nrs[1] /= e;
        ray.e.nrs[2] /= i;
        ray.d.nrs[0] /= a;
        ray.d.nrs[1] /= e;
        ray.d.nrs[2] /= i;
    }
    void applyAsInverseRot(Ray & ray) {
        
    }
    void applyAsInverseScale(Vector3d & v) {
        v.nrs[0] /= a;
        v.nrs[1] /= e;
        v.nrs[2] /= i;
    }
    void applyAsInverseRot(Vector3d & v) {
        
    }
    void applyAsScale(Vector3d & v) {
        v.nrs[0] *= a;
        v.nrs[1] *= e;
        v.nrs[2] *= i;
    }
    void applyAsRot(Vector3d & v) {
        
    }
    
    
};

#endif /*ALGEBRA_C_*/
