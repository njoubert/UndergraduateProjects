/*
 * Solver.h
 *
 *  Created on: Oct 23, 2008
 *      Author: njoubert
 */

#ifndef SOLVER_H_
#define SOLVER_H_

class Solver;
class ImplicitSolver;
class ExplicitSolver;
class NewmarkSolver;

#include "System.h"
#include "Constraint.h"
#include "connectors/MeshTOLargeMatrix.h"
#include "math/CG.h"


class Solver {
public:
	Solver(TriangleMesh*,int);
    virtual ~Solver();
    virtual void calculateState(System* sys, vector<Constraint*> *, vector<VertexToEllipseCollision*> *, double timeStep)=0;
    virtual void solve(System* sys,vector<Constraint*> *constraints,
            double timeStep, vector<VertexToEllipseCollision*> *) = 0;
    LARGE_VECTOR* getDelx();
    LARGE_VECTOR* getDelv();
    LARGE_VECTOR* getf();
    SPARSE_MATRIX* getJP();
    SPARSE_MATRIX* getJV();
    LARGE_VECTOR* getY();

protected:
	TriangleMesh* _mesh;
	LARGE_VECTOR* _delx;
	LARGE_VECTOR* _delv;
	LARGE_VECTOR* _f;
	SPARSE_MATRIX* _JP;
    SPARSE_MATRIX* _JV;
    LARGE_VECTOR* _y;
};

class ImplicitSolver: public Solver {
public:
	ImplicitSolver(TriangleMesh*, int);
    ~ImplicitSolver();
    void calculateState(System* sys, vector<Constraint*> *constraints,
    		vector<VertexToEllipseCollision*> *collisions);
    void solve(System* sys, vector<Constraint*> *constraints, double timeStep,
    		vector<VertexToEllipseCollision*> *collisions);
    LARGE_VECTOR* getY();
private:
    DEFAULT_CG cg;
	float _gamma;
	SPARSE_MATRIX* _JP;
	SPARSE_MATRIX* _JV;
	LARGE_VECTOR* _y;   //Position modification

	SPARSE_MATRIX* A;
	LARGE_VECTOR* b;
	LARGE_VECTOR* t1;
};
//*
class ExplicitSolver: public Solver {
public:
	ExplicitSolver(TriangleMesh*, int);
    ~ExplicitSolver();
    void calculateState(System* sys, vector<Constraint*> *, vector<VertexToEllipseCollision*> *, double timeStep);
    void solve(System* sys,vector<Constraint*> *constraints,double timeStep, vector<VertexToEllipseCollision*> *);
private:
	DEFAULT_CG cg;
    LARGE_VECTOR* _xTMP;
    LARGE_VECTOR* _vTMP;
    SPARSE_MATRIX* _M;
};
//*/
class NewmarkSolver: public Solver {
public:
	NewmarkSolver(TriangleMesh*, int);
    ~NewmarkSolver();
    LARGE_VECTOR* getY();
    LARGE_VECTOR* getZ();
    void calculateState(System* sys, vector<Constraint*> *, vector<VertexToEllipseCollision*> *, double);
    void solve(System* sys,vector<Constraint*> *, double , vector<VertexToEllipseCollision*> *);

    vec3 calculateNewVelocity(vec3 v0, vec3 delv);
    vec3 calculateNewPosition(vec3 x0, vec3 v0, vec3 delv, double h);
    vec3 calculateNewDelv(vec3 v0, vec3 vnew, double h);


private:
    DEFAULT_CG cg;
	float _gamma;
	//LARGE_VECTOR* _y;   //Position modification
	LARGE_VECTOR* _z; 	//Velocity modification

	SPARSE_MATRIX* A;
	LARGE_VECTOR* b;
	LARGE_VECTOR* t1;
	LARGE_VECTOR* _bmod;
	bool correctMesh;

    LARGE_VECTOR* _xTMP;
    LARGE_VECTOR* _vTMP;
};

class BaraffNewmarkSolver: public Solver {
public:
	BaraffNewmarkSolver(TriangleMesh*, int);
    ~BaraffNewmarkSolver();
    SPARSE_MATRIX* getJP();
    SPARSE_MATRIX* getJV();
    LARGE_VECTOR* getY();
    void calculateState(System* sys, vector<Constraint*> *, vector<VertexToEllipseCollision*> *);
    void solve(System* sys,vector<Constraint*> *constraints,double timeStep, vector<VertexToEllipseCollision*> *);
private:
    DEFAULT_CG cg;
	float _gamma;
	LARGE_VECTOR* _y;   //Position modification

	SPARSE_MATRIX* A;
	LARGE_VECTOR* b;
	LARGE_VECTOR* t1;

};

#endif /* SOLVER_H_ */
