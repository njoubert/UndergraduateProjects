/*
 * System.cpp
 *
 *  Created on: Sep 7, 2008
 *      Author: njoubert
 */

#include "System.h"
#define GRAVITY -9.8

System::System(TriangleMesh* m): mesh(m) {
    time = 0;
    ks = 10000;
    kd = .7;
    //kb = 3;
    kb = 0;
    mouseSelected = NULL;


//*******************GET REST ANGLE AND STORE IT IN MESH************************************
    TriangleMeshVertex *a, *b;
    TriangleMeshTriangle *A, *B;

    EdgesIterator edg_it = mesh->getEdgesIterator();
    do {
        a = (*edg_it)->getVertex(0);
        b = (*edg_it)->getVertex(1);
        A = (*edg_it)->getParentTriangle(0);
        B = (*edg_it)->getParentTriangle(1);

        if (A != NULL && B != NULL) {
                assert(A != B);
                //GET NORMALS
                vec3 NA = A->getNormal();
                vec3 NB = B->getNormal();

                //CALCULATE BEND FORCES
                vec3 e = b - a;
                e.normalize();
                vec3 crossNANB = NA^NB;
                double eDotN = crossNANB*e;
                double theta = asin(eDotN);

                (*edg_it)->setRestAngle(theta);
//			    cout<<"Edge: "<<(*it)<<" Rest Angle: "<<(*it)->getRestAngle()<<endl;
//				cout<<endl;
            }

	} while (edg_it.next());
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

vec3 System::f_spring( vec3 & pa, vec3 & pb, double rl, double Ks){
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
vec3 System::f_damp( vec3 & pa, vec3 & pb, vec3 & va, vec3 & vb, double rl, double Kd){
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

void System::f_bend(TriangleMeshTriangle* A, TriangleMeshTriangle* B, TriangleMeshVertex* a, TriangleMeshVertex* b, TriangleMeshEdge* edge){
	vec3 F(0);
	if(A != NULL && B != NULL && A != B){
		//GET NORMALS
		vec3 NA = A->getNormal();
		vec3 NB = B->getNormal();

		//CALCULATE BEND FORCES
		vec3 e = b - a;
		e.normalize();
		vec3 crossNANB = NA^NB;
		double eDotN = crossNANB*e;
		double theta = asin(eDotN);
	//	cout<<"Edge: "<<edge<<" Theta: "<<theta<<endl;


		//BUGGED CODE FOR REST ANGLE
	//	vec3 Fa = -getKb() * (abs(theta) - abs(edge->getRestAngle())) * (NA/NA.length());
//		vec3 Fb = -getKb() * (abs(theta) - abs(edge->getRestAngle())) * (NB/NB.length());

		vec3 Fa = -getKb() * (theta) * (NA/NA.length());
		vec3 Fb = -getKb() * (theta) * (NB/NB.length());

		//FIND WHICH VERTEXES FORCES MUST BE APPLIED TO AND STORE THEM
		TriangleMeshVertex** ai = A->getVertices();
		TriangleMeshVertex** bi = B->getVertices();
		/*
		cout<<endl;cout<<"a: "<<endl;cout<<a<<endl;cout<<"b: "<<endl;cout<<b<<endl;
		cout<<"ai: "<<endl;cout<<ai[0]<<endl;cout<<ai[1]<<endl;cout<<ai[2]<<endl;
		cout<<"bi: "<<endl;cout<<bi[0]<<endl;cout<<bi[1]<<endl;cout<<bi[2]<<endl;
		//*/
		int save_ai_Index;
		int save_bi_Index;
		for(int i = 0; i < 3; i++){
			if(ai[i] != a && ai[i] != b){
				save_ai_Index = i;
				//ai[i]->setF(Fa);
				//cout<<"ai: "<<ai[i]<<" Fa: "<<Fa.length()<<endl;
			}
			if(bi[i] != a && bi[i] != b){
				save_bi_Index = i;
				//bi[i]->setF(Fb;
				//cout<<"bi: "<<bi[i]<<" Fb: "<<Fb.length()<<endl;
			}

		}
		vec3 v = ai[save_ai_Index]->getvX() - bi[save_bi_Index]->getvX();

		//BUGGED CODE MUST GET CORRECT EQUATION FOR DAMPING FORCE ON A ANGULAR SPRING
		vec3 Fdampa = -getKd()* (v.length2()*theta) * (NA/NA.length());
		vec3 Fdampb = -getKd() * (v.length2()*theta) * (NB/NB.length());

		ai[save_ai_Index]->setF(Fa);// + Fdampa);
		bi[save_bi_Index]->setF(Fb);// + Fdampb);

		//cout<<endl;
	}


	//return(F);
}

inline mat3 System::dfdx_spring(vec3 & pa, vec3 & pb, double rl, double Ks){
    mat3 I = identity2D();
    vec3 l = pa - pb;
    double L = l.length();
    mat3 oProd = outerProduct(l,l);
    double lDOT = l*l;
    return -Ks * ((1 - (rl/L)) * (I - (oProd/lDOT)) + (oProd/lDOT));
}
inline mat3 System::dfdx_damp(vec3 & pa, vec3 & pb, vec3 & va, vec3 & vb, double rl, float Kd){
    mat3 I = identity2D();
    vec3 P = pa - pb;
    vec3 V = va - vb;
    return -Kd * ( (outerProduct(P, V)/(P*P)) + (2*((-P*V)/((P*P)*(P*P)))*outerProduct(P, P)) + (((P*V)/(P*P))*I) );
}
mat3 System::dfdv_damp(vec3 & pa, vec3 & pb, double rl, double Kd){
    mat3 I = identity2D();
    vec3 l = pa - pb;
    mat3 oProd = outerProduct(l,l);
    double lDOT = l*l;
    mat3 RETURN = -Kd * (oProd/lDOT);
    return RETURN;
}

void System::calculateInternalForces() {
    vec3 F0(0,0,0); //Output net force.
    TriangleMeshVertex *a, *b;
    EdgesIterator edg_it = mesh->getEdgesIterator();
    do {
        a = (*edg_it)->getVertex(0);
        b = (*edg_it)->getVertex(1);

        /* Calculate internal forces here.
         * a = first point, b = second point. */

		 vec3 pa = a->getX(); vec3 va = a->getvX();
		 vec3 pb = b->getX(); vec3 vb = b->getvX();
		 vec3 Ua = a->getU(); vec3 Ub = b->getU();
		 vec3 RL = Ua - Ub;

		 double rl = RL.length();

		//CALCULATES BEND FORCE AND STORES IT IN MESH
		f_bend((*edg_it)->getParentTriangle(0), (*edg_it)->getParentTriangle(1), a, b, (*edg_it));

		//----------Finternal_i--------------------------------------------------------
        F0 =  f_spring(pa, pb, rl, getKs()) + f_damp(pa, pb, va, vb, rl, getKd());
        // cout<<"F0i "<<F0i[0]<<" "<<F0i[1]<<" "<<F0i[2]<<endl;

        a->setF(F0);
        b->setF(-1 * F0);
    } while (edg_it.next());
    //cout<<"Fpoint: "<<F0.length()<<endl;
}

void System::calculateExternalForces() {
    TriangleMeshVertex* a;
    vec3 gravity(0, GRAVITY, 0);
    for (int i = 0; i < mesh->countVertices(); i++) {
        a = mesh->getVertex(i);
        a->setF(gravity*a->getm() += f_mouse(a));
    }
}

void System::enableMouseForce(vec3 mousePosition) {
    //Find closest vertex
    //cout << "Mouse is at: " << mousePosition << endl;
    double currentDistance = 900000.0, d;
    TriangleMeshVertex* currentClosest;
    for (unsigned int i = 0; i < mesh->vertices.size(); i++) {
        d = (mousePosition - mesh->getVertex(i)->getX()).length();
        if (d < currentDistance) {
            currentDistance = d;
            currentClosest = mesh->getVertex(i);
            mouseP = mousePosition;
        }
    }

    //sets mouseSelected to this vertex.
    mouseSelected = currentClosest;
}

void System::updateMouseForce(vec3 mousePosition) {
    mouseP = mousePosition;
}

void System::disableMouseForce() {
    mouseSelected = NULL;
}

bool System::isMouseEnabled() {
    return (mouseSelected != NULL);
}

vec3 System::f_mouse( TriangleMeshVertex* selected ) {
    if (selected != mouseSelected)
        return vec3(0,0,0);

    double rl = 1;
    vec3 l = selected->getX() - mouseP;
    double L = l.length();
    vec3 f = -getKs() * (l/L) * (L - rl);
    return(f);
}

std::pair<mat3,mat3> System::calculateForcePartials(int pointIndex) {
	mat3 dFx(0), dFv(0);
	dFx = identity2D();
	dFv = identity2D();

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

/**
 * @return Returns the S contrain matrix
 */
mat3 System::calculateContraints(int pointIndex) {
    TriangleMeshVertex* a = mesh->getVertex(pointIndex);
	return a->getConstaint();
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

    solver->calculateState(this); //evalDeriv function

    for (unsigned int i = 0; i < mesh->countVertices(); i++) {
        std::pair<vec3,vec3> deltas =
            solver->solve(this, timeStep, i, mesh->getVertex(i));
        changes[i] = deltas;
    }

    TriangleMeshVertex* v;
    for (unsigned int i = 0; i < mesh->vertices.size(); i++) {
        v = mesh->getVertex(i);

        //**********************CONSTRAINTS********************************************
        vec3 p = v->getX();
        vec3 V = v->getvX();
        vec3 table(0,.41,0);
        float tableR = .5;
        float eps = .01;
        float eps2 = 4;
        if((sqrt(p[0]*p[0] + p[2]*p[2]) < tableR) &&
        		(p[1] < table[1] + eps && p[1] > table[1] - eps)){

			if(v->getvX()[1] > 0) {
				v->getX()[1] = table[1]-eps2*eps;
				v->getvX() *= -.1;
			}

			else
				v->getX()[1]  = table[1];
			//v->getvX() = 0;
        }
        else{
        v->getX() += changes[i].first;
        v->getvX() += changes[i].second;
        }
    }

    time += timeStep;
}

float System::getKs() {
    return ks;
}

float System::getKd() {
    return kd;
}

float System::getKb() {
	return kb;
}

Solver::~Solver() {

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

    if (point->getConstaint() == identity2D())
        return make_pair(vec3(0,0,0), vec3(0,0,0)); //Lame contraints for explicit

    //cout << "Forces on particle " << pointIndex << " is (" << F[0] << ", " << F[1] << ", " << F[2] << ")" << endl;

    return make_pair(deltaX, deltaV);
}

