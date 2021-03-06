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
	void loadMeshFromState();
	void setHighQmesh(TriangleMesh* cMesh);
	LARGE_VECTOR* getX();
	LARGE_VECTOR* getV();
	LARGE_VECTOR* getA();
	SPARSE_MATRIX* getM();
	SPARSE_MATRIX* getW();
	SPARSE_MATRIX* getI();
    //-----------------------------------------

	mat3 outerProduct(vec3 a, vec3 b);

	//-----------------------------------------
	//CALCULATE STATE FOR SOLVER:
	void calculateInternalForces(Solver*);
	void calculateExternalForces(Solver*);
	void calculateCollisionDamping(Solver*, SPARSE_MATRIX*, SPARSE_MATRIX*, vector<VertexToEllipseCollision*> *, double);
	void calculatePosVelCollisionChange(Solver*, double, vector<VertexToEllipseCollision*> *);
	void calculateDampingToLimitStrain(Solver*, SPARSE_MATRIX*, double Kld);

	bool strainLimitSolverMatrices(SPARSE_MATRIX*, LARGE_VECTOR*);
	bool calcStrainLimitJacobi(Solver*, LARGE_VECTOR*, double);

	void calculateForcePartials(NewmarkSolver*);
	void calculateForcePartials(ImplicitSolver* solver);
	void applyConstraints(Solver*, vector<Constraint*> *);
	void applyCollisions(Solver* solver, vector<VertexToEllipseCollision*> *collisions);
	void calculateWindForces(LARGE_VECTOR*, SPARSE_MATRIX*, int);

	bool correctWithMeshSync(Solver*, LARGE_VECTOR*, LARGE_VECTOR*, double);
	bool correctExplicitlyWithMeshSync(Solver*, LARGE_VECTOR*, LARGE_VECTOR*, double);
	bool correctSolverMatrices(SPARSE_MATRIX*,LARGE_VECTOR*);
	bool calculateMeshDifference();

	void bendForceJacobian(TriangleMeshTriangle* A, TriangleMeshTriangle* B,
			TriangleMeshVertex* a, TriangleMeshVertex* b, TriangleMeshEdge* edge,
			 SPARSE_MATRIX* JV, SPARSE_MATRIX* JP);
	//-----------------------------------------

	void takeStep(Solver*, vector<Constraint*> *, vector<VertexToEllipseCollision*> *, double, double);

	void enableMouseForce(vec3 mousePosition);
	void updateMouseForce(vec3 mousePosition);
	void disableMouseForce();
	bool isMouseEnabled();
	int setVertexPos2MousePos();
	void applyMouseConst2Matrices(SPARSE_MATRIX* A, LARGE_VECTOR* b);

	int getMaxNrCollTests();
	int getNrCollTests();
	void setNrCollTests(int);
	void setMaxNrCollTests(int);


	vec3 f_mouse( TriangleMeshVertex* selected );
	vec3 f_spring( vec3 & pa, vec3 & pb, double rl, double Ks);
    vec3 f_damp( vec3 & pa, vec3 & pb, vec3 & va, vec3 & vb, double rl, double Kd);
    void bendForce(TriangleMeshTriangle*, TriangleMeshTriangle*, TriangleMeshVertex*,
    		TriangleMeshVertex*, TriangleMeshEdge*, LARGE_VECTOR*);
    inline mat3 dfdx_spring(vec3 & pa, vec3 & pb, double rl, double Ks);
    inline mat3 dfdx_damp(vec3 & pa, vec3 & pb, vec3 & va, vec3 & vb, double rl, float Kd);
    mat3 dfdv_damp(vec3 & pa, vec3 & pb, double rl, double Kd);

    //Utilities
    vector<double> getPosError();
    vector<double> getVelError();
    vector<double> getTimeOfError();
    int getErrorInd();
    void initializeSyncTimes();


private:
    TriangleMesh* mesh;
    TriangleMesh* _cMesh;	//Correction Mesh: Pulls data from this mesh to correct unstable data in the regular mesh
    LARGE_VECTOR* _x;
    LARGE_VECTOR* _v;
    LARGE_VECTOR* _a;
    SPARSE_MATRIX* _M;
    SPARSE_MATRIX* _S;
    SPARSE_MATRIX* _W;
    SPARSE_MATRIX* _I;
    Material* _mat;
    TriangleMeshVertex* mouseSelected;
    vec3 mouseP;
    vector<bool>_correctedIndices;
    vector<bool>_strainCorrectedIndices;

    //Keep Track of Error Data
    vector<double>_posError;
    vector<double>_velError;
    vector<double>_timeOfError;
    int _errorInd;
    double _syncCounter;
    double _syncStep;

    LARGE_VECTOR* _x0;
    LARGE_VECTOR* _v0;
    int _nrCollTests;
    int _maxNrCollTests;
    float _Kcoll;
    bool _USECOLLJACOBIAN;
};

#endif /* SYSTEM_H_ */
