/*
 * System.h
 *
 *  Created on: Sep 7, 2008
 *      Author: njoubert
 */

#include "global.h"
#include "Mesh.h"
#include "intel_SparseMatrix.h"

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

	void Forces(TriangleMeshTriangle* A, TriangleMeshTriangle* B, TriangleMeshVertex* a, TriangleMeshVertex* b, TriangleMeshEdge* edge);


	void calculateInternalForces();
	void calculateExternalForces();

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

	void enableMouseForce(vec3 mousePosition);

	void updateMouseForce(vec3 mousePosition);

	void disableMouseForce();

	bool isMouseEnabled();

	vec3 f_mouse( TriangleMeshVertex* selected );
	vec3 f_spring( vec3 & pa, vec3 & pb, double rl, double Ks);
    vec3 f_damp( vec3 & pa, vec3 & pb, vec3 & va, vec3 & vb, double rl, double Kd);
    void f_bend(TriangleMeshTriangle* a, TriangleMeshTriangle* b,
            TriangleMeshVertex* a, TriangleMeshVertex* b, TriangleMeshEdge* edge);
    inline mat3 dfdx_spring(vec3 & pa, vec3 & pb, double rl, double Ks);
    inline mat3 dfdx_damp(vec3 & pa, vec3 & pb, vec3 & va, vec3 & vb, double rl, float Kd);
    mat3 dfdv_damp(vec3 & pa, vec3 & pb, double rl, double Kd);


private:
    float getKs();
    float getKd();
    float getKbe();
    float getKbd();
    float Ks, Kd, Kbe, Kbd; //Ks -Spring constant, Kd - Damping, Kbe - Bend Force Elastic, Kbd - Bend Force Damping
    TriangleMesh* mesh;
    double time;
    TriangleMeshVertex* mouseSelected;
    vec3 mouseP;

    /////////////INTEL INSIDE!!!!!///////////////////////////
    int m_iParticles, m_iInverseIterations;
	Physics_LargeVector m_Positions;
	Physics_LargeVector m_Velocities;
	Physics_LargeVector m_TotalForces_int, m_TotalForces_ext;
	////////////////////////////////////////////////////////
};


class Solver {
public:
    virtual ~Solver();
    virtual void calculateState(System* sys)=0;
    virtual std::pair<vec3,vec3> solve(System* sys,
            double timeStep, int pointIndex, TriangleMeshVertex* point) = 0;
};

class ImplicitSolver: public Solver {
    ~ImplicitSolver();
    void calculateState(System* sys);
    std::pair<vec3,vec3> solve(System* sys,
            double timeStep, int pointIndex, TriangleMeshVertex* point);
};

class ExplicitSolver: public Solver {
    ~ExplicitSolver();
    void calculateState(System* sys);
    std::pair<vec3,vec3> solve(System* sys,
            double timeStep, int pointIndex, TriangleMeshVertex* point);
};


#endif /* SYSTEM_H_ */
