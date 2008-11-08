/*
 * Solver.cpp
 *
 *  Created on: Oct 23, 2008
 *      Author: njoubert
 */

#include "Solver.h"

/******************************************************************************
 *                                                                             *
 *                 virtual SOLVER                                              *
 *                                                                             *
 *******************************************************************************/

Solver::Solver(TriangleMesh* mesh, int n) {
	_mesh = mesh;
    _f = new LARGE_VECTOR(n);
    _delx = new LargeVec3Vector(n);
    _delv = new LargeVec3Vector(n);
}

Solver::~Solver() {

}

LARGE_VECTOR* Solver::getDelx() {
    return _delx;
}

LARGE_VECTOR* Solver::getDelv() {
    return _delv;
}

LARGE_VECTOR* Solver::getf() {
    return _f;
}
/******************************************************************************
 *                                                                             *
 *                 ImplicitSolver                                              *
 *                                                                             *
 *******************************************************************************/
/*

ImplicitSolver::ImplicitSolver(TriangleMesh* mesh, int n) : Solver(mesh,n) {

}

ImplicitSolver::~ImplicitSolver() {

}

void ImplicitSolver::calculateState(System* sys) {
    sys->calculateInternalForces();
    sys->calculateExternalForces();
}

void ImplicitSolver::solve(System* sys, double timeStep,
        int pointIndex, TriangleMeshVertex* point) {

	TriangleMeshVertex* a = point;
	double h = timeStep;

	double m = a->getm();
	mat3 S = sys->calculateContraints(pointIndex);
	mat3 W = (1/m)*S;
	vec3 Z(0,0,0);

	vec3 v0 = a->getvX(); mat3 I = identity2D();
	vec3 F = a->getF();
	std::pair<mat3,mat3> partials = sys->calculateForcePartials(pointIndex);
	mat3 dFx = partials.first;
	mat3 dFv = partials.second;

	mat3 A = I - h*W*dFv - h*h*W*dFx;
	vec3 b = h*W*(F + h*dFx*v0) + Z;
	vec3 deltaV = A.inverse()*b;
	vec3 deltaX = h*(v0 + deltaV);

	//cout << "Forces on particle " << pointIndex << " is (" << F[0] << ", " << F[1] << ", " << F[2] << ")" << endl;

	//return make_pair(deltaX, deltaV);

}
*/

/******************************************************************************
 *                                                                             *
 *                 ExplicitSolver                                              *
 *                                                                             *
 *******************************************************************************/
/*
ExplicitSolver::ExplicitSolver(TriangleMesh* mesh, int n) : Solver(mesh, n) {
}

ExplicitSolver::~ExplicitSolver() {

}

void ExplicitSolver::calculateState(System* sys) {
    sys->calculateInternalForces();
    sys->calculateExternalForces();
}

void ExplicitSolver::solve(System* sys, double timeStep,
        int pointIndex, TriangleMeshVertex* point) {

	vec3 deltaV = (timeStep ) * point->getF() / (double) point->getm();
    vec3 deltaX = timeStep * (point->getvX() + deltaV);


    //if (sys->getT() > 0.1) {
    //    cout << "deltaV=" << deltaV << endl;
    //    cout << "deltaX=" << deltaX << endl;
    //    cout << "F=" << point->getF() << endl;
    //  cout << "vX=" << point->getvX() << endl;
    //   exit(1);
    //}

    point->clearF();

    //Contraints set in OBJ file
    //if (point->getConstaint() == identity2D())
      //  return make_pair(vec3(0,0,0), vec3(0,0,0)); //Lame contraints for explicit

    //cout << "Forces on particle " << pointIndex << " is (" << F[0] << ", " << F[1] << ", " << F[2] << ")" << endl;
    //----------------------------------------------------
    //return make_pair(deltaX, deltaV);
}
*/

/******************************************************************************
 *                                                                             *
 *                 NewmarkSolver                                               *
 *                                                                             *
 *******************************************************************************/
NewmarkSolver::NewmarkSolver(TriangleMesh* mesh, int n):
        Solver(mesh, n) {
    vector<vector<int> > sparsePattern = MeshTOLargeMatrix::CalculateSparsityPattern(mesh);
    _JP = new SPARSE_MATRIX(n,n,sparsePattern,false);
    _JV = new SPARSE_MATRIX(n,n,sparsePattern,false);
    A = new SPARSE_MATRIX(n,n,sparsePattern,false);
	b = new LARGE_VECTOR(n);
    _gamma = DEFAULT_GAMMA;
}


NewmarkSolver::~NewmarkSolver() {

}

SPARSE_MATRIX* NewmarkSolver::getJP() {
    return _JP;
}

SPARSE_MATRIX* NewmarkSolver::getJV() {
    return _JV;
}

void NewmarkSolver::calculateState(System* sys, vector<Constraint*> *constraints) {
    //Zero our current data structures
    _JP->zeroValues();
    _JV->zeroValues();
    _f->zeroValues();
    _delv->zeroValues();
    _delx->zeroValues();
    A->zeroValues();
    b->zeroValues();

    //TODO: Apply constraints to points right here.
    sys->applyConstraints(this, constraints);

    //Compute _JP, _JV, _f
    sys->calculateInternalForces(this);
    sys->calculateExternalForces(this);
    sys->calculateForcePartials(this);

}

void NewmarkSolver::solve(System* sys, vector<Constraint*> *constraints, double timeStep) {
    //A = M - g*h*JV - g*h^2*JP;
    A->insertMatrixIntoDenserMatrix(*(sys->getM()));
    (*_JV) *= (timeStep*_gamma);            //JV = g*h*JV
    (*_JP) *= (timeStep*timeStep*_gamma);   //JP = g*h^2*JP
    (*A) -= (*_JV);
    (*A) -= (*_JP);

    //b = h*f + g*h^2*v*JP);
    (*_f) *= timeStep;
    (*_JP).postMultiply((*sys->getV()), *b);
    (*b) += (*_f);

    //Apply constraints (filter):
	for (unsigned int i = 0; i < constraints->size(); i++) {
		(*constraints)[i]->applyConstraintToSolverMatrices(A, b);
	}

    //delV = b/A
    simpleCG((*A), (*b), (*_delv), MAX_CG_ITER, MAX_CG_ERR);

    //delX = h*(v + g*delV);
    (*_delx) = (*_delv);
    (*_delx) *= (_gamma);
    (*_delx) += *(sys->getV());
    (*_delx) *= timeStep;

}


