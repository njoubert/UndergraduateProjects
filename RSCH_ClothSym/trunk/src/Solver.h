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
#include "connectors/MeshTOLargeMatrix.h"

class Solver {
public:
	Solver(TriangleMesh*,int);
    virtual ~Solver();
    virtual void calculateState(System* sys)=0;
    virtual void solve(System* sys,
            double timeStep) = 0;
    LARGE_VECTOR* getDelx();
    LARGE_VECTOR* getDelv();
    LARGE_VECTOR* getf();

protected:
	TriangleMesh* _mesh;
	LARGE_VECTOR* _delx;
	LARGE_VECTOR* _delv;
	LARGE_VECTOR* _f;
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
    SPARSE_MATRIX* getJP();
    SPARSE_MATRIX* getJV();
    void calculateState(System* sys);
    void solve(System* sys,double timeStep);
private:
	float _gamma;
	SPARSE_MATRIX* _JP;
	SPARSE_MATRIX* _JV;
	SPARSE_MATRIX* A;
	LARGE_VECTOR* b;

};

#endif /* SOLVER_H_ */
