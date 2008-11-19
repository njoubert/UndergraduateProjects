/*
 * Constraint.cpp
 *
 *  Created on: Oct 27, 2008
 *      Author: njoubert
 */

#include "Constraint.h"


/****************************************************************
*                                                               *
*               Constraint                                      *
*                                                               *
****************************************************************/

Constraint::Constraint() {
   // TODO Auto-generated constructor stub
}

Constraint::~Constraint() {
    // TODO Auto-generated destructor stub
}

void Constraint::setLead(AniElliModel* lead, int nr) {
    _lead = lead;
    _leadIndex = nr;

}

void Constraint::setFollow(TriangleMeshVertex* follow) {
    _follow = follow;
}

TriangleMeshVertex* Constraint::getFollow() {
	return _follow;
}

void Constraint::setCollisionMesh(TriangleMesh* mesh) {
    _mesh = mesh;
}

void Constraint::setCollisionEllipsoids(AniElliModel* ellipsoids) {
    _ellipsoids = ellipsoids;
}

/****************************************************************
*                                                               *
*               FixedConstraint                                 *
*                                                               *
****************************************************************/

FixedConstraint::FixedConstraint() {

}

void FixedConstraint::applyConstraintToPoints(LARGE_VECTOR* x, LARGE_VECTOR* v) {
	   //Update the sparse matrices to honor this constraint
}

void FixedConstraint::applyConstraintToSolverMatrices(
        SPARSE_MATRIX* A, LARGE_VECTOR* b) {
    int cVertex = _follow->getIndex();
	A->zeroRowCol(cVertex, cVertex, true);
	(*b)[cVertex] = vec3(0,0,0);
}


/****************************************************************
*                                                               *
*               FixedBaraffConstraint                           *
*                                                               *
****************************************************************/

FixedBaraffConstraint::FixedBaraffConstraint() {

}

void FixedBaraffConstraint::applyConstraintToPoints(LARGE_VECTOR* x, LARGE_VECTOR* v) {
	   //Update the sparse matrices to honor this constraint
}

void FixedBaraffConstraint::applyConstraintToSolverMatrices(
        SPARSE_MATRIX* A, LARGE_VECTOR* b) {

}

void FixedBaraffConstraint::applyMassMod( SPARSE_MATRIX* M ) {
	mat3 S;
    for(int i = 0; i < _mesh->countVertices(); i++) {
    	if(_mesh->getVertex(i)->getConstaint())
    		S = mat3(0);
    	else
    		S = mat3(identity2D());

    	cout<<"this should be the mass of a particle: "<<(*M)(i,i)[0][0]<<endl;
    	(*M)(i,i) = (1/(*M)(i,i)[0][0])*S;
    }
}

/****************************************************************
*                                                               *
*               VertexToAnimatedVertexConstraint                *
*                                                               *
****************************************************************/

VertexToAnimatedVertexConstraint::VertexToAnimatedVertexConstraint() {

}

void VertexToAnimatedVertexConstraint::applyConstraintToPoints(LARGE_VECTOR* x, LARGE_VECTOR* v) {

}

void VertexToAnimatedVertexConstraint::applyConstraintToSolverMatrices(
        SPARSE_MATRIX*, LARGE_VECTOR*) {

}


/****************************************************************
*                                                               *
*               VertexToAnimatedEllipseConstraint               *
*                                                               *
****************************************************************/

VertexToAnimatedEllipseConstraint::VertexToAnimatedEllipseConstraint() {

}

void VertexToAnimatedEllipseConstraint::applyConstraintToPoints(LARGE_VECTOR* X, LARGE_VECTOR* V) {
    //Move the follow's points to the same position as the lead's points.
//Get Ellipsoid Position From Ellipsoid Transformation
	vec4 origin(0);
	origin[3] = 1;
	origin[1] = 1.3;
	int index = _leadIndex;
	mat4 ElliTransform(0);
	ElliTransform = _lead->getEllipsoid(index);
	vec4 ElliCenter = origin * ElliTransform;

	int cVertex = _follow->getIndex();
	(*X)[cVertex] = vec3(ElliCenter, VW);

	vec3 vf = _lead->getFutureOrigin(_leadIndex);
	vec3 vp = _lead->getPastOrigin(_leadIndex);
	double t = 2*_lead->getTimeStep();
	vec3 v = (vf-vp)/t;
	(*V)[cVertex] = v;
	_vel = v;
	//cout<<"GOT HERE"<<endl;
	//for(int i = 0; i < 3; i++)
	//	_follow->getX()[i] = ElliCenter[i];
	//cout<<"Follow Vertex is now "<<_follow->getX()<<endl;
}

void VertexToAnimatedEllipseConstraint::applyConstraintToSolverMatrices(
        SPARSE_MATRIX* A, LARGE_VECTOR* b) {
    //Update the sparse matrices to honor this constraint
	int cVertex = _follow->getIndex();
    A->zeroRowCol(cVertex, cVertex, true);
    //TODO: Should this be taking the ellipse speed into account?
    (*b)[cVertex] = _vel;//vec3(0,0,0);
}

/****************************************************************
*                                                               *
*               VertexToEllipseCollision		                *
*                                                               *
****************************************************************/

VertexToEllipseCollision::VertexToEllipseCollision() {

}

void VertexToEllipseCollision::applyConstraintToPoints(LARGE_VECTOR* X, LARGE_VECTOR* V) {
    //Move the follow's points to the same position as the lead's points.
	//Get Ellipsoid Position From Ellipsoid Transformation
	cout<<"This Should not be called"<<endl;
}

void VertexToEllipseCollision::applyConstraintToSolverMatrices(
        SPARSE_MATRIX* A, LARGE_VECTOR* b) {
    //Update the sparse matrices to honor this constraint
	for(unsigned int i = 0; i < _collisionIndices.size(); i++) {
    A->zeroRowCol(_collisionIndices[i], _collisionIndices[i], true);
    //TODO: Should this be taking the ellipse speed into account?
    (*b)[_collisionIndices[i]] = _collisionVelocities[i];
	}
	_collisionIndices.clear();
	_collisionVelocities.clear();
}

void VertexToEllipseCollision::frictionForce(int j, TriangleMeshVertex* p, vec3 Velli, LARGE_VECTOR* F) {
	vec3 n = _ellipsoids->getNormal(j, p->getX());
	vec3 Vrel = p->getvX() - Velli;

	vec3 N = (*F)[p->getIndex()]*n;
	if( p->getvX().length() == 0 && p->getF().length() < (_ellipsoids->getMu_s()*N.length()) ) {
		p->getF() = 0;
		cout<<"static Friction Applied"<<endl;
	}
	else {
		vec3 Friction = -1*_ellipsoids->getMu_d()*N.length()*(Vrel/Vrel.length());
		(*F)[p->getIndex()][0] += Friction[0];
		(*F)[p->getIndex()][1] += Friction[1];
		(*F)[p->getIndex()][2] += Friction[2];
		cout<<"Unit V relative "<<(Vrel/Vrel.length())<<endl;
		cout<<"Normal Force of "<<N.length()<<endl;
		cout<<"Mu Dynamic of "<<_ellipsoids->getMu_d()<<endl;
		cout<<"Friction Magnitude of "<<Friction.length()<<endl;
		cout<<"Friction Force of "<<Friction<<" was applied."<<endl<<endl;
	}
}

void VertexToEllipseCollision::changeDelvToAvoidCollisions(LARGE_VECTOR* delv, double h, NewmarkSolver* solver) {
	TriangleMeshVertex* Vert;

	for(int i = 0; i < _mesh->countVertices(); i++) {
		for(int j = 0; j < _ellipsoids->getSize(); j++) {
			Vert = _mesh->getVertex(i);

			//Calculate new position for this vertex according to our current delv:
			vec3 newWorldX = solver->calculateNewPosition(Vert->getX(), Vert->getvX(), (*delv)[Vert->getIndex()], h);
			vec3 newWorldV = solver->calculateNewVelocity(Vert->getX(), (*delv)[Vert->getIndex()]);
			vec4 Xc_elliSpace = _ellipsoids->convertPoint2ElliSpace(j, newWorldX);

			if( _ellipsoids->isPointInsideElli(j, Xc_elliSpace) ) {

				//	CurrentTime: Xc -> position of collision, 				Xo -> point of ellipsoid origin (should be center of mass)
				//	FutureTime: Xc_f -> position of collision in future, 	Xo_f -> point of future ellipsoid origin (should be center of mass)
				//	PastTime: Xc_p -> position of collision in past, 		Xo_f -> point of past ellipsoid origin (should be center of mass)
				vec3 Xc = _ellipsoids->getPointInsideElli2Surface(j, Xc_elliSpace);
				vec3 Xc_f = _ellipsoids->getPointInFuture(j, Xc);
				vec3 Xc_p = _ellipsoids->getPointInPast(j, Xc);
				vec3 Xo = _ellipsoids->getOrigin(j);
				vec3 Xo_f = _ellipsoids->getFutureOrigin(j);
				vec3 Xo_p = _ellipsoids->getPastOrigin(j);
				double elliTimeStep = _ellipsoids->getTimeStep();

				//	V0 = Linear Velocity, W = Angular Velocity, Velli = Total Velocity of Ellipsoid
				vec3 Vo = (Xo_f - Xo_p)/(2*elliTimeStep);
				vec3 r = (Xc - Xo), r_p = (Xc_p - Xo_p), r_f = (Xc_f - Xo_f);
				vec3 Vr = (r_f - r_p)/(2*elliTimeStep);
				vec3 W = (r^Vr)/(r.length()*r.length());
				vec3 Wtan = W^r;
				vec3 Vobj = Vo + Wtan;

				//Vert->getX() - Xc_p
				vec3 normal = (newWorldX - Xc).normalize();
				//this uses the positions at the beginning of the timestep to calculate
				//the velocity such that the collision does not occur.
				//we use the calculated velocity as an approximation to the velocity at the beginning
				//of the timestep
				double newVnormal = Vobj*normal - (1.0/h)*(newWorldX - Xc).length();
				vec3 newVtangential = vec3(0,0,0);
				vec3 Vnew = newVnormal*normal + newVtangential;
				vec3 newDelv = solver->calculateNewDelv(Vert->getvX(), Vnew, h);
				cout << "New normal: " << newVnormal << ", old normal: " << (Vert->getvX()*normal) << endl;

				(*delv)[Vert->getIndex()] = newDelv;

			}
		}
	}
}

bool VertexToEllipseCollision::applyCollisions(LARGE_VECTOR* x, LARGE_VECTOR* x0, LARGE_VECTOR* v0, double simTimeStep) {
	bool collisions = false;
	for(int i = 0; i < x->size(); i++) {
		for(int j = 0; j < _ellipsoids->getSize(); j++) {
			//TriangleMeshVertex* Vert = _mesh->getVertex(i);
			vec4 Xc_elliSpace = _ellipsoids->convertPoint2ElliSpace(j, (*x)[i]);

			if( _ellipsoids->isPointInsideElli(j, Xc_elliSpace) ) {
				//cout<<"colision detected"<<endl;
				collisions = true;

				//	CurrentTime: Xc -> position of collision, 				Xo -> point of ellipsoid origin (should be center of mass)
				//	FutureTime: Xc_f -> position of collision in future, 	Xo_f -> point of future ellipsoid origin (should be center of mass)
				//	PastTime: Xc_p -> position of collision in past, 		Xo_f -> point of past ellipsoid origin (should be center of mass)
				vec3 Xc = _ellipsoids->getPointInsideElli2Surface(j, Xc_elliSpace);
				vec3 Xc_f = _ellipsoids->getPointInFuture(j, Xc);
				vec3 Xc_p = _ellipsoids->getPointInPast(j, Xc);
				vec3 Xo = _ellipsoids->getOrigin(j);
				vec3 Xo_f = _ellipsoids->getFutureOrigin(j);
				vec3 Xo_p = _ellipsoids->getPastOrigin(j);
				double elliTimeStep = _ellipsoids->getTimeStep();

				//	V0 = Linear Velocity, W = Angular Velocity, Velli = Total Velocity of Ellipsoid
				vec3 Vo = (Xo_f - Xo_p)/(2*elliTimeStep);
				vec3 r = (Xc - Xo), r_p = (Xc_p - Xo_p), r_f = (Xc_f - Xo_f);
				vec3 Vr = (r_f - r_p)/(2*elliTimeStep);
				vec3 W = (r^Vr)/(r.length()*r.length());
				vec3 Wtan = W^r;
				vec3 Velli = Vo + Wtan;
				if(i == 326) {
				//cout<<"{"<<endl;
				//cout<<"Particle: "<<i <<" has x0 of: "<<(*x0)[i]<<endl;
				//cout<<"Particle: "<<i <<" has position after timestep: "<<(*x)[i]<<endl;
				//cout<<"Particle: "<<i <<" has velocity before collision: "<<(*v0)[i]<<endl;
				}
				vec3 save_v0 = (*v0)[i];
				//Xc += .0001*(Xc-Xo).normalize();
				//Xc[0] += levelSet;
				//Xc[1] += levelSet;
				//Xc[2] += levelSet;

				vec3 delV = (( Xc - (*x0)[i] )/simTimeStep) - (*v0)[i];
				cout<<"Xc: "<<Xc<<endl;
				//vec3 delV = 0;
				//(*v0)[i] = Velli;
				//(*v0)[i] = (( Xc - (*x0)[i] )/simTimeStep);
				//cout<<"delV: "<<delV<<endl;

				_collisionVelocities.push_back(delV);
				_collisionIndices.push_back(i);
				if(i == 326) {
				//cout<<"Particle: "<<i <<" velocity after collision: "<<(*v0)[i]<<endl;
				//cout<<"Particle: "<<i <<" has Xc of: "<<Xc<<endl;
				//cout<<"timeStep is: "<<simTimeStep<<endl;
				//cout<<"Particle: "<<i <<"'s velocity should force it to: "<<(*x0)[i]+(*v0)[i]*simTimeStep<<endl<<endl;
				//cout<<"}"<<endl;
				}

				//if((*v0)[i].length() > save_v0.length()) {
				//	cout<<"SHIT GOT FUCKED"<<endl;
				//	exit(1);
				//}
			//	double diff = (*v0)[i].length() - ( (*delX)[i] - ((*x)[i] - Xc) ).length();
			//	if(!( diff < 0.01 && diff > -0.01 ))
			//		cout<<"I screwed up my differences!!!!!!"<<endl;


				//(*Y)[Vert->getIndex()] = Xc - (*X)[Vert->getIndex()];

				//(*X)[Vert->getIndex()][0] = Xc[0];// + .1*(Xc[0] - X->m_pData[Vert->getID()].x);
				//(*X)[Vert->getIndex()][1] = Xc[1];// + .1*(Xc[1] - X->m_pData[Vert->getID()].y);
				//(*X)[Vert->getIndex()][2] = Xc[2];// + .1*(Xc[2] - X->m_pData[Vert->getID()].z);

				//(*V)[Vert->getIndex()][0] = -.1*(*V)[Vert->getIndex()][0] + Velli[0];//+ Wtan[0];// + Vo[0];//
				//(*V)[Vert->getIndex()][1] = -.1*(*V)[Vert->getIndex()][1] + Velli[1];//+ Wtan[1];// + Vo[1];//
				//(*V)[Vert->getIndex()][2] = -.1*(*V)[Vert->getIndex()][2] + Velli[2];//+ Wtan[2];// + Vo[2];//

				//frictionForce(j, Vert, Velli, F);
			}

		}
	}
return collisions;
}

void VertexToEllipseCollision::applyCollisionToMesh(LARGE_VECTOR* X, LARGE_VECTOR* V, LARGE_VECTOR* Y ) {

	for(int i = 0; i < _mesh->countVertices(); i++) {
		for(int j = 0; j < _ellipsoids->getSize(); j++) {
			TriangleMeshVertex* Vert = _mesh->getVertex(i);
			vec4 Xc_elliSpace = _ellipsoids->convertPoint2ElliSpace(j, Vert->getX());

			if( _ellipsoids->isPointInsideElli(j, Xc_elliSpace) ) {
				//cout<<"colision detected"<<endl;

				//	CurrentTime: Xc -> position of collision, 				Xo -> point of ellipsoid origin (should be center of mass)
				//	FutureTime: Xc_f -> position of collision in future, 	Xo_f -> point of future ellipsoid origin (should be center of mass)
				//	PastTime: Xc_p -> position of collision in past, 		Xo_f -> point of past ellipsoid origin (should be center of mass)
				vec3 Xc = _ellipsoids->getPointInsideElli2Surface(j, Xc_elliSpace);
				vec3 Xc_f = _ellipsoids->getPointInFuture(j, Xc);
				vec3 Xc_p = _ellipsoids->getPointInPast(j, Xc);
				vec3 Xo = _ellipsoids->getOrigin(j);
				vec3 Xo_f = _ellipsoids->getFutureOrigin(j);
				vec3 Xo_p = _ellipsoids->getPastOrigin(j);
				double elliTimeStep = _ellipsoids->getTimeStep();

				//	V0 = Linear Velocity, W = Angular Velocity, Velli = Total Velocity of Ellipsoid
				vec3 Vo = (Xo_f - Xo_p)/(2*elliTimeStep);
				vec3 r = (Xc - Xo), r_p = (Xc_p - Xo_p), r_f = (Xc_f - Xo_f);
				vec3 Vr = (r_f - r_p)/(2*elliTimeStep);
				vec3 W = (r^Vr)/(r.length()*r.length());
				vec3 Wtan = W^r;
				vec3 Velli = Vo + Wtan;

				(*Y)[Vert->getIndex()] = Xc - (*X)[Vert->getIndex()];

//				(*X)[Vert->getIndex()] = Xc;
				//_collisionVelocities.push_back( (*V)[Vert->getIndex()][0]);
				_collisionVelocities.push_back(0);
				(*V)[Vert->getIndex()] = Velli;

				_collisionIndices.push_back(Vert->getIndex());

				//frictionForce(j, Vert, Velli, F);
			}
		}
	}
}

vec3 VertexToEllipseCollision::f_dampCollision(vec3 Vn, vec3 vEllipsoid, double Kcd) {

	vec3 f = -Kcd * Vn;
    return f;
}

void VertexToEllipseCollision::applyDampedCollisions(double Kcd, SPARSE_MATRIX* JV, LARGE_VECTOR* F ) {

	for(int i = 0; i < _mesh->countVertices(); i++) {
		for(int j = 0; j < _ellipsoids->getSize(); j++) {
			TriangleMeshVertex* Vert = _mesh->getVertex(i);
			vec4 Xc_elliSpace = _ellipsoids->convertPoint2ElliSpace(j, Vert->getX());

			if( _ellipsoids->isPointInsideElli(j, Xc_elliSpace) ) {
				//cout<<"colision detected"<<endl;

				//	CurrentTime: Xc -> position of collision, 				Xo -> point of ellipsoid origin (should be center of mass)
				//	FutureTime: Xc_f -> position of collision in future, 	Xo_f -> point of future ellipsoid origin (should be center of mass)
				//	PastTime: Xc_p -> position of collision in past, 		Xo_f -> point of past ellipsoid origin (should be center of mass)
				vec3 Xc = _ellipsoids->getPointInsideElli2Surface(j, Xc_elliSpace);
				vec3 Xc_f = _ellipsoids->getPointInFuture(j, Xc);
				vec3 Xc_p = _ellipsoids->getPointInPast(j, Xc);
				vec3 Xo = _ellipsoids->getOrigin(j);
				vec3 Xo_f = _ellipsoids->getFutureOrigin(j);
				vec3 Xo_p = _ellipsoids->getPastOrigin(j);
				double elliTimeStep = _ellipsoids->getTimeStep();

				//	V0 = Linear Velocity, W = Angular Velocity, Velli = Total Velocity of Ellipsoid
				vec3 Vo = (Xo_f - Xo_p)/(2*elliTimeStep);
				vec3 r = (Xc - Xo), r_p = (Xc_p - Xo_p), r_f = (Xc_f - Xo_f);
				vec3 Vr = (r_f - r_p)/(2*elliTimeStep);
				vec3 W = (r^Vr)/(r.length()*r.length());
				vec3 Wtan = W^r;
				vec3 Velli = Vo + Wtan;


				vec3 n = _ellipsoids->getNormal(j, Xc_elliSpace);
				vec3 Vn = ((Vert->getvX()-Velli)*n)*n;
				//vec3 Vn = (Vert->getvX());

				//damping force only applied if moving into ellipse.
				if ((Vn * n) < 0) {
				//	cout << "woo apply damping forces! n= " << n << " Vn=" << Vn << endl;
					vec3 dampingForce = f_dampCollision( Vn, Velli, Kcd);

					(*F)[i]  += dampingForce;
					mat3 jv(0);
					/*
					jv[0][0] = -Kcd;
					jv[1][1] = -Kcd;
					jv[2][2] = -Kcd;
					//*/
					//*
					jv[0][0] = -Kcd * n[0]*n[0];
					jv[0][1] = -Kcd * n[0]*n[1];
					jv[0][2] = -Kcd * n[0]*n[2];
					jv[1][0] = -Kcd * n[0]*n[1];
					jv[1][1] = -Kcd * n[1]*n[1];
					jv[1][2] = -Kcd * n[1]*n[2];
					jv[2][0] = -Kcd * n[0]*n[2];
					jv[2][1] = -Kcd * n[1]*n[2];
					jv[2][2] = -Kcd * n[2]*n[2];
					//*/
					(*JV)(i,i) += jv;
				} else {
			//		cout << "NONE damping forces! n= " << n << " Vn=" << Vn << endl;
				}
				//frictionForce(j, Vert, Velli, F);
			}
		}
	}
}
