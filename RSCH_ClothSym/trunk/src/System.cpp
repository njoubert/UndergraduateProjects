/*
 * System.cpp
 *
 *  Created on: Sep 7, 2008
 *      Author: njoubert
 */

#include "System.h"

System::System(TriangleMesh* m): mesh(m) {
    time = 0;
}

vec3 System::calculateForces(int pointIndex) {
    TriangleMeshVertex* vertex = mesh->getVertex(pointIndex);
    vec3 F;


    return F;
}

std::pair<mat3,mat3> System::calculateForcePartials(int pointIndex) {
    TriangleMeshVertex* vertex = mesh->getVertex(pointIndex);
    mat3 dFx, dFv;


    return make_pair(dFx,dFv);
}

mat3 System::calculateContraints(int pointIndex) {
    TriangleMeshVertex* vertex = mesh->getVertex(pointIndex);
    mat3 S;

    return S;
}

void System::takeStep(Solver* solver, double timeStep) {





    time += timeStep;
}

void System::draw() {

}

void ImplicitSolver::solve(System* sys, int pointIndex) {


}

void ExplicitSolver::solve(System* sys, int pointIndex) {


}

