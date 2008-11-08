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
#include "math/LargeVector.h"
#include "connectors/MeshTOLargeMatrix.h"

/// \brief Represents the cloth mesh system
///
/// Contains all the data needed to store a cloth mesh.
class System {
public:
	System(TriangleMesh* m, Material* mat);

    //-----------------------------------------
	//UPDATING AND ACCESSING STATE FOR SOLVER
	void loadStateFromMesh();
	LARGE_VECTOR* getX();
	LARGE_VECTOR* getV();
	SPARSE_MATRIX* getM();
    //-----------------------------------------

	mat3 outerProduct(vec3 a, vec3 b);

	//-----------------------------------------
	//CALCULATE STATE FOR SOLVER:
	void calculateInternalForces(Solver*);
	void calculateExternalForces(Solver*);
	void calculateForcePartials(NewmarkSolver*);
	void applyConstraints(Solver*, vector<Constraint*> *);
	//-----------------------------------------

	void takeStep(Solver* solver, vector<Constraint*> *constraints, double timeStep);

	void enableMouseForce(vec3 mousePosition);

	void updateMouseForce(vec3 mousePosition);

	void disableMouseForce();

	bool isMouseEnabled();

	vec3 f_mouse( TriangleMeshVertex* selected );
	vec3 f_spring( vec3 & pa, vec3 & pb, double rl, double Ks);
    vec3 f_damp( vec3 & pa, vec3 & pb, vec3 & va, vec3 & vb, double rl, double Kd);
    inline mat3 dfdx_spring(vec3 & pa, vec3 & pb, double rl, double Ks);
    inline mat3 dfdx_damp(vec3 & pa, vec3 & pb, vec3 & va, vec3 & vb, double rl, float Kd);
    mat3 dfdv_damp(vec3 & pa, vec3 & pb, double rl, double Kd);


private:
    TriangleMesh* mesh;
    LARGE_VECTOR* _x;
    LARGE_VECTOR* _v;
    SPARSE_MATRIX* _M;
    Material* _mat;
    TriangleMeshVertex* mouseSelected;
    vec3 mouseP;
};

#endif /* SYSTEM_H_ */
