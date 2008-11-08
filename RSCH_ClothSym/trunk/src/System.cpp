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
}

/*
 * This function loads the current positions and velocities from the mesh into
 * the state of this system. Used at the beginning of Model.advance()
 */
void System::loadStateFromMesh() {
    TriangleMeshVertex* v;
    for (int i = 0; i < mesh->countVertices(); i++) {
        v = mesh->getVertex(i);
        (*_x)[i] = v->getX();
        (*_v)[i] = v->getvX();
    }
}

LARGE_VECTOR* System::getX() {
    return _x;
}

LARGE_VECTOR* System::getV() {
    return _v;
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
    double currentDistance = 900000.0, d;
    TriangleMeshVertex* currentClosest = NULL;
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

vec3 System::f_mouse(TriangleMeshVertex* selected) {
    if (selected != mouseSelected)
        return vec3(0, 0, 0);

    double rl = 1;
    vec3 l = selected->getX() - mouseP;
    double L = l.length();
    vec3 f = -_mat->_ks * (l / L) * (L - rl);
    return (f);
}

void System::calculateExternalForces(Solver* solver) {
    TriangleMeshVertex* a;
    vec3 gravity(GRAVITY);
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

        F0 = f_spring(pa, pb, rl, _mat->_ks) + f_damp(pa, pb, va, vb, rl, _mat->_kd);

        //Update force vector
        (*f)[a->getIndex()] += F0;
        (*f)[b->getIndex()] += (-1 * F0);

    } while (edg_it.next());
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

        ia = a->getIndex(); ib = b->getIndex();

        vec3 pa = a->getX(); vec3 va = a->getvX();
        vec3 pb = b->getX(); vec3 vb = b->getvX();
        vec3 Ua = a->getU(); vec3 Ub = b->getU();
        vec3 RL = Ua - Ub; double rl = RL.length();

        JP_fa_xa = dfdx_damp(pa, pb, va, vb, rl, _mat->_kd) + dfdx_spring(pa, pb, rl, _mat->_ks);
        JV_fa_xa = dfdv_damp(pa, pb, rl, _mat->_kd);

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
	for (unsigned int i = 0; i < constraints->size(); i++) {
		(*constraints)[i]->applyConstraintToPoints(_x, _v);
	}
}
void System::takeStep(Solver* solver, vector<Constraint*> *constraints,
        double timeStep) {

    //Calculate the current derivatives and forces
    solver->calculateState(this, constraints); //evalDeriv function

    //Run the solver to populate delx, delv
    solver->solve(this, constraints, timeStep);

    //Update x, y new delx, delv
    LARGE_VECTOR* delx = solver->getDelx();
    LARGE_VECTOR* delv = solver->getDelv();

    (*_x) += (*delx);
    (*_v) += (*delv);

    //Write back to mesh
    TriangleMeshVertex* v;
    for (unsigned int i = 0; i < mesh->vertices.size(); i++) {
        v = mesh->getVertex(i);
        v->getX() = (*_x)[i];
        v->getvX() = (*_v)[i];
    }
}

