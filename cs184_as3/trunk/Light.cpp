#ifndef LIGHT_C_
#define LIGHT_C_

#include "Debug.cpp"
#include "Position.cpp"
#include "Color.cpp"
#include "Vector3d.cpp"

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
	
	virtual Vector3d getIncidence(AbsPosition3d & point, float multiplier) = 0;
	
	void setPosition(float x, float y, float z) {
		pos.setPositionValues(x,y,z);
	}
	
	const RelPosition3d & getPosition() const {
		return pos;
	}
	
	const Color & getColor() const {
		return illumination;
	}
	
	void debugMe(bool partial) {
		if (!DEBUG)
			return;
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
protected:
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
	Vector3d getIncidence(AbsPosition3d & point, float multiplier) {
		Vector3d incidence;
		AbsPosition3d light_pos = pos.getAbsolutePosition(multiplier);
		incidence.calculateFromPositions(&point,&light_pos);
		return incidence;
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
	Vector3d getIncidence(AbsPosition3d & point, float multiplier) {
		Vector3d incidence;
		AbsPosition3d origin;
		AbsPosition3d light_pos = pos.getAbsolutePosition(multiplier);
		origin.setPositionValues(0,0,0);
		incidence.calculateFromPositions(&origin,&light_pos);
		return incidence;
	}
	void printName() {
		cout << "DirectionalLight";
	}
private:
	
};

#endif /*TEST_H_*/



