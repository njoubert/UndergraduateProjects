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

void FixedConstraint::applyConstraintToPoints() {

}

void FixedConstraint::applyConstraintToSolverMatrices(double time,
		SparseMatrix* A, Physics_LargeVector* b) {

}



/****************************************************************
*                                                               *
*               VertexToAnimatedVertexConstraint                *
*                                                               *
****************************************************************/

VertexToAnimatedVertexConstraint::VertexToAnimatedVertexConstraint() {

}

void VertexToAnimatedVertexConstraint::applyConstraintToPoints() {

}

void VertexToAnimatedVertexConstraint::applyConstraintToSolverMatrices(
        double time, SparseMatrix* A, Physics_LargeVector* b) {

}


/****************************************************************
*                                                               *
*               VertexToAnimatedEllipseConstraint               *
*                                                               *
****************************************************************/

VertexToAnimatedEllipseConstraint::VertexToAnimatedEllipseConstraint() {

}

void VertexToAnimatedEllipseConstraint::applyConstraintToPoints() {
    //Move the follow's points to the same position as the lead's points.
//*
	//Get Ellipsoid Position From Ellipsoid Transformation
	vec4 origin(0);
	origin[3] = 1;
	origin[1] = 1.3;
	int index = _leadIndex;
	mat4 ElliTransform(0);
	ElliTransform = _lead->getEllipsoid(index);
	vec4 ElliCenter = origin * ElliTransform;
	//Set Follow Position to Ellipsoid Position (converts from vec4 -> vec3)
	//cout<<"Constraining Follow Vertex "<<_follow->getX()<<"the lead vertex "<<ElliCenter<<endl;
	for(int i = 0; i < 3; i++)
		_follow->getX()[i] = ElliCenter[i];
	//cout<<"Follow Vertex is now "<<_follow->getX()<<endl;
//*/
}
void VertexToAnimatedEllipseConstraint::applyConstraintToSolverMatrices(
        double time, SparseMatrix* m_A, Physics_LargeVector* m_b) {
    //Update the sparse matrices to honor this constraint

	int cVertex = _follow->getID();

	for (int i = 0; i < m_A->m_iRows; i++) {
		for (int j = 0; j < 9; j++) {
			(*m_A)(cVertex, i).m_Mx[j] = 0;
		}
	}
	for (int i = 0; i < 9; i++) {
		(*m_A)(cVertex, cVertex).m_Mx[i] = 1;
	}
	for (int p = 0; p < m_b->m_iElements; p++) {
		m_b->m_pData[cVertex].x = 0;
		m_b->m_pData[cVertex].y = 0;
		m_b->m_pData[cVertex].z = 0;
	}

}

void VertexToAnimatedEllipseConstraint::frictionForce(TriangleMeshVertex* p, vec3 Velli, int j) {
	vec3 n = _ellipsoids->getNormal(j, p->getX());
	vec3 Vrel = p->getvX() - Velli;
	vec3 saveF = p->getF();

	vec3 N = p->getF()*-1*n;
	if( p->getvX().length() == 0 && p->getF().length() < (_ellipsoids->getMu_s()*N.length()) )
		p->getF() = 0;
	else {
		vec3 F = -1*_ellipsoids->getMu_d()*N.length()*(Vrel/Vrel.length());
		p->getF() += F;
		cout<<"Friction Force of "<<F<<" was applied."<<endl;
	}

}

void VertexToAnimatedEllipseConstraint::applyCollisionToMesh(Physics_LargeVector* X, Physics_LargeVector* V) {
	for(int i = 0; i < _mesh->countVertices(); i++) {
		for(int j = 0; j < _ellipsoids->getSize(); j++) {
			TriangleMeshVertex* Vert = _mesh->getVertex(i);
			vec4 Xc_elliSpace = _ellipsoids->convertPoint2ElliSpace(j, Vert->getX());

			if( _ellipsoids->isPointInsideElli(j, Xc_elliSpace) ) {
				//cout<<"colision detected"<<endl;



					//Get Origin of Current Ellipsoid
				/*	vec4 origin(0); origin[1] = 1;
					vec4 Xo_4 = origin * elliTransform;
					vec3 Xo;
					for(int k = 0; k < 3; k++)
						Xo[k] = Xo_4[k];
				//*/
				//Get Linear Velocity
				//vec3 linearVelocity = (ElliCenterFuture - ElliCenterPast)/(2*elliTimeStep);
				//cout<<"Past ellipsoid Position: "<<ElliCenterPast<<endl;
				//cout<<"future ellipsoid Position: "<<ElliCenterFuture<<endl;
				//cout<<"timeStep * 2 "<<elliTimeStep*2<<endl;
				//cout<<"linearVelocity "<<linearVelocity<<endl<<endl;
			//*



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

				vec3 n = _ellipsoids->getNormal(j, Vert->getX());

				//cout<<n<<endl;
				//vec3 Velocity = Velli.length()*-1*n;

				//frictionForce(Vert, Velli, j);

				//cout<<"Angular Component: "<<Wtan<<endl;
				//cout<<"Linear Component: "<<Vo<<endl;
				//cout<<"Total Velocity: "<<Velli<<endl;
//*
				X->m_pData[Vert->getID()].x = Xc[0] + 10*(Xc[0] - X->m_pData[Vert->getID()].x);
				X->m_pData[Vert->getID()].y = Xc[1] + 10*(Xc[1] - X->m_pData[Vert->getID()].y);
				X->m_pData[Vert->getID()].z = Xc[2] + 10*(Xc[2] - X->m_pData[Vert->getID()].z);
/*
				vec3 Vvert;
				Vvert[0] = V->m_pData[i].x;
				Vvert[1] = V->m_pData[i].y;
				Vvert[2] = V->m_pData[i].z;

				for (int k = 0; k < 3; k++) {
					if (Vvert[k] == 0) {
						Vvert[k] = Velli[k];
						cout<<"case1"<<endl;
					}
					else if (Velli[k] == 0) {
						Vvert[k] = -.1*Velli[k];
						cout<<"case2"<<endl;
					}
					else if ((Velli[k] > 0 && Vvert[k] < 0) || (Velli[k] < 0
							&& Vvert[k] > 0)) {
						Vvert[k] = Velli[k] + .1*(-Vvert[k]);
						cout<<"case3"<<endl;
					}
					else if ((Velli[k] > 0 && Vvert[k] > 0) || (Velli[k] < 0
							&& Vvert[k] < 0)) {
						Vvert[k] = Velli[k] + .1*Vvert[k];
						cout<<"case4"<<endl;
					}
					else {
						cout<<"Unkown Case Came Up"<<endl;
						cout<<"Vvert: "<<Vvert<<endl;
						cout<<"Velli: "<<Velli<<endl;
						exit(1);
					}
				}
//*/
				V->m_pData[Vert->getID()].x = -.1*V->m_pData[Vert->getID()].x + Velli[0];//+ Wtan[0];// + Vo[0];//
				V->m_pData[Vert->getID()].y = -.1*V->m_pData[Vert->getID()].y + Velli[1];//+ Wtan[1];// + Vo[1];//
				V->m_pData[Vert->getID()].z = -.1*V->m_pData[Vert->getID()].z + Velli[2];//+ Wtan[2];// + Vo[2];//
//*/
			}
		}
	}
}

