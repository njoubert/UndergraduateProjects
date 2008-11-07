/*
 * System.cpp
 *
 *  Created on: Sep 7, 2008
 *      Author: njoubert
 */

#include "System.h"
#define GRAVITY -9.8

/////////////////////////////WHERE IS THIS CALLED////////////////////////////////////
System::System(TriangleMesh* m, int verticeCount) :
	mesh(m), m_Positions(verticeCount), m_Velocities(verticeCount), m_dv(
			verticeCount), m_dp(verticeCount), m_identity(verticeCount,
			verticeCount), m_vTemp1(verticeCount), m_vTemp2(verticeCount),
			m_PosTemp(verticeCount), m_TotalForces(verticeCount), m_Masses(
					verticeCount), m_MassInv(verticeCount), m_MxMasses(
					verticeCount, verticeCount), m_TotalForces_dp(verticeCount,
					verticeCount),
			m_TotalForces_dv(verticeCount, verticeCount),  m_P(verticeCount), m_PInv(verticeCount),
			m_MxTemp1(verticeCount, verticeCount), m_MxTemp2(verticeCount,
					verticeCount), m_z(verticeCount), m_b(verticeCount), m_r(
					verticeCount), m_c(verticeCount), m_q(verticeCount), m_s(
					verticeCount), m_y(verticeCount), r(verticeCount), d(verticeCount), q(verticeCount), x(verticeCount) {
	m_TotalForces_dp.Zero();
	m_TotalForces_dv.Zero();
	m_identity.Zero();
	m_PInv.Zero();

	globalCount = 1;

	numVertices = mesh->countVertices();
	cout << numVertices << endl;

	for (int i = 0; i < numVertices; i++)
		mesh->getVertex(i)->setID(i);
	cout<<"Just set ID"<<endl;
	//for (int i = 0; i < numVertices; i++) {
	//	m_z.m_pData[i].x = 0;
		//m_z.m_pData[i].y = 0;
	//	m_z.m_pData[i].z = 0;
	//}
	m_z.Zero();
	/*
	 for(int i = 0; i < numVertices; i++){
	 cout<<mesh->getVertex(i)->getID()<<endl;
	 cout<<mesh->getVertex(i)<<endl<<endl;
	 }
	 exit(1);
	 //*/
	m_S = new Physics_Matrix3x3[numVertices];
	mat3 MatrixConverter;
	for (int i = 0; i < numVertices; i++) {
		/*
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
		m_identity(i, i).m_Mx[0] = 1;
		m_identity(i, i).m_Mx[4] = 1;
		m_identity(i, i).m_Mx[8] = 1;

		m_S[i].SetIdentity();
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
	Ks = 100;
	Kd = 5;
	Kbe = .1;
	Kbd = .01;
	g = 1.0;
	//kb = 0;
	mouseSelected = NULL;
	cout<<"about to set rest angles"<<endl;
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
			for (int i = 0; i < 3; i++) {
				if (aList[i] != a && aList[i] != b)
					ai = i;

				if (bList[i] != a && bList[i] != b)
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
			vec3 N1unit = N1 / N1mag;
			vec3 N2unit = N2 / N2mag;
			vec3 Eunit = E / Emag;

			double sign = ((N1unit ^ N2unit) * Eunit);
			if (sign > 0)
				sign = 1;
			else
				sign = -1;

			double mustBePos = abs(1 - N1unit * N2unit);
			if (mustBePos < 0.000001)
				mustBePos = 0;

			double sinThetaOver2 = sign * sqrt((mustBePos) / 2);

			/*
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
	cout<<"Done Constructing System"<<endl;
}

/***
 * Copies state from mesh to matrices used for numerical integration.
 */
void System::loadMatrices() {
	//if(numVertices == m_Positions.Size())
	//	cout<<"SUPER"<<endl;
	//	else
	//		cout<<"oh crap"<<endl;


	for (int i = 0; i < numVertices; i++) {
		m_Positions.m_pData[i].x = mesh->getVertex(i)->getX()[0];
		m_Positions.m_pData[i].y = mesh->getVertex(i)->getX()[1];
		m_Positions.m_pData[i].z = mesh->getVertex(i)->getX()[2];

		m_Velocities.m_pData[i].x = mesh->getVertex(i)->getvX()[0];
		m_Velocities.m_pData[i].y = mesh->getVertex(i)->getvX()[1];
		m_Velocities.m_pData[i].z = mesh->getVertex(i)->getvX()[2];

		if (m_S[i].m_Mx[0] == 0 && m_S[i].m_Mx[4] == 0 && m_S[i].m_Mx[8] == 0) {
			m_Masses.m_pData[i].x = 0;
			m_Masses.m_pData[i].y = 0;
			m_Masses.m_pData[i].z = 0;
		} else {
			m_Masses.m_pData[i].x = 1 / mesh->getVertex(i)->getm();
			m_Masses.m_pData[i].y = 1 / mesh->getVertex(i)->getm();
			m_Masses.m_pData[i].z = 1 / mesh->getVertex(i)->getm();
		}

		m_MxMasses(i, i).m_Mx[0] = mesh->getVertex(i)->getm();
		m_MxMasses(i, i).m_Mx[4] = mesh->getVertex(i)->getm();
		m_MxMasses(i, i).m_Mx[8] = mesh->getVertex(i)->getm();

		m_TotalForces.m_pData[i].x = mesh->getVertex(i)->getF()[0];
		m_TotalForces.m_pData[i].y = mesh->getVertex(i)->getF()[1];
		m_TotalForces.m_pData[i].z = mesh->getVertex(i)->getF()[2];

	}
	//m_Masses.Invert(m_MassInv);

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

mat3 System::outerProduct(vec3 a, vec3 b) {
	mat3 C;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			C[i][j] = a[i] * b[j];
		}
	}
	return C;
}

vec3 System::f_spring(vec3 & pa, vec3 & pb, double rl, double Ks) {
	vec3 l = pa - pb;
	double L = l.length();

	double e = .0000001;
	if (L < rl + e && L > rl - e)
		L = rl;

	//cout<<"Spring"<<endl<<"L "<<L<<endl<<"RL "<<rl<<endl<<"L-rl: "<<L-rl<<endl<<endl;
	vec3 f = -Ks * (l / L) * (L - rl);
	//cout<<"f ";Print(f);

	return (f);
}
vec3 System::f_damp(vec3 & pa, vec3 & pb, vec3 & va, vec3 & vb, double rl,
		double Kd) {
	//PrintVec3("pa: ", pa);
	//PrintVec3("pb: ", pb);

	vec3 l = pa - pb;
	//PrintVec3("pa - pb: ", l);
	vec3 w = va - vb;
	//PrintVec3("va - vb: ", w);
	double L = l.length();
	//cout<<"Damping"<<endl<<"L "<<L<<endl<<"RL "<<rl<<endl<<endl;
	//cout<<"Norm[pa-pb]: "<<L<<endl<<endl;
	vec3 f = -Kd * ((l * w) / (L * L)) * l;
	//PrintVec3("Fd: ", f);
	return (f);
}

void System::Forces(TriangleMeshTriangle* A, TriangleMeshTriangle* B,
		TriangleMeshVertex* a, TriangleMeshVertex* b, TriangleMeshEdge* edge) {
	TriangleMeshVertex** aList = A->getVertices();
	TriangleMeshVertex** bList = B->getVertices();

	if (A != NULL && B != NULL && A != B) {

		//Dirty way to figure out which point is which
		int ai, bi;
		for (int i = 0; i < 3; i++) {
			if (aList[i] != a && aList[i] != b)
				ai = i;

			if (bList[i] != a && bList[i] != b)
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
		vec3 N1unit = N1 / N1mag;
		vec3 N2unit = N2 / N2mag;
		vec3 Eunit = E / Emag;

		vec3 u[4];
		u[0] = Emag * (N1 / (N1mag * N1mag));
		u[1] = Emag * (N2 / (N2mag * N2mag));
		u[2] = (((x1 - x4) * E) / Emag) * (N1 / (N1mag * N1mag)) + (((x2 - x4)
				* E) / Emag) * (N2 / (N2mag * N2mag));
		u[3] = -(((x1 - x3) * E) / Emag) * (N1 / (N1mag * N1mag)) - (((x2 - x3)
				* E) / Emag) * (N2 / (N2mag * N2mag));

		double sign = ((N1unit ^ N2unit) * Eunit);
		if (sign > 0)
			sign = 1;
		else
			sign = -1;

		double mustBePos = abs(1 - N1unit * N2unit);
		if (mustBePos < 0.0000001)
			mustBePos = 0;

		double sinThetaOver2 = sign * sqrt((mustBePos) / 2);

		vec3 Fe[4];
		for (int i = 0; i < 4; i++)
			Fe[i] = _Ke * ((Emag * Emag) / (N1mag + N2mag)) * sinThetaOver2
					* u[i];

		vec3 Fd[4];
		double DthetaDt = u[0] * v1 + u[1] * v2 + u[2] * v3 + u[3] * v4;
		if (DthetaDt < 0.000001 && DthetaDt > -0.000001)
			DthetaDt = 0;

		for (int i = 0; i < 4; i++)
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
		for (int i = 0; i < 4; i++)
			F[i] = Fe[i] + Fd[i];

		aList[ai]->setF(F[1]);
		bList[bi]->setF(F[0]);
		a->setF(F[3]);
		b->setF(F[2]);
	}
	//*/
}

void System::f_bend(TriangleMeshTriangle* A, TriangleMeshTriangle* B,
		TriangleMeshVertex* a, TriangleMeshVertex* b, TriangleMeshEdge* edge) {
	vec3 F(0);
	if (A != NULL && B != NULL && A != B) {
		//GET NORMALS
		vec3 NA = A->getNormal();
		vec3 NB = B->getNormal();

		//CALCULATE BEND FORCES
		vec3 e = b - a;
		e.normalize();
		vec3 crossNANB = NA ^ NB;
		double eDotN = crossNANB * e;
		double theta = asin(eDotN);
		//	cout<<"Edge: "<<edge<<" Theta: "<<theta<<endl;


		//BUGGED CODE FOR REST ANGLE
		//	vec3 Fa = -getKb() * (abs(theta) - abs(edge->getRestAngle())) * (NA/NA.length());
		//		vec3 Fb = -getKb() * (abs(theta) - abs(edge->getRestAngle())) * (NB/NB.length());

		vec3 Fa = -getKbe() * (theta) * (NA / NA.length());
		vec3 Fb = -getKbe() * (theta) * (NB / NB.length());

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
		for (int i = 0; i < 3; i++) {
			if (ai[i] != a && ai[i] != b) {
				save_ai_Index = i;
				//ai[i]->setF(Fa);
				//cout<<"ai: "<<ai[i]<<" Fa: "<<Fa.length()<<endl;
			}
			if (bi[i] != a && bi[i] != b) {
				save_bi_Index = i;
				//bi[i]->setF(Fb;
				//cout<<"bi: "<<bi[i]<<" Fb: "<<Fb.length()<<endl;
			}

		}
		vec3 v = ai[save_ai_Index]->getvX() - bi[save_bi_Index]->getvX();

		//BUGGED CODE MUST GET CORRECT EQUATION FOR DAMPING FORCE ON A ANGULAR SPRING
		vec3 Fdampa = -getKd() * (v.length2() * theta) * (NA / NA.length());
		vec3 Fdampb = -getKd() * (v.length2() * theta) * (NB / NB.length());

		ai[save_ai_Index]->setF(Fa);// + Fdampa);
		bi[save_bi_Index]->setF(Fb);// + Fdampb);

		//cout<<endl;
	}

	//return(F);
}

inline mat3 System::dfdx_spring(vec3 & pa, vec3 & pb, double rl, double Ks) {
	mat3 I = identity2D();
	vec3 l = pa - pb;
	double L = l.length();
	mat3 oProd = outerProduct(l, l);
	double lDOT = l * l;
	return -Ks * ((1 - (rl / L)) * (I - (oProd / lDOT)) + (oProd / lDOT));
}
inline mat3 System::dfdx_damp(vec3 & pa, vec3 & pb, vec3 & va, vec3 & vb,
		double rl, float Kd) {
	mat3 I = identity2D();
	vec3 P = pa - pb;
	vec3 V = va - vb;
	return -Kd * ((outerProduct(P, V) / (P * P)) + (2 * ((-P * V) / ((P * P)
			* (P * P))) * outerProduct(P, P)) + (((P * V) / (P * P)) * I));
}
mat3 System::dfdv_damp(vec3 & pa, vec3 & pb, double rl, double Kd) {
	mat3 I = identity2D();
	vec3 l = pa - pb;
	mat3 oProd = outerProduct(l, l);
	double lDOT = l * l;
	mat3 RETURN = -Kd * (oProd / lDOT);
	return RETURN;
}

void System::calculateInternalForces() {
	vec3 F0(0, 0, 0); //Output net force.
	TriangleMeshVertex *a, *b;

	EdgesIterator edg_it = mesh->getEdgesIterator();
	do {
		/* Calculate internal forces here.
		 * a = first point, b = second point. */
		a = (*edg_it)->getVertex(0);
		b = (*edg_it)->getVertex(1);

		vec3 pa = a->getX();
		vec3 va = a->getvX();
		vec3 pb = b->getX();
		vec3 vb = b->getvX();
		vec3 Ua = a->getU();
		vec3 Ub = b->getU();
		vec3 RL = Ua - Ub;
		double rl = RL.length();

		//STRETCH AND SHEAR FORCES
		F0 = f_spring(pa, pb, rl, getKs())
				+ f_damp(pa, pb, va, vb, rl, getKd());
		//These must be stored "explicitly"
		a->setF(F0);
		b->setF(-1 * F0);

		//THE GOOD BEND FORCE
		//Forces((*edg_it)->getParentTriangle(0), (*edg_it)->getParentTriangle(1), a, b, (*edg_it));

		mat3 DfDx[4], DfDv[4];
		int verterxPos[2]; //vertexPos[0]= xi, vertex[1] = xj
		verterxPos[0] = a->getID();
		verterxPos[1] = b->getID();

		//  cout<<"aX: "<<a->getX()<<endl;
		//  cout<<"bX: "<<b->getX()<<endl;
		//  cout<<"aV: "<<a->getvX()<<endl;
		//  cout<<"bV: "<<b->getvX()<<endl;
		//  cout<<"RL: "<<rl<<endl;

		mat3 garbage(9);
		//	Dfa/xa
		DfDx[0] = dfdx_spring(pa, pb, rl, Ks) + dfdx_damp(pa, pb, va, vb, rl,
				Kd);
		//	Dfb/xb
		DfDx[2] = DfDx[0];
		//	Dfa/xb
		DfDx[1] = -1 * DfDx[0];
		//	Dfb/xb NOT PUT INTO MARIX
		DfDx[3] = garbage;//- DfDx[1];

		//	Dfa/va
		DfDv[0] = dfdv_damp(pa, pb, rl, Kd);
		//	Dfb/vb
		DfDv[2] = DfDv[0];
		//	Dfa/va
		DfDv[1] = -1 * DfDv[0];
		//	Dfj/vj
		DfDv[3] = garbage;//- DfDv[1];

		/*
		 double zeroCut = .00001;
		 //get rid of any round off
		 for(int k = 0; k < 3; k++)
		 for(int i = 0; i < 3; i++)
		 for(int j = 0; j < 3; j++)
		 if(DfDx[k][i][j] < zeroCut && DfDx[k][i][j] > -zeroCut)
		 DfDx[k][i][j] = 0;

		 for(int k = 0; k < 3; k++)
		 for(int i = 0; i < 3; i++)
		 for(int j = 0; j < 3; j++)
		 if(DfDv[k][i][j] < zeroCut && DfDv[k][i][j] > -zeroCut)
		 DfDv[k][i][j] = 0;

		 //*/
		//cout<<"DfDx: "<<endl<<DfDx[0]<<endl;
		//cout<<"DfDv: "<<endl<<DfDv[0]<<endl<<endl;

		//PLACE MINI JACOBIANS INTO LARGE JACOBIAN
		int Jcounter = 0;
		for (int i = 0; i < 2; i++) {
			for (int j = i; j < 2; j++) {
				//if(i == 0 &&  j == 1){
				// cout<<Jcounter<<endl;
				m_TotalForces_dp(verterxPos[i], verterxPos[j]).m_Mx[0]
						+= DfDx[Jcounter][0][0];
				m_TotalForces_dp(verterxPos[i], verterxPos[j]).m_Mx[1]
						+= DfDx[Jcounter][0][1];
				m_TotalForces_dp(verterxPos[i], verterxPos[j]).m_Mx[2]
						+= DfDx[Jcounter][0][2];
				m_TotalForces_dp(verterxPos[i], verterxPos[j]).m_Mx[3]
						+= DfDx[Jcounter][1][0];
				m_TotalForces_dp(verterxPos[i], verterxPos[j]).m_Mx[4]
						+= DfDx[Jcounter][1][1];
				m_TotalForces_dp(verterxPos[i], verterxPos[j]).m_Mx[5]
						+= DfDx[Jcounter][1][2];
				m_TotalForces_dp(verterxPos[i], verterxPos[j]).m_Mx[6]
						+= DfDx[Jcounter][2][0];
				m_TotalForces_dp(verterxPos[i], verterxPos[j]).m_Mx[7]
						+= DfDx[Jcounter][2][1];
				m_TotalForces_dp(verterxPos[i], verterxPos[j]).m_Mx[8]
						+= DfDx[Jcounter][2][2];

				m_TotalForces_dv(verterxPos[i], verterxPos[j]).m_Mx[0]
						+= DfDv[Jcounter][0][0];
				m_TotalForces_dv(verterxPos[i], verterxPos[j]).m_Mx[1]
						+= DfDv[Jcounter][0][1];
				m_TotalForces_dv(verterxPos[i], verterxPos[j]).m_Mx[2]
						+= DfDv[Jcounter][0][2];
				m_TotalForces_dv(verterxPos[i], verterxPos[j]).m_Mx[3]
						+= DfDv[Jcounter][1][0];
				m_TotalForces_dv(verterxPos[i], verterxPos[j]).m_Mx[4]
						+= DfDv[Jcounter][1][1];
				m_TotalForces_dv(verterxPos[i], verterxPos[j]).m_Mx[5]
						+= DfDv[Jcounter][1][2];
				m_TotalForces_dv(verterxPos[i], verterxPos[j]).m_Mx[6]
						+= DfDv[Jcounter][2][0];
				m_TotalForces_dv(verterxPos[i], verterxPos[j]).m_Mx[7]
						+= DfDv[Jcounter][2][1];
				m_TotalForces_dv(verterxPos[i], verterxPos[j]).m_Mx[8]
						+= DfDv[Jcounter][2][2];
				// }
				Jcounter++;
			}
		}
		//out<<endl;
	} while (edg_it.next());

	//cout<<"Fpoint: "<<F0.length()<<endl;
}

void System::calculateExternalForces() {
	TriangleMeshVertex* a;
	vec3 gravity(0, GRAVITY, 0);
	//vec3 gravity(0, 0, GRAVITY);
	for (int i = 0; i < mesh->countVertices(); i++) {
		a = mesh->getVertex(i);
		a->setF(gravity * a->getm());// += f_mouse(a));
	}
}

void System::enableMouseForce(vec3 mousePosition) {
	//Find closest vertex
	//cout << "Mouse is at: " << mousePosition << endl;
	double currentDistance = 900000.0, d, d1, d2;
	TriangleMeshVertex* currentClosest;
	for (unsigned int i = 0; i < mesh->vertices.size(); i++) {
		d1 = (mousePosition[0] - mesh->getVertex(i)->getX()[0]);
		d2 = (mousePosition[1] - mesh->getVertex(i)->getX()[1]);
		d = sqrt(d1*d1 + d2*d2);
		if (d < currentDistance) {
			currentDistance = d;
			currentClosest = mesh->getVertex(i);
			mouseP = mousePosition;
		}
	}

	//sets mouseSelected to this vertex.
	mouseSelected = currentClosest;
	cout<<"Selected Vertex: "<<mouseSelected<<endl;
	cout<<"Mouse Position: "<<mouseP<<endl;
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

vec3 System::f_mouse(TriangleMeshVertex* selected) {
	if (selected != mouseSelected)
		return vec3(0, 0, 0);

	double rl = 1;
	vec3 l = selected->getX() - mouseP;
	double L = l.length();
	vec3 f = -getKs() * (l / L) * (L - rl);
	return (f);
}

int System::setVertexPos2MousePos() {
	TriangleMeshVertex* selected;
	for (int i = 0; i < mesh->countVertices(); i++) {
		selected = mesh->getVertex(i);
		if (selected == mouseSelected) {
			selected->getX() = mouseP;
			m_Positions.m_pData[i].x = mouseP[0];
			m_Positions.m_pData[i].y = mouseP[1];
			m_Positions.m_pData[i].z = mouseP[2];
			return i;
		}
	}
	return -1;
}

vector<int> System::setVertexPos2NewPos() {
	TriangleMeshVertex* simVertex;
	vector<int> constVertices;
	for(int i = 0; i < numVertices; i++){
		simVertex = mesh->getVertex(i);
		if(simVertex->getDynamConst() != NULL) {
			simVertex->getX() = *simVertex->getDynamConst();
			m_Positions.m_pData[i].x = simVertex->getX()[0];
			m_Positions.m_pData[i].y = simVertex->getX()[1];
			m_Positions.m_pData[i].z = simVertex->getX()[2];
			constVertices.push_back(i);
		}
	}
	return constVertices;
}

std::pair<mat3, mat3> System::calculateForcePartials(int pointIndex) {
	mat3 dFx(0), dFv(0);
	//dFx = identity2D();
	//dFv = identity2D();

	TriangleMeshVertex* a = mesh->getVertex(pointIndex);
	TriangleMeshVertex* b;
	std::vector<TriangleMeshEdge*>::const_iterator it =
			a->getEdgesBeginIterator();
	while (it != a->getEdgesEndIterator()) {
		b = (*it)->getOtherVertex(a);

		/* Calculate internal forces here.
		 * a = first point, b = second point. */

		vec3 pa = a->getX();
		vec3 va = a->getvX();
		vec3 pb = b->getX();
		vec3 vb = b->getvX();
		vec3 Ua = a->getU();
		vec3 Ub = b->getU();
		vec3 RL = Ua - Ub;
		double rl = RL.length();
		//double rl  = (*it)->getRestLength();
		double Ks = getKs();
		double Kd = getKd();
		//CALCULATE FORCES FOR EACH SPRING CONNECTED TO PARTICLE
		mat3 DFsDxi = dfdx_spring(pa, pb, rl, Ks); //W.R.T. u[i]
		mat3 DFdDxi = dfdx_damp(pa, pb, va, vb, rl, Kd);
		mat3 DFdDvi = dfdv_damp(pa, pb, rl, Kd);
		mat3 DFDxi = DFsDxi + DFdDxi;

		//ADD FORCES FOR EACH SPRING CONNECTED TO PARTICLE
		//dFx = dFx * dFddvi;
		//dFv = dFv * dFdxi;

		it++;
	}

	/* Calculate external forces here... */

	return make_pair(dFx, dFv);
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

void System::CG(SparseMatrix* A){
	int i = 0, iMAX = 2000, resCutOff = 5;
	double EPS = 1e-5;
	double delta_0, delta_new, delta_old, alpha, beta;
	//Physics_LargeVector x(numVertices);
	m_dv.Zero();
	r.Zero();
	d.Zero();
	q.Zero();
	m_vTemp1.Zero();
	r = m_b;
	d = r;
	delta_new = r.DotProduct(r);
	delta_0 = delta_new;

	do{
		//cout<<delta_new<<" >? "<<EPS*EPS*delta_0<<endl;
		//q = A*d
		A->PostMultiply(d, q);
		//alpha = delta_new/(dT*q)
		alpha = delta_new/(d.DotProduct(q));
		//x = x + alpha*d
		d.Scale(alpha, m_vTemp1),
		m_dv.Add(m_vTemp1, m_dv);

		if(i < resCutOff) {
			//b = b - Ax
			A->PostMultiply(m_dv, m_vTemp1);
			m_b.Subtract(m_vTemp1, r);
		}
		else {
			//r = r - alpha*q
			q.Scale(alpha, m_vTemp1);
			r.Subtract(m_vTemp1, r);
		}
		//delta_old = delta_new
		delta_old = delta_new;
		//delta_new = rT*r
		delta_new = r.DotProduct(r);
		//beta = delta_new/delta_old
		beta = delta_new/delta_old;
		//d = r + beta*d
		d.Scale(beta, m_vTemp1);
		r.Add(m_vTemp1, d);
		//i = i + 1
		i = i + 1;
	} while(i < iMAX && delta_new > (EPS*EPS*delta_0));
	cout<<"Conjugate Gradient Converged at iteration "<<i<<" with a residual of "<<delta_new<<endl;
}

void System::SolveImplicit(double timeStep, vector<Constraint*>* constraints) {
	int i;



	//DEFINE WHICH VERTICES SHOULD BE CONSTRAINED
	vector<int> constrainBlock;

	//Mouse Force Constraint
	int constVertexMouse = setVertexPos2MousePos();
	if (constVertexMouse != -1){
		constrainBlock.push_back(constVertexMouse);
		//cout<<"just added Mouse Constraint on vertex: "<<constrainBlock[0]<<endl;
	}

/*	OUTDATED
	//Dynamic Constraints
	vector<int> constVertexDynam = setVertexPos2NewPos();
	for(i = 0; i < constVertexDynam.size(); i++){
			constrainBlock.push_back(constVertexDynam[i]);
			cout<<"Just Added A Dynamic Constraint for Vertex: "<<constVertexDynam[i]<<endl;
	}
//*/

	//Static Constraints (from Obj File via "fixed points" notation)
	mat3 constraint;
	for (i = 0; i < numVertices; i++) {
		constraint = calculateContraints(i);
		if (constraint[0] == 0) {
			constrainBlock.push_back(i);
		}
	}

	//constrainBlock.push_back(10);
	//cout<<"constrainblock size: "<<constrainBlock.size()<<endl<<endl;
	//for(int l = 0; l < constrainBlock.size(); l++){
	//	cout<<l<<endl;
	//	cout<<"constrainBlock "<<l<<" = "<<constrainBlock[l]<<endl;
	//}

	//THIS SHOULD NOT BE HERE UNFORTUENTLY IT MUST!!!!!
	SparseMatrix m_A(numVertices,numVertices);

	// Form the symmetric matrix A = M - h * df/dv - h^2 * df/dx
	// We regroup this as A = M - h * (df/dv + h * df/dx)
	//
	//	m_MxTemp1 = h*dfdx
	m_TotalForces_dp.Scale(timeStep, m_MxTemp1);
	//	m_MxTemp2 = (df/dv + h * df/dx)
	m_TotalForces_dv.Add(m_MxTemp1, m_MxTemp2);
	//	m_MxTemp1 = h*(df/dv + h * df/dx)
	m_MxTemp2.Scale(timeStep, m_MxTemp1);
	//	m_MxTemp1 = g*h*(df/dv + h * df/dx)
	m_MxTemp1.Scale(g, m_MxTemp1);
	//	m_A = M - h * (df/dv + h * df/dx)
	m_MxMasses.Subtract(m_MxTemp1, m_A);

	//
	// Compute b = h * ( f(0) + h * df/dx * v(0) + df/dx * y )
	//
	//	m_vTemp2 = h*v(0)
	m_Velocities.Scale(timeStep, m_vTemp2);
	//	m_vTemp2 = h*v(0) + y
	m_vTemp2.Add(m_y, m_vTemp2);
	//	m_vTemp1 = df/dx*(h*v(0) + y)
	m_TotalForces_dp.PostMultiply(m_vTemp2, m_vTemp1);
	//		m_vTemp1.Scale( fStep, m_vTemp2 );
	//	m_vTemp1 = g*df/dx*(h*v(0) + y)
	m_vTemp1.Scale(g, m_vTemp1);
	//	m_vTemp1 = f(0) + h*df/dx*v(0) + df/dx*y
	m_TotalForces.Add(m_vTemp1, m_vTemp1);
	//	m_b = h*(f(0) + h*df/dx*v(0) + df/dx*y)
	m_vTemp1.Scale(timeStep, m_b);


	//CONSTRAINTS

	for(int i = 0; i < constraints->size(); i++)
		(*constraints)[i]->applyConstraintToSolverMatrices(time, &m_A, &m_b);


	//*

	for (unsigned int k = 0; k < constrainBlock.size(); k++) {

		for (i = 0; i < numVertices; i++) {
			for (int j = 0; j < 9; j++){
				//cout<<constrainBlock[k]<<endl;
				//if(constrainBlock[k] == 10){
					//cout<<"constrainBlock "<<k<<" = "<<constrainBlock[k]<<endl;
				//}
				//cout<<m_A(10,i).m_Mx[j]<<endl<<endl;

				m_A(constrainBlock[k], i).m_Mx[j] = 0;
				//if(constrainBlock[k] == 10){
				//	cout<<"it should not get to this point: "<<m_A(constrainBlock[k], i).m_Mx[j]<<endl;
				//}
			}

		}
		//cout << "got here1" << endl;
		for (i = 0; i < 9; i++) {
			m_A(constrainBlock[k], constrainBlock[k]).m_Mx[i] = 1;
		}
		//cout << "got here2" << endl;
		//*
		for (int p = 0; p < numVertices; p++) {
			m_b.m_pData[constrainBlock[k]].x = 0;
			m_b.m_pData[constrainBlock[k]].y = 0;
			m_b.m_pData[constrainBlock[k]].z = 0;
		}
		//cout << "got here3" << endl;
	}


	//*/
	/*
	 cout<<"global count: "<<globalCount<<endl;
	 if(globalCount == 1){
	 ofstream file("../A_and_b.txt");
	 if (file.is_open())
	 {
	 file<<"TimeStep (h) = "<<timeStep<<endl<<endl;
	 for( i = 0; i < numVertices; i++){
	 file<<"Vertex: "<<mesh->getVertex(i)->getID()<<
	 "  Position: "<<mesh->getVertex(i)->getX()<<
	 "  Velocity: "<<mesh->getVertex(i)->getvX()<<endl;
	 }
	 file<<"v: "<<endl;
	 for(i = 0; i < numVertices; i++)
	 file<<  m_Velocities.m_pData[i].x<<endl<<m_Velocities.m_pData[i].y <<endl<<m_Velocities.m_pData[i].z<<endl;
	 file<<"F: "<<endl;
	 for(i = 0; i < numVertices; i++)
	 file<< m_TotalForces.m_pData[i].x<<endl<< setprecision(2) << setw(4) << m_TotalForces.m_pData[i].y <<endl<<m_TotalForces.m_pData[i].z<<endl;
	 file<<endl;
	 file<<"DfDx: "<<endl;
	 for(i = 0; i < numVertices; i++) {
	 for(int j = 0; j < numVertices; j++) {
	 file << setprecision(4) << setw(2) << m_TotalForces_dp(i, j).m_Mx[0] << "    ";
	 file << setprecision(4) << setw(2) << m_TotalForces_dp(i, j).m_Mx[1] << "    ";
	 file << setprecision(4) << setw(2) << m_TotalForces_dp(i, j).m_Mx[2] << "    ";
	 }
	 file<<endl;
	 for(int j = 0; j < numVertices; j++) {
	 file << setprecision(4) << setw(2)<< m_TotalForces_dp(i, j).m_Mx[3] << "    ";
	 file << setprecision(4) << setw(2)<< m_TotalForces_dp(i, j).m_Mx[4] << "    ";
	 file << setprecision(4) << setw(2)<< m_TotalForces_dp(i, j).m_Mx[5] << "    ";
	 }
	 file<<endl;
	 for(int j = 0; j < numVertices; j++) {
	 file << setprecision(4) << setw(2)<< m_TotalForces_dp(i, j).m_Mx[6] << "    ";
	 file << setprecision(4) << setw(2)<< m_TotalForces_dp(i, j).m_Mx[7] << "    ";
	 file << setprecision(4) << setw(2)<< m_TotalForces_dp(i, j).m_Mx[8] << "    ";
	 }
	 file<<endl;
	 }
	 file<<endl<<endl<<endl;
	 cout<<"wrote DfDx to file"<<endl;
	 file<<"DfDv: "<<endl;
	 for(i = 0; i < numVertices; i++) {
	 for(int j = 0; j < numVertices; j++) {
	 file << setprecision(2) << setw(4) << m_TotalForces_dv(i, j).m_Mx[0] << "    ";
	 file << setprecision(2) << setw(4) << m_TotalForces_dv(i, j).m_Mx[1] << "    ";
	 file << setprecision(2) << setw(4) << m_TotalForces_dv(i, j).m_Mx[2] << "    ";
	 }
	 file<<endl;
	 for(int j = 0; j < numVertices; j++) {
	 file << setprecision(2) << setw(4)<< m_TotalForces_dv(i, j).m_Mx[3] << "    ";
	 file << setprecision(2) << setw(4)<< m_TotalForces_dv(i, j).m_Mx[4] << "    ";
	 file << setprecision(2) << setw(4)<< m_TotalForces_dv(i, j).m_Mx[5] << "    ";
	 }
	 file<<endl;
	 for(int j = 0; j < numVertices; j++) {
	 file << setprecision(2) << setw(4)<< m_TotalForces_dv(i, j).m_Mx[6] << "    ";
	 file << setprecision(2) << setw(4)<< m_TotalForces_dv(i, j).m_Mx[7] << "    ";
	 file << setprecision(2) << setw(4)<< m_TotalForces_dv(i, j).m_Mx[8] << "    ";
	 }
	 file<<endl;
	 }
	 file<<endl<<endl<<endl;
	 cout<<"wrote DfDv to file"<<endl;
	 file<<"M: "<<endl;
	 for(i = 0; i < numVertices; i++) {
	 for(int j = 0; j < numVertices; j++) {
	 file << setprecision(2) << setw(4) << m_MxMasses(i, j).m_Mx[0] << "    ";
	 file << setprecision(2) << setw(4) << m_MxMasses(i, j).m_Mx[1] << "    ";
	 file << setprecision(2) << setw(4) << m_MxMasses(i, j).m_Mx[2] << "    ";
	 }
	 file<<endl;
	 for(int j = 0; j < numVertices; j++) {
	 file << setprecision(2) << setw(4)<< m_MxMasses(i, j).m_Mx[3] << "    ";
	 file << setprecision(2) << setw(4)<< m_MxMasses(i, j).m_Mx[4] << "    ";
	 file << setprecision(2) << setw(4)<< m_MxMasses(i, j).m_Mx[5] << "    ";
	 }
	 file<<endl;
	 for(int j = 0; j < numVertices; j++) {
	 file << setprecision(2) << setw(4)<< m_MxMasses(i, j).m_Mx[6] << "    ";
	 file << setprecision(2) << setw(4)<< m_MxMasses(i, j).m_Mx[7] << "    ";
	 file << setprecision(2) << setw(4)<< m_MxMasses(i, j).m_Mx[8] << "    ";
	 }
	 file<<endl;
	 }
	 file<<endl<<endl<<endl;
	 cout<<"wrote M to file"<<endl;
	 file<<"A: "<<endl;
	 for(i = 0; i < numVertices; i++) {
	 for(int j = 0; j < numVertices; j++) {
	 file << setprecision(2) << setw(4) << m_A(i, j).m_Mx[0] << "    ";
	 file << setprecision(2) << setw(4) << m_A(i, j).m_Mx[1] << "    ";
	 file << setprecision(2) << setw(4) << m_A(i, j).m_Mx[2] << "    ";
	 }
	 file<<endl;
	 for(int j = 0; j < numVertices; j++) {
	 file << setprecision(2) << setw(4)<< m_A(i, j).m_Mx[3] << "    ";
	 file << setprecision(2) << setw(4)<< m_A(i, j).m_Mx[4] << "    ";
	 file << setprecision(2) << setw(4)<< m_A(i, j).m_Mx[5] << "    ";
	 }
	 file<<endl;
	 for(int j = 0; j < numVertices; j++) {
	 file << setprecision(2) << setw(4)<< m_A(i, j).m_Mx[6] << "    ";
	 file << setprecision(2) << setw(4)<< m_A(i, j).m_Mx[7] << "    ";
	 file << setprecision(2) << setw(4)<< m_A(i, j).m_Mx[8] << "    ";
	 }
	 file<<endl;
	 }
	 file<<endl<<endl<<endl;
	 cout<<"wrote A to file"<<endl;
	 file<<"b: "<<endl;
	 for(int p = 0; p < numVertices; p++)
	 file<< m_b.m_pData[p].x<<endl<<setprecision(4) << setw(4)<<m_b.m_pData[p].y <<endl<<m_b.m_pData[p].z<<endl;
	 cout<<"wrote b to file"<<endl;
	 }
	 file.close();
	 cout<<"close file and exit"<<endl;
	 exit(1);
	 }
	 //*/

	//CG(&m_A);

	//*
	int iIterations = 0, iMaxIterations = 2000;
	double alpha, Delta_0, Delta_old, Delta_new;
	double Eps = 1e-5;
	double Eps_Sq = Eps*Eps;

	//double FLOOR_Y = -0.99;
	//
	// Setup the inverse of preconditioner -- We use a vector for memory efficiency.  Technically it's the diagonal of a matrix
	//
	for (i = 0; i < numVertices; i++) {
		m_PInv.m_pData[i].x = m_A(i, i).m_Mx[0];
		m_PInv.m_pData[i].y = m_A(i, i).m_Mx[4];
		m_PInv.m_pData[i].z = m_A(i, i).m_Mx[8];
		//m_PInv.m_pData[i].x = 1;
		//m_PInv.m_pData[i].y = 1;
		//m_PInv.m_pData[i].z = 1;
	}
	m_PInv.Invert(m_P);

	//
	// Modified Preconditioned Conjugate Gradient method
	//

	m_dv = m_z;

	// delta_0 = DotProduct( filter( b ), P * filter( b ) );
	m_b.ElementMultiply(m_S, m_vTemp1);
	m_P.ElementMultiply(m_vTemp1, m_vTemp2);
	Delta_0 = m_vTemp2.DotProduct(m_vTemp1);
	if (Delta_0 < 0) {
		m_b.Dump("b:\r\n");
		m_P.Dump("P:\r\n");
		cout
				<< "Negative Delta_0 most likely caused by a non-Positive Definite matrix\r\n"
				<< endl;
	}

	// r = filter( b - A * dv )
	m_A.PostMultiply(m_dv, m_vTemp1);
	m_b.Subtract(m_vTemp1, m_vTemp2);
	m_vTemp2.ElementMultiply(m_S, m_r);

	// c = filter( Pinv * r )
	m_PInv.ElementMultiply(m_r, m_vTemp1);
	m_vTemp1.ElementMultiply(m_S, m_c);

	Delta_new = m_r.DotProduct(m_c);

	if (Delta_new < Eps_Sq * Delta_0) {
		//m_b.Dump("b: \r\n");
		//m_P.Dump("P: \r\n");
		cout << "This isn't good!  Probably a non-Positive Definite matrix\r\n"
				<< endl;
	}

	while ((Delta_new > Eps_Sq * Delta_0) && (iIterations < iMaxIterations)) {
		m_A.PostMultiply(m_c, m_vTemp1);

		m_vTemp1.ElementMultiply(m_S, m_q);

		alpha = Delta_new / (m_c.DotProduct(m_q));
		m_c.Scale(alpha, m_vTemp1);
		m_dv.Add(m_vTemp1, m_dv);

		m_q.Scale(alpha, m_vTemp1);
		m_r.Subtract(m_vTemp1, m_r);

		m_PInv.ElementMultiply(m_r, m_s);
		Delta_old = Delta_new;
		Delta_new = m_r.DotProduct(m_s);

		m_c.Scale(Delta_new / Delta_old, m_vTemp1);
		m_s.Add(m_vTemp1, m_vTemp2);
		m_vTemp2.ElementMultiply(m_S, m_c);

		iIterations++;
	}
	//cout<<"Intel's CG Converged at iteration "<<iIterations<<" with a residual of "<<Delta_new<<" < "<<Eps_Sq * Delta_0<<endl;
	//*/
	/*
	 double cutIt = 0.001;
	 for(int i = 0; i < m_dv.Size(); i++) {
	 m_dv.m_pData[i].x = 0;
	 //m_dv.m_pData[i].y = 0;
	 m_dv.m_pData[i].z = 0;
	 if(m_dv.m_pData[i].x < cutIt && m_dv.m_pData[i].x > -cutIt){
	 //cout<<"value Cut"<<endl;
	 m_dv.m_pData[i].x = 0;
	 }
	 if(m_dv.m_pData[i].y < cutIt && m_dv.m_pData[i].y > -cutIt){
	 m_dv.m_pData[i].y = 0;
	 }
	 if(m_dv.m_pData[i].z < cutIt && m_dv.m_pData[i].z > -cutIt){
	 m_dv.m_pData[i].z = 0;
	 }
	 }
	 //*/

	m_Velocities.Add(m_dv, m_Velocities);
	m_dv.Scale(g, m_dv);
	m_Velocities.Add(m_dv, m_vTemp1);
	m_vTemp1.Scale(timeStep, m_vTemp1);
	//CRAPPY CONSTRAINTS
	//m_Velocities.m_pData[0].x = 0;
	//m_Velocities.m_pData[0].y = 0;
	//m_Velocities.m_pData[0].z = 0;
	//Implicit Euler
	//m_Velocities.Scale( timeStep, m_vTemp1 );
	// for(int i = 0; i < numVertices; i++) {
	//  cout<<setprecision(2) << setw(4)<<mesh->getVertex(i)->getvX()[0]<<" "<<setprecision(2) << setw(4)<<mesh->getVertex(i)->getvX()[1]<<" "<<setprecision(2) << setw(4)<<mesh->getVertex(i)->getvX()[2]<<endl;
	// }
	m_Positions.Add(m_vTemp1, m_Positions);
	/*
	 for(int i =0; i < m_Velocities.Size(); i++)
	 cout<<m_Velocities.m_pData[i].x<<" "<<m_Velocities.m_pData[i].y<<" "<<m_Velocities.m_pData[i].z<<endl;

	 for(int i =0; i < m_Positions.Size(); i++)
	 cout<<m_Positions.m_pData[i].x<<" "<<m_Positions.m_pData[i].y<<" "<<m_Positions.m_pData[i].z<<endl;
	 //*/
	/*
	 file<<"v: "<<endl;
	 for(i = 0; i < numVertices; i++)
	 file<<  m_Velocities.m_pData[i].x<<endl<<m_Velocities.m_pData[i].y <<endl<<m_Velocities.m_pData[i].z<<endl;
	 file<<"p: "<<endl;
	 for(i = 0; i < numVertices; i++)
	 file<<  m_Positions.m_pData[i].x<<endl<<m_Positions.m_pData[i].y <<endl<<m_Positions.m_pData[i].z<<endl;
	 file.close();
	 exit(1);
	 //*/

	/*
	 for( i=0; i<m_iParticles; i++ )
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
	 fTotal += (Physics_t)fabs( fStep );
	 */

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
void System::setTableConstraints() {
	Physics_Matrix3x3 zeroMat;
	zeroMat.Zero();
	double x, y, z, Rtable, Htable;
	double eps_table = .01;

	for (int i = 0; i < numVertices; i++) {
		x = m_Positions.m_pData[i].x;
		y = m_Positions.m_pData[i].y;
		z = m_Positions.m_pData[i].z;
		Rtable = .5;
		Htable = .4;
		if (sqrt(x * x + z * z) < Rtable && y < Htable + eps_table && y
				> Htable - eps_table) {
			m_S[i].m_Mx[0] = 0;
			m_S[i].m_Mx[1] = 0;
			m_S[i].m_Mx[2] = 0;
			m_S[i].m_Mx[3] = 0;
			m_S[i].m_Mx[4] = 0;
			m_S[i].m_Mx[5] = 0;
			m_S[i].m_Mx[6] = 0;
			m_S[i].m_Mx[7] = 0;
			m_S[i].m_Mx[8] = 0;
		}
	}
}
void System::takeStep(Solver* solver, vector<Constraint*>* constraints, vector<Constraint*>* collisions, double timeStep) {
	std::vector<std::pair<vec3, vec3> > changes(mesh->vertices.size(),
			make_pair(vec3(0, 0, 0), vec3(0, 0, 0)));
	/*
	 if(globalCount == 1){
	 for(int i = 0; i < numVertices; i++) {
	 mesh->getVertex(i)->getvX()[0] = 2.5;
	 mesh->getVertex(i)->getvX()[1] = 2.5;
	 mesh->getVertex(i)->getvX()[2] = 2.5;
	 }
	 }
	 //*/
	/*
	 cout<<"Position: "<<endl;
	 for(int i = 0; i < numVertices; i++) {
	 cout<<setprecision(2) << setw(4)<<mesh->getVertex(i)->getX()[0]<<" "<<setprecision(2) << setw(4)<<mesh->getVertex(i)->getX()[1]<<" "<<setprecision(2) << setw(4)<<mesh->getVertex(i)->getX()[2]<<endl;
	 } cout<<endl;
	 cout<<"Velocity: "<<endl;
	 for(int i = 0; i < numVertices; i++) {
	 cout<<setprecision(2) << setw(4)<<mesh->getVertex(i)->getvX()[0]<<" "<<setprecision(2) << setw(4)<<mesh->getVertex(i)->getvX()[1]<<" "<<setprecision(2) << setw(4)<<mesh->getVertex(i)->getvX()[2]<<endl;
	 } cout<<endl;
	 //*/

	//cout<<"Calculate State, load force matrix"<<endl;

	//solver->calculateState(this); //evalDeriv function
	calculateInternalForces();
	calculateExternalForces();

	//cout<<"Force: "<<endl;
	//for(int i = 0; i < numVertices; i++) {
	//cout<<setprecision(2) << setw(4)<<mesh->getVertex(i)->getF()[0]<<" "<<setprecision(2) << setw(4)<<mesh->getVertex(i)->getF()[1]<<" "<<setprecision(2) << setw(4)<<mesh->getVertex(i)->getF()[2]<<endl;
	//}cout<<endl<<endl;

	/*
	 double cut = 0.0001;

	 for(int i = 0; i < numVertices; i++) {
	 for(int j = 0; j < numVertices; j++) {
	 for(int k = 0; k < 8; k++) {
	 if( m_TotalForces_dp(i, j).m_Mx[k] < cut && m_TotalForces_dp(i, j).m_Mx[k] > -cut && m_TotalForces_dp(i, j).m_Mx[k] != 0){
	 //m_TotalForces_dp(i, j).m_Mx[k] = 0;
	 cout<<"roundoff in zeros of Position Jacobian: "<<m_TotalForces_dp(i, j).m_Mx[k]<<endl;
	 }
	 }

	 }
	 }

	 for(int i = 0; i < numVertices; i++) {
	 for(int j = 0; j < numVertices; j++) {
	 for(int k = 0; k < 8; k++) {
	 if( m_TotalForces_dv(i, j).m_Mx[k] < cut && m_TotalForces_dv(i, j).m_Mx[k] > -cut && m_TotalForces_dp(i, j).m_Mx[k] != 0) {
	 cout<<"roundoff in zeros of Velocity Jacobian "<<m_TotalForces_dv(i, j).m_Mx[k]<<endl;
	 //m_TotalForces_dp(i, j).m_Mx[k] = 0;
	 }
	 }
	 }
	 }

	 //exit(1);
	 //*/
	for(int i = 0; i < constraints->size(); i++)
		(*constraints)[i]->applyConstraintToPoints();

	loadMatrices();

	if(collisions->size() > 0)
		(*collisions)[0]->applyCollisionToMesh(&m_Positions, &m_Velocities);

	/*
	 double cutIt = 0.001;
	 for(int i = 0; i < m_TotalForces.Size(); i++) {
	 if(m_TotalForces.m_pData[i].x < cutIt && m_TotalForces.m_pData[i].x > -cutIt){
	 //cout<<"value Cut"<<endl;
	 m_TotalForces.m_pData[i].x = 0;
	 }
	 if(m_TotalForces.m_pData[i].y < cutIt && m_TotalForces.m_pData[i].y > -cutIt)
	 m_TotalForces.m_pData[i].y = 0;
	 if(m_TotalForces.m_pData[i].z < cutIt && m_TotalForces.m_pData[i].z > -cutIt)
	 m_TotalForces.m_pData[i].z = 0;
	 }
	 //*/
	//	cout<<"Force: "<<endl;
	//	for(int i =0; i < m_TotalForces.Size(); i++)
	//		cout<<m_TotalForces.m_pData[i].x<<" "<<m_TotalForces.m_pData[i].y<<" "<<m_TotalForces.m_pData[i].z<<endl<<endl;
	/*

	 ofstream file ("SparseMatrix.txt");
	 if (file.is_open())
	 {
	 for(int i = 0; i < numVertices; i++){
	 file<<"Vertex: "<<mesh->getVertex(i)->getID()<<
	 "  Position: "<<mesh->getVertex(i)->getX()<<
	 "  Velocity: "<<mesh->getVertex(i)->getvX()<<endl;
	 }
	 file<<endl;
	 file<<"DfDx: "<<endl;
	 for(int i = 0; i < numVertices; i++) {
	 for(int j = 0; j < numVertices; j++) {
	 file << setprecision(2) << setw(4) << m_TotalForces_dp(i, j).m_Mx[0] << "  ";
	 file << setprecision(2) << setw(4) << m_TotalForces_dp(i, j).m_Mx[1] << "  ";
	 file << setprecision(2) << setw(4) << m_TotalForces_dp(i, j).m_Mx[2] << "  ";
	 }
	 file<<endl;
	 for(int j = 0; j < numVertices; j++) {
	 file << setprecision(2) << setw(4)<< m_TotalForces_dp(i, j).m_Mx[3] << "  ";
	 file << setprecision(2) << setw(4)<< m_TotalForces_dp(i, j).m_Mx[4] << "  ";
	 file << setprecision(2) << setw(4)<< m_TotalForces_dp(i, j).m_Mx[5] << "  ";
	 }
	 file<<endl;
	 for(int j = 0; j < numVertices; j++) {
	 file << setprecision(2) << setw(4)<< m_TotalForces_dp(i, j).m_Mx[6] << "  ";
	 file << setprecision(2) << setw(4)<< m_TotalForces_dp(i, j).m_Mx[7] << "  ";
	 file << setprecision(2) << setw(4)<< m_TotalForces_dp(i, j).m_Mx[8] << "  ";
	 }
	 file<<endl;
	 }
	 file<<endl<<endl<<endl;

	 file<<"DfDv: "<<endl;
	 for(int i = 0; i < numVertices; i++) {
	 for(int j = 0; j < numVertices; j++) {
	 file << setprecision(2) << setw(4) << m_TotalForces_dv(i, j).m_Mx[0] << "  ";
	 file << setprecision(2) << setw(4) << m_TotalForces_dv(i, j).m_Mx[1] << "  ";
	 file << setprecision(2) << setw(4) << m_TotalForces_dv(i, j).m_Mx[2] << "  ";
	 }
	 file<<endl;
	 for(int j = 0; j < numVertices; j++) {
	 file << setprecision(2) << setw(4) << m_TotalForces_dv(i, j).m_Mx[3] << "  ";
	 file << setprecision(2) << setw(4) << m_TotalForces_dv(i, j).m_Mx[4] << "  ";
	 file << setprecision(2) << setw(4) << m_TotalForces_dv(i, j).m_Mx[5] << "  ";
	 }
	 file<<endl;
	 for(int j = 0; j < numVertices; j++) {
	 file << setprecision(2) << setw(4) << m_TotalForces_dv(i, j).m_Mx[6] << "  ";
	 file << setprecision(2) << setw(4) << m_TotalForces_dv(i, j).m_Mx[7] << "  ";
	 file << setprecision(2) << setw(4) << m_TotalForces_dv(i, j).m_Mx[8] << "  ";
	 }
	 file<<endl;
	 }
	 file<<endl<<endl<<endl;
	 file.close();
	 }
	 exit(1);
	 //*/
	//*

	// setTableConstraints();
	//m_TotalForces_dp.Zero();
	//m_TotalForces_dv.Zero();
	SolveImplicit(timeStep, constraints);
	//SolveExplicit(timeStep);
	/*
	 cout<<"Position: "<<endl;
	 for(int i = 0; i < numVertices; i++) {
	 cout<<setprecision(2) << setw(4)<<mesh->getVertex(i)->getX()[0]<<" "<<setprecision(2) << setw(4)<<mesh->getVertex(i)->getX()[1]<<" "<<setprecision(2) << setw(4)<<mesh->getVertex(i)->getX()[2]<<endl;
	 } cout<<endl;
	 cout<<"Velocity: "<<endl;
	 for(int i = 0; i < numVertices; i++) {
	 cout<<setprecision(2) << setw(4)<<mesh->getVertex(i)->getvX()[0]<<" "<<setprecision(2) << setw(4)<<mesh->getVertex(i)->getvX()[1]<<" "<<setprecision(2) << setw(4)<<mesh->getVertex(i)->getvX()[2]<<endl;
	 } cout<<endl;
	 //*/
	//if(globalCount == 2)
	//exit(1);


	m_TotalForces.Zero();
	m_TotalForces_dp.Zero();
	m_TotalForces_dv.Zero();
	m_MxTemp1.Zero();
	m_MxTemp2.Zero();
	m_z.Zero();
	m_PInv.Zero();

	m_dv.Zero();
	m_dp.Zero();
	m_PosTemp.Zero();
	//m_A.Zero();
	m_P.Zero();
	m_b.Zero();
	m_r.Zero();
	m_c.Zero();
	m_q.Zero();
	m_s.Zero();
	m_y.Zero();

	//*/

	/*
	 for (unsigned int i = 0; i < mesh->countVertices(); i++) {
	 std::pair<vec3,vec3> deltas =
	 solver->solve(this, timeStep, i, mesh->getVertex(i));
	 changes[i] = deltas;
	 }
	 //*/
	//cout<<"f u"<<endl;
	//exit(1);
	//cout<<mesh->getVertex(mesh->vertices.size())->getX()<<endl;
	TriangleMeshVertex* v;
	for (unsigned int i = 0; i < mesh->vertices.size(); i++) {
		v = mesh->getVertex(i);
		//        v->getX() += changes[i].first;
		//        v->getvX() += changes[i].second;

		//*
		v->getX()[0] = m_Positions.m_pData[i].x;
		v->getX()[1] = m_Positions.m_pData[i].y;
		v->getX()[2] = m_Positions.m_pData[i].z;

		v->getvX()[0] = m_Velocities.m_pData[i].x;
		v->getvX()[1] = m_Velocities.m_pData[i].y;
		v->getvX()[2] = m_Velocities.m_pData[i].z;
		//  cout<<v->getvX()[0]<<" "<<v->getvX()[1] <<" "<<v->getvX()[2] <<endl;
		//  cout<<"F: "<<v->getF()<<endl;
		v->clearF();

	}
	//cout<<globalCount<<endl;
	//cout<<"i got here"<<endl;
	if (globalCount == 2) {
		//    	exit(1);
		//		file.close();
	}

	globalCount++;

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
/*
Solver::~Solver() {

}

ImplicitSolver::~ImplicitSolver() {

}

void ImplicitSolver::calculateState(System* sys) {
	sys->calculateInternalForces();
	sys->calculateExternalForces();
}

std::pair<vec3, vec3> ImplicitSolver::solve(System* sys, double timeStep,
		int pointIndex, TriangleMeshVertex* point) {

	//*
	TriangleMeshVertex* a = point;
	double h = timeStep;

	double m = a->getm();
	mat3 S = sys->calculateContraints(pointIndex);
	mat3 W = (1 / m) * S;
	vec3 Z(0, 0, 0);

	vec3 v0 = a->getvX();
	mat3 I = identity2D();
	vec3 F = a->getF();
	std::pair<mat3, mat3> partials = sys->calculateForcePartials(pointIndex);
	mat3 dFx = partials.first;
	mat3 dFv = partials.second;

	mat3 A = I - h * W * dFv - h * h * W * dFx;
	vec3 b = h * W * (F + h * dFx * v0) + Z;
	vec3 deltaV = A.inverse() * b;
	vec3 deltaX = h * (v0 + deltaV);

	//cout << "Forces on particle " << pointIndex << " is (" << F[0] << ", " << F[1] << ", " << F[2] << ")" << endl;

	return make_pair(deltaX, deltaV);
}

ExplicitSolver::~ExplicitSolver() {

}

void ExplicitSolver::calculateState(System* sys) {
	sys->calculateInternalForces();
	sys->calculateExternalForces();
}

std::pair<vec3, vec3> ExplicitSolver::solve(System* sys, double timeStep,
		int pointIndex, TriangleMeshVertex* point) {

	vec3 deltaV = (timeStep) * point->getF() / (double) point->getm();
	vec3 deltaX = timeStep * (point->getvX() + deltaV);

	/*
	 if (sys->getT() > 0.1) {
	 cout << "deltaV=" << deltaV << endl;
	 cout << "deltaX=" << deltaX << endl;
	 cout << "F=" << point->getF() << endl;
	 cout << "vX=" << point->getvX() << endl;
	 exit(1);
	 }
	 //*/
	//cout<<"Force: "<<point->getF()/point->getm()<<endl;
//	point->clearF();

	//Contraints set in OBJ file
//	if (point->getConstaint()[0] == 0)
//		return make_pair(vec3(0, 0, 0), vec3(0, 0, 0)); //Lame contraints for explicit

	//cout << "Forces on particle " << pointIndex << " is (" << F[0] << ", " << F[1] << ", " << F[2] << ")" << endl;
	//******************************************************************************
//	return make_pair(deltaX, deltaV);
//}

