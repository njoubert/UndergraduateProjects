/*
 * System.cpp
 *
 *  Created on: Sep 7, 2008
 *      Author: njoubert
 */

#include "System.h"

/////////////////////////////WHERE IS THIS CALLED////////////////////////////////////
System::System(TriangleMesh* m, Material* mat) :
	mesh(m), _mat(mat) {
	mouseSelected = NULL;
cout<<"			Creating System"<<endl;
	_nrCollTests = 1;
	_maxNrCollTests = 3;

	//*  THIS MUST BE UNCOMMENTED FOR STRAIN LIMITING OR MESH SYNCING TO WORK
	for(int k = 0; k < m->countVertices(); k++)
		_correctedIndices.push_back(false);
	for(int l = 0; l < m->countVertices(); l++)
		_strainCorrectedIndices.push_back(false);
//*
	for(int h = 0; h < 2500; h++) {
		_posError.push_back(-1.0);
		_velError.push_back(-1.0);
		_timeOfError.push_back(-1.0);
	}
	_errorInd = -1;

	//*/
//*/

	//TAKES VALUE FROM GLOBALS FOR COLLISION DAMPING
	_Kcoll = Kcoll;
	_USECOLLJACOBIAN = USECOLLJACOBIAN;

	//THIS INITIALIZES THE VALUES FOR DRAWING COLLISION DETECTION
	 for(int i = 0; i < m->countVertices(); i++)
		 m->getVertex(i)->detectedCollision() = false;
	 for(int i = 0; i < m->countVertices(); i++)
		 m->getVertex(i)->setMeshDiff(vec3(0));

	cout << "		Creating Sparse Mass Matrix..." << endl;
	int n = m->countVertices();
	vector<vector<int> > sparsePattern =
			MeshTOLargeMatrix::GenerateIdentityPattern(n);
	_M = new SPARSE_MATRIX(n, n, sparsePattern, true);

	mat3 mass;
	for (int i = 0; i < m->countVertices(); i++) {
		mass = identity2D();
		mass *= m->getVertex(i)->getm();
		(*_M)(i, i) = mass;
	}
	cout << "		Done Creating Sparse Mass Matrix." << endl;


	cout<<"			Initialized Strain Limiting"<<endl;

	//MATRICES FOR BARAFF CONSTRAINTS
	/*
	 int n = m->countVertices();
	 vector<vector<int> > sparsePatternNx1 = MeshTOLargeMatrix::GenerateNByOnePattern(n);
	 _S = new SPARSE_MATRIX(n,1,sparsePatternNx1,true);


	 _W = new SPARSE_MATRIX(n,n,sparsePattern,true);

	 mat3 S;
	 for(int i = 0; i < m->countVertices(); i++) {
	 if(mesh->getVertex(i)->getConstaint())
	 S = mat3(0);
	 else
	 S = mat3(identity2D());

	 mass = identity2D();
	 mass *= (1/m->getVertex(i)->getm());
	 mass = mass*S;
	 (*_W)(i,i) = mass;
	 }

	 _I = new SPARSE_MATRIX(n,n,sparsePattern,true);

	 for(int i = 0; i < m->countVertices(); i++)
	 (*_I)(i,i) = mat3(identity2D());

	 //*/
	//Check Edge Lengths and Decrease the short ones
	EdgesIterator edgs = mesh->getEdgesIterator();
	double averageRL = 0;
	double maxRL = 0;
	double minRL = 10e10;
	double edgeCounter = 0;
	do {
		TriangleMeshVertex* v1 = (*edgs)->getVertex(0);
		TriangleMeshVertex* v2 = (*edgs)->getVertex(1);
		if((*edgs)->getRestLength() > maxRL)
			maxRL = (*edgs)->getRestLength();
		if((*edgs)->getRestLength() < minRL)
			minRL = (*edgs)->getRestLength();


		averageRL += (*edgs)->getRestLength();
		edgeCounter++;
		//cout<<"Edge: "<<v1->getIndex()<<" - "<<v2->getIndex()<<" Rest Length: "<<(*edgs)->getRestLength()<<endl;
	} while (edgs.next());
	averageRL /= edgeCounter;
	cout<<"REST LENGTH - Average: "<<averageRL<<" Max: "<<maxRL<<" Min: "<<minRL<<" Range: "<<maxRL - minRL;

	edgs = mesh->getEdgesIterator();
	double squaredDiff, aveDiffs = 0, standardDev;
	do {
		squaredDiff = (averageRL - (*edgs)->getRestLength());
		squaredDiff *= squaredDiff;
		aveDiffs += squaredDiff;
	} while (edgs.next());
	standardDev = sqrt(aveDiffs/edgeCounter);
	cout<<" Standard Dev: "<<standardDev<<endl;

	double newMinRL = 10e10;
	edgs = mesh->getEdgesIterator();
	do {
		//if((*edgs)->getRestLength() < averageRL - standardDev)
			//(*edgs)->setRestLength(averageRL - standardDev);
		if((*edgs)->getRestLength() < newMinRL)
			newMinRL = (*edgs)->getRestLength();
	} while (edgs.next());
	//cout<<"Rest Lengths Floored at a minimum of "<<newMinRL<< " -- Should be: "<<averageRL - standardDev<<endl;

	bool REST_ANGLES = true;
	if (REST_ANGLES) {
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
				int ai = 0, bi = 0;
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
				if (mustBePos < 0)
					mustBePos = 0;

				double sinThetaOver2 = sign * sqrt((mustBePos) / 2);

				//*
				//		cout<<"x1: "<<x1<<"  x2: "<<x2<<"  x3: "<<x3<<"  x4: "<<x4<<endl;
				// 		cout<<"N1unit: "<<N1unit<<"  N2unit: "<<N2unit<<"  Eunit: "<<Eunit<<endl;
				// cout << "Rest sinThetaOver2 " << sinThetaOver2 << endl;
				// 		cout<<endl;
				//*/
				(*edg_it)->setRestAngle(sinThetaOver2);
				//			    cout<<"Edge: "<<(*it)<<" Rest Angle: "<<(*it)->getRestAngle()<<endl;
				//				cout<<endl;
			}

		} while (edg_it.next());
	}
	//*******************DONE WITH REST ANGLE ************************************


	//********************* INITIALIZE X AND VX************************************

	_x = new LargeVec3Vector(m->countVertices());
	_v = new LargeVec3Vector(m->countVertices());

	_x0 = new LargeVec3Vector(m->countVertices());
	_v0 = new LargeVec3Vector(m->countVertices());

	_a = new LargeVec3Vector(m->countVertices());

	// cout<<_x->size()<<" "<<_x0->size()<<"	"<<_v->size()<<" "<<_v0->size()<<endl;
}

/*
 * This function loads the current positions and velocities from the mesh into
 * the state of this system. Used at the beginning of Model.advance()
 */
void System::loadStateFromMesh() {
	profiler.frametimers.switchToTimer("loadfrom mesh");
	TriangleMeshVertex* v;
	for (int i = 0; i < mesh->countVertices(); i++) {
		v = mesh->getVertex(i);
		(*_x)[i] = v->getX();
		(*_v)[i] = v->getvX();
	}
	_a->zeroValues();
	profiler.frametimers.switchToGlobal();
}

void System::loadMeshFromState() {
	profiler.frametimers.switchToTimer("loadfrom mesh");
	TriangleMeshVertex* v;
	for (int i = 0; i < mesh->countVertices(); i++) {
		v = mesh->getVertex(i);
		v->getX() = (*_x)[i];
		v->getvX() = (*_v)[i];
	}
	_a->zeroValues();
	profiler.frametimers.switchToGlobal();
}

void System::setHighQmesh(TriangleMesh* cMesh) {
	_cMesh = cMesh;
//	cout<<"Testing Correction Mesh..."<<endl;
//	TriangleMesh* test = _cMesh;
//	cout<<"test: "<<test->getVertex(0)->getX()<<endl;
}

void System::initializeSyncTimes() {
	_syncStep = SYNCSTEP;
	_syncCounter = _syncStep;
	cout<<"SYNCSTEP IS: "<<SYNCSTEP<<" syncCOUNTER is inialized to "<<_syncCounter<<endl;
}

LARGE_VECTOR* System::getX() {
	return _x;
}

LARGE_VECTOR* System::getV() {
	return _v;
}
LARGE_VECTOR* System::getA() {
	return _a;
}
SPARSE_MATRIX* System::getM() {
	return _M;
}

SPARSE_MATRIX* System::getW() {
	return _W;
}

SPARSE_MATRIX* System::getI() {
	return _I;
}

int System::getNrCollTests() {
	return _nrCollTests;
}

int System::getMaxNrCollTests() {
	return _maxNrCollTests;
}

void System::setNrCollTests(int nrCollTets) {
	_nrCollTests = nrCollTets;
}

void System::setMaxNrCollTests(int maxNrCollTests) {
	_maxNrCollTests = maxNrCollTests;
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
	vec3 f;
	if(rl > L)
		f = -(Ks_comp) * (l / L) * (L - rl);
	else
		f = -Ks * (l / L) * (L - rl);

	return f;
}
vec3 System::f_damp(vec3 & pa, vec3 & pb, vec3 & va, vec3 & vb, double rl,
		double Kd) {
	vec3 l = pa - pb;
	vec3 w = va - vb;
	double L = l.length();
	vec3 f = -Kd * ((l * w) / (L * L)) * l;
	return f;
}

void System::bendForce(TriangleMeshTriangle* A, TriangleMeshTriangle* B,
		TriangleMeshVertex* a, TriangleMeshVertex* b, TriangleMeshEdge* edge,
		LARGE_VECTOR* F) {
	TriangleMeshVertex** aList = A->getVertices();
	TriangleMeshVertex** bList = B->getVertices();

	if (A != NULL && B != NULL && A != B) {

		//Dirty way to figure out which point is which
		int ai = 0, bi = 0;
		for (int i = 0; i < 3; i++) {
			if (aList[i] != a && aList[i] != b)
				ai = i;

			if (bList[i] != a && bList[i] != b)
				bi = i;
		}

		//double _Ke = _mat->getKBe();
		//double _Kd = _mat->getKBd();
		double _Ke = KBe;
		double _Kd = KBd;

		//_Ke = 1;
		//_Kd = 0;

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
		//	for (int i = 0; i < 4; i++)
		//		Fe[i] = _Ke * ((Emag * Emag) / (N1mag + N2mag)) * (sinThetaOver2 - sin(edge->getRestAngle()/2))
		//				* u[i];


		for (int i = 0; i < 4; i++)
			Fe[i] = _Ke * ((Emag * Emag) / (N1mag + N2mag)) * (sinThetaOver2
					- edge->getRestAngle()) * u[i];

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
		vec3 Fb[4];
		for (int i = 0; i < 4; i++)
			Fb[i] = Fe[i] + Fd[i];

		//aList[ai]->setF(F[1]);
		//bList[bi]->setF(F[0]);
		//a->setF(F[3]);
		//b->setF(F[2]);

		(*F)[aList[ai]->getIndex()] += Fb[1];
		(*F)[bList[bi]->getIndex()] += Fb[0];
		(*F)[a->getIndex()] += Fb[3];
		(*F)[b->getIndex()] += Fb[2];

	}
	//*/
}

//This should be in algebra3.h but when compiling with free image it causes an error
//saying that Abs has already been defined...
inline double Abs(double value) {
	return abs(value);
}

void System::bendForceJacobian(TriangleMeshTriangle* A,
		TriangleMeshTriangle* B, TriangleMeshVertex* a, TriangleMeshVertex* b,
		TriangleMeshEdge* edge, SPARSE_MATRIX* JV, SPARSE_MATRIX* JP) {

	TriangleMeshVertex** aList = A->getVertices();
	TriangleMeshVertex** bList = B->getVertices();

	if (A != NULL && B != NULL && A != B) {

		//Dirty way to figure out which point is which
		int ai = 0, bi = 0;
		for (int i = 0; i < 3; i++) {
			if (aList[i] != a && aList[i] != b)
				ai = i;

			if (bList[i] != a && bList[i] != b)
				bi = i;
		}

		//double _Ke = _mat->getKBe();
		//double _Kd = _mat->getKBd();
		double ke = KBe;
		double kd = KBd;

		//_Ke = 1;
		//_Kd = 0;

		//double Ke = 0;
		//double Kd = 0;

		//*
		vec3 x2 = aList[ai]->getX();
		vec3 x1 = bList[bi]->getX();
		vec3 x4 = a->getX();
		vec3 x3 = b->getX();

		int ix2 = aList[ai]->getIndex();
		int ix1 = bList[bi]->getIndex();
		int ix4 = a->getIndex();
		int ix3 = b->getIndex();

		vec3 v2 = aList[ai]->getvX();
		vec3 v1 = bList[bi]->getvX();
		vec3 v4 = a->getvX();
		vec3 v3 = b->getvX();

		int iv2 = aList[ai]->getIndex();
		int iv1 = bList[bi]->getIndex();
		int iv4 = a->getIndex();
		int iv3 = b->getIndex();

		double x10 = x1[0];
		double x11 = x1[1];
		double x12 = x1[2];
		double x20 = x2[0];
		double x21 = x2[1];
		double x22 = x2[2];
		double x30 = x3[0];
		double x31 = x3[1];
		double x32 = x3[2];
		double x40 = x4[0];
		double x41 = x4[1];
		double x42 = x4[2];

		/*
		 (*JV)(ix1, iv1) += Df1_Dv1;
		 (*JV)(ix1, iv2) += Df1_Dv2;
		 (*JV)(ix1, iv3) += Df1_Dv3;
		 (*JV)(ix1, iv4) += Df1_Dv4;

		 (*JV)(ix2, iv1) += Df2_Dv1;
		 (*JV)(ix2, iv2) += Df2_Dv2;
		 (*JV)(ix2, iv3) += Df2_Dv3;
		 (*JV)(ix2, iv4) += Df2_Dv4;

		 (*JV)(ix3, iv1) += Df3_Dv1;
		 (*JV)(ix3, iv2) += Df3_Dv2;
		 (*JV)(ix3, iv3) += Df3_Dv3;
		 (*JV)(ix3, iv4) += Df3_Dv4;

		 (*JV)(ix4, iv1) += Df4_Dv1;
		 (*JV)(ix4, iv2) += Df4_Dv2;
		 (*JV)(ix4, iv3) += Df4_Dv3;
		 (*JV)(ix4, iv4) += Df4_Dv4;
		 //*/
	}

}

inline mat3 System::dfdx_spring(vec3 & pa, vec3 & pb, double rl, double Ks) {
	/* Jacobian Check with Matlab File "springsJacobCheck.nb"
	pa[0] = 0.2523, pa[1] = -0.2352, pa[2] = 0.5933;
	pb[0] = -1.52, pb[1] = -1.2, pb[2] = .01;
	rl = .987, Ks = 100;
	//*/
	mat3 I = identity2D();
	vec3 l = pa - pb;
	double L = l.length();
	mat3 oProd = outerProduct(l, l);
	double lDOT = l * l;
	mat3 jacob;
	if(rl > L)
		jacob = -(Ks_comp) * ((1 - (rl / L)) * (I - (oProd / lDOT)) + (oProd / lDOT));
	else
		jacob = -Ks * ((1 - (rl / L)) * (I - (oProd / lDOT)) + (oProd / lDOT));
	//cout<<"dfdx_spring: " <<endl<<jacob<<endl;
	return jacob;
}
inline mat3 System::dfdx_damp(vec3 & pa, vec3 & pb, vec3 & va, vec3 & vb,
		double rl, float Kd) {
	/* Jacobian Check with Matlab File "springsJacobCheck.nb"
	pa[0] = 0.2523, pa[1] = -0.2352, pa[2] = 0.5933;
	va[0] = 1.623, va[1] = 1.572, va[2] = -1.573;
	pb[0] = -1.52, pb[1] = -1.2, pb[2] = .01;
	vb[0] = 2, vb[1] = -2, vb[2] = 5.2;
	rl = .987, Kd = 1;
	//*/
	mat3 I = identity2D();
	vec3 P = pa - pb;
	vec3 V = va - vb;
	mat3 jacob = -Kd * ((outerProduct(P, V) / (P * P)) + (2 * ((-P * V) / ((P * P)
			* (P * P))) * outerProduct(P, P)) + (((P * V) / (P * P)) * I));
	//cout<<"dfdx_damp: "<<endl<<jacob<<endl;
	return jacob;
}
mat3 System::dfdv_damp(vec3 & pa, vec3 & pb, double rl, double Kd) {
	/* Jacobian Check with Matlab File "springsJacobCheck.nb"
	pa[0] = 0.2523, pa[1] = -0.2352, pa[2] = 0.5933;
	pb[0] = -1.52, pb[1] = -1.2, pb[2] = .01;
	rl = .987, Kd = 1;
	//*/
	mat3 I = identity2D();
	vec3 l = pa - pb;
	mat3 oProd = outerProduct(l, l);
	double lDOT = l * l;
	mat3 RETURN = -Kd * (oProd / lDOT);
	//cout<<"dfdv_damp: "<<endl<<RETURN<<endl;
	return RETURN;
}

void System::enableMouseForce(vec3 mousePosition) {
	//Find closest vertex
	//cout << "Mouse is at: " << mousePosition << endl;
	double currentDistance = 900000.0, d, d1, d2;
	TriangleMeshVertex* currentClosest = NULL;
	for (unsigned int i = 0; i < mesh->vertices.size(); i++) {
		d1 = (mousePosition[0] - mesh->getVertex(i)->getX()[0]);
		d2 = (mousePosition[1] - mesh->getVertex(i)->getX()[1]);
		d = sqrt(d1 * d1 + d2 * d2);
		if (d < currentDistance) {
			currentDistance = d;
			currentClosest = mesh->getVertex(i);
			mouseP = mousePosition;
		}
	}

	//sets mouseSelected to this vertex.
	mouseSelected = currentClosest;
	//cout<<"Selected Vertex: "<<mouseSelected<<endl;
	//cout<<"Mouse Position: "<<mouseP<<endl;
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
	vec3 f = -_mat->getKe() * (l / L) * (L - rl);
	return (f);
}

int System::setVertexPos2MousePos() {
	TriangleMeshVertex* selected;
	for (int i = 0; i < mesh->countVertices(); i++) {
		selected = mesh->getVertex(i);
		if (selected == mouseSelected) {
			selected->getX() = mouseP;
			(*_x)[i][0] = mouseP[0];
			(*_x)[i][1] = mouseP[1];
			(*_x)[i][2] = mouseP[2];
			return i;
		}
	}
	cout << "PROBLEM WITH MOUSE FORCE!!!??" << endl;
	return 0;
}

void System::applyMouseConst2Matrices(SPARSE_MATRIX* A, LARGE_VECTOR* b) {
	TriangleMeshVertex* selected;
	for (int i = 0; i < mesh->countVertices(); i++) {
		selected = mesh->getVertex(i);
		if (selected == mouseSelected) {
			//Update the sparse matrices to honor this constraint
			A->zeroRowCol(i, i, true);
			//TODO: Should this be taking the ellipse speed into account?
			(*b)[i] = vec3(0, 0, 0);
		}
	}
}

void System::calculateWindForces(LARGE_VECTOR* f, SPARSE_MATRIX* JV, int i) {
	//cout<<"Wind Being Calculated"<<endl;
	double Kv = KDRAG;
	//vec3 u(0.0, 0.00, 0.01);
	vec3 u(WIND);
	//cout<<"u; "<<u<<endl;
	mat3 jv;

	//	for (int i = 0; i < mesh->countVertices(); i++) {
	//		cout<<"i: "<<i<<endl;
	TriangleMeshVertex* p = mesh->getVertex(i);
	vec3 n = p->getNormal();
	vec3 v = p->getvX();
	//u = u*(p->getX().length());
	vec3 vrel = v - u;
	if (!(n * vrel > 0))
		n = -1 * n;
	vec3 dragForce = Kv * (n * vrel) * -vrel;
	vec3 windForce = (n * u) * n;
	//vec3 windForce = vrel.length()*(n*vrel)*((n^vrel)^vrel);
	//vec3 windForce = (n*vrel)*((n^vrel)^vrel);
	(*f)[i] += windForce;
	(*f)[i] += dragForce;
	//cout<<"n: "<<n<<endl;
	//cout<<"u: "<<u<<endl;
	//cout<<"v: "<<v<<endl;

	//cout<<"vrel: "<<vrel<<endl;
	//cout<<"windforce: "<<windForce<<endl;
	//(*f)[i] += dragForce;
	//*
	jv[0][0] = Kv * n[0] * (u[0] - v[0]) - Kv * n[0] * (-u[0] + v[0]);
	jv[0][1] = 0;
	jv[0][2] = 0;
	jv[1][0] = 0;
	jv[1][1] = Kv * n[1] * (u[1] - v[1]) - Kv * n[1] * (-u[1] + v[1]);
	jv[1][2] = 0;
	jv[2][0] = 0;
	jv[2][1] = 0;
	jv[2][2] = Kv * n[2] * (u[2] - v[2]) - Kv * n[2] * (-u[2] + v[2]);
	(*JV)(i, i) += jv;
	//*/
	/*
	 jv[0][0] = Kw*n[0]*(-u[0]+v[0])*(-u[0]+v[0])*(-n[1]*u[1]-n[2]*u[2]+n[1]*v[1]+n[2]*v[2])+2*Kw*n[0]*(-u[0]+v[0])*(n[1]*u[0]*
	 u[1]-n[0]*u[1]*u[1]+n[2]*u[0]*u[2]-n[0]*u[2]*u[2]-n[1]*u[1]*v[0]-n[2]*u[2]*v[0]-n[1]*u[0]*v[1]+2*n[0]*u[1]*v[1]+n[1]*
	 v[0]*v[1]-n[0]*v[1]*v[1]-n[2]*u[0]*v[2]+2*n[0]*u[2]*v[2]+n[2]*v[0]*v[2]-n[0]*v[2]*v[2]);
	 jv[0][1] = Kw*n[0]*(-u[0]+v[0])*(-u[0]+v[0])*(-n[1]*u[0]+2*n[0]*u[1]+n[1]*v[0]-2*n[0]*v[1]);
	 jv[0][2] = Kw*n[0]*(-u[0]+v[0])*(-u[0]+v[0])*(-n[2]*u[0]+2*n[0]*u[2]+n[2]*v[0]-2*n[0]*v[2]);
	 jv[1][0] = Kw*n[1]*(-u[1]+v[1])*(-u[1]+v[1])*(2*n[1]*u[0]-n[0]*u[1]-2*n[1]*v[0]+n[0]*v[1]);
	 jv[1][1] = Kw*n[1]*(-u[1]+v[1])*(-u[1]+v[1])*(-n[0]*u[0]-n[2]*u[2]+n[0]*v[0]+n[2]*v[2])+2*Kw*n[1]*(-u[1]+v[1])*(-n[1]*
	 u[0]*u[0]+n[0]*u[0]*u[1]+n[2]*u[1]*u[2]-n[1]*u[2]*u[2]+2*n[1]*u[0]*v[0]-n[0]*u[1]*v[0]-n[1]*v[0]*v[0]-n[0]*u[0]*
	 v[1]-n[2]*u[2]*v[1]+n[0]*v[0]*v[1]-n[2]*u[1]*v[2]+2*n[1]*u[2]*v[2]+n[2]*v[1]*v[2]-n[1]*v[2]*v[2]);
	 jv[1][2] = Kw*n[1]*(-u[1]+v[1])*(-u[1]+v[1])*(-n[2]*u[1]+2*n[1]*u[2]+n[2]*v[1]-2*n[1]*v[2]);
	 jv[2][0] = Kw*n[2]*(-u[2]+v[2])*(-u[2]+v[2])*(2*n[2]*u[0]-n[0]*u[2]-2*n[2]*v[0]+n[0]*v[2]);
	 jv[2][1] = Kw*n[2]*(-u[2]+v[2])*(-u[2]+v[2])*(2*n[2]*u[1]-n[1]*u[2]-2*n[2]*v[1]+n[1]*v[2]);
	 jv[2][2] = Kw*n[2]*(-n[0]*u[0]-n[1]*u[1]+n[0]*v[0]+n[1]*v[1])*(-u[2]+v[2])*(-u[2]+v[2])+2*Kw*n[2]*(-u[2]+v[2])*(-n[2]*
	 u[0]*u[0]-n[2]*u[1]*u[1]+n[0]*u[0]*u[2]+n[1]*u[1]*u[2]+2*n[2]*u[0]*v[0]-n[0]*u[2]*v[0]-n[2]*v[0]*v[0]+2*n[2]*u[1]*
	 v[1]-n[1]*u[2]*v[1]-n[2]*v[1]*v[1]-n[0]*u[0]*v[2]-n[1]*u[1]*v[2]+n[0]*v[0]*v[2]+n[1]*v[1]*v[2]);
	 (*JV)(i, i) += jv;
	 //*/
	/*
	 jv[0][0] = Kv * n[0] * n[0];
	 jv[0][1] = Kv * n[0] * n[1];
	 jv[0][2] = Kv * n[0] * n[2];
	 jv[1][0] = Kv * n[0] * n[1];
	 jv[1][1] = Kv * n[1] * n[1];
	 jv[1][2] = Kv * n[1] * n[2];
	 jv[2][0] = Kv * n[0] * n[2];
	 jv[2][1] = Kv * n[1] * n[2];
	 jv[2][2] = Kv * n[2] * n[2];
	 (*JV)(i, i) += jv;
	 //*/
	/*
	 jv[0][0] = -3*Kw*n[0]*(u[0]-v[0])*(-u[0]+v[0])*(-u[0]+v[0])+Kw*n[0]*(-u[0]+v[0])*(-u[0]+v[0])*(-u[0]+v[0]);
	 jv[0][1] = 0;
	 jv[0][2] = 0;
	 jv[1][0] = 0;
	 jv[1][1] = -3*Kw*n[0]*(u[0]-v[0])*(-u[0]+v[0])*(-u[0]+v[0])+Kw*n[0]*(-u[0]+v[0])*(-u[0]+v[0])*(-u[0]+v[0]);
	 jv[1][2] = 0;
	 jv[2][0] = 0;
	 jv[2][1] = 0;
	 jv[2][2] = -3*Kw*n[0]*(u[0]-v[0])*(-u[0]+v[0])*(-u[0]+v[0])+Kw*n[0]*(-u[0]+v[0])*(-u[0]+v[0])*(-u[0]+v[0]);
	 (*JV)(i, i) += jv;
	 //*/
	//	}
}

void System::calculateExternalForces(Solver* solver) {
	TriangleMeshVertex* a;
	vec3 gravity(GRAVITY);
	//vec3 gravity(0,0,-9.8);
	LARGE_VECTOR* f = solver->getf();
	for (int i = 0; i < mesh->countVertices(); i++) {
		a = mesh->getVertex(i);
		(*f)[i] += (gravity * a->getm() += f_mouse(a));
		if(isWIND)
			calculateWindForces(f, solver->getJV(), i);
	/*
		if(i == 300) {
			vec3 n(0,0,1);
			double Kcd = 10000;
			(*f)[300] += (a->getvX()*n)*n*-Kcd;
			SPARSE_MATRIX* JV = solver->getJV();
			mat3 jv(0);
			jv[0][0] = -Kcd * n[0] * n[0];
			jv[0][1] = -Kcd * n[0] * n[1];
			jv[0][2] = -Kcd * n[0] * n[2];
			jv[1][0] = -Kcd * n[0] * n[1];
			jv[1][1] = -Kcd * n[1] * n[1];
			jv[1][2] = -Kcd * n[1] * n[2];
			jv[2][0] = -Kcd * n[0] * n[2];
			jv[2][1] = -Kcd * n[1] * n[2];
			jv[2][2] = -Kcd * n[2] * n[2];
			(*JV)(300, 300) += jv;
		}
		//*/
	}
}

void System::calculateInternalForces(Solver* solver) {
	LARGE_VECTOR* f = solver->getf();
	vec3 F0(0, 0, 0);
	TriangleMeshVertex *a, *b;
	EdgesIterator edg_it = mesh->getEdgesIterator();

	do {
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
		F0 = f_spring(pa, pb, rl, _mat->getKe()) + f_damp(pa, pb, va, vb, rl,
				_mat->getKd());

		/* Jacobian Check with Matlab File "springsJacobCheck.nb"
			pa[0] = 0.2523, pa[1] = -0.2352, pa[2] = 0.5933;
			va[0] = 1.623, va[1] = 1.572, va[2] = -1.573;
			pb[0] = -1.52, pb[1] = -1.2, pb[2] = .01;
			vb[0] = 2, vb[1] = -2, vb[2] = 5.2;
			rl = .987;
			double Ks = 100;
			double Kd = 1;
			cout<<"f_spring: "<<f_spring(pa, pb, rl, Ks)<<endl;
			cout<<"f_damp: "<<f_damp(pa, pb, va, vb, rl,Kd)<<endl;
			//*/

		//Calculate Bend Forces if they are to be calculated
		if (BEND_FORCES)
			bendForce((*edg_it)->getParentTriangle(0),
					(*edg_it)->getParentTriangle(1), a, b, (*edg_it), f);

		//Update force vector
		(*f)[a->getIndex()] += F0;
		(*f)[b->getIndex()] += (-1 * F0);

	} while (edg_it.next());
}

void System::calculateDampingToLimitStrain(Solver* solver, SPARSE_MATRIX* JV,
		double Ksd) {
	LARGE_VECTOR* f = solver->getf();
	vec3 F0(0, 0, 0);
	TriangleMeshVertex *a, *b;
	int ia, ib;
	EdgesIterator edg_it = mesh->getEdgesIterator();
	do {
		a = (*edg_it)->getVertex(0);
		b = (*edg_it)->getVertex(1);

		ia = a->getIndex();
		ib = b->getIndex();

		vec3 pa = a->getX();
		vec3 va = a->getvX();
		vec3 pb = b->getX();
		vec3 vb = b->getvX();
		vec3 Ua = a->getU();
		vec3 Ub = b->getU();
		vec3 RL = Ua - Ub;
		double rl = RL.length();

		vec3 strain = (pa - pb) - RL;
		vec3 p_dir = (pa - pb).normalize();
		double maxStrainPercent = .10;
		double strainPercent = strain.length() / rl;

		if (strainPercent > maxStrainPercent)
			F0 = -Ksd * ((va - vb) * p_dir) * p_dir;

		//Update Force vector
		(*f)[a->getIndex()] += F0;
		(*f)[b->getIndex()] += (-1 * F0);

		//Update Velocity Jacobian Matrix
		//*
		mat3 jv(0);
		//A
		double norm = ((pa[0] - pb[0]) * (pa[0] - pb[0]) + (pa[1] - pb[1])
				* (pa[1] - pb[1]) + (pa[2] - pb[2]) * (pa[2] - pb[2]));
		jv[0][0] = (-1 * Ksd * (pa[0] - pb[0]) * (pa[0] - pb[0])) / norm;
		jv[0][1] = (-1 * Ksd * (pa[0] - pb[0]) * (pa[1] - pb[1])) / norm;
		jv[0][2] = (-1 * Ksd * (pa[0] - pb[0]) * (pa[2] - pb[2])) / norm;
		jv[1][0] = (-1 * Ksd * (pa[0] - pb[0]) * (pa[1] - pb[1])) / norm;
		jv[1][1] = (-1 * Ksd * (pa[1] - pb[1]) * (pa[1] - pb[1])) / norm;
		jv[1][2] = (-1 * Ksd * (pa[1] - pb[1]) * (pa[2] - pb[2])) / norm;
		jv[2][0] = (-1 * Ksd * (pa[0] - pb[0]) * (pa[2] - pb[2])) / norm;
		jv[2][1] = (-1 * Ksd * (pa[1] - pb[1]) * (pa[2] - pb[2])) / norm;
		jv[2][2] = (-1 * Ksd * (pa[2] - pb[2]) * (pa[2] - pb[2])) / norm;
		/*
		 jv[0][0] = -1*Ksd;
		 jv[1][1] = -1*Ksd;
		 jv[2][2] = -1*Ksd;
		 //*/
		//*
		(*JV)(ia, ia) += jv;
		(*JV)(ia, ib) += -1 * jv;
		(*JV)(ib, ia) += -1 * jv;
		(*JV)(ib, ib) += jv;

		//*/

	} while (edg_it.next());
}

bool System::strainLimitSolverMatrices(SPARSE_MATRIX* A, LARGE_VECTOR* b) {
//*
	for (int i = 0; i < mesh->countVertices(); i++) {
		if(_strainCorrectedIndices[i]) {
			cout<<" Vertex: "<<i<<"'s Velocity is: "<<(*b)[i]<<endl;
			A->constrainSystem(i, i, b, (*_v)[i]);
			_strainCorrectedIndices[i] = false;
			cout<<" Vertex: "<<i<<"'s Velocity to: "<<(*_v)[i]<<endl;
		}
	}

	return true;
//*/
}

bool System::calcStrainLimitJacobi(Solver* solver, LARGE_VECTOR* y,
		double timeStep) {
	LARGE_VECTOR* f = solver->getf();
	vec3 F0(0, 0, 0);
	TriangleMeshVertex *a, *b;
	int ia, ib;
	EdgesIterator edg_it = mesh->getEdgesIterator();
	bool isOverStrained = false;
	vec3 maxStrain;

	do {
		a = (*edg_it)->getVertex(0);
		b = (*edg_it)->getVertex(1);
		ia = a->getIndex();
		ib = b->getIndex();
		vec3 pa = a->getX();
		vec3 pb = b->getX();
		vec3 paNew = a->getX();
		vec3 Ua = a->getU();
		vec3 Ub = b->getU();
		vec3 RL = Ua - Ub;
		double rl = RL.length();

		bool pipeOverstrained = false;

		double strain = (pa - pb).length() - rl;
		double strainPercent = ((strain) / rl) * 100;

		//*
		double savepapb = (pa - pb).length();
		vec3 saveVel = a->getvX();
//*
		if (abs(strainPercent) > 100) {

			//maxStrain = ((pa - (strain - 2*RL)) - pb) - RL;
			//(*y)[ia] = (pb + rl*(pa- pb)) - (*_x)[ia];
			//(*y)[ia] = (pa - (strain)) - (*_x)[ia];
			//(*y)[ia] = (pa - (strain - maxStrain)) - (*_x)[ia];
			//*
			//pa = pb + ((rl + .5*strain)/(rl + strain))*(pa - pb);
			if ((ia == 36) || (ia == 18)) {
				//(*y)[ib] = (pa + ((rl + .5*rl)/(rl + strain))*(pb - pa)) - (*_x)[ib];
				//pb = pa + ((rl + .5 * rl) / (rl + strain)) * (pb - pa);
				isOverStrained = false;

			} else if ((ib == 36) || (ib == 18)) {
				//(*y)[ia] = (pb + ((rl + .5*rl)/(rl + strain))*(pa - pb)) - (*_x)[ia];
				//pa = pb + ((rl + .5 * rl) / (rl + strain)) * (pa - pb);
				isOverStrained = false;
			} else {

				paNew = pb + ((rl + 1.0 * (strainPercent / 100) * rl) / (rl
						+ strain)) * (pa - pb);
				paNew = (*_x)[ia];
	//			(*y)[ia] = paNew - (*_x)[ia];
	//			mesh->getVertex(ia)->getX() = paNew;
	//			(*_v)[ia] = a->getvX();//(paNew - pa)/timeStep;
	//			mesh->getVertex(ia)->getvX() = a->getvX();//(paNew - pa)/timeStep;
				_strainCorrectedIndices[ia] = true;
				isOverStrained = true;
				pipeOverstrained = true;
			}
			//pa = (pa - (strain));

			//(*_v)[ia] = (*y)[ia] / timeStep;

			//(*_y)[ib] = (pb + (strain - .01*RL)/2) - (*_x)[ib];
		}
		//*/

		/*
		 if(pipeOverstrained) {
			 cout<<"before pa-pb: "<<savepapb<<endl;
			 cout<<"Velocity Before: "<<saveVel<<endl;
			 cout<<"Velocity After: "<<(*_v)[ia]<<endl;
			 cout<<"Edge Between Vert: "<<ia<<" and Vert: "<<ib<<endl;
			 cout<<"strainPercent Before; " << strainPercent << endl;
			 cout<<"strainPercent After: "<< (((mesh->getVertex(ia)->getX()  - mesh->getVertex(ib)->getX() ).length() - rl) / rl) * 100<<endl;
			 cout<<"is pa - pb: "<<(mesh->getVertex(ia)->getX()  - mesh->getVertex(ib)->getX()).length()<<" =  rl: "<<rl<<" ?"<<endl;
		 }
		 //*/
	} while (edg_it.next());

	return isOverStrained;
}

vector<double> System::getPosError(){ return _posError; }
vector<double> System::getVelError(){ return _velError; }
vector<double> System::getTimeOfError(){ return _timeOfError; }
int System::getErrorInd(){ return _errorInd; }

bool System::correctSolverMatrices(SPARSE_MATRIX* A, LARGE_VECTOR* b) {
	for (int i = 0; i < mesh->countVertices(); i++) {
		if (_correctedIndices[i] == true) {
			A->constrainSystem(i, i, b, _cMesh->getVertex(i)->getvX());
			_correctedIndices[i] = false;
		}
		//else
			//cout<<"something is up"<<endl;
	}
}

bool System::correctWithMeshSync(Solver* solver, LARGE_VECTOR* y,
		LARGE_VECTOR* bmod, double timeStep) {

	if(_cMesh == NULL || TIME < _syncCounter) {
		return false;
	}
	else
		_syncCounter += _syncStep;
	cout<<"SYNCED AT TIME: "<<TIME<<"S"<<endl;

	LARGE_VECTOR* f = solver->getf();
	vec3 F0(0, 0, 0);
	TriangleMeshVertex *a, *b, *ca, *cb;
	int ia, ib;
	EdgesIterator edg_it = mesh->getEdgesIterator();
	//bool isOverStrained = false;
	vec3 maxStrain;

	int totalVertsi = mesh->countVertices();
	float totalVerts = float(mesh->countVertices());
	//*
	double positionDiff = 0;
	double velocityDiff = 0;
		for(int i = 0; i < totalVertsi; i++) {
			positionDiff += abs(mesh->getVertex(i)->getX()[0] - _cMesh->getVertex(i)->getX()[0]);
			positionDiff += abs(mesh->getVertex(i)->getX()[1] - _cMesh->getVertex(i)->getX()[1]);
			positionDiff += abs(mesh->getVertex(i)->getX()[2] - _cMesh->getVertex(i)->getX()[2]);

			velocityDiff += abs(mesh->getVertex(i)->getvX()[0] - _cMesh->getVertex(i)->getvX()[0]);
			velocityDiff += abs(mesh->getVertex(i)->getvX()[1] - _cMesh->getVertex(i)->getvX()[1]);
			velocityDiff += abs(mesh->getVertex(i)->getvX()[2] - _cMesh->getVertex(i)->getvX()[2]);
		}

		if(_errorInd > 2498)
			_errorInd = 0;
		else
			_errorInd++;

		_posError[_errorInd] = positionDiff/totalVerts;
		_velError[_errorInd] = velocityDiff/totalVerts;
		_timeOfError[_errorInd] = TIME;
		if(_posError[_errorInd] > 0.01 || _velError[_errorInd] > 0.1) {
			if(DEBUG) {
		cout<<"					Average Error Between POSITIONS in Slave and Master Meshes: "<<_posError[_errorInd]<<endl;
		cout<<"					Average Error Between VELOCITIES in Slave and Master Meshes: "<<_velError[_errorInd]<<endl;
			}
		}
		//*/
//*

	do {
		a = (*edg_it)->getVertex(0);
		b = (*edg_it)->getVertex(1);
		ia = a->getIndex();
		ib = b->getIndex();
		vec3 pa = a->getX();
		vec3 pb = b->getX();
		vec3 Ua = a->getU();
		vec3 Ub = b->getU();
		vec3 RL = Ua - Ub;
		double rl = RL.length();
		bool pipeOverstrained = false;
		double strain = (pa - pb).length() - rl;
		double strainPercent = (strain / rl) * 100;

		//cout<<strainPercent<<endl;

		/*
		if (_cMesh->getVertex(ia)->getX()[0] != mesh->getVertex(ia)->getX()[0])
			cout << "Meshes Not the Same " <<ia<<" "<<ib<< endl;
		if (_cMesh->getVertex(ib)->getX()[0] != mesh->getVertex(ib)->getX()[0])
			cout<<"Meshes Not the Same "<<ia<<" "<<ib<< endl;
		if (_cMesh->getVertex(ia)->getvX()[0] != mesh->getVertex(ia)->getvX()[0])
			cout << "Meshes Not the Same " <<ia<<" "<<ib<< endl;
		if (_cMesh->getVertex(ib)->getvX()[0] != mesh->getVertex(ib)->getvX()[0])
			cout<<"Meshes Not the Same "<<ia<<" "<<ib<< endl;
		//*/

		//*
//		if (abs(strainPercent) > .000000000000010) {
			//cout<<"strainPercent: "<<strainPercent<<endl;
			//			if ((ia == 36) || (ia == 18)) {
			//				pipeOverstrained = false;
			//			} else if ((ib == 36) || (ib == 18)) {
			//				pipeOverstrained = false;
			//			} else {
			if (_correctedIndices[ia] == false) {
				_correctedIndices[ia] = true;
				//(*y)[ia] = _cMesh->getVertex(ia)->getX() - (*_x)[ia];
				(*_x)[ia] = _cMesh->getVertex(ia)->getX();
				mesh->getVertex(ia)->getX() = _cMesh->getVertex(ia)->getX();
				(*_v)[ia] = _cMesh->getVertex(ia)->getvX();
				mesh->getVertex(ia)->getvX() = _cMesh->getVertex(ia)->getvX();
			}
			if (_correctedIndices[ib] == false) {
				_correctedIndices[ib] = true;
				//(*y)[ib] = _cMesh->getVertex(ib)->getX() - (*_x)[ib];
				(*_x)[ib] = _cMesh->getVertex(ib)->getX();
				mesh->getVertex(ib)->getX() = _cMesh->getVertex(ib)->getX();
				(*_v)[ib] = _cMesh->getVertex(ib)->getvX();
				mesh->getVertex(ib)->getvX() = _cMesh->getVertex(ib)->getvX();
			}
	//		pipeOverstrained = true;
//		}

		//*/

			//*
			if (_cMesh->getVertex(ia)->getX()[0] != mesh->getVertex(ia)->getX()[0])
				cout << "Meshes Not the Same " <<ia<<" "<<ib<< endl;
			if (_cMesh->getVertex(ib)->getX()[0] != mesh->getVertex(ib)->getX()[0])
				cout<<"Meshes Not the Same "<<ia<<" "<<ib<< endl;
			if (_cMesh->getVertex(ia)->getvX()[0] != mesh->getVertex(ia)->getvX()[0])
				cout << "Meshes Not the Same " <<ia<<" "<<ib<< endl;
			if (_cMesh->getVertex(ib)->getvX()[0] != mesh->getVertex(ib)->getvX()[0])
				cout<<"Meshes Not the Same "<<ia<<" "<<ib<< endl;
			//*/

		//*
		 if(pipeOverstrained) {
			 double strainPercentAfter = (((mesh->getVertex(ia)->getX() - mesh->getVertex(ib)->getX() ).length() - rl) / rl) * 100;
			 if(abs(strainPercentAfter) > abs(strainPercent)) {
				 cout<<"strainPercent Before; " << strainPercent << endl;
				 cout<<"strainPercent After: "<< strainPercentAfter <<endl;
			 }
		 }
		 //*/
	} while (edg_it.next());

	int replacedVertCount = 0;
	for(int i = 0; i < totalVertsi; i++)
		if(_correctedIndices[i]==true) {
			replacedVertCount++;
			//_correctedIndices[i] = false;
		}
	float percentReplaced = float((float(replacedVertCount)/float(totalVerts))*100);
	if(DEBUG){
	cout<<"					Percent of Slave Mesh Replaced: "<<percentReplaced<<"%"<<endl;
	cout<<endl;
	}

	return true;
}

bool System::correctExplicitlyWithMeshSync(Solver* solver, LARGE_VECTOR* y,
		LARGE_VECTOR* bmod, double timeStep) {
	//cout<<"TIME IS: "<<TIME<<"		SYNC AT TIME: "<<_syncCounter<<endl;
	if(_cMesh == NULL || TIME < _syncCounter) {
		return false;
	}
	else
		_syncCounter += _syncStep;
	cout<<"SYNCED AT TIME: "<<TIME<<"S"<<endl;


	int totalVertsi = mesh->countVertices();
	float totalVerts = float(mesh->countVertices());
	//*
	double positionDiff = 0;
	double velocityDiff = 0;
		for(int i = 0; i < totalVertsi; i++) {
			positionDiff += abs(mesh->getVertex(i)->getX()[0] - _cMesh->getVertex(i)->getX()[0]);
			positionDiff += abs(mesh->getVertex(i)->getX()[1] - _cMesh->getVertex(i)->getX()[1]);
			positionDiff += abs(mesh->getVertex(i)->getX()[2] - _cMesh->getVertex(i)->getX()[2]);

			velocityDiff += abs(mesh->getVertex(i)->getvX()[0] - _cMesh->getVertex(i)->getvX()[0]);
			velocityDiff += abs(mesh->getVertex(i)->getvX()[1] - _cMesh->getVertex(i)->getvX()[1]);
			velocityDiff += abs(mesh->getVertex(i)->getvX()[2] - _cMesh->getVertex(i)->getvX()[2]);
		}

		if(_errorInd > 2498)
			_errorInd = 0;
		else
			_errorInd++;

		_posError[_errorInd] = positionDiff/totalVerts;
		_velError[_errorInd] = velocityDiff/totalVerts;
		_timeOfError[_errorInd] = TIME;
		//if(_posError[_errorInd] > 0.01 || _velError[_errorInd] > 0.1) {
		if(DEBUG){
		cout<<"					Average Error Between POSITIONS in Slave and Master Meshes: "<<_posError[_errorInd]<<endl;
		cout<<"					Average Error Between VELOCITIES in Slave and Master Meshes: "<<_velError[_errorInd]<<endl;
		}

	LARGE_VECTOR* f = solver->getf();
	vec3 F0(0, 0, 0);
	TriangleMeshVertex *a, *b, *ca, *cb;
	int ia, ib;
	EdgesIterator edg_it = mesh->getEdgesIterator();
	//bool isOverStrained = false;
	vec3 maxStrain;

		//}
		//*/
//*

	do {
		a = (*edg_it)->getVertex(0);
		b = (*edg_it)->getVertex(1);
		ia = a->getIndex();
		ib = b->getIndex();
		vec3 pa = a->getX();
		vec3 pb = b->getX();
		vec3 Ua = a->getU();
		vec3 Ub = b->getU();
		vec3 RL = Ua - Ub;
		double rl = RL.length();
		bool pipeOverstrained = false;
		double strain = (pa - pb).length() - rl;
		double strainPercent = (strain / rl) * 100;

		//cout<<strainPercent<<endl;

		/*
		if (_cMesh->getVertex(ia)->getX()[0] != mesh->getVertex(ia)->getX()[0])
			cout << "Meshes Not the Same " <<ia<<" "<<ib<< endl;
		if (_cMesh->getVertex(ib)->getX()[0] != mesh->getVertex(ib)->getX()[0])
			cout<<"Meshes Not the Same "<<ia<<" "<<ib<< endl;
		if (_cMesh->getVertex(ia)->getvX()[0] != mesh->getVertex(ia)->getvX()[0])
			cout << "Meshes Not the Same " <<ia<<" "<<ib<< endl;
		if (_cMesh->getVertex(ib)->getvX()[0] != mesh->getVertex(ib)->getvX()[0])
			cout<<"Meshes Not the Same "<<ia<<" "<<ib<< endl;
		//*/

		//*
//		if (abs(strainPercent) > .000000000000010) {
			//cout<<"strainPercent: "<<strainPercent<<endl;
			//			if ((ia == 36) || (ia == 18)) {
			//				pipeOverstrained = false;
			//			} else if ((ib == 36) || (ib == 18)) {
			//				pipeOverstrained = false;
			//			} else {
			if (_correctedIndices[ia] == false) {
				_correctedIndices[ia] = true;
				//(*y)[ia] = _cMesh->getVertex(ia)->getX() - (*_x)[ia];
				(*_x)[ia] = _cMesh->getVertex(ia)->getX();
				mesh->getVertex(ia)->getX() = _cMesh->getVertex(ia)->getX();
				(*_v)[ia] = _cMesh->getVertex(ia)->getvX();
				mesh->getVertex(ia)->getvX() = _cMesh->getVertex(ia)->getvX();
			}
			if (_correctedIndices[ib] == false) {
				_correctedIndices[ib] = true;
				//(*y)[ib] = _cMesh->getVertex(ib)->getX() - (*_x)[ib];
				(*_x)[ib] = _cMesh->getVertex(ib)->getX();
				mesh->getVertex(ib)->getX() = _cMesh->getVertex(ib)->getX();
				(*_v)[ib] = _cMesh->getVertex(ib)->getvX();
				mesh->getVertex(ib)->getvX() = _cMesh->getVertex(ib)->getvX();
			}
	//		pipeOverstrained = true;
//		}

		//*/

			//*
			if (_cMesh->getVertex(ia)->getX()[0] != mesh->getVertex(ia)->getX()[0])
				cout << "Meshes Not the Same " <<ia<<" "<<ib<< endl;
			if (_cMesh->getVertex(ib)->getX()[0] != mesh->getVertex(ib)->getX()[0])
				cout<<"Meshes Not the Same "<<ia<<" "<<ib<< endl;
			if (_cMesh->getVertex(ia)->getvX()[0] != mesh->getVertex(ia)->getvX()[0])
				cout << "Meshes Not the Same " <<ia<<" "<<ib<< endl;
			if (_cMesh->getVertex(ib)->getvX()[0] != mesh->getVertex(ib)->getvX()[0])
				cout<<"Meshes Not the Same "<<ia<<" "<<ib<< endl;
			//*/

		//*
		 if(pipeOverstrained) {
			 double strainPercentAfter = (((mesh->getVertex(ia)->getX() - mesh->getVertex(ib)->getX() ).length() - rl) / rl) * 100;
			 if(abs(strainPercentAfter) > abs(strainPercent)) {
				 cout<<"strainPercent Before; " << strainPercent << endl;
				 cout<<"strainPercent After: "<< strainPercentAfter <<endl;
			 }
		 }
		 //*/
	} while (edg_it.next());

	int replacedVertCount = 0;
	for(int i = 0; i < totalVertsi; i++)
		if(_correctedIndices[i]==true) {
			replacedVertCount++;
			_correctedIndices[i] = false;
		}
	float percentReplaced = float((float(replacedVertCount)/float(totalVerts))*100);
	if(DEBUG) {
	cout<<"					Percent of Slave Mesh Replaced: "<<percentReplaced<<"%"<<endl;
	cout<<endl;
	}

	return true;
}

bool System::calculateMeshDifference() {
	//cout<<"TIME IS: "<<TIME<<"		SYNC AT TIME: "<<_syncCounter<<endl;
	if(_cMesh == NULL || TIME < _syncCounter) {
		return false;
	}
	else
		_syncCounter += _syncStep;
	cout<<"SYNCED AT TIME: "<<TIME<<"S"<<endl;

		for(int i = 0; i < mesh->countVertices(); i++) {
			mesh->getVertex(i)->setMeshDiff(mesh->getVertex(i)->getX() - _cMesh->getVertex(i)->getX());
		}

	return true;
}

void System::calculatePosVelCollisionChange(Solver* solver, double timeStep,
		vector<VertexToEllipseCollision*> *collisions) {

	for (unsigned int i = 0; i < (*collisions).size(); i++)
		(*collisions)[i]->applyPosVelChangeCollision(timeStep, solver->getY(), _x, _v);
}

void System::calculateForcePartials(NewmarkSolver* solver) {
	SPARSE_MATRIX* JP = solver->getJP();
	SPARSE_MATRIX* JV = solver->getJV();

	mat3 JP_fa_xa, JV_fa_xa;
	TriangleMeshVertex *a, *b;
	int ia, ib;
	EdgesIterator edg_it = mesh->getEdgesIterator();

	do {
		a = (*edg_it)->getVertex(0);
		b = (*edg_it)->getVertex(1);

		ia = a->getIndex();
		ib = b->getIndex();

		vec3 pa = a->getX();
		vec3 va = a->getvX();
		vec3 pb = b->getX();
		vec3 vb = b->getvX();
		vec3 Ua = a->getU();
		vec3 Ub = b->getU();
		vec3 RL = Ua - Ub;
		double rl = RL.length();

		mat3 t1, t2;

		t1 = dfdx_damp(pa, pb, va, vb, rl, _mat->getKd());
		t2 = dfdx_spring(
				pa, pb, rl, _mat->getKe());

		JP_fa_xa = t1 + t2;
		JV_fa_xa = dfdv_damp(pa, pb, rl, _mat->getKd());
/*
		cout << "DAMPING JACOBIAN WRT POSITION: ";
		t1.printAvgStdDev(cout) << endl;
		cout << "ELASTIC JACOBIAN WRT POSITION: ";
		t2.printAvgStdDev(cout) << endl;
		cout << "NET JACOBIAN WRT POSITION: ";
		JP_fa_xa.printAvgStdDev(cout) << endl;
		cout << "NET JACOBIAN WRT VELOCITY: ";
		JV_fa_xa.printAvgStdDev(cout) << endl;
//*/
		(*JP)(ia, ia) += JP_fa_xa;
		(*JP)(ia, ib) += -1 * JP_fa_xa;
		(*JP)(ib, ia) += -1 * JP_fa_xa;
		(*JP)(ib, ib) += JP_fa_xa;

		(*JV)(ia, ia) += JV_fa_xa;
		(*JV)(ia, ib) += -1 * JV_fa_xa;
		(*JV)(ib, ia) += -1 * JV_fa_xa;
		(*JV)(ib, ib) += JV_fa_xa;

//		bendForceJacobian((*edg_it)->getParentTriangle(0),
//				(*edg_it)->getParentTriangle(1), a, b, (*edg_it), JP, JV);

	} while (edg_it.next());

}

void System::calculateCollisionDamping(Solver* solver, SPARSE_MATRIX* JP, SPARSE_MATRIX* JV,
		vector<VertexToEllipseCollision*> *collisions, double localTime) {
	LARGE_VECTOR* F = solver->getf();

	for (unsigned int i = 0; i < (*collisions).size(); i++)
		(*collisions)[i]->applyDampedCollisions(_Kcoll, JP, JV, F, localTime, _USECOLLJACOBIAN);
}

void System::calculateForcePartials(ImplicitSolver* solver) {
	SPARSE_MATRIX* JP = solver->getJP();
	SPARSE_MATRIX* JV = solver->getJV();

	mat3 JP_fa_xa, JV_fa_xa;
	TriangleMeshVertex *a, *b;
	int ia, ib;
	EdgesIterator edg_it = mesh->getEdgesIterator();
	do {
		a = (*edg_it)->getVertex(0);
		b = (*edg_it)->getVertex(1);

		ia = a->getIndex();
		ib = b->getIndex();

		vec3 pa = a->getX();
		vec3 va = a->getvX();
		vec3 pb = b->getX();
		vec3 vb = b->getvX();
		vec3 Ua = a->getU();
		vec3 Ub = b->getU();
		vec3 RL = Ua - Ub;
		double rl = RL.length();

		JP_fa_xa = dfdx_damp(pa, pb, va, vb, rl, _mat->getKd()) + dfdx_spring(
				pa, pb, rl, _mat->getKe());
		JV_fa_xa = dfdv_damp(pa, pb, rl, _mat->getKd());

		(*JP)(ia, ia) += JP_fa_xa;
		(*JP)(ia, ib) += -1 * JP_fa_xa;
		(*JP)(ib, ia) += -1 * JP_fa_xa;
		(*JP)(ib, ib) += JP_fa_xa;

		(*JV)(ia, ia) += JV_fa_xa;
		(*JV)(ia, ib) += -1 * JV_fa_xa;
		(*JV)(ib, ia) += -1 * JV_fa_xa;
		(*JV)(ib, ib) += JV_fa_xa;
	} while (edg_it.next());

}

void System::applyConstraints(Solver* solver, vector<Constraint*> *constraints) {
	//LARGE_VECTOR* Y = solver->getY();
	if (DYNAMIC_CONSTRAINTS || STATIC_CONSTRAINTS) {
		for (unsigned int i = 0; i < constraints->size(); i++) {
		}
		//(*constraints)[i]->applyConstraintsToState(_x, Y, _v);
		//(*constraints)[i]->applyConstraintToPoints(_x, _v);
	}
	if (isMouseEnabled())
		setVertexPos2MousePos();
}

void System::applyCollisions(Solver* solver,
		vector<VertexToEllipseCollision*> *collisions) {

}
void System::takeStep(Solver* solver, vector<Constraint*> *constraints, vector<
		VertexToEllipseCollision*> *collisions, double timeStep,
		double localTime) {

	profiler.frametimers.switchToTimer("calculateState");
	//Calculate the current derivatives and forces
	solver->calculateState(this, constraints, collisions, localTime); //evalDeriv function
	profiler.frametimers.switchToGlobal();
	//Run the solver to populate delx, delv
	solver->solve(this, constraints, timeStep, collisions, localTime);

	profiler.frametimers.switchToTimer("writeback mesh");
	//Write back to mesh
	TriangleMeshVertex* v;
	LARGE_VECTOR* delVel = solver->getDelVred();
	LARGE_VECTOR* y = solver->getY();

	vec3 constVel;
	for (unsigned int i = 0; i < mesh->vertices.size(); i++) {
		v = mesh->getVertex(i);

		v->getX() = (*_x)[i];
		v->getvX() = (*_v)[i];

		//_correctedIndices[i]= false;
		//_strainCorrectedIndices[i] = false;
	}
	profiler.frametimers.switchToGlobal();
}

