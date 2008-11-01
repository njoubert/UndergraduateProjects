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
		SparseMatrix* A, Physics_LargeVector* b) {

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
        double time, SparseMatrix* A, Physics_LargeVector* b) {

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
//*
	//Get Ellipsoid Position From Ellipsoid Transformation
	vec4 origin(0);
	origin[3] = 1;
	int index = _leadIndex;
	mat4 ElliTransform(0);
	ElliTransform = _lead->getEllipsoid(index);
	vec4 ElliCenter = origin * ElliTransform;
	//Set Follow Position to Ellipsoid Position (converts from vec4 -> vec3)
	//cout<<"Constraining Follow Vertex "<<_follow->getX()<<"the lead vertex "<<ElliCenter<<endl;
	for(int i = 0; i < 3; i++)
		_follow->getX()[i] = ElliCenter[i];
	//cout<<"Follow Vertex is now "<<_follow->getX()<<endl;
//*/
}
void VertexToAnimatedEllipseConstraint::applyConstraintToSolverMatrices(
        double time, SparseMatrix* m_A, Physics_LargeVector* m_b) {
    //Update the sparse matrices to honor this constraint

	int cVertex = _follow->getID();

	for (int i = 0; i < m_A->m_iRows; i++) {
		for (int j = 0; j < 9; j++) {
			(*m_A)(cVertex, i).m_Mx[j] = 0;
		}
	}
	for (int i = 0; i < 9; i++) {
		(*m_A)(cVertex, cVertex).m_Mx[i] = 1;
	}
	for (int p = 0; p < m_b->m_iElements; p++) {
		m_b->m_pData[cVertex].x = 0;
		m_b->m_pData[cVertex].y = 0;
		m_b->m_pData[cVertex].z = 0;
	}

}


