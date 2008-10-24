/*
 * System.cpp
 *
 *  Created on: Sep 7, 2008
 *      Author: njoubert
 */

#include "System.h"

/////////////////////////////WHERE IS THIS CALLED////////////////////////////////////
System::System(TriangleMesh* m, Material* mat): mesh(m), _mat(mat)
{
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
    	TriangleMeshVertex** aList = A->getVertices();
    	TriangleMeshVertex** bList = B->getVertices();

        if (A != NULL && B != NULL) {
                assert(A != B);
                //GET NORMALS
               // vec3 NA = A->getNormal();
               // vec3 NB = B->getNormal();
        		//Dirty way to figure out which point is which
        		int ai, bi;
        		for(int i = 0; i < 3; i++){
        			if(aList[i] != a && aList[i] != b)
        				ai = i;

        			if(bList[i] != a && bList[i] != b)
        				bi = i;
        		}

                vec3 x2 = aList[ai]->getX();
                vec3 x1 = bList[bi]->getX();
                vec3 x4 = a->getX();
                vec3 x3 = b->getX();

        		vec3 N1 = (x1 - x3) ^ (x1 - x4);
        		vec3 N2 = (x2 - x4) ^ (x2 - x3);
        		vec3 E = x4 - x3;

                //CALCULATE BEND FORCES
                /*
        		vec3 e = b - a;
                e.normalize();
                vec3 crossNANB = NA^NB;
                double eDotN = crossNANB*e;
                double theta = asin(eDotN);
                //*/
        		double N1mag = N1.length();
        		double N2mag = N2.length();
        		double Emag = E.length();
        		vec3 N1unit = N1/N1mag;
        		vec3 N2unit = N2/N2mag;
        		vec3 Eunit = E/Emag;

        		double sign = ((N1unit ^ N2unit)*Eunit);
        		if(sign > 0)
        			sign = 1;
        		else
        			sign = -1;

        		double mustBePos = abs( 1 - N1unit*N2unit);
        		if(mustBePos < 0)
        			mustBePos = 0;

        		double sinThetaOver2 = sign * sqrt( (mustBePos ) /2);

        		//*
       // 		cout<<"x1: "<<x1<<"  x2: "<<x2<<"  x3: "<<x3<<"  x4: "<<x4<<endl;
       // 		cout<<"N1unit: "<<N1unit<<"  N2unit: "<<N2unit<<"  Eunit: "<<Eunit<<endl;
        		cout<<"Rest sinThetaOver2 "<<sinThetaOver2<<endl;
       // 		cout<<endl;
        		//*/
                (*edg_it)->setRestAngle(sinThetaOver2);
//			    cout<<"Edge: "<<(*it)<<" Rest Angle: "<<(*it)->getRestAngle()<<endl;
//				cout<<endl;
            }

	} while (edg_it.next());
}

//void System::loadMatrices() {



//}


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

void System::Forces(TriangleMeshTriangle* A, TriangleMeshTriangle* B, TriangleMeshVertex* a, TriangleMeshVertex* b, TriangleMeshEdge* edge){
	TriangleMeshVertex** aList = A->getVertices();
	TriangleMeshVertex** bList = B->getVertices();

	if(A != NULL && B != NULL && A != B){

		//Dirty way to figure out which point is which
		int ai, bi;
		for(int i = 0; i < 3; i++){
			if(aList[i] != a && aList[i] != b)
				ai = i;

			if(bList[i] != a && bList[i] != b)
				bi = i;
		}

//*
		vec3 x2 = aList[ai]->getX();
		vec3 x1 = bList[bi]->getX();
		vec3 x4 = a->getX();
		vec3 x3 = b->getX();

		vec3 v2 = aList[ai]->getvX();
		vec3 v1 = bList[bi]->getvX();
		vec3 v4 = a->getvX();
		vec3 v3 = b->getvX();

		vec3 N1 = (x1 - x3) ^ (x1 - x4);
		vec3 N2 = (x2 - x4) ^ (x2 - x3);
		//vec3 N2 = A->getNormal();
		//vec3 N1 = B->getNormal();
		vec3 E = x4 - x3;

		double N1mag = N1.length();
		double N2mag = N2.length();
		double Emag = E.length();
		//if(N1mag > 1)
	//		N1mag = 1;
	//	if(N2mag > 1)
	//		N2mag = 1;
	//	if(Emag > 1)
	//		Emag = 1;
		//vec3 N1unit = N1.normalize();
		//vec3 N2unit = N2.normalize();
		//vec3 Eunit = E.normalize();
		vec3 N1unit = N1/N1mag;
		vec3 N2unit = N2/N2mag;
		vec3 Eunit = E/Emag;

		vec3 u[4];
		u[0] = Emag * (N1/(N1mag * N1mag));
		u[1] = Emag * (N2/(N2mag * N2mag));
		u[2] =  ( ((x1-x4)*E)/Emag ) * (N1/(N1mag*N1mag)) + ( ((x2-x4)*E)/Emag )*(N2/(N2mag*N2mag));
		u[3] = -( ((x1-x3)*E)/Emag ) * (N1/(N1mag*N1mag)) - ( ((x2-x3)*E)/Emag )*(N2/(N2mag*N2mag));

		double sign = ((N1unit ^ N2unit)*Eunit);
		if(sign > 0)
			sign = 1;
		else
			sign = -1;

		double mustBePos = abs( 1 - N1unit*N2unit);
		if(mustBePos < 0)
			mustBePos = 0;

		double sinThetaOver2 = sign * sqrt( (mustBePos ) /2);

		vec3 Fe[4];
		for(int i = 0; i < 4; i++)
			Fe[i] = _mat->_kbe * ((Emag*Emag)/(N1mag + N2mag)) * sinThetaOver2 * u[i];

		vec3 Fd[4];
		double DthetaDt = u[0]*v1 + u[1]*v2 + u[2]*v3 + u[3]*v4;
		if(DthetaDt < 0.1 && DthetaDt > -0.1)
			DthetaDt = 0;

		for(int i = 0; i < 4; i++)
			Fd[i] = -_mat->_kbd * Emag * DthetaDt * u[i];
	/*
	//	if(!(F[0][0] < 0.001 && F[0][0] > -.001)) {
	//		cout<<"x1: "<<x1<<"  x2: "<<x2<<"  x3: "<<x3<<"  x4: "<<x4<<endl;
	//		exit(1);
	//		cout<<"N1: "<<N1<<"  N2: "<<N2<<"  E: "<<E<<endl;
			if(N1unit.length() > 1 || N2unit.length() > 1 || Eunit.length() > 1 || (sinThetaOver2 > 2.2*3.14 || sinThetaOver2 < -2.2*3.14)) {
	//		if(sinThetaOver2 < .01) {
			cout<<"Unit Normal is > 1!!!!! --- BREAK"<<endl;
			cout<<"N1unit: "<<N1unit<<"  N2unit: "<<N2unit<<"  Eunit: "<<Eunit<<endl;
			cout<<"N1mag: "<<N1mag<<"  N2mag: "<<N2mag<<"  Emag: "<<Emag<<endl;
			cout<<"N1unit_length: "<<N1unit.length()<<"  N2unit_length: "<<N2unit.length()<<"  Eunit_length: "<<Eunit.length()<<endl;
	//		cout<<"normalDotProd: "<<N1unit*N2unit<<endl<<endl;
	//		exit(0);
			}
	//		cout<<"N1unit: "<<N1unit<<"  N2unit: "<<N2unit<<"  Eunit: "<<Eunit<<endl;
	//		cout<<"N1unit: "<<N1unit.length()<<"  N2unit: "<<N2unit.length()<<"  Eunit: "<<Eunit.length()<<endl;
	//		exit(1);
	//		cout<<"U1: "<<u[0]<<"  U2: "<<u[1]<<"  U3: "<<u[2]<<"  U4: "<<u[3]<<endl;
	//		if(sinThetaOver2 > 0.01)
	//		cout<<"Sign: "<<sign<<"   Theta: "<<sinThetaOver2<<endl;
	//		cout<<"F1: "<<F[0]<<"  F2: "<<F[1]<<"  F3: "<<F[2]<<"  F4: "<<F[3]<<endl;
	//		cout<<endl;

	//	}
	//*/
		vec3 F[4];
		for(int i = 0; i < 4; i++)
			F[i] = Fe[i] + Fd[i];

		aList[ai]->setF(F[1]);
		bList[bi]->setF(F[0]);
		a->setF(F[3]);
		b->setF(F[2]);
	}
//*/
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

		vec3 Fa = -_mat->_kbe * (theta) * (NA/NA.length());
		vec3 Fb = -_mat->_kbe * (theta) * (NB/NB.length());

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
		vec3 Fdampa = -_mat->_kd* (v.length2()*theta) * (NA/NA.length());
		vec3 Fdampb = -_mat->_kd * (v.length2()*theta) * (NB/NB.length());

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
//		f_bend((*edg_it)->getParentTriangle(0), (*edg_it)->getParentTriangle(1), a, b, (*edg_it));

		//----------Finternal_i--------------------------------------------------------
       F0 =  f_spring(pa, pb, rl, _mat->_ks) + f_damp(pa, pb, va, vb, rl, _mat->_kd);
        // cout<<"F0i "<<F0i[0]<<" "<<F0i[1]<<" "<<F0i[2]<<endl;

       //Store Forces
       a->setF(F0);
       b->setF(-1 * F0);

       //VECTOR3_ADD( m_TotalForces_int.m_pData[m_iParticle[0]], force, m_TotalForces_int.m_pData[m_iParticle[0]] );
       //VECTOR3_ADD( m_TotalForces_int.m_pData[m_iParticle[1]], force, m_TotalForces_int.m_pData[m_iParticle[1]] );

	 Forces((*edg_it)->getParentTriangle(0), (*edg_it)->getParentTriangle(1), a, b, (*edg_it));

    } while (edg_it.next());
    //cout<<"Fpoint: "<<F0.length()<<endl;
}

void System::calculateExternalForces() {
    TriangleMeshVertex* a;
    vec3 gravity(0, GRAVITY, 0);
 //   vec3 gravity(0, 0, GRAVITY);
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
    vec3 f = -_mat->_ks * (l/L) * (L - rl);
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
		 //CALCULATE FORCES FOR EACH SPRING CONNECTED TO PARTICLE
         mat3 dFsdxi = dfdx_spring(pa, pb, rl, _mat->_ks);  //W.R.T. u[i]
         mat3 dFddxi = dfdx_damp(pa, pb, va, vb, rl, _mat->_kd);
         mat3 dFddvi = dfdv_damp(pa, pb, rl, _mat->_kd);
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
        v->getX() += changes[i].first;
        v->getvX() += changes[i].second;
        //**********************CONSTRAINTS********************************************
/*
        vec3 p = v->getX();
        vec3 V = v->getvX();
        vec3 table(0,.41,0);
        float tableR = .5;
        float eps = .01;
        float eps2 = 4;
        if((sqrt(p[0]*p[0] + p[2]*p[2]) < tableR) &&
        		(p[1] < table[1] + eps && p[1] > table[1] - eps)){

			if(v->getvX()[1] > eps) {
				v->getX()[1] = table[1]-eps2*eps;
				v->getvX() *= -.1;
			}

			else
				v->getX()[1]  = table[1];

        }
        else{
        v->getX() += changes[i].first;
        v->getvX() += changes[i].second;
        }
 //*/
    }

}

