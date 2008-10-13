/*
 * System.cpp
 *
 *  Created on: Sep 7, 2008
 *      Author: njoubert
 */

#include "System.h"
#define GRAVITY -9.8

/////////////////////////////WHERE IS THIS CALLED////////////////////////////////////
System::System(TriangleMesh* m, int verticeCount): mesh(m),
	m_Positions( verticeCount ), m_Velocities( verticeCount ),
	m_dv( verticeCount ), m_dp( verticeCount ),
	m_vTemp1( verticeCount ), m_vTemp2( verticeCount ), m_PosTemp( verticeCount ),
	m_TotalForces( verticeCount ),
	m_Masses( verticeCount ), m_MassInv( verticeCount ),
	m_MxMasses( verticeCount, verticeCount ),
	m_TotalForces_dp( verticeCount, verticeCount ),
	m_TotalForces_dv( verticeCount, verticeCount ),
	m_A( verticeCount, verticeCount ),
	m_P( verticeCount ),
	m_PInv( verticeCount ),
	m_MxTemp1( verticeCount, verticeCount ), m_MxTemp2( verticeCount, verticeCount ),
	m_z( verticeCount ), m_b( verticeCount ), m_r( verticeCount ),
	m_c( verticeCount ), m_q( verticeCount ), m_s( verticeCount ), m_y( verticeCount )
{
	numVertices = mesh->countVertices();
	m_S = new Physics_Matrix3x3[numVertices];
	mat3 MatrixConverter;
	for(int i = 0; i < numVertices; i++){
		//*
		MatrixConverter	 = calculateContraints(i);
		m_S[i].m_Mx[0] =  MatrixConverter[0][0];
		m_S[i].m_Mx[1] =  MatrixConverter[0][1];
		m_S[i].m_Mx[2] =  MatrixConverter[0][2];
		m_S[i].m_Mx[3] =  MatrixConverter[1][0];
		m_S[i].m_Mx[4] =  MatrixConverter[1][1];
		m_S[i].m_Mx[5] =  MatrixConverter[1][2];
		m_S[i].m_Mx[6] =  MatrixConverter[2][0];
		m_S[i].m_Mx[7] =  MatrixConverter[2][1];
		m_S[i].m_Mx[8] =  MatrixConverter[2][2];

		//*/
//		m_S[i].SetIdentity();
	}
	//m_S->Zero();

    time = 0;
 //    ks = 20000;
 //   kd = 20;
//	ks = 5000;
//	kd = .05;
 //   Ks = 7;
 //   Kd = 3;
 //   Kbe = 10;
  //  Kbd = 9;
    Ks  = 20;
    Kd  = 10;
    Kbe = .1;
    Kbd = .01;
    //kb = 0;
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
        		if(mustBePos < 0.000001)
        			mustBePos = 0;

        		double sinThetaOver2 = sign * sqrt( (mustBePos ) /2);

        		//*
        		// 		cout<<"x1: "<<x1<<"  x2: "<<x2<<"  x3: "<<x3<<"  x4: "<<x4<<endl;
        		cout<<"N1unit: "<<N1unit<<"  N2unit: "<<N2unit<<"  Eunit: "<<Eunit<<endl;
        		cout<<"N1unit Length: "<<N1unit.length();
        		cout<<"    N2unit Length: "<<N2unit.length()<<endl;
        		cout<<"mustBePos: "<<mustBePos<<endl;
        		cout<<"Rest sinThetaOver2 "<<sinThetaOver2<<endl;
        		cout<<endl;
        		//*/
                (*edg_it)->setRestAngle(sinThetaOver2);
//			    cout<<"Edge: "<<(*it)<<" Rest Angle: "<<(*it)->getRestAngle()<<endl;
//				cout<<endl;
            }

	} while (edg_it.next());
}

/***
 * Copies state from mesh to matrices used for numerical integration.
 */
void System::loadMatrices() {
	//if(numVertices == m_Positions.Size())
	//	cout<<"SUPER"<<endl;
//	else
//		cout<<"oh crap"<<endl;

	double a, b, c;
	for(int i = 0; i < numVertices; i++)
	{
		m_Positions.m_pData[i].x = mesh->getVertex(i)->getX()[0];
		m_Positions.m_pData[i].y = mesh->getVertex(i)->getX()[1];
		m_Positions.m_pData[i].z = mesh->getVertex(i)->getX()[2];

		m_Velocities.m_pData[i].x = mesh->getVertex(i)->getvX()[0];
		m_Velocities.m_pData[i].y = mesh->getVertex(i)->getvX()[1];
		m_Velocities.m_pData[i].z = mesh->getVertex(i)->getvX()[2];

		if(m_S[i].m_Mx[0] == 0 && m_S[i].m_Mx[4] == 0 && m_S[i].m_Mx[8] == 0) {
			m_Masses.m_pData[i].x = 0;
			m_Masses.m_pData[i].y = 0;
			m_Masses.m_pData[i].z = 0;
		}
		else{
			m_Masses.m_pData[i].x = 1/mesh->getVertex(i)->getm();
			m_Masses.m_pData[i].y = 1/mesh->getVertex(i)->getm();
			m_Masses.m_pData[i].z = 1/mesh->getVertex(i)->getm();
		}

		m_MxMasses(i,i).m_Mx[0] = mesh->getVertex(i)->getm();
		m_MxMasses(i,i).m_Mx[4] = mesh->getVertex(i)->getm();
		m_MxMasses(i,i).m_Mx[8] = mesh->getVertex(i)->getm();;

		m_TotalForces.m_pData[i].x = mesh->getVertex(i)->getF()[0];
		m_TotalForces.m_pData[i].y = mesh->getVertex(i)->getF()[1];
		m_TotalForces.m_pData[i].z = mesh->getVertex(i)->getF()[2];

	    //for(int j=0; j < 2; j++ ) {
	    	//for(int k=j; k < 2 ; k++ ) {

		m_TotalForces_dp(i,i).m_Mx[0] = mesh->getVertex(i)->getDfDx()[0][0];
		m_TotalForces_dp(i,i).m_Mx[1] = mesh->getVertex(i)->getDfDx()[0][1];
		m_TotalForces_dp(i,i).m_Mx[2] = mesh->getVertex(i)->getDfDx()[0][2];
		m_TotalForces_dp(i,i).m_Mx[3] = mesh->getVertex(i)->getDfDx()[1][0];
		m_TotalForces_dp(i,i).m_Mx[4] = mesh->getVertex(i)->getDfDx()[1][1];
		m_TotalForces_dp(i,i).m_Mx[5] = mesh->getVertex(i)->getDfDx()[1][2];
		m_TotalForces_dp(i,i).m_Mx[6] = mesh->getVertex(i)->getDfDx()[2][0];
		m_TotalForces_dp(i,i).m_Mx[7] = mesh->getVertex(i)->getDfDx()[2][1];
		m_TotalForces_dp(i,i).m_Mx[8] = mesh->getVertex(i)->getDfDx()[2][2];

		m_TotalForces_dv(i,i).m_Mx[0] = mesh->getVertex(i)->getDfDv()[0][0];
		m_TotalForces_dv(i,i).m_Mx[1] = mesh->getVertex(i)->getDfDv()[0][1];
		m_TotalForces_dv(i,i).m_Mx[2] = mesh->getVertex(i)->getDfDv()[0][2];
		m_TotalForces_dv(i,i).m_Mx[3] = mesh->getVertex(i)->getDfDv()[1][0];
		m_TotalForces_dv(i,i).m_Mx[4] = mesh->getVertex(i)->getDfDv()[1][1];
		m_TotalForces_dv(i,i).m_Mx[5] = mesh->getVertex(i)->getDfDv()[1][2];
		m_TotalForces_dv(i,i).m_Mx[6] = mesh->getVertex(i)->getDfDv()[2][0];
		m_TotalForces_dv(i,i).m_Mx[7] = mesh->getVertex(i)->getDfDv()[2][1];
		m_TotalForces_dv(i,i).m_Mx[8] = mesh->getVertex(i)->getDfDv()[2][2];

	    	//}
	    //}
	}
	m_Masses.Invert(m_MassInv);
/*
	for(int i = 0; i < numVertices; i++) {
		if(!(m_Positions.m_pData[i].x == mesh->getVertex(i)->getX()[0] &&
		     m_Positions.m_pData[i].y == mesh->getVertex(i)->getX()[1] &&
		     m_Positions.m_pData[i].z == mesh->getVertex(i)->getX()[2]))
				 cout<<"WARNING: Positions Loaded with Error (possibly roundoff)"<<endl;
		if(!(m_Velocities.m_pData[i].x == mesh->getVertex(i)->getvX()[0] &&
			 m_Velocities.m_pData[i].y == mesh->getVertex(i)->getvX()[1] &&
			 m_Velocities.m_pData[i].z == mesh->getVertex(i)->getvX()[2]))
				 cout<<"WARNING: Velocites Loaded with Error (possibly roundoff)"<<endl;
		if(!(m_Masses.m_pData[i].x == 1/mesh->getVertex(i)->getm() &&
			 m_Masses.m_pData[i].x == 1/mesh->getVertex(i)->getm() &&
			 m_Masses.m_pData[i].x == 1/mesh->getVertex(i)->getm()))
				 cout<<"WARNING: Masses Large Vector Loaded with Error (possibly roundoff)"<<endl;
		if(!(m_MxMasses(i,i).m_Mx[0] == mesh->getVertex(i)->getm() &&
			 m_MxMasses(i,i).m_Mx[4] == mesh->getVertex(i)->getm() &&
			 m_MxMasses(i,i).m_Mx[8] == mesh->getVertex(i)->getm()))
				 cout<<"WARNING: Masses Sparse Matrix Loaded with Error (possibly roundoff)"<<endl;
		if(!(m_TotalForces.m_pData[i].x == mesh->getVertex(i)->getF()[0] &&
			 m_TotalForces.m_pData[i].y == mesh->getVertex(i)->getF()[1] &&
			 m_TotalForces.m_pData[i].z == mesh->getVertex(i)->getF()[2]))
				 cout<<"WARNING: Forces Loaded with Error (possibly roundoff)"<<endl;
	}
//*/

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

		double _Ke = getKbe();
		double _Kd = getKbd();

		//double Ke = 1000;
		//double Kd = 1.1;

//		double Ke = 0;
//		double Kd = 0;

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
		if(mustBePos < 0.0000001)
			mustBePos = 0;

		double sinThetaOver2 = sign * sqrt( (mustBePos ) /2);

		vec3 Fe[4];
		for(int i = 0; i < 4; i++)
			Fe[i] = _Ke * ((Emag*Emag)/(N1mag + N2mag)) * sinThetaOver2 * u[i];

		vec3 Fd[4];
		double DthetaDt = u[0]*v1 + u[1]*v2 + u[2]*v3 + u[3]*v4;
		if(DthetaDt < 0.000001 && DthetaDt > -0.000001)
			DthetaDt = 0;

		for(int i = 0; i < 4; i++)
			Fd[i] = -_Kd * Emag * DthetaDt * u[i];
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

		vec3 Fa = -getKbe() * (theta) * (NA/NA.length());
		vec3 Fb = -getKbe() * (theta) * (NB/NB.length());

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
    vec3 p[2], v[2];
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
       F0 =  f_spring(pa, pb, rl, getKs()) + f_damp(pa, pb, va, vb, rl, getKd());
        // cout<<"F0i "<<F0i[0]<<" "<<F0i[1]<<" "<<F0i[2]<<endl;

       //Store Forces
       a->setF(F0);
       b->setF(-1 * F0);

       //THE GOOD BEND FORCE
	   //Forces((*edg_it)->getParentTriangle(0), (*edg_it)->getParentTriangle(1), a, b, (*edg_it));

       p[0] = pa;
       p[1] = pb;
       v[0] = va;
       v[1] = vb;
       mat3 DfDx, DfDv;
//*
       DfDx = dfdx_spring(p[0], p[1], rl, Ks) + dfdx_damp(p[0], p[1], v[0], v[1], rl, Kd);
       a->setDfDx(DfDx);
       DfDx = dfdx_spring(p[1], p[0], rl, Ks) + dfdx_damp(p[1], p[0], v[1], v[0], rl, Kd);
       b->setDfDx(DfDx);

       DfDv = dfdv_damp(p[0], p[1], rl, Kd);
       a->setDfDv(DfDv);
       DfDv = dfdv_damp(p[1], p[0], rl, Kd);
       b->setDfDv(DfDv);


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
    vec3 f = -getKs() * (l/L) * (L - rl);
    return(f);
}

std::pair<mat3,mat3> System::calculateForcePartials(int pointIndex) {
	mat3 dFx(0), dFv(0);
	//dFx = identity2D();
	//dFv = identity2D();

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
         mat3 DFsDxi = dfdx_spring(pa, pb, rl, Ks);  //W.R.T. u[i]
         mat3 DFdDxi = dfdx_damp(pa, pb, va, vb, rl, Kd);
         mat3 DFdDvi = dfdv_damp(pa, pb, rl, Kd);
         mat3 DFDxi = DFsDxi + DFdDxi;

         //ADD FORCES FOR EACH SPRING CONNECTED TO PARTICLE
         //dFx = dFx * dFddvi;
         //dFv = dFv * dFdxi;


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

void System::SolveExplicit(double timeStep) {
	//a = F/m, a stored in m_TotalForces
	m_TotalForces.ElementMultiply(m_Masses, m_TotalForces);
	//*
	//deltaV = F*h
	m_TotalForces.Scale(timeStep, m_vTemp2);
	// V = V0 + deltaV
	m_Velocities.Add(m_vTemp2, m_Velocities);
	// deltaP = V*h
	m_Velocities.Scale(timeStep, m_vTemp1);
	//	P = P0 + deltaP
	m_Positions.Add(m_vTemp1, m_Positions);
	//*/


	//vec3 deltaV = (timeStep ) * m_TotalForces / (double) point->getm();
    //vec3 deltaX = timeStep * (point->getvX() + deltaV);
}

void System::SolveImplicit(double timeStep) {
	//
			// Form the symmetric matrix A = M - h * df/dv - h^2 * df/dx
			// We regroup this as A = M - h * (df/dv + h * df/dx)
			//
			m_TotalForces_dp.Scale( timeStep, m_MxTemp1 );
			m_TotalForces_dv.Add( m_MxTemp1, m_MxTemp2 );
			m_MxTemp2.Scale( timeStep, m_MxTemp1 );
			m_MxMasses.Subtract( m_MxTemp1, m_A );

			//
			// Compute b = h * ( f(0) + h * df/dx * v(0) + df/dx * y )
			//
			m_Velocities.Scale( timeStep, m_vTemp2 );
			m_vTemp2.Add( m_y, m_vTemp2 );
			m_TotalForces_dp.PostMultiply( m_vTemp2, m_vTemp1 );
	//		m_vTemp1.Scale( timeStep, m_vTemp2 );
			m_TotalForces.Add( m_vTemp1, m_vTemp1 );
			m_vTemp1.Scale( timeStep, m_b );

			int i, iIterations = 0, iMaxIterations = (int)sqrt(numVertices)*3+3;
			double alpha, Delta_0, Delta_old, Delta_new;
			double Eps_Sq = 1e-22;
			//double FLOOR_Y = -0.99;
			//
			// Setup the inverse of preconditioner -- We use a vector for memory efficiency.  Technically it's the diagonal of a matrix
			//
			for( i=0; i< numVertices; i++ )
			{
				m_PInv.m_pData[i].x = m_A(i,i).m_Mx[0];
				m_PInv.m_pData[i].y = m_A(i,i).m_Mx[4];
				m_PInv.m_pData[i].z = m_A(i,i).m_Mx[8];
			}
			m_PInv.Invert( m_P );

			//
			// Modified Preconditioned Conjugate Gradient method
			//

			m_dv = m_z;

			// delta_0 = DotProduct( filter( b ), P * filter( b ) );
			m_b.ElementMultiply( m_S, m_vTemp1 );
			m_P.ElementMultiply( m_vTemp1, m_vTemp2 );
			Delta_0 = m_vTemp2.DotProduct( m_vTemp1 );
			if( Delta_0 < 0 )
			{
				m_b.Dump( "b:\r\n" );
				m_P.Dump( "P:\r\n" );
				cout<< "Negative Delta_0 most likely caused by a non-Positive Definite matrix\r\n" ;
			}

			// r = filter( b - A * dv )
			m_A.PostMultiply( m_dv, m_vTemp1 );
			m_b.Subtract( m_vTemp1, m_vTemp2 );
			m_vTemp2.ElementMultiply( m_S, m_r );

			// c = filter( Pinv * r )
			m_PInv.ElementMultiply( m_r, m_vTemp1 );
			m_vTemp1.ElementMultiply( m_S, m_c );

			Delta_new = m_r.DotProduct( m_c );

			if( Delta_new < Eps_Sq * Delta_0 )
			{
				m_b.Dump( "b: \r\n" );
				m_P.Dump( "P: \r\n" );
				cout<< "This isn't good!  Probably a non-Positive Definite matrix\r\n" <<endl;
			}

			while( (Delta_new > Eps_Sq * Delta_0) && (iIterations < iMaxIterations) )
			{
				m_A.PostMultiply( m_c, m_vTemp1 );

				m_vTemp1.ElementMultiply( m_S, m_q );

				alpha = Delta_new / (m_c.DotProduct( m_q ) );
				m_c.Scale( alpha, m_vTemp1 );
				m_dv.Add( m_vTemp1, m_dv );

				m_q.Scale( alpha, m_vTemp1 );
				m_r.Subtract( m_vTemp1, m_r );

				m_PInv.ElementMultiply( m_r, m_s );
				Delta_old = Delta_new;
				Delta_new = m_r.DotProduct( m_s );

				m_c.Scale( Delta_new / Delta_old, m_vTemp1 );
				m_s.Add( m_vTemp1, m_vTemp2 );
				m_vTemp2.ElementMultiply( m_S, m_c );

				iIterations++;
			}

			m_Velocities.Add( m_dv, m_Velocities );
			m_Velocities.Scale( timeStep, m_vTemp1 );
			m_Positions.Add( m_vTemp1, m_Positions );

	/*
			for( i=0; i<numVertices; i++ )
				if( m_Positions.m_pData[i].y < FLOOR_Y )
				{
					//
					// Constraint the velocity of the particle to NOT move in the Y plane
					// Basically we modify the constraint matrix manually
					//
					m_S[i].SetIdentity();
					m_S[i].m_Mx[4] = 0;
					m_y.m_pData[i].y = (FLOOR_Y - m_Positions.m_pData[i].y);
					m_Positions.m_pData[i].y = FLOOR_Y;
					m_Velocities.m_pData[i].y = 0;
				}
				else
					m_y.m_pData[i].y = 0;
	//*/

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
void System::setTableConstraints(){
	Physics_Matrix3x3 zeroMat;
	zeroMat.Zero();
	double x, y, z, Rtable, Htable;
	double eps_table = .01;

	for(int i = 0; i < numVertices; i++){
		x = m_Positions.m_pData[i].x;
		y = m_Positions.m_pData[i].y;
		z = m_Positions.m_pData[i].z;
		Rtable = .5;
		Htable = .4;
		if( sqrt(x*x + z*z) < Rtable && y < Htable+eps_table && y > Htable-eps_table ) {
			m_S[i].m_Mx[0] =  0;
			m_S[i].m_Mx[1] =  0;
			m_S[i].m_Mx[2] =  0;
			m_S[i].m_Mx[3] =  0;
			m_S[i].m_Mx[4] =  0;
			m_S[i].m_Mx[5] =  0;
			m_S[i].m_Mx[6] =  0;
			m_S[i].m_Mx[7] = 0;
			m_S[i].m_Mx[8] =  0;
		}
	}
}
void System::takeStep(Solver* solver, double timeStep) {

    std::vector<std::pair<vec3,vec3> > changes(mesh->vertices.size(),
            make_pair(vec3(0,0,0), vec3(0,0,0)));

    solver->calculateState(this); //evalDeriv function
/*
    loadMatrices();
   // setTableConstraints();
	//m_TotalForces_dp.Zero();
	//m_TotalForces_dv.Zero();
    SolveImplicit(timeStep);
    //SolveExplicit(timeStep);

    m_TotalForces.Zero();
	m_TotalForces_dp.Zero();
	m_TotalForces_dv.Zero();
	m_MxTemp1.Zero();
	m_MxTemp2.Zero();
	m_z.Zero();
//*/

//*
    for (unsigned int i = 0; i < mesh->countVertices(); i++) {
    	std::pair<vec3,vec3> deltas =
            solver->solve(this, timeStep, i, mesh->getVertex(i));
        changes[i] = deltas;
    }
  //*/
  //cout<<mesh->getVertex(mesh->vertices.size())->getX()<<endl;
    TriangleMeshVertex* v;
    for (unsigned int i = 0; i < mesh->vertices.size(); i++) {
        v = mesh->getVertex(i);
        v->getX() += changes[i].first;
        v->getvX() += changes[i].second;

/*
        v->getX()[0] = m_Positions.m_pData[i].x;
        v->getX()[1] = m_Positions.m_pData[i].y;
        v->getX()[2] = m_Positions.m_pData[i].z;

        v->getvX()[0] = m_Velocities.m_pData[i].x;
        v->getvX()[1] = m_Velocities.m_pData[i].y;
        v->getvX()[2] = m_Velocities.m_pData[i].z;
       // cout<<"F: "<<v->getF()<<endl;
        v->clearF();
        v->clearDfDx();
        v->clearDfDv();
//*/
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

    time += timeStep;
}


float System::getKs() {
    return Ks;
}

float System::getKd() {
    return Kd;
}

float System::getKbe() {
	return Kbe;
}

float System::getKbd() {
	return Kbd;
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

	//*
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
    //cout<<"Force: "<<point->getF()/point->getm()<<endl;
    point->clearF();




    //Contraints set in OBJ file
    if (point->getConstaint()[0] == 0)
        return make_pair(vec3(0,0,0), vec3(0,0,0)); //Lame contraints for explicit

    //cout << "Forces on particle " << pointIndex << " is (" << F[0] << ", " << F[1] << ", " << F[2] << ")" << endl;
    //******************************************************************************
    return make_pair(deltaX, deltaV);
}

