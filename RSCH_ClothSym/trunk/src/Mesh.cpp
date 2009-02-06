/*
 * Mesh.cpp
 *
 *  Created on: Sep 17, 2008
 *      Author: njoubert
 */

#include "Mesh.h"

/****************************************************************
*                                                               *
*               TriangleMeshVertex                              *
*                                                               *
****************************************************************/

TriangleMeshVertex::TriangleMeshVertex(double x, double y, double z, int i):
    F(0,0,0), X(x,y,z), U(x,y,z),vX(0,0,0), _index(i), edges() {
    _fixed = false, _dynamicConstIndex = -1;

}

vec3 & TriangleMeshVertex::getX() { return X; }

vec3 & TriangleMeshVertex::getU() { return U; }

vec3 & TriangleMeshVertex::getvX() { return vX; }

double TriangleMeshVertex::getm() { return m; }

void TriangleMeshVertex::setm(double mass) { m = mass; }

int TriangleMeshVertex::getIndex() { return _index; }

vec3 TriangleMeshVertex::getNormal() {
    std::vector<TriangleMeshEdge*>::const_iterator it =
        getEdgesBeginIterator();
    vec3 n(0,0,0);
    while (it != getEdgesEndIterator()) {
        n += (*it)->getParentTriangle(0)->getNormal();
        if ((*it)->isPartOfTwoTriangles())
            n += (*it)->getParentTriangle(1)->getNormal();
        it++;
    }
    return n.normalize();
}

bool & TriangleMeshVertex::getConstaint() { return _fixed; }

void TriangleMeshVertex::setConstraint(bool fixed) { _fixed = fixed; }

int TriangleMeshVertex::getDynamicConstraint() { return _dynamicConstIndex; }

void TriangleMeshVertex::setDynamicConstraint(int EllipsoidIndex) { _dynamicConstIndex = EllipsoidIndex; }

vec3 & TriangleMeshVertex::getF() { return F; }

void TriangleMeshVertex::setF(vec3 f) { F += f; }

void TriangleMeshVertex::clearF() { F = vec3(0); }

void TriangleMeshVertex::addToEdge(TriangleMeshEdge* edge) {
    edges.push_back(edge);
}

int TriangleMeshVertex::edgesSize() {
    return edges.size();
}

std::vector<TriangleMeshEdge*>::const_iterator TriangleMeshVertex::getEdgesBeginIterator() {
    return edges.begin();
}
std::vector<TriangleMeshEdge*>::const_iterator TriangleMeshVertex::getEdgesEndIterator() {
    return edges.end();
}
/****************************************************************
*                                                               *
*               TriangleMeshEdge                                *
*                                                               *
****************************************************************/

TriangleMeshEdge::TriangleMeshEdge(TriangleMesh* callingMesh, int v1,int v2) {
    TriangleMeshVertex *vt1, *vt2;
    vt1 = callingMesh->getVertex(v1);
    vt2 = callingMesh->getVertex(v2);
    vt1->addToEdge(this);
    vt2->addToEdge(this);
    vertices[0] = vt1;
    vertices[1] = vt2;
    triangles[0] = triangles[1] = NULL;
    rl = (vertices[0]->getU() - vertices[1]->getU()).length();
}

double TriangleMeshEdge::getRestLength() { return rl; }

double TriangleMeshEdge::getRestAngle() { return theta0; }

void TriangleMeshEdge::setRestAngle(double theta) { this->theta0 = theta; }

bool TriangleMeshEdge::isValid() {
    return ((triangles[0] != NULL) || (triangles[1] != NULL)) &&
        ((vertices[0] != NULL) && (vertices[1] != NULL));
}

int TriangleMeshEdge::addParentTriangle(TriangleMeshTriangle* parent) {
    if (triangles[0] == NULL) {
        triangles[0] = parent;
        return 0;
    } else if (triangles[1] == NULL) {
        triangles[1] = parent;
        return 1;
    } else
        return -1;
}

bool TriangleMeshEdge::isPartOfTwoTriangles() {
    meshDebug("checking for triangles...")
    return (triangles[0] != NULL && triangles[1] != NULL);
}

TriangleMeshTriangle* TriangleMeshEdge::getParentTriangle(int i) {
    if (i < 2 && i >= 0)
        return triangles[i];
    return NULL;
}

TriangleMeshTriangle* TriangleMeshEdge::getOtherTriangle(TriangleMeshTriangle* one) {
    if (triangles[0] == one)
        return triangles[1];
    return triangles[0];
}

TriangleMeshVertex* TriangleMeshEdge::getVertex(int i) {
    if (i < 2 && i >= 0)
        return vertices[i];
    return NULL;
}

TriangleMeshVertex* TriangleMeshEdge::getOtherVertex(TriangleMeshVertex* one) {
    if (vertices[0] == one)
        return vertices[1];
    return vertices[0];
}

bool TriangleMeshEdge::setParentTriangle(int i,
        TriangleMeshTriangle* parent) {
    if (i < 2 && i >= 0) {
        triangles[i] = parent;
        return true;
    }
    return false;
}


/****************************************************************
*                                                               *
*               TriangleMeshTriangle                            *
*                                                               *
****************************************************************/

TriangleMeshTriangle::TriangleMeshTriangle(TriangleMesh* callingMesh,
        int v1,
        int v2,
        int v3) {
    myParent = callingMesh;

    assert(myParent != NULL);

    vertices[0] = callingMesh->getVertex(v1);
    vertices[1] = callingMesh->getVertex(v2);
    vertices[2] = callingMesh->getVertex(v3);

    assert(vertices[0] != NULL);
    assert(vertices[1] != NULL);
    assert(vertices[2] != NULL);

    edges[0] = callingMesh->getEdgeBetweenVertices(v1,v2);
    edges[1] = callingMesh->getEdgeBetweenVertices(v1,v3);
    edges[2] = callingMesh->getEdgeBetweenVertices(v2,v3);

    assert(edges[0] != NULL);
    assert(edges[1] != NULL);
    assert(edges[2] != NULL);

    meshDebug("Found edge 0: " << edges[0]);
    meshDebug("Found edge 1: " << edges[1]);
    meshDebug("Found edge 2: " << edges[2]);

    if ((edges[0]->addParentTriangle(this) < 0) ||
        (edges[1]->addParentTriangle(this) < 0) ||
        (edges[2]->addParentTriangle(this) < 0))
        std::cout << __FILE__ <<":"<<__LINE__<< "COULDN'T ADD TRIANGLE TO EDGE!" << std::endl;

}

bool TriangleMeshTriangle::isValid() {
    return (edges[0] != NULL && edges[0]->isValid()) &&
    (edges[1] != NULL && edges[1]->isValid()) &&
    (edges[2] != NULL && edges[2]->isValid());
}

vec3 TriangleMeshTriangle::getNormal() {
    vec3 ab = vertices[1]->getX() - vertices[0]->getX();
    vec3 ac = vertices[2]->getX() - vertices[0]->getX();
    vec3 cross = (ab^ac).normalize();
    return cross;
}

TriangleMeshEdge** TriangleMeshTriangle::getEdges() {
    return edges;
}

TriangleMeshVertex** TriangleMeshTriangle::getVertices() {
    return vertices;
}

ostream& operator <<(ostream& s, const TriangleMeshVertex* v) {
    s << "(";
    s << v->U[0];
    s << ",";
    s << v->U[1];
    s << ",";
    s << v->U[2];
    s << ")";
    return s;
}

ostream& operator <<(ostream& s, const TriangleMeshEdge* e) {
    s << "[";
    s << e->vertices[0] << "," << e->vertices[1];
    s << "]";
    return s;
}

ostream& operator <<(ostream& s, const TriangleMeshTriangle* t) {
    s << "{";
    s << " E=" << t->edges[0] << t->edges[1] << t->edges[2];
    s << " V=" << t->vertices[0] << t->vertices[1] << t->vertices[2];
    s << "}";
    return s;
}

/*
bool operator <(TriangleEdgeKey& first, TriangleEdgeKey& second) {
    cout << "WEEEEEEEEEEEEEEEEEEEEEEEE" << endl;
    return false;
}
*/
/****************************************************************
*                                                               *
*               TriangleMesh                                    *
*                                                               *
****************************************************************/


TriangleMesh::TriangleMesh() {
    // TODO Auto-generated constructor stub
}

TriangleMesh::~TriangleMesh() {
	for (unsigned int i = 0; i < triangles.size(); i++)
		delete triangles[i];
	for (unsigned int i = 0; i < vertices.size(); i++) {
		delete vertices[i].first;
		for (unsigned int j = 0; j < vertices[i].second->size(); j++)
			delete (*vertices[i].second)[j].second;
	}
}

int TriangleMesh::createVertex(double x, double y, double z) {
    TriangleMeshVertex* newV = new TriangleMeshVertex(x,y,z, vertices.size());
    vertices.push_back(make_pair(newV,
            new vector<std::pair< int, TriangleMeshEdge*> >()));
    return vertices.size() - 1;
}

/**
 * This function creates a new triangle given vertice
 * indices. This is here to help parse .OBJ files.
 *
 * Input:
 *      A
 *     / \
 *    e1  e2
 *   /      \
 *  B---e3---C
 *
 * This runs the following algorithm:
 * //Get the given vertices
 *       //If they don't exist, error out
 *   //Get the 3 edges between these vectors
 *       //If any does not exist
 *           //Create them and add them
 *       //If any of them are already full - error out
 *   //Create the triangle
 *       //(This will take in given vertices and get edges.)
 * @return index of new triangle in triangle vector.
 */
int TriangleMesh::createTriangle(int A, int B, int C) {
    if (A > (int) vertices.size() ||
            B > (int) vertices.size() ||
            C > (int) vertices.size())
        return -1;

    TriangleMeshVertex *vp1, *vp2, *vp3;
    TriangleMeshEdge *ep1, *ep2, *ep3;
    vp1 = vertices[A].first;
    vp2 = vertices[B].first;
    vp3 = vertices[C].first;

    meshDebug("Vertices are okay. Starting Edge 1.");

    if (NULL == (ep1 = getEdgeBetweenVertices(A,B))) {
        ep1 = new TriangleMeshEdge(this, A,B);
        assert(insertEdgeForVertices(A,B,ep1));
    }

    meshDebug("Edge 1 is okay: "<< (int) ep1<<". Starting Edge 2.");

    if (NULL == (ep2 = getEdgeBetweenVertices(A,C))) {
        ep2 = new TriangleMeshEdge(this, A,C);
        assert(insertEdgeForVertices(A,C,ep2));
    }

    meshDebug("Edge 2 is okay. Starting Edge 3.");

    if (NULL == (ep3 = getEdgeBetweenVertices(B,C))) {
        ep3 = new TriangleMeshEdge(this, B,C);
        assert(insertEdgeForVertices(B,C,ep3));
    }

    meshDebug("Edge 3 is okay. Checking edge triangles...");

    if (ep1->isPartOfTwoTriangles() ||
            ep2->isPartOfTwoTriangles() ||
            ep3->isPartOfTwoTriangles())
        return -1;  //Cannot add a triangle if an edge already has 2 triangles.

    meshDebug("Edges are good!");

    TriangleMeshTriangle *t = new TriangleMeshTriangle(this,
            A, B, C);
    triangles.push_back(t);
    return triangles.size() - 1;
}

TriangleMeshTriangle* TriangleMesh::getTriangle(int i) {
    if (i < 0 || i > (int) triangles.size()-1)
        return NULL;
    return triangles[i];
}

TriangleMeshVertex* TriangleMesh::getVertex(int i) {
    if (i < 0 || i > (int) vertices.size()-1)
        return NULL;
#ifdef CATCHERRORS
    assert(i == vertices[i].first->getIndex());
#endif
    return vertices[i].first;
}

int TriangleMesh::countVertices() {
    return vertices.size();
}

int TriangleMesh::countTriangles() {
    return triangles.size();
}

void TriangleMesh::setGlobalMassPerParticle(double m) {
    cout << "Setting mass per particle = " << m << " over " << vertices.size() << " vertices." << endl;
    TriangleMeshVertex *v;
    for (unsigned int i = 0; i < vertices.size(); i++) {
        v = getVertex(i);
        v->setm(m);
    }
}

TriangleMeshEdge* TriangleMesh::getEdgeBetweenVertices(
        int v1,
        int v2) {
    assert(v1 >= 0 && v1 < (int)vertices.size());
    assert(v2 >= 0 && v2 < (int)vertices.size());
    TriangleMeshVertex *vt1, *vt2;
    vt1 = vertices[v1].first;
    vt2 = vertices[v2].first;
    applyNaturalOrdering(&vt1,&vt2,&v1,&v2);

    TriangleMeshEdge *ret = NULL;

    std::vector< std::pair <int, TriangleMeshEdge* > >::const_iterator it =
        vertices[v1].second->begin();
    while (it != vertices[v1].second->end()) {
        if ((*it).first == v2) {
            ret = (*it).second;
            meshDebug("Found edge!");
            break;
        }
        it++;
    }
    return ret;
}

bool TriangleMesh::insertEdgeForVertices(int v1,
        int v2,
        TriangleMeshEdge* e) {
    assert(v1 >= 0 && v1 < (int)vertices.size());
    assert(v2 >= 0 && v2 < (int)vertices.size());
    TriangleMeshVertex* vt1 = vertices[v1].first;
    TriangleMeshVertex* vt2 = vertices[v2].first;
    applyNaturalOrdering(&vt1,&vt2, &v1,&v2);

    meshDebug("Natural ordering applied...");

    //DEBUG PURPOSES:
    std::vector< std::pair <int, TriangleMeshEdge* > >::const_iterator it =
        vertices[v1].second->begin();
    while (it != vertices[v1].second->end()) {
        if ((*it).first == v2) {
            meshDebug("Edge already defined!!! WTF!!!");
            assert((*it).first != v2);
            return false;
        }
        it++;
    }
    //END DEBUG

    vertices[v1].second->push_back(make_pair(v2, e));
    meshDebug("Inserted edge into vertices-edges datastructure");
    //if insert fails
    return true;
}

EdgesIterator TriangleMesh::getEdgesIterator() {
    return EdgesIterator(this);
}

std::vector<TriangleMeshTriangle*>::const_iterator TriangleMesh::getTrianglesBeginIterator() {
    return triangles.begin();
}

void TriangleMesh::exportAsOBJ(string filename) {
    ofstream outfile (filename.c_str());

    //Iterate over vertices
    TriangleMeshVertex* v;
    for (int i = 0; i < countVertices(); i++) {
        v = getVertex(i);
        outfile << "v " << v->getX()[0] << " " << v->getX()[1] << " " << v->getX()[2] << endl;
    }
    outfile << "# FACES FOLLOW" << endl;

    //Iterate over faces (triangles).
    TriangleMeshTriangle* t;
    for (int i = 0; i < countTriangles(); i++) {
        t = getTriangle(i);
        outfile << "f " << t->getVertices()[0]->getIndex()+1 << " "
            << t->getVertices()[1]->getIndex()+1 << " "
            << t->getVertices()[2]->getIndex()+1 << endl;
    }

    outfile.close();

}

std::vector<TriangleMeshTriangle*>::const_iterator TriangleMesh::getTrianglesEndIterator() {
    return triangles.end();
}

/**
 * Enforces a natural ordering:
 * x[0] < y[0]
 *  or x[0] == y[0] and x[1] < y[1]
 *  or x[0] == y[0] and x[1] == y[1] and x[2] < y[2]
 * @param v1
 * @param v2
 */
void TriangleMesh::applyNaturalOrdering(TriangleMeshVertex** vt1,
        TriangleMeshVertex** vt2, int* v1, int* v2) {
    bool swap = false;

    //First Coordinate:
    if ((*vt1)->getU()[0] < (*vt2)->getU()[0]) {
        return;
    } else {
        if ((*vt1)->getU()[0] == (*vt2)->getU()[0]) {

            //Second Coordinate:
            if ((*vt1)->getU()[1] < (*vt2)->getU()[1]) {
                return;
            } else {
                if ((*vt1)->getU()[1] == (*vt2)->getU()[1]) {

                    //Third Coordinate:
                    if ((*vt1)->getU()[2] < (*vt2)->getU()[2]) {
                        return;
                    } else {

                        if ((*vt1)->getU()[2] == (*vt2)->getU()[2]) {
                            return; //Two of the same vectors...
                        } else {
                            swap = true;
                        }
                    }

                } else {
                    swap = true;
                }
            }

        } else {
            swap = true;
        }
    }
    if (swap) {
        TriangleMeshVertex* tempP;
        tempP = *vt1;
        *vt1 = *vt2;
        *vt2 = tempP;

        int tempI;
        tempI = *v1;
        *v1 = *v2;
        *v2 = tempI;
    }
}

void printVertex(TriangleMesh * m, int i) {
    TriangleMeshVertex* t = m->getVertex(i);
    std::cout << "Point " << i;
    cout << " at " << t->getX();
    cout << " with " << t->edgesSize() << " edges locally and ";
/*        << m->vertices[i].second->size() << " edges globally.";
       std::vector< std::pair <int, TriangleMeshEdge* > >::const_iterator it =
                    m->vertices[i].second->begin();
        while (it != m->vertices[i].second->end()) {
            cout << (*it).second;
            it++;
        }
        */
    cout << endl;

}

