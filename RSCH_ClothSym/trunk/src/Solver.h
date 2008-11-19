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
    virtual void calculateState(System* sys, vector<Constraint*> *, vector<VertexToEllipseCollision*> *)=0;
    virtual void solve(System* sys,vector<Constraint*> *constraints,
            double timeStep) = 0;
    LARGE_VECTOR* getDelx();
    LARGE_VECTOR* getDelv();
    LARGE_VECTOR* getf();
    SPARSE_MATRIX* getJP();
    SPARSE_MATRIX* getJV();

protected:
	TriangleMesh* _mesh;
	LARGE_VECTOR* _delx;
	LARGE_VECTOR* _delv;
	LARGE_VECTOR* _f;
	SPARSE_MATRIX* _JP;
    SPARSE_MATRIX* _JV;
};

/*
class ImplicitSolver: public Solver {
public:
	ImplicitSolver(TriangleMesh*, int);
    ~ImplicitSolver();
    void calculateState(System* sys);
    void solve(System* sys,
            double timeStep);
};

class ExplicitSolver: public Solver {
public:
	ExplicitSolver(TriangleMesh*, int);
    ~ExplicitSolver();
    void calculateState(System* sys);
    void solve(System* sys,
            double timeStep);
};
*/
class NewmarkSolver: public Solver {
public:
	NewmarkSolver(TriangleMesh*, int);
    ~NewmarkSolver();
    LARGE_VECTOR* getY();
    void calculateState(System* sys, vector<Constraint*> *, vector<VertexToEllipseCollision*> *);
    void solve(System* sys,vector<Constraint*> *constraints,double timeStep);
private:
    DEFAULT_CG cg;
	float _gamma;
	LARGE_VECTOR* _y;   //Position modification

	SPARSE_MATRIX* A;
	LARGE_VECTOR* b;
	LARGE_VECTOR* t1;

};

#endif /* SOLVER_H_ */
