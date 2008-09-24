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
    vec3 F; //Output net force.

    TriangleMeshVertex* a = mesh->getVertex(pointIndex);
    TriangleMeshVertex* b;
    TriangleMeshEdge* edge;
    std::vector< std::pair <int, TriangleMeshEdge* > >::const_iterator it =
            mesh->vertices[pointIndex].second->begin();
    while (it != mesh->vertices[pointIndex].second->end()) {
        b = mesh->getVertex((*it).first);
        edge = (*it).second;

        /* Calculate internal forces here.
         * a = first point, b = second point. */

        it++;
    }

    /* Calculate external forces here... */


    return F;
}

std::pair<mat3,mat3> System::calculateForcePartials(int pointIndex) {
    mat3 dFx, dFv;

    TriangleMeshVertex* a = mesh->getVertex(pointIndex);
    TriangleMeshVertex* b;
    TriangleMeshEdge* edge;
    std::vector< std::pair <int, TriangleMeshEdge* > >::const_iterator it =
            mesh->vertices[pointIndex].second->begin();
    while (it != mesh->vertices[pointIndex].second->end()) {
        b = mesh->getVertex((*it).first);
        edge = (*it).second;

        /* Calculate internal forces here.
         * a = first point, b = second point. */

        it++;
    }

    /* Calculate external forces here... */

    return make_pair(dFx,dFv);
}

mat3 System::calculateContraints(int pointIndex) {
    TriangleMeshVertex* vertex = mesh->getVertex(pointIndex);
    mat3 S;

    TriangleMeshVertex* a = mesh->getVertex(pointIndex);

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

