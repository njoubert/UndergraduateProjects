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

class NewmarkSolver: public Solver {
    ~NewmarkSolver();
    void calculateState(System* sys);
    std::pair<vec3,vec3> solve(System* sys,
            double timeStep, int pointIndex, TriangleMeshVertex* point);
};

#endif /* SOLVER_H_ */
