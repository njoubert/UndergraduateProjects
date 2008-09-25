/*
 * System.cpp
 *
 *  Created on: Sep 7, 2008
 *      Author: njoubert
 */

#include "System.h"

vec3 System::f_spring( vec3 pa, vec3 pb, double rl, double Ks){
    vec3 l = pa - pb;
    double L = l.length();

    double e = .0000001;
    if (L < rl + e && L > rl - e)
        L = rl;

    //cout<<"Spring"<<endl<<"L "<<L<<endl<<"RL "<<rl<<endl<<"L-rl: "<<L-rl<<endl<<endl;
    vec3 f = -Ks * (l/L) * (L - rl);
    //cout<<"f ";Print(f);
    return(f);
}
vec3 System::f_damp( vec3 pa, vec3 pb, vec3 va, vec3 vb, double rl, double Kd){
    //PrintVec3("pa: ", pa);
    //PrintVec3("pb: ", pb);

    vec3 l = pa - pb;
    //PrintVec3("pa - pb: ", l);
    vec3 w = va - vb;
    //PrintVec3("va - vb: ", w);
    double L = l.length();
    //cout<<"Damping"<<endl<<"L "<<L<<endl<<"RL "<<rl<<endl<<endl;
    //cout<<"Norm[pa-pb]: "<<L<<endl<<endl;
    vec3 f = -Kd * ((l*w)/(L*L)) * l;
    //PrintVec3("Fd: ", f);
    return(f);
}

inline mat3 System::dfdx_spring(vec3 pa, vec3 pb, double rl, double Ks){
    mat3 I = identity2D();
    vec3 l = pa - pb;
    double L = l.length();
    mat3 oProd = outerProduct(l,l);
    double lDOT = l*l;
    return -Ks * ((1 - (rl/L)) * (I - (oProd/lDOT)) + (oProd/lDOT));
}
inline mat3 System::dfdx_damp(vec3 pa, vec3 pb, vec3 va, vec3 vb, double rl, float Kd){
    mat3 I = identity2D();
    vec3 P = pa - pb;
    vec3 V = va - vb;
    return -Kd * ( (outerProduct(P, V)/(P*P)) + (2*((-P*V)/((P*P)*(P*P)))*outerProduct(P, P)) + (((P*V)/(P*P))*I) );
}
mat3 System::dfdv_damp(vec3 pa, vec3 pb, double rl, double Kd){
    mat3 I = identity2D();
    vec3 l = pa - pb;
    mat3 oProd = outerProduct(l,l);
    double lDOT = l*l;
    mat3 RETURN = -Kd * (oProd/lDOT);
    return RETURN;
}


System::System(TriangleMesh* m): mesh(m) {
    time = 0;
    ks = 20;
    kd = 15;
}

double System::getT() {
    return time;
}

mat3 System::outerProduct(vec3 a, vec3 b){
	mat3 C;
	for(int i = 0; i < 3; i++) {
		for(int j = 0; j < 3; j++) {
			C[i][j] = a[i]*b[j];
		}
	}
return C;
}

vec3 System::calculateForces(int pointIndex) {
    vec3 F0(0,0,0); //Output net force.

    TriangleMeshVertex* a = mesh->getVertex(pointIndex);
    TriangleMeshVertex* b;
    std::vector< TriangleMeshEdge* >::const_iterator it = a->getEdgesBeginIterator();
    while (it != a->getEdgesEndIterator()) {
        b = (*it)->getOtherVertex(a);

        /* Calculate internal forces here.
         * a = first point, b = second point. */

		 vec3 pa = a->getX(); vec3 va = a->getvX();
		 vec3 pb = b->getX(); vec3 vb = b->getvX();
		 vec3 Ua = a->getU(); vec3 Ub = b->getU();
		 vec3 RL = Ua - Ub;
		 double rl = RL.length();
		 //double rl  = (*it)->getRestLength();

		//----------Finternal_i--------------------------------------------------------
        vec3 F0i =  f_spring(pa, pb, rl, getKs()) + f_damp(pa, pb, va, vb, rl, getKd());
        // cout<<"F0i "<<F0i[0]<<" "<<F0i[1]<<" "<<F0i[2]<<endl;
         F0 += F0i;

		//----------Finternal Accumulation----------------------------------------------

         it++;
    }

    vec3 Fexternal(0, -9.8, 0);

    F0 = F0 + Fexternal;

    return F0;
}

std::pair<mat3,mat3> System::calculateForcePartials(int pointIndex) {
    vec3 zero(0,0,0);
	mat3 dFx(zero, zero, zero), dFv(zero, zero, zero);

    TriangleMeshVertex* a = mesh->getVertex(pointIndex);
	TriangleMeshVertex* b;
    std::vector< TriangleMeshEdge* >::const_iterator it = a->getEdgesBeginIterator();
    while (it != a->getEdgesEndIterator()) {
        b = (*it)->getOtherVertex(a);

        /* Calculate internal forces here.
         * a = first point, b = second point. */

		 vec3 pa = a->getX(); vec3 va = a->getvX();
		 vec3 pb = b->getX(); vec3 vb = b->getvX();
		 vec3 Ua = a->getU(); vec3 Ub = b->getU();
         vec3 RL = Ua - Ub;
         double rl = RL.length();
		 //double rl  = (*it)->getRestLength();
		 double Ks = getKs(); double Kd = getKd();
		 //CALCULATE FORCES FOR EACH SPRING CONNECTED TO PARTICLE
         mat3 dFsdxi = dfdx_spring(pa, pb, rl, Ks);  //W.R.T. u[i]
         mat3 dFddxi = dfdx_damp(pa, pb, va, vb, rl, Kd);
         mat3 dFddvi = dfdv_damp(pa, pb, rl, Kd);
         mat3 dFdxi = dFsdxi + dFddxi;

         //ADD FORCES FOR EACH SPRING CONNECTED TO PARTICLE
         dFx = dFx * dFddvi;
         dFv = dFv * dFdxi;


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

float System::getKs() {
    return ks;
}

float System::getKd() {
    return kd;
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

    if (pointIndex == 0 || pointIndex == 73)
        return make_pair(vec3(0,0,0), vec3(0,0,0));

    //cout << "Forces on particle " << pointIndex << " is (" << F[0] << ", " << F[1] << ", " << F[2] << ")" << endl;

    return make_pair(deltaX, deltaV);
}

