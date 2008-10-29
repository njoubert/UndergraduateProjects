/*
 * System.h
 *
 *  Created on: Sep 7, 2008
 *      Author: njoubert
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

class System;

#include "global.h"
#include "Mesh.h"
#include "Material.h"
#include "Solver.h"
#include "Constraint.h"

/// \brief Represents the cloth mesh system
///
/// Contains all the data needed to store a cloth mesh.
class System {
public:
	System(TriangleMesh* m, Material* mat);

	mat3 outerProduct(vec3 a, vec3 b);

	/**
	 * Gets the current absolute time this system is at.
	 * @return
	 */
	//double getT();

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
	void takeStep(Solver* solver, vector<Constraint*> *constraints, double timeStep);

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

	int m_iParticles, m_iInverseIterations;

private:
    TriangleMesh* mesh;
    Material* _mat;
    TriangleMeshVertex* mouseSelected;
    vec3 mouseP;
};





#endif /* SYSTEM_H_ */
