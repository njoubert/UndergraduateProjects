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
    virtual void applyConstraintToPoints(LARGE_VECTOR*, LARGE_VECTOR*)=0;
    virtual void applyConstraintToSolverMatrices(SPARSE_MATRIX*, LARGE_VECTOR*)=0;
    //virtual void applyCollisionToMesh(LARGE_VECTOR*, LARGE_VECTOR*, LARGE_VECTOR*)=0;
    void setLead(AniElliModel*, int);
    void setFollow(TriangleMeshVertex*);
    TriangleMeshVertex* getFollow();

    //COLLISION
    void setCollisionMesh(TriangleMesh* mesh);
    void setCollisionEllipsoids(AniElliModel* ellipsoids);

	protected:
    AniElliModel* _lead;
    int _leadIndex;
    TriangleMeshVertex* _follow;

    //FOR COLLISIONS
    TriangleMesh* _mesh;
    AniElliModel* _ellipsoids;
};

class FixedConstraint : public Constraint {
public:
    FixedConstraint();
    void applyConstraintToPoints(LARGE_VECTOR*,LARGE_VECTOR*);
    void applyConstraintToSolverMatrices(SPARSE_MATRIX*, LARGE_VECTOR*);
};

class VertexToAnimatedVertexConstraint : public Constraint {
public:
    VertexToAnimatedVertexConstraint();
    void applyConstraintToPoints(LARGE_VECTOR*,LARGE_VECTOR*);
    void applyConstraintToSolverMatrices(SPARSE_MATRIX*, LARGE_VECTOR*);
};

class VertexToAnimatedEllipseConstraint : public Constraint {
public:
    VertexToAnimatedEllipseConstraint();
    void applyConstraintToPoints(LARGE_VECTOR*,LARGE_VECTOR*);
    void applyConstraintToSolverMatrices(SPARSE_MATRIX*, LARGE_VECTOR*);
};

class VertexToEllipseCollision : public Constraint {
public:
	VertexToEllipseCollision();
    void applyConstraintToPoints(LARGE_VECTOR*,LARGE_VECTOR*);
    void applyConstraintToSolverMatrices(SPARSE_MATRIX*, LARGE_VECTOR*);
    void applyCollisionToMesh(LARGE_VECTOR*, LARGE_VECTOR*, LARGE_VECTOR*);
    void frictionForce(int, TriangleMeshVertex*, vec3,  LARGE_VECTOR*);
};

#endif /* CONSTRAINT_H_ */
