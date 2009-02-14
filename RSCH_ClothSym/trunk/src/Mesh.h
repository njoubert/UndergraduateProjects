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

#include <vector>
#include <iostream>
#include "global.h"
#include "assert.h"

using namespace std;

//#define meshDebug(A) std::cout << __FILE__ << "::" << __LINE__ << "::" << __FUNCTION__ << ":: " << A << std::endl;
#define meshDebug(A) ;

class TriangleMesh;
class TriangleMeshTriangle;
class TriangleMeshEdge;
class TriangleMeshVertex;
class EdgesIterator;

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
    TriangleMeshVertex(double x, double y, double z, int i);
    void addToEdge(TriangleMeshEdge* edge);
    vec3 & getX();
    vec3 & getU();
    vec3 & getvX();
    double getm();
    vec3 getNormalAtCollisionPt();
    void setNormalAtCollisionPt(vec3);
    vec3 getMeshDiff();
    void setMeshDiff(vec3);
    bool & detectedCollision();

    int getIndex();

    void setm(double);
    vec3 getNormal();
    bool & getConstaint();
    void setConstraint(bool);
    int getDynamicConstraint();
    void setDynamicConstraint(int);
    vec3 & getF();
    void setF(vec3);
    void clearF();

    friend ostream& operator <<(ostream&, const TriangleMeshVertex*);

    int edgesSize();
    std::vector<TriangleMeshEdge*>::const_iterator getEdgesBeginIterator();
    std::vector<TriangleMeshEdge*>::const_iterator getEdgesEndIterator();

private:
    bool _fixed;
    int _dynamicConstIndex;
    vec3 F;		//Force
    vec3 X;     //Position
    vec3 U;     //Original Position
    vec3 vX;    //Velocity
    double m;
    int _index;
    std::vector<TriangleMeshEdge*> edges; //Parents. Can have many edges per vertex.

    //For Drawing Stuff
    bool _detectedCollision;
    vec3 _normalAtCollisionPt;
    vec3 _meshDiff;
};

/**
 * Stores the metadata about which vertices make an edge.
 * This is stored according to the vertices' Natural Order
 * 2 triangles can share an edge.
 * each edge has 2 vertices.
 */
class TriangleMeshEdge {
public:
    TriangleMeshEdge(TriangleMesh*, int, int);
    /**
     * @return true if it has two legal vertices and at least one non-null parent.
     */
    bool isValid();
    /**
     * Adds the given triangle as one of the two parents to this edge
     * ONLY IF there arent already two parents. In that case, return -1
     * @return -1 if fail, else the indice of the given parent in this edge.
     */
    double getRestLength();
    double getRestAngle();
    void setRestAngle(double);
    int addParentTriangle(TriangleMeshTriangle*);
    bool isPartOfTwoTriangles();
    bool setParentTriangle(int, TriangleMeshTriangle*);
    TriangleMeshTriangle* getParentTriangle(int);
    TriangleMeshTriangle* getOtherTriangle(TriangleMeshTriangle*);
    TriangleMeshVertex* getVertex(int);
    TriangleMeshVertex* getOtherVertex(TriangleMeshVertex*);
    friend ostream& operator <<(ostream& s, const TriangleMeshEdge* e);

private:
    TriangleMeshTriangle* triangles[2];   //PARENTS
    TriangleMeshVertex* vertices[2];    //CHILDREN
    float rl;							//Rest Length
    float theta0;						//Rest Angle
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
    TriangleMeshTriangle(TriangleMesh* callingMesh, int, int, int);
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

    TriangleMeshTriangle* getTriangle(int i);
    TriangleMeshVertex* getVertex(int i);
    int countVertices();
    int countTriangles();
    void setGlobalMassPerParticle(double);

    /**
     * @return NULL if none exists, else pointer to edge object.
     */
    TriangleMeshEdge* getEdgeBetweenVertices(int,int);

    bool insertEdgeForVertices(int, int, TriangleMeshEdge* e);

    //TODO: Iterater over all vertices

    EdgesIterator getEdgesIterator();

    std::vector<TriangleMeshTriangle*>::const_iterator getTrianglesBeginIterator();
    std::vector<TriangleMeshTriangle*>::const_iterator getTrianglesEndIterator();

    void exportAsOBJ(string filename);

public:
    void applyNaturalOrdering(TriangleMeshVertex** v1, TriangleMeshVertex** v2, int*, int*);

public:
    //TODO: Write a comment on how the hell this datastructure actually looks...
    /**
     *
     */
    std::vector< TriangleMeshTriangle* > triangles;
    std::vector< std::pair < TriangleMeshVertex*, std::vector< std::pair< int, TriangleMeshEdge* > > * > > vertices;

};

/**
 * The EdgesIterator allow you to iterate over all the edges in the mesh.
 * This does not follow the usual C++ iterator pattern in C++, but rather a modification
 * of the Java pattern, since it is easier to implement.
 *
 * Example code:
 *
 *    EdgesIterator edg_it = myMesh.getEdgesIterator();
 *    do {
 *       cout << "  edge " << (*edg_it) << endl;
 *    } while (edg_it.next());
 *
 */
class EdgesIterator {
public:
    EdgesIterator(TriangleMesh* mesh) {
        this->mesh = mesh;
        findFirst();
    }
    bool next() {
        return findNext();
    }

    TriangleMeshEdge* operator*() const { return _current; }
    TriangleMeshEdge* operator->() const { return _current; }

private:
    bool findFirst() {
        _currentVertex = _currentEdgeOnVertex = 0;
        while (mesh->vertices[_currentVertex].second->empty() &&
                    _currentVertex < mesh->vertices.size()) {
            _currentVertex++;
        }
        if (mesh->vertices[_currentVertex].second->empty())
            return false; //Couldnt find any edges...
        _current = mesh->vertices[_currentVertex].second->at(_currentEdgeOnVertex).second;
        return true;
    }

    bool findNext() {
        if (_currentVertex >= mesh->vertices.size())
            return false;
        if (_currentEdgeOnVertex < mesh->vertices[_currentVertex].second->size()-1) {
            _currentEdgeOnVertex = _currentEdgeOnVertex + 1;
            //cout << "Moving to next edge on same vertex..." << _currentEdgeOnVertex << endl;

        } else if (_currentVertex < mesh->vertices.size()-1) {
            _currentEdgeOnVertex = 0;
            //cout << "Moving to next vertex..." << _currentVertex << " to ";
            do  {
                _currentVertex++;
            } while (_currentVertex < mesh->vertices.size() &&
                    mesh->vertices[_currentVertex].second->empty());
            //cout << _currentVertex << endl;
            if (_currentVertex >= mesh->vertices.size() ||
                    mesh->vertices[_currentVertex].second->empty())
                return false; //Couldnt find any edges...
        } else {
            return false;
        }
        _current = mesh->vertices[_currentVertex].second->at(_currentEdgeOnVertex).second;
        return true;

    }
private:
    TriangleMesh* mesh;
    TriangleMeshEdge* _current;
    unsigned int _currentVertex;
    unsigned int _currentEdgeOnVertex;
};

void printVertex(TriangleMesh*, int);

#endif /* MESH_H_ */
