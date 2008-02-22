#ifndef COLOR_C_
#define COLOR_C_

#include <iostream>
#include <algorithm>
#include "Debug.cpp"

#ifdef _WIN32
#	include <windows.h>
#else
#	include <sys/time.h>
#endif

#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

using namespace std;

class Color;

class Color {
public:
	
	Color() {r=0;g=0;b=0;}
	Color(float r, float g, float b) { setColor(r,g,b); }
	
	void setColor(float r, float g, float b) { this->r = r; this->g = g; this->b = b; }
	GLclampf getGlR(int min, int max) { return linearScale(min, max, 0, 1, r); }
	GLclampf getGlG(int min, int max) { return linearScale(min, max, 0, 1, g); }
	GLclampf getGlB(int min, int max) { return linearScale(min, max, 0, 1, b); }
	
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
	
	void debugMe(bool partial) {
		if (partial) {
			printPartialDebug("Color=(" << r << "," << g << "," << b << ")");
		} else {
			printDebug("Color=(" << r << "," << g << "," << b << ")");
		}
	}
	
	float r, g, b;
private:
	float linearScale(float in_low, float in_high, float out_low, float out_high, float x) {
		return (x - in_low)/(in_high-in_low)*(out_high-out_low) + out_low;
	}
	
};

#endif /*COLOR_C_*/
