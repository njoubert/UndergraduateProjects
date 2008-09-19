/*
 * Mesh.h
 *
 *  Created on: Sep 17, 2008
 *      Author: njoubert
 */

#ifndef MESH_H_
#define MESH_H_

#include <vector>
#include "algebra3.h"

using namespace std;

/**
 * Represents a point on the mesh.
 * Contains only information for that point, NOT for its position in the mesh.
 */
class TriangleMeshVertex {
public:
    vec3 X;     //Position
    vec3 vX;    //Velocity
    vec3 f;     //Force
};

/**
 * Represents an arbitrary mesh.
 * Gives you
 */
class TriangleMesh {
public:
    TriangleMesh();
    virtual ~TriangleMesh();
private:
    vector< TriangleMeshVertex > vertices;
    //vector< MeshEdge > edges;
};

#endif /* MESH_H_ */
