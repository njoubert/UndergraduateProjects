#ifndef ALGEBRA_C_
#define ALGEBRA_C_

#include <cmath>
#include "Debug.cpp"

class Vector3d;
class Point;
class Color;
class Ray;

class Vector3d {
public:
	Vector3d() {}
	Vector3d(double x, double y, double z) { setPos(x,y,z); }
	
	inline void calculateFromPositions(const Vector3d * const start, const Vector3d * const end) {
	    setX(end->x - start->x);
	    setY(end->y - start->y);
	    setZ(end->z - start->z);
	}
	
	inline void calculateReflective(const Vector3d & in, const Vector3d & normal) {
		float scale = 2.0f*(in.dot(&normal));
		this->x = -in.x + scale*normal.x;
		this->y = -in.y + scale*normal.y;
		this->z = -in.z + scale*normal.z;
	}

	inline double dot(const Vector3d * other) const {
		return x*other->x + y*other->y + z*other->z;
	}
	inline void flip() {
	    x *= -1.0;
	    y *= -1.0;
	    z *= -1.0;
	}
	inline double length() {
	    return sqrt(x*x + y*y + z*z);
	}
	inline void normalize() {
	    double l = length();
		x = x/l;
		y = y/l;
		z = z/l;
	}
	inline void setPos(double x, double y, double z) {
	    this->x = x;
	    this->y = y;
	    this->z = z;
	}
    inline Vector3d & operator-=(Vector3d & v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }
    inline Vector3d operator-(Vector3d & v) {
        Vector3d result = *this;
        result -= v;
        return result;
    }
    inline Vector3d & operator+=(Vector3d & v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }
    inline Vector3d operator+(Vector3d & v) {
        Vector3d result = *this;
        result += v;
        return result;
    }
    inline Vector3d & operator*=(double t) {
        x *= t;
        y *= t;
        z *= t;
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
    
	inline double getX() { return x; }
	inline double getY() { return y; }
	inline double getZ() { return z; }
	inline void setX(double x) { this->x = x; }
	inline void setY(double y) { this->y = y; }
	inline void setZ(double z) { this->z = z; }
	inline void debugMe() {
		printDebug(5, "Vector3d=(" << x << "," << y << "," << z << ")");
	}
	double x, y, z;
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
        ray.e.x /= a;
        ray.e.y /= e;
        ray.e.z /= i;
        ray.d.x /= a;
        ray.d.y /= e;
        ray.d.z /= i;
    }
    void applyAsInverseRot(Ray & ray) {
        
    }
    void applyAsInverseScale(Vector3d & v) {
        v.x /= a;
        v.y /= e;
        v.z /= i;
    }
    void applyAsInverseRot(Vector3d & v) {
        
    }
    void applyAsScale(Vector3d & v) {
        v.x *= a;
        v.y *= e;
        v.z *= i;
    }
    void applyAsRot(Vector3d & v) {
        
    }
    
    
};

#endif /*ALGEBRA_C_*/
