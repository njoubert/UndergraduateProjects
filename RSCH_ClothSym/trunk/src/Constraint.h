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
#include "math/LargeVector.h"
#include "math/LargeMatrix.h"

class Constraint {
public:
    Constraint();
    virtual ~Constraint();
    virtual void applyConstraintToPoints(LARGE_VECTOR*, LARGE_VECTOR*)=0;
    virtual void applyConstraintToSolverMatrices(SPARSE_MATRIX*, LARGE_VECTOR*)=0;
    void setLead(AniElliModel*, int);
    void setFollow(TriangleMeshVertex*);
    TriangleMeshVertex* getFollow();
protected:
    AniElliModel* _lead;
    int _leadIndex;
    TriangleMeshVertex* _follow;
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

#endif /* CONSTRAINT_H_ */
