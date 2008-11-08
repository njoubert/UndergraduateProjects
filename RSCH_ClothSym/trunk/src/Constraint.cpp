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


/****************************************************************
*                                                               *
*               FixedConstraint                                 *
*                                                               *
****************************************************************/

FixedConstraint::FixedConstraint() {

}

void FixedConstraint::applyConstraintToPoints(LARGE_VECTOR* x, LARGE_VECTOR* v) {
	   //Update the sparse matrices to honor this constraint
}

void FixedConstraint::applyConstraintToSolverMatrices(
        SPARSE_MATRIX* A, LARGE_VECTOR* b) {
    int cVertex = _follow->getIndex();
	A->zeroRowCol(cVertex, cVertex, true);
	(*b)[cVertex] = vec3(0,0,0);
}



/****************************************************************
*                                                               *
*               VertexToAnimatedVertexConstraint                *
*                                                               *
****************************************************************/

VertexToAnimatedVertexConstraint::VertexToAnimatedVertexConstraint() {

}

void VertexToAnimatedVertexConstraint::applyConstraintToPoints(LARGE_VECTOR* x, LARGE_VECTOR* v) {

}

void VertexToAnimatedVertexConstraint::applyConstraintToSolverMatrices(
        SPARSE_MATRIX*, LARGE_VECTOR*) {

}


/****************************************************************
*                                                               *
*               VertexToAnimatedEllipseConstraint               *
*                                                               *
****************************************************************/

VertexToAnimatedEllipseConstraint::VertexToAnimatedEllipseConstraint() {

}

void VertexToAnimatedEllipseConstraint::applyConstraintToPoints(LARGE_VECTOR* x, LARGE_VECTOR* v) {
    //Move the follow's points to the same position as the lead's points.
//Get Ellipsoid Position From Ellipsoid Transformation
	vec4 origin(0);
	origin[3] = 1;
	origin[1] = 1.3;
	int index = _leadIndex;
	mat4 ElliTransform(0);
	ElliTransform = _lead->getEllipsoid(index);
	vec4 ElliCenter = origin * ElliTransform;

	int cVertex = _follow->getIndex();
	(*x)[cVertex] = vec3(ElliCenter, VW);

	//for(int i = 0; i < 3; i++)
	//	_follow->getX()[i] = ElliCenter[i];
	//cout<<"Follow Vertex is now "<<_follow->getX()<<endl;
}
void VertexToAnimatedEllipseConstraint::applyConstraintToSolverMatrices(
        SPARSE_MATRIX* A, LARGE_VECTOR* b) {
    //Update the sparse matrices to honor this constraint

	int cVertex = _follow->getIndex();
    A->zeroRowCol(cVertex, cVertex, true);
    //TODO: Should this be taking the ellipse speed into account?
    (*b)[cVertex] = vec3(0,0,0);

}

