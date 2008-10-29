/*
 * Constraint.cpp
 *
 *  Created on: Oct 27, 2008
 *      Author: njoubert
 */

#include "Constraint.h"

Constraint::Constraint() {
    // TODO Auto-generated constructor stub

}

Constraint::~Constraint() {
    // TODO Auto-generated destructor stub
}



FixedConstraint::FixedConstraint() {

}
void FixedConstraint::setTarget(TriangleMeshVertex* v) {
    target = v;
}
void FixedConstraint::applyConstraint(double time, SPARSE_MATRIX*, LARGE_VECTOR*) {

}


VertexToVertexConstraint::VertexToVertexConstraint() {

}
void VertexToVertexConstraint::setSource(TriangleMeshVertex* v) {
    source = v;
}
void VertexToVertexConstraint::setTarget(TriangleMeshVertex* v) {
    target = v;
}

void VertexToVertexConstraint::applyConstraint(double time, SPARSE_MATRIX*, LARGE_VECTOR*) {

}
