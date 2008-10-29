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

class Constraint {
public:
    Constraint();
    virtual ~Constraint();
    virtual void applyConstraint(double time, SPARSE_MATRIX*, LARGE_VECTOR*)=0;
};

class FixedConstraint : public Constraint {
public:
    FixedConstraint();
    void setTarget(TriangleMeshVertex*);
    void applyConstraint(double time, SPARSE_MATRIX*, LARGE_VECTOR*);
private:
    TriangleMeshVertex* target;
};

class VertexToVertexConstraint : public Constraint {
public:
    VertexToVertexConstraint();
    void setSource(TriangleMeshVertex*);
    void setTarget(TriangleMeshVertex*);
    void applyConstraint(double time, SPARSE_MATRIX*, LARGE_VECTOR*);
private:
    TriangleMeshVertex* source;
    TriangleMeshVertex* target;
};

#endif /* CONSTRAINT_H_ */
