/*
 * Constraint.h
 *
 *  Created on: Oct 27, 2008
 *      Author: njoubert
 */

#ifndef CONSTRAINT_H_
#define CONSTRAINT_H_

class Constraint;

#include "global.h"
#include "Mesh.h"
#include "Model.h"
#include "System.h"

class Constraint {
public:
    Constraint();
    virtual ~Constraint();
    virtual void applyConstraintToPoints()=0;
    virtual void applyConstraintToSolverMatrices(double time, SparseMatrix* A, Physics_LargeVector* b)=0;
    virtual void applyCollisionToMesh(Physics_LargeVector*, Physics_LargeVector*)=0;
    void setLead(AniElliModel*, int);
    void setFollow(TriangleMeshVertex*);
    TriangleMeshVertex* getFollow();
    void setCollisionMesh(TriangleMesh* mesh);
    void setCollisionEllipsoids(AniElliModel* ellipsoids);
//private:
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
    void applyConstraintToPoints();
    void applyConstraintToSolverMatrices(double time, SparseMatrix* A, Physics_LargeVector* b);
};

class VertexToAnimatedVertexConstraint : public Constraint {
public:
    VertexToAnimatedVertexConstraint();
    void applyConstraintToPoints();
    void applyConstraintToSolverMatrices(double time, SparseMatrix* A, Physics_LargeVector* b);
};

class VertexToAnimatedEllipseConstraint : public Constraint {
public:
    VertexToAnimatedEllipseConstraint();
    void applyConstraintToPoints();
    void applyConstraintToSolverMatrices(double time, SparseMatrix* A, Physics_LargeVector* b);
    void applyCollisionToMesh(Physics_LargeVector* X, Physics_LargeVector* V);
};

#endif /* CONSTRAINT_H_ */
