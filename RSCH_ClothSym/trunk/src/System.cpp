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
    TriangleMeshEdge* edge;
    std::vector< std::pair <int, TriangleMeshEdge* > >::const_iterator it =
            mesh->vertices[pointIndex].second->begin();
    while (it != mesh->vertices[pointIndex].second->end()) {
        b = mesh->getVertex((*it).first);
        edge = (*it).second;

        /* Calculate internal forces here.
         * a = first point, b = second point. */
		 
		 vec3 pa = a->getX(); vec3 va = a->getvX();
		 vec3 pb = b->getX(); vec3 vb = a->getvX();
		 double rl  = edge->getRestLength();
		 double Ks = 100; double Kd = 4;
		 
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
    TriangleMeshEdge* edge;
    std::vector< std::pair <int, TriangleMeshEdge* > >::const_iterator it =
            mesh->vertices[pointIndex].second->begin();
    while (it != mesh->vertices[pointIndex].second->end()) {
        b = mesh->getVertex((*it).first);
        edge = (*it).second;

        /* Calculate internal forces here.
         * a = first point, b = second point. */
		
		 vec3 pa = a->getX(); vec3 va = a->getvX();
		 vec3 pb = b->getX(); vec3 vb = a->getvX();
		 double rl  = edge->getRestLength();
		 double Ks = 100; double Kd = 4;
		
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
	mat3 I = identity2D();
	mat3 S = I;

    return S;
}

void System::takeStep(Solver* solver, double timeStep) {
	

    time += timeStep;
}

void System::draw() {

}

std::pair<vec3,vec3> ImplicitSolver::solve(System* sys, int pointIndex) {
	std::pair<vec3,vec3> newState;
	
	TriangleMeshVertex* a = mesh->getVertex(pointIndex);
	
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
	
return(newState);
}

std::pair<vec3,vec3> ExplicitSolver::solve(System* sys, int pointIndex) {


}

