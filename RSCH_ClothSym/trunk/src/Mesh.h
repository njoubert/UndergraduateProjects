/*
 * Mesh.h
 *
 *  Created on: Sep 17, 2008
 *      Author: njoubert
 *
 *
 *      The concept of this mesh is as follows:
 *
 *      The TriangleMesh is a container which stores:
 *          - a collection of TriangleMeshVertex objects
 *              to represent all the points in the mesh
 *          - a collectino of TriangleMeshEdge objects
 *              to represent the connectivity between vertices.
 *          - a collection of TriangleMeshTriangle objects
 *              to represent the collection of edges that
 *              creates a triangle.
 *
 *      *CACHING*
 *      The TriangleMesh allows you to initialize, read and write
 *      information about the mesh, and all of this happens through
 *      function calls so that we can cache data later on if necessary.
 *
 *      *COMPUTATION*
 *      The Mesh does *not* compute derivatives or run solvers. It is
 *      merely a data structure with accessor functions. All computation
 *      is done inside the System class, which will have access to the Mesh.
 *
 *      *DATASTRUCTURE DESIGN*
 *
 *      We need the following operations:
 *          - Iterate over vertices
 *          - Get the edges a vertex takes part in.
 *          - Get the triangles a vertex takes part in.
 *
 *          - Create new mesh (HARDER ONE!)
 *
 */

#ifndef MESH_H_
#define MESH_H_

#include <utility>
#include <ext/hash_map>
#include <vector>
#include <iostream>
#include "algebra3.h"
#include "assert.h"

using namespace std;

#define meshDebug(A) std::cout << A << std::endl;

class TriangleMesh;
class TriangleMeshTriangle;
class TriangleMeshEdge;
class TriangleMeshVertex;
class TriangleEdgeKey;
typedef TriangleMeshEdge* edgeValue;
typedef TriangleEdgeKey edgeKey;
//typedef pair<TriangleMeshVertex*, TriangleMeshVertex*> edgeKey;

ostream& operator <<(ostream& s, const edgeKey &obj);
ostream& operator <<(ostream& s, const TriangleMeshVertex* v);
ostream& operator <<(ostream& s, const TriangleMeshEdge* e);
ostream& operator <<(ostream& s, const TriangleMeshTriangle* t);
/**
 * Represents a point on the mesh.
 * Contains only information for that point in space.
 * Any information related to its position in a triangle should NOT be stored here.
 * Don't expose private data since we might want to cache some things later on.
 */
class TriangleMeshVertex {
public:
    /**
     * Creates a new vertex at position (x,y,z) with no velocity or force on it.
     */
    TriangleMeshVertex(double x, double y, double z);
    void addToEdge(TriangleMeshEdge* edge);
    vec3 & getX();
    vec3 & getU();
    vec3 & getvX();
    vec3 & getf();

    friend ostream& operator <<(ostream&, const TriangleMeshVertex*);
private:
    vec3 X;     //Position
    vec3 U;     //Original Position
    vec3 vX;    //Velocity
    vec3 f;     //Force
    std::vector<TriangleMeshEdge*> edges; //Parents. Can have many edges per vertex.
};


/**
 * Stores the metadata about which vertices make an edge.
 * 2 triangles can share an edge.
 * each edge has 2 vertices.
 */
class TriangleMeshEdge {
public:
    TriangleMeshEdge(
            TriangleMeshVertex* v1,
            TriangleMeshVertex* v2);
    /**
     * @return true if it has two legal vertices and at least one non-null parent.
     */
    bool isValid();
    /**
     * Adds the given triangle as one of the two parents to this edge
     * ONLY IF there arent already two parents. In that case, return -1
     * @return -1 if fail, else the indice of the given parent in this edge.
     */
    int addParentTriangle(TriangleMeshTriangle*);
    bool isPartOfTwoTriangles();
    bool setParentTriangle(int, TriangleMeshTriangle*);
    TriangleMeshTriangle* getParentTriangle(int);
    TriangleMeshTriangle* getOtherTriangle(TriangleMeshTriangle*);

    friend ostream& operator <<(ostream& s, const TriangleMeshEdge* e);
private:
    TriangleMeshTriangle* triangles[2];   //PARENTS
    TriangleMeshVertex* vertices[2];    //CHILDREN
};

/**
 * Represents one triangle in the mesh.
 * Here we store information relevant to the whole triangle only.
 */
class TriangleMeshTriangle {
public:
    /**
     * Creates a new triangle mesh.
     * YOU MUST SPECIFY THE VERTICES IN COUNTERCLOCKWISE ORDER!
     * THE EDGES MUST ALREADY EXIST!
     * This changes how we calculate the normal.
     *    a
     *   / \
     *  b---c
     *
     */
    TriangleMeshTriangle(TriangleMesh* callingMesh,
            TriangleMeshVertex* a,
            TriangleMeshVertex* b,
            TriangleMeshVertex* c);
   /**
    * @return true if it has three legal edges.
    */
    bool isValid();
    vec3 getNormal();
    friend ostream& operator <<(ostream&, const TriangleMeshTriangle*);
    TriangleMeshEdge** getEdges();
    TriangleMeshVertex** getVertices();

private:
    TriangleMesh* myParent;
    TriangleMeshEdge* edges[3];
    TriangleMeshVertex* vertices[3]; //order dictates normal
};



/**
 * For lookup in the edge map.
 */
//*
class TriangleEdgeKey {
public:
    TriangleEdgeKey(TriangleMeshVertex* a, TriangleMeshVertex* b):
        first(a), second(b) { }
public:
    TriangleMeshVertex* first;
    TriangleMeshVertex* second;
};

/*********************************
 * Hashing Operations
 *********************************/
namespace __gnu_cxx {  // Import string operator.
  template<>
  struct hash<std::string> {
  public:
    size_t operator()(std::string str) const {
      __gnu_cxx::hash<char const*> H;
      return H(str.c_str() );
    }
  };

  template<>
  struct hash<TriangleEdgeKey> {
      size_t operator()( const TriangleEdgeKey& key) const {
          vec3 distsq = key.first->getU() - key.second->getU();
          return distsq.length();
      }
  };

  struct eqedge {
      bool operator()(const TriangleEdgeKey & e1, const TriangleEdgeKey & e2) const {
          return ((e1.first == e2.first && e1.second == e2.second) ||
                  (e1.first == e2.second && e1.second == e2.first));
      }
  };
};

typedef __gnu_cxx::hash_map< edgeKey, edgeValue, __gnu_cxx::hash<TriangleEdgeKey>, __gnu_cxx::eqedge > hashmap;

/**
 * Represents an arbitrary mesh.
 */
class TriangleMesh {
public:
    TriangleMesh();
    virtual ~TriangleMesh();


    /**
     * Creates a new vertex with the given position.
     * @return an index for the new vertex.
     */
    int createVertex(double, double, double);
    /**
     * Creates a new triangle with the given vertex indices.
     * @return an index for the triangle in the mesh' triangles vector,
     *          or -1 if we couldn't create this triangle.
     */
    int createTriangle(int A, int B, int C);

    TriangleMeshVertex* getVertex(int i);

    TriangleMeshEdge* getEdgeFromMap(TriangleMeshVertex* v1, TriangleMeshVertex* v2);

    TriangleMeshTriangle* getTriangle(int i);

public:
    std::vector< TriangleMeshVertex* > vertices;
    hashmap edgesMap; //TODO: Possibly use a more efficient map?
    std::vector< TriangleMeshTriangle* > triangles;

};

#endif /* MESH_H_ */
