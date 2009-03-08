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

void Constraint::setHierarchy(int hIndex) {
	_hierarchyIndex = hIndex;
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

vec3 Constraint::getVel() { return _vel; }

/****************************************************************
 *                                                               *
 *               FixedConstraint                                 *
 *                                                               *
 ****************************************************************/

FixedConstraint::FixedConstraint() {

}

void FixedConstraint::applyConstraintToPoints(LARGE_VECTOR* x, LARGE_VECTOR* v,
		LARGE_VECTOR* y, double timeStep) {
/*
	int cVertex = _follow->getIndex();
	double vx = 1.1, vy = 1.1, vz = -1.1;
	//cout<<int(TIME)%2<<endl;
	if(int(TIME)%2 > 0) {
		vx = -1.1*vx;
		vy = -1.1*vy;
		vz = -1.1*vz;
	}
	vec3 vel = vec3(vx,vy,vz);
	cout<<vel<<endl;
	(*y)[cVertex] = (vel*timeStep) - (*x)[cVertex];
	_follow->getX() = vel*timeStep;
	//(*X)[cVertex] = vec3(ElliCenter, VW);  Don't uncomment this line unless you like explosions

	(*v)[cVertex] = vel;
	_follow->getvX() = vel;
	//*/
	//Update the sparse matrices to honor this constraint
}

void FixedConstraint::applyConstraintToSolverMatrices(SPARSE_MATRIX* A,
		LARGE_VECTOR* b) {
	int cVertex = _follow->getIndex();
	/*

	double x = 1.1, y = 1.1, z = -1.1;
	//cout<<int(TIME)%2<<endl;
	if(int(TIME)%2 > 0) {
		x = -1.1*x;
		y = -1.1*y;
		z = -1.1*z;
	}
//*/

	vec3 zero(0);
	A->constrainSystem(cVertex,cVertex,b,zero);
}

void FixedConstraint::applyExplicitConstraints(LARGE_VECTOR* X,
		LARGE_VECTOR* V, double localTime) {

}

/****************************************************************
 *                                                               *
 *               FixedBaraffConstraint                           *
 *                                                               *
 ****************************************************************/

FixedBaraffConstraint::FixedBaraffConstraint() {

}

void FixedBaraffConstraint::applyConstraintToPoints(LARGE_VECTOR* x,
		LARGE_VECTOR* v, LARGE_VECTOR* y, double timeStep) {
	//Update the sparse matrices to honor this constraint
}

void FixedBaraffConstraint::applyConstraintToSolverMatrices(SPARSE_MATRIX* A,
		LARGE_VECTOR* b) {

}

void FixedBaraffConstraint::applyMassMod(SPARSE_MATRIX* M) {
	mat3 S;
	for (int i = 0; i < _mesh->countVertices(); i++) {
		if (_mesh->getVertex(i)->getConstaint())
			S = mat3(0);
		else
			S = mat3(identity2D());

		cout << "this should be the mass of a particle: " << (*M)(i, i)[0][0]
				<< endl;
		(*M)(i, i) = (1 / (*M)(i, i)[0][0]) * S;
	}
}

void FixedBaraffConstraint::applyExplicitConstraints(LARGE_VECTOR* X,
		LARGE_VECTOR* V, double) {

}

/****************************************************************
 *                                                               *
 *               VertexToAnimatedVertexConstraint                *
 *                                                               *
 ****************************************************************/

VertexToAnimatedVertexConstraint::VertexToAnimatedVertexConstraint() {

}

void VertexToAnimatedVertexConstraint::applyConstraintToPoints(LARGE_VECTOR* x,
		LARGE_VECTOR* v, LARGE_VECTOR* y, double timeStep) {

}

void VertexToAnimatedVertexConstraint::applyConstraintToSolverMatrices(
		SPARSE_MATRIX*, LARGE_VECTOR*) {

}

void VertexToAnimatedVertexConstraint::applyExplicitConstraints(
		LARGE_VECTOR* X, LARGE_VECTOR* V, double) {

}

/****************************************************************
 *                                                               *
 *               VertexToAnimatedEllipseConstraint               *
 *                                                               *
 ****************************************************************/

VertexToAnimatedEllipseConstraint::VertexToAnimatedEllipseConstraint() {

}

void VertexToAnimatedEllipseConstraint::applyConstraintToPoints(
		LARGE_VECTOR* X, LARGE_VECTOR* V, LARGE_VECTOR* Y, double localTime) {
	//Move the follow's points to the same position as the lead's points.
	//Get Ellipsoid Position From Ellipsoid Transformation
	vec4 origin(0);
	origin[3] = 1;
	//origin[1] = -1.3;
	//origin[2] = -1.3;
	//origin[0] = -1.3;
	int index = _leadIndex;
	mat4 ElliTransform(0);
	ElliTransform = _lead->getEllipsoid(index);
	//vec4 ElliCenter = origin * ElliTransform;
	int cVertex = _follow->getIndex();

	double elliTimeStep = _lead->getTimeStep();
	double elliTime[3];
	//elliTime[0] = _lead->getElliTimePast();
	elliTime[0] = _lead->getElliTimeCurrent() - elliTimeStep;
	elliTime[1] = _lead->getElliTimeCurrent();
	//elliTime[2] = _lead->getElliTimeFuture();
	elliTime[2] = _lead->getElliTimeCurrent() + elliTimeStep;
	if (abs((elliTime[2] - elliTime[0]) - 2 * elliTimeStep) > 0.0001)
		cout << "WTF: " << elliTime[2] - elliTime[0] << " != " << 2
				* elliTimeStep << endl;

	vec3 xf = _lead->getFutureOrigin(_leadIndex);
	vec3 xp = _lead->getPastOrigin(_leadIndex);
	vec3 x0 = _lead->getOrigin(_leadIndex);
	vec3 x0prime, v;
//*
	if ((localTime - elliTime[1]) < -0.0000000001) {
		x0prime = x0 + (localTime - elliTime[0]) * ((x0 - xp) / (elliTime[1]
				- elliTime[0]));
		v = (x0 - xp) / (elliTime[1] - elliTime[0]);
	} else if ((localTime - elliTime[1]) > 0.0000000001) {
		x0prime = x0 + (localTime - elliTime[1]) * ((xf - x0) / (elliTime[2]
				- elliTime[1]));
		v = (xf - x0) / (elliTime[2] - elliTime[1]);
	} else {
		x0prime = x0;
		v = (xf - x0) / (elliTime[2] - elliTime[1]);
	}
//*/
	x0prime = x0;
	v = (xf - x0) / (elliTime[2] - elliTime[1]);
	//cout << "Position of Constraint: " << x0prime << endl;
	//cout << "Velocity of Constraint (Vector: " << v << endl;
	//cout << "Velocity of Constraint (Mag): " << v.length() << endl;
	//x0prime =
	//All these must be updated
	(*Y)[cVertex] = x0prime - (*X)[cVertex];
	_follow->getX() = x0prime;
	//(*X)[cVertex] = vec3(ElliCenter, VW);  Don't uncomment this line unless you like explosions

	(*V)[cVertex] = v;
	_follow->getvX() = v;
	_vel = v;


}

void VertexToAnimatedEllipseConstraint::applyConstraintToSolverMatrices(
		SPARSE_MATRIX* A, LARGE_VECTOR* b) {
	//Update the sparse matrices to honor this constraint
	int cVertex = _follow->getIndex();
	/*
	if(TIME > 10.01)
		A->constrainSystem2(cVertex,cVertex,b,_vel);
	else
	//*/

	A->constrainSystem(cVertex,cVertex,b,_vel);

}



void VertexToAnimatedEllipseConstraint::applyExplicitConstraints(
		LARGE_VECTOR* X, LARGE_VECTOR* V, double localTime) {

	int cVertex = _follow->getIndex();

	double elliTimeStep = _lead->getTimeStep();
	double elliTime[3];
	//elliTime[0] = _lead->getElliTimePast();
	elliTime[0] = _lead->getElliTimeCurrent() - elliTimeStep;
	elliTime[1] = _lead->getElliTimeCurrent();
	//elliTime[2] = _lead->getElliTimeFuture();
	elliTime[2] = _lead->getElliTimeCurrent() + elliTimeStep;
	if (abs((elliTime[2] - elliTime[0]) - 2 * elliTimeStep) > 0.0001)
		cout << "WTF: " << elliTime[2] - elliTime[0] << " != " << 2
				* elliTimeStep << endl;

	vec3 xf = _lead->getFutureOrigin(_leadIndex);
	vec3 xp = _lead->getPastOrigin(_leadIndex);
	vec3 x0 = _lead->getOrigin(_leadIndex);
	vec3 x0prime, v;

	if ((localTime - elliTime[1]) < -0.0000000001) {
		x0prime = x0 + (localTime - elliTime[0]) * ((x0 - xp) / (elliTime[1]
				- elliTime[0]));
		v = (x0 - xp) / (elliTime[1] - elliTime[0]);
	} else if ((localTime - elliTime[1]) > 0.0000000001) {
		x0prime = x0 + (localTime - elliTime[1]) * ((xf - x0) / (elliTime[2]
				- elliTime[1]));
		v = (xf - x0) / (elliTime[2] - elliTime[1]);
	} else {
		x0prime = x0;
		v = (xf - x0) / (elliTime[2] - elliTime[1]);
	}

	//All these must be updated
	_follow->getX() = x0prime;
	(*X)[cVertex] = x0prime;

	(*V)[cVertex] = v;
	_follow->getvX() = v;
}

/****************************************************************
 *                                                              *
 *               VertexToEllipseCollision		                *
 *                                                              *
 ****************************************************************/

VertexToEllipseCollision::VertexToEllipseCollision() {

}

void VertexToEllipseCollision::applyConstraintToPoints(LARGE_VECTOR* X,
		LARGE_VECTOR* V, LARGE_VECTOR* y, double timeStep) {
	//Move the follow's points to the same position as the lead's points.
	//Get Ellipsoid Position From Ellipsoid Transformation
	cout << "This Should not be called" << endl;
}

void VertexToEllipseCollision::applyConstraintToSolverMatrices(
		SPARSE_MATRIX* A, LARGE_VECTOR* b) {
	//Update the sparse matrices to honor this constraint
	for (unsigned int i = 0; i < _collisionIndices.size(); i++) {
		A->constrainSystem(_collisionIndices[i], _collisionIndices[i], b, _collisionVelocities[i]);

		//cout<<"Modified Solver For Collisions"<<endl;
		if (_mesh->getVertex(_collisionIndices[i])->getvX().length()
				!= _collisionVelocities[i].length()) {
			cout << "Vertex: " << _collisionIndices[i]
					<< " State  has a velocity of  : " << _mesh->getVertex(
					_collisionIndices[i])->getvX() << endl;
			cout << "Vertex: " << _collisionIndices[i]
					<< " Solver has a velocity of  : "
					<< _collisionVelocities[i] << endl;
			cout << endl;
		}

	}
	_collisionIndices.clear();
	_collisionVelocities.clear();
}

vec3 VertexToEllipseCollision::f_dampCollision(vec3 Vn, double Kcd) {
	vec3 f = -Kcd * Vn;
	return f;
}
vec3 VertexToEllipseCollision::f_friction(vec3 Vt, double Mu) {
	vec3 f = -Mu * Vt;
	return f;
}

void VertexToEllipseCollision::applyExplicitConstraints(LARGE_VECTOR* X,
		LARGE_VECTOR* V, double localTime) {
	for (int i = 0; i < _mesh->countVertices(); i++) {
		for (int j = 0; j < _ellipsoids->getSize(); j++) {
			TriangleMeshVertex* Vert = _mesh->getVertex(i);
			vec4 Xc_elliSpace = _ellipsoids->convertPoint2ElliSpace(j,
					Vert->getX());

			if (_ellipsoids->isPointInsideElli(j, Xc_elliSpace)) {
				//cout<<"colision detected"<<endl;

				vec3 Xc = _ellipsoids->getPointInsideElli2Surface(j,
						Xc_elliSpace);

				vec3 v(0);
				v = -0.01 * (*V)[i];

				//cout<<"Vertex: "<<i<<" had a velocity of      : "<<_mesh->getVertex(i)->getvX().length()<<endl;
				//cout<<"Vertex: "<<i<<" now has a velocity of  : "<<v.length()<<endl;
				//cout<<endl;

				//START: ALL THESE MUST BE UPDATED
				//cout<<(Xc - Vert->getX()).length()<<endl;
				//if((Xc - Vert->getX()).length() > 0.1) {
				//cout<<"Modified State for Collisions"<<endl;
				(*X)[i] = Xc;
				_mesh->getVertex(i)->getX() = Xc;
				(*V)[i] = v;
				_mesh->getVertex(i)->getvX() = v;
				//END: UPDATING STATE AND SOLVER
			}
		}
	}
}

void VertexToEllipseCollision::applyPosVelChangeCollision(double timeStep,
		LARGE_VECTOR* Y, LARGE_VECTOR* X, LARGE_VECTOR* V) {
	for (int i = 0; i < _mesh->countVertices(); i++) {
		for (int j = 0; j < _ellipsoids->getSize(); j++) {
			TriangleMeshVertex* Vert = _mesh->getVertex(i);
			vec4 Xc_elliSpace = _ellipsoids->convertPoint2ElliSpace(j,
					Vert->getX());

			if (_ellipsoids->isPointInsideElli(j, Xc_elliSpace)) {
				//cout<<"colision detected"<<endl;

				vec3 Xc = _ellipsoids->getPointInsideElli2Surface(j,
						Xc_elliSpace);

				vec3 v(0);
				v = (Xc - Vert->getX()) / timeStep;

				//cout<<"Vertex: "<<i<<" had a velocity of      : "<<_mesh->getVertex(i)->getvX().length()<<endl;
				//cout<<"Vertex: "<<i<<" now has a velocity of  : "<<v.length()<<endl;
				//cout<<endl;

				//START: ALL THESE MUST BE UPDATED
				cout << (Xc - Vert->getX()).length() << endl;
				if ((Xc - Vert->getX()).length() > 0.1) {
					cout << "Modified State for Collisions" << endl;
					(*Y)[i] = Xc - (*X)[i];
					_mesh->getVertex(i)->getX() = Xc;
					(*V)[i] = v;
					_mesh->getVertex(i)->getvX() = v;
					_collisionIndices.push_back(i); //4 Solver Matrices
					_collisionVelocities.push_back(v); //4 Solver Matrices
				}
				//END: UPDATING STATE AND SOLVER
			}
		}
	}
}

void VertexToEllipseCollision::applyDampedCollisions(double Kcd,
		SPARSE_MATRIX* JP, SPARSE_MATRIX* JV, LARGE_VECTOR* F,
		double localTime, bool use_collision_jacobian) {
	//cout<<"calling applyDampedCollisions---------------------"<<endl;
	for (int i = 0; i < _mesh->countVertices(); i++) {
		for (int j = 0; j < _ellipsoids->getSize(); j++) {
			TriangleMeshVertex* Vert = _mesh->getVertex(i);
			vec4 Xc_elliSpace = _ellipsoids->convertPoint2ElliSpace(j,
					Vert->getX());

			if (_ellipsoids->isPointInsideElli(j, Xc_elliSpace)
					&& !Vert->getConstaint()) {
				//cout<<"colision detected"<<endl;
				//*
				Vert->detectedCollision() = true;
				vec3 xs = _ellipsoids->getPointInsideElli2Surface(j,
						Xc_elliSpace);
				vec3 x0 = Vert->getX();
				//vec3 n(0,0,-1);
				vec3 n = _ellipsoids->getNormal(j, Xc_elliSpace);
				//vec3 n = (Xc_surf - x0).normalize();
				Vert->setNormalAtCollisionPt(n);

				//cout<<"Index: "<<Vert->getIndex()<<"i: "<<i<<endl;

				//vec3 Xo_f = _ellipsoids->getFutureOrigin(j);
				//vec3 Xo_p = _ellipsoids->getPastOrigin(j);

				vec3 xf = _ellipsoids->getPointInFuture(j, x0);
				vec3 xp = _ellipsoids->getPointInPast(j, x0);
				double elliTimeStep = _ellipsoids->getTimeStep();
				double elliTime[3];
				elliTime[1] = _ellipsoids->getElliTimeCurrent();
				elliTime[0] = _ellipsoids->getElliTimeCurrent() - elliTimeStep;
				elliTime[2] = _ellipsoids->getElliTimeCurrent() + elliTimeStep;

				vec3 x0prime, v;
				if ((localTime - elliTime[1]) < -0.0000000001) {
					x0prime = x0 + (localTime - elliTime[0]) * ((x0 - xp)
							/ (elliTime[1] - elliTime[0]));
					v = (x0 - xp) / (elliTime[1] - elliTime[0]);
				} else if ((localTime - elliTime[1]) > 0.0000000001) {
					x0prime = x0 + (localTime - elliTime[1]) * ((xf - x0)
							/ (elliTime[2] - elliTime[1]));
					v = (xf - x0) / (elliTime[2] - elliTime[1]);
				} else {
					x0prime = x0;
					v = (xf - x0) / (elliTime[2] - elliTime[1]);
				}

				//	V0 = Velocity of Point on Ellipsoid
				//vec3 Vo = (Xc_f - Xc_p) / (2 * elliTimeStep);

				//vec3 Va(0);
				//Va = _ellipsoids->calcAngularVel(j, Xc);

				vec3 vb = v;
				/*
				 if(Va.length() > 0){
				 cout<<"Frame: "<<_ellipsoids->getFrameCount()<<endl;
				 cout<<"Ellipsoid: " << j << endl;
				 cout<<"Linear Velocity = 	"<<Vo<<endl;
				 cout<<"Angular Velocity = 	"<<Va<<endl;
				 cout<<"Total Velocity = 	"<<vb<<endl;
				 cout<<"Percent Angular = 	"<<( Va.length()/(Vo.length()+Va.length()) )*100<<endl;
				 cout<<endl;
				 }
				 //*/

				//if(n.length() != 1 && Xc_elliSpace[0] == 0) {
				//if(n[2] < 0) {
				//cout<<"Collision Point(World Space): "<<x0<<", Collision Point (Local Space): "<<Xc_elliSpace<<endl;
				//cout<<"Normal Length Is: "<<n.length()<<"  Normal is: "<<n<<endl;
				//exit(1);
				//}
				//_ellipsoids->setNormal(Xc, n);
				vec3 vp = Vert->getvX();
				vec3 Vrel = vp - vb;
				vec3 Vn = (Vrel * n) * n;

				vec3 t = ((n ^ Vrel) ^ n).normalize();
				vec3 Vt = (Vrel * t) * t;
				//vec3 Vt = (n ^ Vrel) ^ n;

				//cout<<"Vrel: "<<Vrel<<endl;
				//cout<<"Vn: "<<Vn<<endl;
				//cout<<"Vt: "<<Vt<<endl;

				double mu = MUd;
				//cout << "mu: " << mu << endl;
				//mu = .6;
				//damping force only applied if moving into ellipse.
				if ((Vn * n) < 0) {
					//	cout << "woo apply damping forces! n= g" << n << " Vn=" << Vn << endl;
					int vIndex = Vert->getIndex();
					vec3 dampingForce = f_dampCollision(Vn, Kcd);
					//vec3 dampingForce = -Kcd*Vrel;
					(*F)[i] += dampingForce;
					mat3 jv1(0);
					//Kcr = 1;
					vec3 nt(0, 0, 1);
					vec3 exitForce = Kcr * n * (xs - x0).length();
					(*F)[i] += exitForce;//(Xc_surf - x0);
					//cout << "Damping Force: " << dampingForce<< endl;
					//cout << "Exit Force: "<< exitForce << endl;
					jv1[0][0] = 0;
					jv1[0][1] = 0;
					jv1[0][2] = 0;
					jv1[1][0] = 0;
					jv1[1][1] = 0;
					jv1[1][2] = 0;
					jv1[2][0] = 0;
					jv1[2][1] = 0;
					jv1[2][2] = 0;
					//(*JP)(vIndex, vIndex) += jv1;


					mat3 jv(0);

					//*
					//Jacobian For Damping in Normal Direction
					jv[0][0] = -Kcd * n[0] * n[0];
					jv[0][1] = -Kcd * n[0] * n[1];
					jv[0][2] = -Kcd * n[0] * n[2];
					jv[1][0] = -Kcd * n[0] * n[1];
					jv[1][1] = -Kcd * n[1] * n[1];
					jv[1][2] = -Kcd * n[1] * n[2];
					jv[2][0] = -Kcd * n[0] * n[2];
					jv[2][1] = -Kcd * n[1] * n[2];
					jv[2][2] = -Kcd * n[2] * n[2];
					//cout<<"Normal: "<<n<<"  Kcd: "<<Kcd<<endl;
					//cout<<"Jacobian: "<<jv<<endl;
					//*/

					//*
					mat3 jp(0);
					/* //Test Jacobian Depth Based Force
					 Kcr = 10e5;
					 n = vec3(0.290221, .623975, -.725553);
					 x0 = vec3(4, -1.7, -3);
					 xs = vec3(2, -2, 9);
					 //*/

					//Jacobian For Damping in Normal Direction
					double magOfDepth = sqrt((xs[0] - x0[0]) * (xs[0] - x0[0])
							+ (xs[1] - x0[1]) * (xs[1] - x0[1]) + (xs[2]
							- x0[2]) * (xs[2] - x0[2]));
					//cout<<magOfDepth<<endl;
					//cout<<xs[0] - x0[0]<<endl;
					//cout<<-Kcr * n[0] * (xs[0] - x0[0])<<endl;
					jp[0][0] = (-Kcr * n[0] * (xs[0] - x0[0]));// / magOfDepth;
					jp[0][1] = (-Kcr * n[0] * (xs[1] - x0[1]));// / magOfDepth;
					jp[0][2] = (-Kcr * n[0] * (xs[2] - x0[2]));// / magOfDepth;
					jp[1][0] = (-Kcr * n[1] * (xs[0] - x0[0]));// / magOfDepth;
					jp[1][1] = (-Kcr * n[1] * (xs[1] - x0[1]));// / magOfDepth;
					jp[1][2] = (-Kcr * n[1] * (xs[2] - x0[2]));// / magOfDepth;
					jp[2][0] = (-Kcr * n[2] * (xs[0] - x0[0]));// / magOfDepth;
					jp[2][1] = (-Kcr * n[2] * (xs[1] - x0[1]));// / magOfDepth;
					jp[2][2] = (-Kcr * n[2] * (xs[2] - x0[2]));// / magOfDepth;
					//*/

					//cout<<jp<<endl;
					//cout<<"+"<<endl;
					//cout<<(*JP)(vIndex, vIndex)<<endl;
					//cout<<" = "<<endl;
					mat3 addCheck(0);
					mat3 oldJP = (*JP)(vIndex, vIndex);
					for (int indi = 0; indi < 3; indi++) {
						for (int indj = 0; indj < 3; indj++) {
							addCheck[indi][indj] = jp[indi][indj] + (*JP)(vIndex, vIndex)[indi][indj];
						}
					}

					if (use_collision_jacobian) {
						(*JP)(vIndex, vIndex) += jp;
						(*JV)(vIndex, vIndex) += jv;
					}
/*
					bool wtf = false;
					for (int indi = 0; indi < 3; indi++) {
						for (int indj = 0; indj < 3; indj++) {
							if(abs(addCheck[indi][indj] - (*JP)(vIndex, vIndex)[indi][indj]) > 10 && use_collision_jacobian) {
								cout<<"WTF"<<endl;
							cout<<abs(addCheck[indi][indj] - (*JP)(vIndex, vIndex)[indi][indj])<<endl;
							wtf = true;
							}
						}
					}
					if(wtf && use_collision_jacobian){
						cout<<oldJP<<endl;
						cout<<"+"<<endl;
						cout<<jp<<endl;
						cout<<" = "<<endl;
						cout<<"Jacobain shoudl be this: "<<endl;
						cout<<addCheck<<endl;
						cout<<"But it's this: "<<endl;
						cout<<(*JP)(vIndex, vIndex)<<endl;
						cout<<endl;
					}
					//cout<<"Got Through Checks A-OK"<<endl;
					//cout<<(*JP)(vIndex, vIndex)<<endl;
//*/
				}

				/*
				 if (MUd > 0.000000001) {
				 cout<<"Calc Friction"<<endl;
				 vec3 frictionForce = f_friction(Vt, MUd);
				 //cout<<"Friction Force "<<frictionForce<<endl;
				 (*F)[i] += frictionForce;
				 }
				 //*/
				//double vl = v.length();
				//cout<<vl<<endl;
				//cout<<MUs<<endl;
				/*
				 if(vl > -0.1 && vl < 0.1 && MUs > 0.000000001) {
				 vec3 staticFriction = - MUs * (*F)[i];
				 (*F)[i] += staticFriction;
				 cout<<"Static Friction Applied: "<<staticFriction<<endl;
				 }
				 //*/

				/*
				 //Jacobian For Damping in Tangental Direction ("Friction")
				 if (MUd > 0.000000001) {
				 cout<<"calc Dynamic Friction Jacobian"<<endl;
				 jv[0][0] = -MUd * t[0] * t[0];
				 jv[0][1] = -MUd * t[0] * t[1];
				 jv[0][2] = -MUd * t[0] * t[2];
				 jv[1][0] = -MUd * t[0] * t[1];
				 jv[1][1] = -MUd * t[1] * t[1];
				 jv[1][2] = -MUd * t[1] * t[2];
				 jv[2][0] = -MUd * t[0] * t[2];
				 jv[2][1] = -MUd * t[1] * t[2];
				 jv[2][2] = -MUd * t[2] * t[2];
				 (*JV)(i, i) += jv;
				 }
				 if(vl > -0.1 && vl < 0.1 && MUs > 0.000000001) {
				 cout<<"calc Static Friction Jacobian"<<endl;
				 jv[0][0] = -MUs;
				 jv[1][1] = -MUs;
				 jv[2][2] = -MUs;
				 }
				 //*/
				//Various Old Jacobians
				/*
				 jv[0][0] = -mu*pow(n[0],1+v[0]-vb[0])*pow(pow(n[0],v[0]-vb[0]),-1+n[0])*log(n[0]);
				 jv[1][1] = -mu*pow(n[1],1+v[1]-vb[1])*pow(pow(n[1],v[1]-vb[1]),-1+n[1])*log(n[1]);
				 jv[2][2] = -mu*pow(n[2],1+v[2]-vb[2])*pow(pow(n[2],v[2]-vb[2]),-1+n[2])*log(n[2]);
				 //*/
				/*
				 mat3 jv_temp(0);
				 //Jacobian For Damping in Tangental Direction ("Friction")
				 //Test Numbers for Mathematica
				 //n[0] = .1; n[1] = .7, n[2] = .1;
				 //v[0] = 1, v[1] = 3; v[2] = 0.037;
				 //vb[0] = .27; vb[1] = 2.02; vb[2] = 5.083;

				 jv_temp[0][0] = -mu*n[0]*pow(n[0],(v[0]-vb[0])*n[0])*log(n[0]);
				 jv_temp[1][1] = -mu*n[1]*pow(n[1],(v[1]-vb[1])*n[1])*log(n[1]);
				 jv_temp[2][2] = -mu*n[2]*pow(n[2],(v[2]-vb[2])*n[2])*log(n[2]);
				 cout<<endl<<jv_temp<<endl;
				 //*/

			}
		}
	}
	///cout<<endl;
}

