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
class VertexToEllipseCollision;
class System {
public:
	System(TriangleMesh* m, Material* mat);

    //-----------------------------------------
	//UPDATING AND ACCESSING STATE FOR SOLVER
	void loadStateFromMesh();
	LARGE_VECTOR* getX();
	LARGE_VECTOR* getV();
	LARGE_VECTOR* getA();
	SPARSE_MATRIX* getM();
    //-----------------------------------------

	mat3 outerProduct(vec3 a, vec3 b);

	//-----------------------------------------
	//CALCULATE STATE FOR SOLVER:
	void calculateInternalForces(Solver*);
	void calculateExternalForces(Solver*);
	void calculateForcePartials(Solver*);
	void applyConstraints(Solver*, vector<Constraint*> *);
	void applyCollisions(Solver* solver, vector<VertexToEllipseCollision*> *collisions);
	//-----------------------------------------

	void takeStep(Solver*, vector<Constraint*> *, vector<VertexToEllipseCollision*> *, double);

	void enableMouseForce(vec3 mousePosition);
	void updateMouseForce(vec3 mousePosition);
	void disableMouseForce();
	bool isMouseEnabled();
	int setVertexPos2MousePos();
	void applyMouseConst2Matrices(SPARSE_MATRIX* A, LARGE_VECTOR* b);

	vec3 f_mouse( TriangleMeshVertex* selected );
	vec3 f_spring( vec3 & pa, vec3 & pb, double rl, double Ks);
    vec3 f_damp( vec3 & pa, vec3 & pb, vec3 & va, vec3 & vb, double rl, double Kd);
    void bendForce(TriangleMeshTriangle*, TriangleMeshTriangle*, TriangleMeshVertex*,
    		TriangleMeshVertex*, TriangleMeshEdge*, LARGE_VECTOR*);
    inline mat3 dfdx_spring(vec3 & pa, vec3 & pb, double rl, double Ks);
    inline mat3 dfdx_damp(vec3 & pa, vec3 & pb, vec3 & va, vec3 & vb, double rl, float Kd);
    mat3 dfdv_damp(vec3 & pa, vec3 & pb, double rl, double Kd);


private:
    TriangleMesh* mesh;
    LARGE_VECTOR* _x;
    LARGE_VECTOR* _v;
    LARGE_VECTOR* _a;
    SPARSE_MATRIX* _M;
    Material* _mat;
    TriangleMeshVertex* mouseSelected;
    vec3 mouseP;
};

#endif /* SYSTEM_H_ */
