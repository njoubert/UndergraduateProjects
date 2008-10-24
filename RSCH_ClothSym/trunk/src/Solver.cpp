/*
 * Solver.cpp
 *
 *  Created on: Oct 23, 2008
 *      Author: njoubert
 */

#include "Solver.h"

Solver::Solver(TriangleMesh* mesh) {
	_mesh = mesh;
}

Solver::~Solver() {

}

ImplicitSolver::ImplicitSolver(TriangleMesh* mesh) : Solver(mesh) {

}

ImplicitSolver::~ImplicitSolver() {

}

void ImplicitSolver::calculateState(System* sys) {
    sys->calculateInternalForces();
    sys->calculateExternalForces();
}

std::pair<vec3,vec3> ImplicitSolver::solve(System* sys, double timeStep,
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

	return make_pair(deltaX, deltaV);

}

ExplicitSolver::ExplicitSolver(TriangleMesh* mesh) : Solver(mesh) {
}

ExplicitSolver::~ExplicitSolver() {

}

void ExplicitSolver::calculateState(System* sys) {
    sys->calculateInternalForces();
    sys->calculateExternalForces();
}

std::pair<vec3,vec3> ExplicitSolver::solve(System* sys, double timeStep,
        int pointIndex, TriangleMeshVertex* point) {

	vec3 deltaV = (timeStep ) * point->getF() / (double) point->getm();
    vec3 deltaX = timeStep * (point->getvX() + deltaV);

    /*
    if (sys->getT() > 0.1) {
        cout << "deltaV=" << deltaV << endl;
        cout << "deltaX=" << deltaX << endl;
        cout << "F=" << point->getF() << endl;
        cout << "vX=" << point->getvX() << endl;
        exit(1);
    }
    */
    point->clearF();




    //Contraints set in OBJ file
    //if (point->getConstaint() == identity2D())
      //  return make_pair(vec3(0,0,0), vec3(0,0,0)); //Lame contraints for explicit

    //cout << "Forces on particle " << pointIndex << " is (" << F[0] << ", " << F[1] << ", " << F[2] << ")" << endl;
    //******************************************************************************
    return make_pair(deltaX, deltaV);
}

NewmarkSolver::NewmarkSolver(TriangleMesh* mesh) : Solver(mesh) {
	_gamma = DEFAULT_GAMMA;
	int n = mesh->vertices.size();
	_JP(n,n,0,NULL,NULL,NULL);
	_JV(n,n,0,NULL,NULL,NULL);
	_f(n);
	_x(n);
	_v(n);

}

NewmarkSolver::~NewmarkSolver() {

}

void NewmarkSolver::calculateState(System* sys) {

}

std::pair<vec3,vec3> NewmarkSolver::solve(System* sys, double timeStep,
        int pointIndex, TriangleMeshVertex* point) {


}


