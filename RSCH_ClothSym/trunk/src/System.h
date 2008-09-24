/*
 * System.h
 *
 *  Created on: Sep 7, 2008
 *      Author: njoubert
 */

#include "global.h"
#include "Mesh.h"

#ifndef SYSTEM_H_
#define SYSTEM_H_

#define GRAVITY 0.98

class System;
class Solver;
class ImplicitSolver;
class ExplicitSolver;


/// \brief Represents the cloth mesh system
///
/// Contains all the data needed to store a cloth mesh.
class System {
public:
	System(TriangleMesh* m);

	mat3 outerProduct(vec3 a, vec3 b);

	double getT();

	vec3 calculateForces(int pointIndex);

	std::pair<mat3,mat3> calculateForcePartials(int pointIndex);

	mat3 calculateContraints(int pointIndex);

	void takeStep(Solver* solver, double timeStep);

	void draw();

private:
    TriangleMesh* mesh;
    double time;
};


class Solver {
public:
    virtual ~Solver();
    virtual std::pair<vec3,vec3> solve(System* sys, int pointIndex) = 0;
};

class ImplicitSolver: public Solver {
    virtual ~ImplicitSolver();
    std::pair<vec3,vec3> solve(System* sys, int pointIndex);
};

class ExplicitSolver: public Solver {
    virtual ~ExplicitSolver();
    std::pair<vec3,vec3> solve(System* sys, int pointIndex);
};


#endif /* SYSTEM_H_ */
