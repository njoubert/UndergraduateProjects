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

class Constraint {
public:
    Constraint();
    virtual ~Constraint();
    virtual void applyConstraintToPoints()=0;
    virtual void applyConstraintToSolverMatrices(double time, SPARSE_MATRIX*, LARGE_VECTOR*)=0;
    void setLead(Model*, int);
    void setFollow(TriangleMeshVertex*);
private:
    Model* _lead;
    int _leadIndex;
    TriangleMeshVertex* _follow;
};

class FixedConstraint : public Constraint {
public:
    FixedConstraint();
    void applyConstraintToPoints();
    void applyConstraintToSolverMatrices(double time, SPARSE_MATRIX*, LARGE_VECTOR*);
};

class VertexToAnimatedVertexConstraint : public Constraint {
public:
    VertexToAnimatedVertexConstraint();
    void applyConstraintToPoints();
    void applyConstraintToSolverMatrices(double time, SPARSE_MATRIX*, LARGE_VECTOR*);
};

class VertexToAnimatedEllipseConstraint : public Constraint {
public:
    VertexToAnimatedEllipseConstraint();
    void applyConstraintToPoints();
    void applyConstraintToSolverMatrices(double time, SPARSE_MATRIX*, LARGE_VECTOR*);
};

#endif /* CONSTRAINT_H_ */
