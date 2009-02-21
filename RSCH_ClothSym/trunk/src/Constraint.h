/*
 * Constraint.h
 *
 *  Created on: Oct 27, 2008
 *      Author: njoubert
 */

#ifndef CONSTRAINT_H_
#define CONSTRAINT_H_

class Constraint;
class FixedConstraint;
class VertexToAnimatedVertexConstraint;
class VertexToAnimatedEllipseConstraint;
class VertexToEllipseCollision;

#include "global.h"
#include "Mesh.h"
#include "Model.h"
#include "math/LargeVector.h"
#include "math/LargeMatrix.h"

class Constraint {
public:
    Constraint();
    virtual ~Constraint();
    virtual void applyConstraintToPoints(LARGE_VECTOR*, LARGE_VECTOR*, LARGE_VECTOR*, double)=0;
    //virtual void applyConstraintToPoints(LARGE_VECTOR*, LARGE_VECTOR*, LARGE_VECTOR*)=0;
    virtual void applyConstraintToSolverMatrices(SPARSE_MATRIX*, LARGE_VECTOR*)=0;
    //virtual void applyConstraintsToState(LARGE_VECTOR* X, LARGE_VECTOR* Y, LARGE_VECTOR* V)=0;
    //virtual void applyCollisionToMesh(LARGE_VECTOR*, LARGE_VECTOR*, LARGE_VECTOR*)=0;
    virtual void applyExplicitConstraints(LARGE_VECTOR* X, LARGE_VECTOR* V, double)=0;
    void setLead(AniElliModel*, int);
    void setFollow(TriangleMeshVertex*);
    void setHierarchy(int);
    TriangleMeshVertex* getFollow();

    //COLLISION
    void setCollisionMesh(TriangleMesh* mesh);
    void setCollisionEllipsoids(AniElliModel* ellipsoids);

	protected:
    AniElliModel* _lead;
    int _leadIndex;
    int _hierarchyIndex;
    TriangleMeshVertex* _follow;

    //FOR COLLISIONS
    TriangleMesh* _mesh;
    AniElliModel* _ellipsoids;
};

class FixedConstraint : public Constraint {
public:
    FixedConstraint();
    void applyConstraintToPoints(LARGE_VECTOR*,LARGE_VECTOR*, LARGE_VECTOR*, double);
    void applyExplicitConstraints(LARGE_VECTOR* X, LARGE_VECTOR* V, double);
    void applyConstraintToSolverMatrices(SPARSE_MATRIX*, LARGE_VECTOR*);
};

class FixedBaraffConstraint : public Constraint {
public:
    FixedBaraffConstraint();
    void applyConstraintToPoints(LARGE_VECTOR*,LARGE_VECTOR*,LARGE_VECTOR*, double);
    void applyExplicitConstraints(LARGE_VECTOR* X, LARGE_VECTOR* V, double);
    void applyConstraintToSolverMatrices(SPARSE_MATRIX*, LARGE_VECTOR*);
    void applyMassMod(SPARSE_MATRIX*);
};

class VertexToAnimatedVertexConstraint : public Constraint {
public:
    VertexToAnimatedVertexConstraint();
    void applyConstraintToPoints(LARGE_VECTOR*,LARGE_VECTOR*,LARGE_VECTOR*, double);
    void applyExplicitConstraints(LARGE_VECTOR* X, LARGE_VECTOR* V, double);
    void applyConstraintToSolverMatrices(SPARSE_MATRIX*, LARGE_VECTOR*);
};

class VertexToAnimatedEllipseConstraint : public Constraint {
public:
    VertexToAnimatedEllipseConstraint();
    void applyConstraintToPoints(LARGE_VECTOR*,LARGE_VECTOR*,LARGE_VECTOR*, double);
    void applyConstraintToSolverMatrices(SPARSE_MATRIX*, LARGE_VECTOR*);
    void applyConstraints(LARGE_VECTOR* X, LARGE_VECTOR* V);
    void applyExplicitConstraints(LARGE_VECTOR* X, LARGE_VECTOR* V, double);
private:
	vec3 _vel;
};

class VertexToEllipseCollision : public Constraint {
public:
	VertexToEllipseCollision();
    void applyConstraintToPoints(LARGE_VECTOR*,LARGE_VECTOR*,LARGE_VECTOR*, double);
    void applyConstraintToSolverMatrices(SPARSE_MATRIX*, LARGE_VECTOR*);
    void applyExplicitConstraints(LARGE_VECTOR* X, LARGE_VECTOR* V, double);

    vec3 f_dampCollision(vec3 vPoint, double Kcd);
    vec3 f_friction(vec3 V, double Mu);

    void applyDampedCollisions(double , SPARSE_MATRIX* ,SPARSE_MATRIX*, LARGE_VECTOR*, double localTime, bool);
    void applyPosVelChangeCollision(double, LARGE_VECTOR* Y, LARGE_VECTOR* X, LARGE_VECTOR* V);
private:
	vector <int> _collisionIndices;
	vector <vec3> _collisionVelocities;
	bool _USECOLLJACOBIAN;
};

#endif /* CONSTRAINT_H_ */
