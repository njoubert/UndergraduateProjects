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

class Solver {
public:
	Solver(TriangleMesh*,int);
    virtual ~Solver();
    virtual void calculateState(System* sys)=0;
    virtual std::pair<vec3,vec3> solve(System* sys,
            double timeStep,
            int pointIndex,
            TriangleMeshVertex* point) = 0;
protected:
	TriangleMesh* _mesh;
};

class ImplicitSolver: public Solver {
public:
	ImplicitSolver(TriangleMesh*, int);
    ~ImplicitSolver();
    void calculateState(System* sys);
    std::pair<vec3,vec3> solve(System* sys,
            double timeStep,
            int pointIndex,
            TriangleMeshVertex* point);
};

class ExplicitSolver: public Solver {
public:
	ExplicitSolver(TriangleMesh*, int);
    ~ExplicitSolver();
    void calculateState(System* sys);
    std::pair<vec3,vec3> solve(System* sys,
            double timeStep,
            int pointIndex,
            TriangleMeshVertex* point);
};

class NewmarkSolver: public Solver {
public:
	NewmarkSolver(TriangleMesh*, int);
    ~NewmarkSolver();
    void calculateState(System* sys);
    std::pair<vec3,vec3> solve(System* sys,
            double timeStep,
            int pointIndex,
            TriangleMeshVertex* point);
private:
	float _gamma;
	LARGE_VECTOR _JP;
	LARGE_VECTOR _JV;
	SPARSE_MATRIX _f;
	SPARSE_MATRIX _x;
	SPARSE_MATRIX _v;
};

#endif /* SOLVER_H_ */
