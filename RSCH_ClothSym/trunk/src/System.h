/*
 * System.h
 *
 *  Created on: Sep 7, 2008
 *      Author: njoubert
 */

#include "global.h"

#ifndef SYSTEM_H_
#define SYSTEM_H_


class Particle {
public:
    Particle() {
        x = y = z = vx = vy = vz = fx = fy = fz = m = 0;
        pinned = false;
    }
    double x, y, z;
    double vx, vy, vz;
    double fx, fy, fz;
    double m;
    bool pinned;
};

enum ForceType {
    DEFAULT,
    GRAVITY,
    DRAG,
    SPRING,
    INPUT
};

class Force {
public:
    virtual ~Force();
    virtual void apply(std::vector< std::vector < Particle > > *)=0;
    int u1, v1;
    ForceType type;
};

class GravityForce: public Force {
public:
    GravityForce();
    void apply(std::vector< std::vector < Particle > > *);
};

class DragForce: public Force {
public:
    DragForce();
    void apply(std::vector< std::vector < Particle > > *);
};

class SpringForce: public Force {
public:
    SpringForce();
    void apply(std::vector< std::vector < Particle > > *);
    int u2, v2;
    double ks, kd, r;
};

class InputForce: public Force {
public:
    InputForce();
    void apply(std::vector< std::vector < Particle > > *);
    int xi, yi, zi;
    double ks, kd, r;
    bool enabled;
};

/// \brief Represents the cloth mesh system
///
/// Contains all the data needed to store a cloth mesh.
class System {
public:
	System();
	virtual ~System();

	/// \brief Resets the whole system, clear everything,
	/// and sets it up for this dim
	void setDim(int, int);

	/// \brief Returns the dimensions of the system.
	std::vector<int> getDim();

	/// \brief Gets or Sets the current point in time.
	double getT();
	void setT(double t);

	/// \brief Returns the position vector of the system,
	/// with dimensions as reported by getDim().
	std::vector< std::vector< Particle > > * getX();

	/// \brief Sets the position vector of the system.
	void setX(std::vector< std::vector< Particle > > *);

	/// \brief Evaluates the derivative at the current time.
	std::vector< std::vector< Particle > > * evalDeriv();

	/// \brief Draws the system using OpenGL. Assumes a window exists.
	void draw();

	void addForce(Force*);

	Particle* getClosestParticle(double, double, int*, int*);

private:
    void copyX(std::vector< std::vector< Particle > > *, std::vector< std::vector< Particle > > *);

    double t;
    std::vector< std::vector<Particle> > x;
    std::vector<int> _dim;
    std::vector<Force*> _forces;

};

#endif /* SYSTEM_H_ */
