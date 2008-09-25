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

	/**
	 * Gets the current absolute time this system is at.
	 * @return
	 */
	double getT();

    /**
     *
     * @param pointIndex
     * @return
     */
	vec3 calculateForces(int pointIndex);

	/**
	 * @return pair with first=dFx and second=dFv
	 */
	std::pair<mat3,mat3> calculateForcePartials(int pointIndex);

	/*
	 *
	 */
	mat3 calculateContraints(int pointIndex);

	/**
	 *
	 * @param solver
	 * @param timeStep
	 */
	void takeStep(Solver* solver, double timeStep);

	vec3 f_spring( vec3 pa, vec3 pb, double rl, double Ks);
    vec3 f_damp( vec3 pa, vec3 pb, vec3 va, vec3 vb, double rl, double Kd);
    inline mat3 dfdx_spring(vec3 pa, vec3 pb, double rl, double Ks);
    inline mat3 dfdx_damp(vec3 pa, vec3 pb, vec3 va, vec3 vb, double rl, float Kd);
    mat3 dfdv_damp(vec3 pa, vec3 pb, double rl, double Kd);


private:
    float getKs();
    float getKd();
    float ks, kd;
    TriangleMesh* mesh;
    double time;
};


class Solver {
public:
    virtual ~Solver();
    virtual std::pair<vec3,vec3> solve(System* sys, double timeStep, int pointIndex, TriangleMeshVertex* point) = 0;
};

class ImplicitSolver: public Solver {
    ~ImplicitSolver();
    std::pair<vec3,vec3> solve(System* sys, double timeStep, int pointIndex, TriangleMeshVertex* point);
};

class ExplicitSolver: public Solver {
    ~ExplicitSolver();
    std::pair<vec3,vec3> solve(System* sys, double timeStep, int pointIndex, TriangleMeshVertex* point);
};


#endif /* SYSTEM_H_ */
