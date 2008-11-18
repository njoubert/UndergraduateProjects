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

    int n = m->countVertices();
    vector<vector<int> > sparsePattern = MeshTOLargeMatrix::GenerateIdentityPattern(n);
    _M = new SPARSE_MATRIX(n,n,sparsePattern , true);

    mat3 mass;
    for (int i = 0; i < m->countVertices(); i++) {
        mass = identity2D();
        mass *= m->getVertex(i)->getm();
        (*_M)(i,i) = mass;
    }

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
            // 		cout<<"x1: "<<x1<<"  x2: "<<x2<<"  x3: "<<x3<<"  x4: "<<x4<<endl;
            // 		cout<<"N1unit: "<<N1unit<<"  N2unit: "<<N2unit<<"  Eunit: "<<Eunit<<endl;
            //cout << "Rest sinThetaOver2 " << sinThetaOver2 << endl;
            // 		cout<<endl;
            //*/
            (*edg_it)->setRestAngle(sinThetaOver2);
            //			    cout<<"Edge: "<<(*it)<<" Rest Angle: "<<(*it)->getRestAngle()<<endl;
            //				cout<<endl;
        }

    } while (edg_it.next());

    //*******************DONE WITH REST ANGLE ************************************

    //********************* INITIALIZE X AND VX************************************

    _x = new LargeVec3Vector(m->countVertices());
    _v = new LargeVec3Vector(m->countVertices());
    _a = new LargeVec3Vector(m->countVertices());
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

    vec3 f = -Ks * (l / L) * (L - rl);

    return f;
}
vec3 System::f_damp(vec3 & pa, vec3 & pb, vec3 & va, vec3 & vb, double rl, double Kd) {
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
			int ai=0, bi=0;
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
		//	for (int i = 0; i < 4; i++)
		//		Fe[i] = _Ke * ((Emag * Emag) / (N1mag + N2mag)) * (sinThetaOver2 - sin(edge->getRestAngle()/2))
		//				* u[i];

			for (int i = 0; i < 4; i++)
							Fe[i] = _Ke * ((Emag * Emag) / (N1mag + N2mag)) * (sinThetaOver2)
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

void System::enableMouseForce(vec3 mousePosition) {
	//Find closest vertex
	//cout << "Mouse is at: " << mousePosition << endl;
	double currentDistance = 900000.0, d, d1, d2;
	TriangleMeshVertex* currentClosest = NULL;
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
	cout<<"PROBLEM WITH MOUSE FORCE!!!??"<<endl;
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
		    (*b)[i] = vec3(0,0,0);
		}
	}
}

void System::calculateExternalForces(Solver* solver) {
    TriangleMeshVertex* a;
    vec3 gravity(GRAVITY);
    //vec3 gravity(0,0,-9.8);
    LARGE_VECTOR* f = solver->getf();
    for (int i = 0; i < mesh->countVertices(); i++) {
        a = mesh->getVertex(i);
        (*f)[i] += (gravity * a->getm() += f_mouse(a));
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

        vec3 pa = a->getX(); vec3 va = a->getvX();
        vec3 pb = b->getX(); vec3 vb = b->getvX();
        vec3 Ua = a->getU(); vec3 Ub = b->getU();
        vec3 RL = Ua - Ub; double rl = RL.length();
        F0 = f_spring(pa, pb, rl, _mat->getKe()) + f_damp(pa, pb, va, vb, rl, _mat->getKd());

        //Calculate Bend Forces if they are to be calculated
        if(BEND_FORCES)
        	bendForce((*edg_it)->getParentTriangle(0), (*edg_it)->getParentTriangle(1), a, b, (*edg_it), f);

        //Update force vector
        (*f)[a->getIndex()] += F0;
        (*f)[b->getIndex()] += (-1 * F0);

    } while (edg_it.next());
}

void System::calculateForcePartials(Solver* solver) {
    SPARSE_MATRIX* JP = solver->getJP();
    SPARSE_MATRIX* JV = solver->getJV();

    mat3 JP_fa_xa, JV_fa_xa;
    TriangleMeshVertex *a, *b;
    int ia, ib;
    EdgesIterator edg_it = mesh->getEdgesIterator();
    do {
        a = (*edg_it)->getVertex(0);
        b = (*edg_it)->getVertex(1);

        ia = a->getIndex(); ib = b->getIndex();

        vec3 pa = a->getX(); vec3 va = a->getvX();
        vec3 pb = b->getX(); vec3 vb = b->getvX();
        vec3 Ua = a->getU(); vec3 Ub = b->getU();
        vec3 RL = Ua - Ub; double rl = RL.length();

        JP_fa_xa = dfdx_damp(pa, pb, va, vb, rl, _mat->getKd()) + dfdx_spring(pa, pb, rl, _mat->getKe());
        JV_fa_xa = dfdv_damp(pa, pb, rl, _mat->getKd());

        (*JP)(ia,ia) += JP_fa_xa;
        (*JP)(ia,ib) += -1*JP_fa_xa;
        (*JP)(ib,ia) += -1*JP_fa_xa;
        (*JP)(ib,ib) += JP_fa_xa;

        (*JV)(ia,ia) += JV_fa_xa;
        (*JV)(ia,ib) += -1*JV_fa_xa;
        (*JV)(ib,ia) += -1*JV_fa_xa;
        (*JV)(ib,ib) += JV_fa_xa;
    } while (edg_it.next());

}

void System::applyConstraints(Solver* solver, vector<Constraint*> *constraints) {
	if (DYNAMIC_CONSTRAINTS || STATIC_CONSTRAINTS) {
		for (unsigned int i = 0; i < constraints->size(); i++)
			(*constraints)[i]->applyConstraintToPoints(_x, _v);
	}
	if(isMouseEnabled())
		setVertexPos2MousePos();
}

void System::applyCollisions(Solver* solver, vector<VertexToEllipseCollision*> *collisions) {
	for (unsigned int i = 0; i < collisions->size(); i++)
		(*collisions)[i]->applyCollisionToMesh(_x, _v, ((NewmarkSolver*)solver)->getY());
}
void System::takeStep(Solver* solver, vector<Constraint*> *constraints,
		vector<VertexToEllipseCollision*> *collisions, double timeStep) {

    profiler.frametimers.switchToTimer("calculateState");
    //Calculate the current derivatives and forces
    solver->calculateState(this, constraints, collisions); //evalDeriv function
    profiler.frametimers.switchToGlobal();

    //Run the solver to populate delx, delv
    solver->solve(this, constraints, timeStep);

    profiler.frametimers.switchToTimer("writeback mesh");
    //Write back to mesh
    TriangleMeshVertex* v;
    for (unsigned int i = 0; i < mesh->vertices.size(); i++) {
        v = mesh->getVertex(i);
        v->getX() = (*_x)[i];
        v->getvX() = (*_v)[i];
    }
    profiler.frametimers.switchToGlobal();
}

