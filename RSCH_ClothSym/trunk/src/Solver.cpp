/*
 * Solver.cpp
 *
 *  Created on: Oct 23, 2008
 *      Author: njoubert
 */

#include "Solver.h"

/******************************************************************************
 *                                                                             *
 *                 virtual SOLVER                                              *
 *                                                                             *
 *******************************************************************************/

Solver::Solver(TriangleMesh* mesh, int n) {
	_mesh = mesh;
    _f = new LARGE_VECTOR(n);
    _delx = new LargeVec3Vector(n);
    _delv = new LargeVec3Vector(n);
}

Solver::~Solver() {

}

LARGE_VECTOR* Solver::getDelx() {
    return _delx;
}

LARGE_VECTOR* Solver::getDelv() {
    return _delv;
}

LARGE_VECTOR* Solver::getf() {
    return _f;
}

SPARSE_MATRIX* Solver::getJP() {
    return _JP;
}

SPARSE_MATRIX* Solver::getJV() {
    return _JV;
}

/******************************************************************************
 *                                                                             *
 *                 ImplicitSolver                                              *
 *                                                                             *
 *******************************************************************************/
//*

ImplicitSolver::ImplicitSolver(TriangleMesh* mesh, int n):
    Solver(mesh, n), cg(n) {
    vector<vector<int> > sparsePattern = MeshTOLargeMatrix::CalculateSparsityPattern(mesh);
    _JP = new SPARSE_MATRIX(n,n,sparsePattern,false);
    _JV = new SPARSE_MATRIX(n,n,sparsePattern,false);
    _y = new LARGE_VECTOR(n);
    A = new SPARSE_MATRIX(n,n,sparsePattern,false);
	b = new LARGE_VECTOR(n);
	t1 = new LARGE_VECTOR(n);
	_gamma = GAMMA;
}

ImplicitSolver::~ImplicitSolver() {

}

LARGE_VECTOR* ImplicitSolver::getY() {
    return _y;
}

void ImplicitSolver::calculateState(System* sys, vector<Constraint*> *constraints,
		vector<VertexToEllipseCollision*> *collisions) {
	//Zero our current data structures
    _JP->zeroValues();
    _JV->zeroValues();
    _f->zeroValues();
    _delv->zeroValues();
    _y->zeroValues();
    //_delx->zeroValues(); //dont do this since delx is set from delv
    A->zeroValues();
    //b->zeroValues();      //dont do this since b is set from postmultiply JP and v


    //Apply constraints to points right here.
    sys->applyConstraints(this, constraints);

    //Apply Collisions
    sys->applyCollisions(this, collisions); //TODO: Really here?

    //Compute _JP, _JV, _f
    sys->calculateInternalForces(this);
    sys->calculateExternalForces(this);
    sys->calculateForcePartials(this);

	//sys->calculateInternalForces();
    //sys->calculateExternalForces();
}

void ImplicitSolver::solve(System* sys, vector<Constraint*> *constraints, double timeStep,
		vector<VertexToEllipseCollision*> *collisions) {

	profiler.frametimers.switchToTimer("calculating matrices");

	    //b = h*f + g*h^2*v*JP  + g*h*y*JP;
	    (*_JV) *= (timeStep);            //JV = g*h*JV
	    (*_JP) *= (timeStep);   			//JP = g*h*JP
	    (*_JP).postMultiply(*_y, *t1);			//t1 = g*h*y*JP
	    (*_JP) *= (timeStep);   				//JP = g*h^2*JP
	    (*_JP).postMultiply((*sys->getV()), *b);//b = g*h^2*v*JP
	    (*_f) *= timeStep;
	    (*b) += (*_f);							//b = h*f + g*h^2*v*JP
	    (*b) += (*t1);							//b = h*f + g*h^2*v*JP + g*h*y*JP

	    //TODO: Should y also be factored into the newmark portion of the integrator?
	    //TODO: Should I apply constraints that include this y? ATM: NO!

	    //A = M - g*h*JV - g*h^2*JP;
	    A->insertMatrixIntoDenserMatrix(*(sys->getM()));
	    //A->insertMatrixIntoDenserMatrix(*(sys->getModM()));
	    (*A) -= (*_JV);
	    (*A) -= (*_JP);

	    //Apply constraints (filter):
	    if (DYNAMIC_CONSTRAINTS || STATIC_CONSTRAINTS) {
			for (unsigned int i = 0; i < constraints->size(); i++) {
				(*constraints)[i]->applyConstraintToSolverMatrices(A, b);
			}
		}

		sys->applyMouseConst2Matrices(A, b);

		profiler.frametimers.switchToTimer("CG solving");
	    //delV = b/A
	    cg.solve((*A), (*b), (*_delv), MAX_CG_ITER, MAX_CG_ERR);
	    profiler.frametimers.switchToGlobal();

	    profiler.frametimers.switchToTimer("calculating matrices");
	    //delX = h*(v + g*delV) + y;
	    (*_delx) = (*_delv);
	    //(*_delx) *= (_gamma);
	    (*_delx) += *(sys->getV());
	    (*_delx) *= timeStep;
	    (*_delx) += *_y;


	    //Apply constraints to points right here.
	    //sys->applyConstraints(this, constraints);

	    //Apply Collisions
	    //sys->applyCollisions(this, collisions); //TODO: Really here?

	    profiler.frametimers.switchToGlobal();

	/*
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

	//return make_pair(deltaX, deltaV);
	 */

}
//*/

/******************************************************************************
 *                                                                             *
 *                 ExplicitSolver                                              *
 *                                                                             *
 *******************************************************************************/
/*
ExplicitSolver::ExplicitSolver(TriangleMesh* mesh, int n) : Solver(mesh, n) {
}

ExplicitSolver::~ExplicitSolver() {

}

void ExplicitSolver::calculateState(System* sys) {
    sys->calculateInternalForces();
    sys->calculateExternalForces();
}

void ExplicitSolver::solve(System* sys, double timeStep,
        int pointIndex, TriangleMeshVertex* point) {

	vec3 deltaV = (timeStep ) * point->getF() / (double) point->getm();
    vec3 deltaX = timeStep * (point->getvX() + deltaV);


    //if (sys->getT() > 0.1) {
    //    cout << "deltaV=" << deltaV << endl;
    //    cout << "deltaX=" << deltaX << endl;
    //    cout << "F=" << point->getF() << endl;
    //  cout << "vX=" << point->getvX() << endl;
    //   exit(1);
    //}

    point->clearF();

    //Contraints set in OBJ file
    //if (point->getConstaint() == identity2D())
      //  return make_pair(vec3(0,0,0), vec3(0,0,0)); //Lame contraints for explicit

    //cout << "Forces on particle " << pointIndex << " is (" << F[0] << ", " << F[1] << ", " << F[2] << ")" << endl;
    //----------------------------------------------------
    //return make_pair(deltaX, deltaV);
}
*/

/******************************************************************************
 *                                                                             *
 *                 NewmarkSolver                                               *
 *                                                                             *
 *******************************************************************************/
NewmarkSolver::NewmarkSolver(TriangleMesh* mesh, int n):
        Solver(mesh, n), cg(n) {
	cout<<"		Creating Sparse Matrices and Large Vectors for Newmark Solver..."<<endl;
	vector<vector<int> > sparsePattern = MeshTOLargeMatrix::CalculateSparsityPattern(mesh);
    _JP = new SPARSE_MATRIX(n,n,sparsePattern,false);
    _JV = new SPARSE_MATRIX(n,n,sparsePattern,false);
    _y = new LARGE_VECTOR(n);
    _z = new LARGE_VECTOR(n);
    A = new SPARSE_MATRIX(n,n,sparsePattern,false);
	b = new LARGE_VECTOR(n);
	t1 = new LARGE_VECTOR(n);
	_gamma = GAMMA;
	cout<<"		Done Creating Sparse Matrices and Large Vectors for Newmark Solver."<<endl;
}


NewmarkSolver::~NewmarkSolver() {

}

LARGE_VECTOR* NewmarkSolver::getY() {
    return _y;
}

LARGE_VECTOR* NewmarkSolver::getZ() {
    return _z;
}

void NewmarkSolver::calculateState(System* sys, vector<Constraint*> *constraints,
		vector<VertexToEllipseCollision*> *collisions) {
	//Zero our current data structures
    _JP->zeroValues();
    _JV->zeroValues();
    _f->zeroValues();
    _delv->zeroValues();
    _y->zeroValues();
    _z->zeroValues();
    //_delx->zeroValues(); //dont do this since delx is set from delv
    A->zeroValues();
    //b->zeroValues();      //dont do this since b is set from postmultiply JP and v

    //Apply constraints to points right here.
    sys->applyConstraints(this, constraints);
    //Apply Collisions
 //   sys->applyCollisions(this, collisions);

    //Compute _JP, _JV, _f

    sys->calculateInternalForces(this);
    sys->calculateExternalForces(this);
    sys->calculateForcePartials(this);
    sys->calculateCollisionDamping(this, _JV, collisions);


    //sys->applyCollisions(this, collisions);

    //Apply constraints to points right here.
    //sys->applyConstraints(this, constraints);

}

vec3 NewmarkSolver::calculateNewVelocity(vec3 v0, vec3 delv) {
	return v0 + _gamma*delv;
}

vec3 NewmarkSolver::calculateNewPosition(vec3 x0, vec3 v0, vec3 delv, double h) {
	return x0 + h*(v0 + _gamma*delv);
}
vec3 NewmarkSolver::calculateNewDelv(vec3 v0, vec3 vnew, double h) {
	return vnew - v0;
}

void NewmarkSolver::solve(System* sys, vector<Constraint*> *constraints, double timeStep,
		vector<VertexToEllipseCollision*> *collisions) {
    profiler.frametimers.switchToTimer("calculating matrices");

    //b = h*f + g*h^2*v*JP  + g*h*y*JP;
    (*_JV) *= (timeStep*_gamma);            //JV = g*h*JV
    (*_JP) *= (timeStep*_gamma);   			//JP = g*h*JP
    (*_JP).postMultiply(*_y, *t1);			//t1 = g*h*y*JP
    (*_JP) *= (timeStep);   				//JP = g*h^2*JP
    (*_JP).postMultiply((*sys->getV()), *b);//b = g*h^2*v*JP
    (*_f) *= timeStep;
    (*b) += (*_f);							//b = h*f + g*h^2*v*JP
    (*b) += (*t1);							//b = h*f + g*h^2*v*JP + g*h*y*JP
    //(*_z) *= _gamma;
    (*b) += (*_z);

    //TODO: Should y also be factored into the newmark portion of the integrator?
    //TODO: Should I apply constraints that include this y? ATM: NO!

    //A = M - g*h*JV - g*h^2*JP;
    A->insertMatrixIntoDenserMatrix(*(sys->getM()));
    //A->insertMatrixIntoDenserMatrix(*(sys->getModM()));
    (*A) -= (*_JV);
    (*A) -= (*_JP);

    //Apply constraints (filter):
    if (DYNAMIC_CONSTRAINTS || STATIC_CONSTRAINTS) {
		for (unsigned int i = 0; i < constraints->size(); i++) {
			(*constraints)[i]->applyConstraintToSolverMatrices(A, b);
		}
	}

    //Force Velocities to be Determined by Collisions
//    if(COLLISIONS)
//    	(*collisions)[0]->applyConstraintToSolverMatrices(A,b);

	//sys->applyMouseConst2Matrices(A, b);

	profiler.frametimers.switchToTimer("CG solving");
    //delV = b/A
    cg.solve((*A), (*b), (*_delv), MAX_CG_ITER, MAX_CG_ERR);
    profiler.frametimers.switchToGlobal();

    //(*_z) *= (1/_gamma);
    //(*_delv) += (*_z);

 //   for (int i = 0; i < collisions->size(); i++) {
 //   	(*collisions)[i]->changeDelvToAvoidCollisions(_delv, timeStep, this);
 //   }

    profiler.frametimers.switchToTimer("calculating matrices");
    //delX = h*(v + g*delV) + y;
    (*_delx) = (*_delv);
    (*_delx) *= (_gamma);
    (*_delx) += *(sys->getV());
    (*_delx) *= timeStep;
    (*_delx) += *_y;
    profiler.frametimers.switchToGlobal();

    LARGE_VECTOR* _xTMP = sys->getX();
    LARGE_VECTOR* _vTMP = sys->getV();
    (*_xTMP) += (*_delx);
    (*_vTMP) += (*_delv);

    //Apply constraints to points right here.
     sys->applyConstraints(this, constraints);

//     sys->applyCollisions(this, collisions);

/*
     //Update x, y new delx, delv

//*/



    profiler.frametimers.switchToTimer("update");

}

void BaraffNewmarkSolver::solve(System* sys, vector<Constraint*> *constraints, double timeStep,
		vector<VertexToEllipseCollision*> *collisions) {
//UNFINISHED

    profiler.frametimers.switchToTimer("calculating matrices");

    //b = h*f + g*h^2*v*JP  + g*h*y*JP;
    (*_JV) *= (timeStep*_gamma);            //JV = g*h*JV
    (*_JP) *= (timeStep*_gamma);   			//JP = g*h*JP
    (*_JP).postMultiply(*_y, *t1);			//t1 = g*h*y*JP
    (*_JP) *= (timeStep);   				//JP = g*h^2*JP
    (*_JP).postMultiply((*sys->getV()), *b);//b = g*h^2*v*JP
    (*_f) *= timeStep;
    (*b) += (*_f);							//b = h*f + g*h^2*v*JP
    (*(sys->getW())).postMultiply(*b, *b);
    (*b) += (*t1);							//b = h*f + g*h^2*v*JP + g*h*y*JP

    //TODO: Should y also be factored into the newmark portion of the integrator?
    //TODO: Should I apply constraints that include this y? ATM: NO!

    //A = M - g*h*JV - g*h^2*JP;
    A->insertMatrixIntoDenserMatrix(*(sys->getI()));
    //A->insertMatrixIntoDenserMatrix(*(sys->getModM()));

    //WE NEED MATRIX MULTIPLICATION TO CONTINUE THIS W*df/dx

    (*A) -= (*_JV);
    (*A) -= (*_JP);

    //Apply constraints (filter):
    if (DYNAMIC_CONSTRAINTS || STATIC_CONSTRAINTS) {
		for (unsigned int i = 0; i < constraints->size(); i++) {
			(*constraints)[i]->applyConstraintToSolverMatrices(A, b);
		}
	}

	sys->applyMouseConst2Matrices(A, b);

	profiler.frametimers.switchToTimer("CG solving");
    //delV = b/A
    cg.solve((*A), (*b), (*_delv), MAX_CG_ITER, MAX_CG_ERR);
    profiler.frametimers.switchToGlobal();

    profiler.frametimers.switchToTimer("calculating matrices");
    //delX = h*(v + g*delV) + y;
    (*_delx) = (*_delv);
    (*_delx) *= (_gamma);
    (*_delx) += *(sys->getV());
    (*_delx) *= timeStep;
    (*_delx) += *_y;
    profiler.frametimers.switchToGlobal();

}


