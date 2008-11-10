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
    (*b)[cVertex] = vec3(0,0,0);
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
	int cVertex = _follow->getIndex();
    A->zeroRowCol(cVertex, cVertex, true);
    //TODO: Should this be taking the ellipse speed into account?
    (*b)[cVertex] = vec3(0,0,0);
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

void VertexToEllipseCollision::applyCollisionToMesh(LARGE_VECTOR* X, LARGE_VECTOR* V, LARGE_VECTOR* F ) {
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

				(*X)[Vert->getIndex()][0] = Xc[0];// + .1*(Xc[0] - X->m_pData[Vert->getID()].x);
				(*X)[Vert->getIndex()][1] = Xc[1];// + .1*(Xc[1] - X->m_pData[Vert->getID()].y);
				(*X)[Vert->getIndex()][2] = Xc[2];// + .1*(Xc[2] - X->m_pData[Vert->getID()].z);

				(*V)[Vert->getIndex()][0] = -.1*(*V)[Vert->getIndex()][0] + Velli[0];//+ Wtan[0];// + Vo[0];//
				(*V)[Vert->getIndex()][1] = -.1*(*V)[Vert->getIndex()][1] + Velli[1];//+ Wtan[1];// + Vo[1];//
				(*V)[Vert->getIndex()][2] = -.1*(*V)[Vert->getIndex()][2] + Velli[2];//+ Wtan[2];// + Vo[2];//

				//frictionForce(j, Vert, Velli, F);
			}
		}
	}
}
