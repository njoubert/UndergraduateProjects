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

void Constraint::setLead(Model* lead, int nr) {
    _lead = lead;
    _leadIndex = nr;

}

void Constraint::setFollow(TriangleMeshVertex* follow) {
    _follow = follow;
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
        SPARSE_MATRIX*, LARGE_VECTOR*) {

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
        double time, SPARSE_MATRIX*, LARGE_VECTOR*) {

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
}
void VertexToAnimatedEllipseConstraint::applyConstraintToSolverMatrices(
        double time, SPARSE_MATRIX*, LARGE_VECTOR*) {
    //Update the sparse matrices to honor this constraint
}

