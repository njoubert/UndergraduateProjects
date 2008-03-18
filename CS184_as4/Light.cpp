#ifndef LIGHT_C_
#define LIGHT_C_

#include "Debug.cpp"
#include "Algebra.cpp"

using namespace std;

class Light;
class PointLight;
class DirectionalLight;

class Light {
public:
	Light() { }
	virtual ~Light() { }
	
	Light(float x,float y,float z,float r,float g,float b) {
		illumination.setColor(r,g,b);
		pos.setX(x); pos.setY(y); pos.setZ(z);
	}
	
protected:
	Color illumination;
	Vector3d pos;
};

class PointLight: public Light {
public:
	
};

class DirectionalLight: public Light {
public:

private:
	
};

#endif /*TEST_H_*/



