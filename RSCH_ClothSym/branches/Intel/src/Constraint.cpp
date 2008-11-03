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

void Constraint::setCollisionMesh(TriangleMesh* mesh) {
    _mesh = mesh;
}

void Constraint::setCollisionEllipsoids(AniElliModel* ellipsoids) {
    _ellipsoids = ellipsoids;
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
	origin[1] = 1.3;
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

void VertexToAnimatedEllipseConstraint::applyCollisionToMesh(Physics_LargeVector* X, Physics_LargeVector* V){
	for(int i = 0; i < _mesh->countVertices(); i++) {
		for(int j = 0; j < _ellipsoids->getSize(); j++) {
			mat4 elliTransform = _ellipsoids->getEllipsoid(j);
			TriangleMeshVertex* Vert = _mesh->getVertex(i);

			vec4 v;
			for(int k = 0; k < 3; k++)
				v[k] = Vert->getX()[k];
			v[3] = 1;
			//cout<<"v : "<<v<<endl;
			//cout<<"elliTransform: "<<endl<<elliTransform<<endl;
			vec4 vT = v * elliTransform.inverse();
			//cout<<"vT: "<<vT<<endl;
			double L = sqrt(vT[0]*vT[0] + vT[1]*vT[1] + vT[2]*vT[2]);
			//cout<<"L : "<<L<<endl<<endl;
			if(L < 1) {
				//cout<<"colision detected"<<endl;
				//vT.normalize();
				vec3 vT_3;
				for(int k = 0; k < 3; k++)
					vT_3[k] = vT[k];

				vT_3 = vT_3/sqrt(vT_3[0]*vT_3[0] + vT_3[1]*vT_3[1] + vT_3[2]*vT_3[2]);

				for(int k = 0; k < 3; k++)
					vT[k] = vT_3[k];
				vT[3] = 1;

				v = vT * elliTransform;

				X->m_pData[Vert->getID()].x = v[0];
				X->m_pData[Vert->getID()].y = v[1];
				X->m_pData[Vert->getID()].z = v[2];

				V->m_pData[Vert->getID()].x = -.1 * V->m_pData[Vert->getID()].x;
				V->m_pData[Vert->getID()].y = -.1 * V->m_pData[Vert->getID()].y;
				V->m_pData[Vert->getID()].z = -.1 * V->m_pData[Vert->getID()].z;
			}
		}
	}
}

