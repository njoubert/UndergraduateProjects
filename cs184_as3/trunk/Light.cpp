#ifndef LIGHT_C_
#define LIGHT_C_

#include "Debug.cpp"
#include "Position.cpp"
#include "Color.cpp"

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
		pos.setPositionValues(x,y,z);
	}
	
	void debugMe(bool partial) {
		if (partial) {
			printPartialDebug(" Light ");
		} else {
			printStartDebug(" Light ");
		}
		printName();
		cout << " ";
		illumination.debugMe(true);
		cout << " ";
		pos.debugMe(true);
		if (!partial)
			cout << endl;
	}
	virtual void printName()=0;
private:
	Color illumination;
	RelPosition3d pos;
};

class PointLight: public Light {
public:
	PointLight(float x,float y,float z,float r,float g,float b): Light(x,y,z,r,g,b) {
		printStartDebug("Created new PointLight: ");
		debugMe(true);
		cout << endl;
	}
	void printName() {
		cout << "PointLight";
	}
private:
	
};

class DirectionalLight: public Light {
public:
	DirectionalLight(float x,float y,float z,float r,float g,float b): Light(x,y,z,r,g,b) {
		printStartDebug("Created new DirectionalLight: ");
		debugMe(true);
		cout << endl;
	}
	void printName() {
		cout << "DirectionalLight";
	}
private:
	
};

#endif /*TEST_H_*/



