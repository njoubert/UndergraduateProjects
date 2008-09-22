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

TriangleMeshVertex::TriangleMeshVertex(double x, double y, double z):
        X(x,y,z), U(x,y,z), vX(0,0,0), f(0,0,0), edges() { /* Done */ }


vec3 & TriangleMeshVertex::getX() { return X; }

vec3 & TriangleMeshVertex::getU() { return U; }

vec3 & TriangleMeshVertex::getvX() { return vX; }

vec3 & TriangleMeshVertex::getf() { return f; }

void TriangleMeshVertex::addToEdge(TriangleMeshEdge* edge) {
    edges.push_back(edge);
}

/****************************************************************
*                                                               *
*               TriangleMeshEdge                                *
*                                                               *
****************************************************************/

TriangleMeshEdge::TriangleMeshEdge(
        TriangleMeshVertex* v1,
        TriangleMeshVertex* v2) {
    v1->addToEdge(this);
    v2->addToEdge(this);
    vertices[0] = v1;
    vertices[1] = v2;
    triangles[0] = triangles[1] = NULL;
}

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
        TriangleMeshVertex* a,
        TriangleMeshVertex* b,
        TriangleMeshVertex* c) {
    myParent = callingMesh;

    assert(myParent != NULL);

    vertices[0] = a;
    vertices[1] = b;
    vertices[2] = c;

    assert(vertices[0] != NULL);
    assert(vertices[1] != NULL);
    assert(vertices[2] != NULL);

    edges[0] = callingMesh->getEdgeFromMap(a,b);
    edges[1] = callingMesh->getEdgeFromMap(a,c);
    edges[2] = callingMesh->getEdgeFromMap(b,c);

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
    //FILL THIS IN!
    return vec3(0,0,0);
}

TriangleMeshEdge** TriangleMeshTriangle::getEdges() {
    return edges;
}

TriangleMeshVertex** TriangleMeshTriangle::getVertices() {
    return vertices;
}

ostream& operator <<(ostream& s, const edgeKey &obj) {
    s << "(" ;
    s << obj.first;
    s << ",";
    s << obj.second;
    s << ")";
    return s;
}

ostream& operator <<(ostream& s, const TriangleMeshVertex* v) {
    s << "(";
    s << v->X[0];
    s << ",";
    s << v->X[1];
    s << ",";
    s << v->X[2];
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
    // TODO Auto-generated destructor stub
}

int TriangleMesh::createVertex(double x, double y, double z) {
    TriangleMeshVertex* newV = new TriangleMeshVertex(x,y,z);
    vertices.push_back(newV);
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

    meshDebug("Starting to create triangle.")

    TriangleMeshVertex *vp1, *vp2, *vp3;
    TriangleMeshEdge *ep1, *ep2, *ep3;
    vp1 = vertices[A];
    vp2 = vertices[B];
    vp3 = vertices[C];

    edgeKey e1(vp1,vp2);

    if (edgesMap.count(e1)) {
       ep1 = edgesMap[e1];
       meshDebug("Found edge v1--v2: " << ep1);
    } else {
        ep1 = new TriangleMeshEdge(vp1, vp2);
        edgesMap[e1] = ep1;
        meshDebug("Created edge v1--v2: " << ep1);
    }

    cout << "YOOO!" << endl;

    if (edgesMap.count(edgeKey(vp1,vp3))) {
        ep2 = edgesMap[edgeKey(vp1,vp3)];
        meshDebug("Found edge v1--v3: " << ep2);
    } else {
        ep2 = new TriangleMeshEdge(vp1, vp3);
        edgesMap[edgeKey(vp1,vp3)] = ep2;
        meshDebug("Created edge v1--v3: " << ep2);
    }

    if (edgesMap.count(edgeKey(vp2,vp3))) {
        ep3 = edgesMap[edgeKey(vp2,vp3)];
        meshDebug("Found edge v2--v3: " << ep3);
    } else {
        ep3 = new TriangleMeshEdge(vp2, vp3);
        edgesMap[edgeKey(vp2,vp3)] = ep3;
        meshDebug("Created edge v2--v3: " << ep3);
    }

    if (ep1->isPartOfTwoTriangles() ||
            ep2->isPartOfTwoTriangles() ||
            ep3->isPartOfTwoTriangles())
        return -1;  //Cannot add a triangle if an edge already has 2 triangles.

    meshDebug("All three edges have a side thats open.");

    assert(edgesMap.count(e1) > 0);
    assert(edgesMap.count(edgeKey(vp1,vp2)) > 0);
    assert(edgesMap.count(edgeKey(vp1,vp3)) > 0);
    assert(edgesMap.count(edgeKey(vp2,vp3)) > 0);

    TriangleMeshTriangle *t = new TriangleMeshTriangle(this,
            vp1, vp2, vp3);
    triangles.push_back(t);
    return triangles.size() - 1;

}

TriangleMeshVertex* TriangleMesh::getVertex(int i) {
    if (i < 0 || i > (int) vertices.size()-1)
        return NULL;
    return vertices[i];
}

TriangleMeshEdge* TriangleMesh::getEdgeFromMap(TriangleMeshVertex* v1, TriangleMeshVertex* v2) {
    meshDebug("getEdgeFromMap called with v1=" << v1 <<" and v2=" << v2);
    edgeKey key(v1,v2);
    if (edgesMap.count(key)) {
        meshDebug("   edge Found!");
        return edgesMap[key];
    }
    meshDebug("   edge NOT FOUND!");
    return NULL;
}

TriangleMeshTriangle* TriangleMesh::getTriangle(int i) {
    if (i < 0 || i > (int) triangles.size()-1)
        return NULL;
    return triangles[i];
}
