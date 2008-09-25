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

double System::getT() {
    return time;
}

mat3 System::outerProduct(vec3 a, vec3 b){
	mat3 C;
	int counter = 0;
	for(int i = 0; i < 3; i++) {
		for(int j = 0; j < 3; j++) {
			C[i][j] = a[i]*b[j];
		}
	}
return C;
}

vec3 System::calculateForces(int pointIndex) {
    vec3 F; //Output net force.

	vec3 zero(0,0,0);
	vec3 Finternal(zero);

    TriangleMeshVertex* a = mesh->getVertex(pointIndex);
    TriangleMeshVertex* b;
    std::vector< TriangleMeshEdge* >::const_iterator it = a->getEdgesBeginIterator();
    while (it != a->getEdgesEndIterator()) {
        b = (*it)->getOtherVertex(a);

        /* Calculate internal forces here.
         * a = first point, b = second point. */

        //printVertex(mesh,pointIndex);

		 vec3 pa = a->getX(); vec3 va = a->getvX();
		 vec3 pb = b->getX(); vec3 vb = a->getvX();
		 vec3 Ua = a->getU(); vec3 Ub = b->getU();
		 vec3 RL = Ua - Ub;
		 double rl = RL.length();
		 //double rl  = (*it)->getRestLength();
		 double Ks = 100; double Kd = 100;

		//----------Finternal_i--------------------------------------------------------
			vec3 l = pa - pb;
			double L = l.length();
			vec3 F_spring_i = -Ks * (l/L) * (L - rl);
			vec3 w = va - vb;
			vec3 F_damp_i = -Kd * ((l*w)/(L*L)) * l;
			vec3 Finternal_i = F_spring_i + F_damp_i;

		//----------Finternal Accumulation----------------------------------------------
			Finternal = Finternal + Finternal_i;

        it++;
    }

    /* Calculate external forces here... */
	vec3 Fexternal(0, -9.8, 0);

	F = Finternal + Fexternal;

    return F;
}

std::pair<mat3,mat3> System::calculateForcePartials(int pointIndex) {
    vec3 zero(0,0,0);
	mat3 dFx(zero, zero, zero), dFv(zero, zero, zero);

	//mat3 DFDx(zero, zero, zero); mat3 DFDv(zero, zero, zero);

    TriangleMeshVertex* a = mesh->getVertex(pointIndex);
	TriangleMeshVertex* b;
    std::vector< TriangleMeshEdge* >::const_iterator it = a->getEdgesBeginIterator();
    while (it != a->getEdgesEndIterator()) {
        b = (*it)->getOtherVertex(a);

        /* Calculate internal forces here.
         * a = first point, b = second point. */

		 vec3 pa = a->getX(); vec3 va = a->getvX();
		 vec3 pb = b->getX(); vec3 vb = a->getvX();
		 vec3 Ua = a->getU(); vec3 Ub = b->getU();
         vec3 RL = Ua - Ub;
         double rl = RL.length();
		 //double rl  = (*it)->getRestLength();
		 double Ks = 100; double Kd = 100;

		 //----------DFsDx_i-----------------------------------------------------
		 	mat3 I = identity2D();
			vec3 l = pa - pb;
			double L = l.length();
			mat3 oProd = outerProduct(l,l);
			double lDOT = l*l;
			mat3 DFsDx_i =  -Ks * ((1 - (rl/L)) * (I - (oProd/lDOT)) + (oProd/lDOT));

		//----------DFdDx_i--------------------------------------------------------
			//mat3 I = identity2D();
			vec3 P = pa - pb;
			vec3 V = va - vb;
			mat3 DFdDx_i = -Kd * ( (outerProduct(P, V)/(P*P)) + (2*((-P*V)/((P*P)*(P*P)))*outerProduct(P, P)) + (((P*V)/(P*P))*I) );

		//----------DFdDv_i--------------------------------------------------------
			//mat3 I = identity2D();
			//vec3 l = pa - pb;
			//mat3 oProd = outerProduct(l,l);
			//double lDOT = l*l;
			mat3 DFdDv_i = -Kd * (oProd/lDOT);

		//----------DfDx, DfDv------------------------------------------------------
			dFx = dFx * (DFsDx_i + DFdDx_i);
			dFv = dFv * DFdDv_i;

        it++;
    }

    /* Calculate external forces here... */

    return make_pair(dFx,dFv);
}

mat3 System::calculateContraints(int pointIndex) {
    TriangleMeshVertex* a = mesh->getVertex(pointIndex);
	mat3 S = a->getConstaint();

	mat3 zero(0);
	if (pointIndex == 0 || pointIndex == 73)
	    S = zero;
    return S;
}

/**
 * This is the entry point for simulation. The algorithm is as follows:
 * FOR EACH POINT:
 *      FOR EACH EDGE OF THIS POINT:
 *          calculate forces
 *          calculate partials of forces
 *          accumulate forces
 *      calculate external forces
 *      calculate external partials
 *      accumulate
 *      calculate constraints
 *
 *      use solver to find delV, delX
 *
 *      save delV, delX
 *
 * FOR EACH POINT:
 *    update x with delX and v with delV
 */
void System::takeStep(Solver* solver, double timeStep) {
    std::vector<std::pair<vec3,vec3> > changes(mesh->vertices.size(),
            make_pair(vec3(0,0,0), vec3(0,0,0)));

    for (unsigned int i = 0; i < mesh->vertices.size(); i++) {
        std::pair<vec3,vec3> deltas =
            solver->solve(this, timeStep, i, mesh->getVertex(i));
        changes[i] = deltas;
    }

    TriangleMeshVertex* v;
    for (unsigned int i = 0; i < mesh->vertices.size(); i++) {
        v = mesh->getVertex(i);
        v->getX() += changes[i].first;
        v->getvX() += changes[i].second;
    }

    time += timeStep;
}

Solver::~Solver() {

}

ImplicitSolver::~ImplicitSolver() {

}

std::pair<vec3,vec3> ImplicitSolver::solve(System* sys, double timeStep, int pointIndex, TriangleMeshVertex* point) {

	TriangleMeshVertex* a = point;
	double h = timeStep;

	double m = a->getm();
	mat3 S = sys->calculateContraints(pointIndex);
	mat3 W = (1/m)*S;
	vec3 Z(0,0,0);

	vec3 v0 = a->getvX(); mat3 I = identity2D();
	vec3 F = sys->calculateForces(pointIndex);
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

ExplicitSolver::~ExplicitSolver() {

}

std::pair<vec3,vec3> ExplicitSolver::solve(System* sys, double timeStep, int pointIndex, TriangleMeshVertex* point) {

    vec3 F = sys->calculateForces(pointIndex);
    vec3 deltaV = (timeStep / point->getm()) * F;
    vec3 deltaX = timeStep * (point->getvX() + deltaV);

    //cout << "Forces on particle " << pointIndex << " is (" << F[0] << ", " << F[1] << ", " << F[2] << ")" << endl;

    return make_pair(deltaX, deltaV);
}

