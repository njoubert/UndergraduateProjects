#ifndef POSITION_C_
#define POSITION_C_

#include "Debug.cpp"

using namespace std;

class Position3d;
class RelPosition3d;
class AbsPosition3d;

class Position3d {
public:
	Position3d() { }
	virtual ~Position3d() {	}
	virtual void setPositionValues(float x, float y, float z) { this->x = x; this->y = y; this->z = z; }
	
	void debugMe(bool partial) {
		if (partial) {
			printPartialDebug("Position=(" << x << "," << y << "," << z << ")");
		} else {
			printDebug("Position=(" << x << "," << y << "," << z << ")");
		}
	}
	
	float x,y,z;
};

class AbsPosition3d: public Position3d { };

class RelPosition3d: public Position3d {
public:
	AbsPosition3d getAbsolutePosition(float length) const {
		AbsPosition3d ret; //Make a copy of the current class
		ret.setPositionValues(this->x * length, y * length, z * length);
		return ret;
	}
};

#endif /*POSITION_C_*/
