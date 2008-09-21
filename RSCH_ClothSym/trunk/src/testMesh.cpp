/*
 * testMesh.cpp
 *
 *  Created on: Sep 20, 2008
 *      Author: njoubert
 */

#include <iostream>
#include "Mesh.h"

using namespace std;


void printStateOfMesh(TriangleMesh &myMesh) {

    cout << "************************************" << endl;

    /*************************************
     *  VERTICES
     * ***********************************/

    vector<TriangleMeshVertex*>::const_iterator vert_it =
        myMesh.vertices.begin();

    cout << "Vertices size=" << myMesh.vertices.size();
    cout << " contains:" << endl;
    while (vert_it != myMesh.vertices.end()) {
        cout << " " << *vert_it;
        vert_it++;
    }
    cout << endl;

    /*************************************
     *  EDGES
     * ***********************************/

    map< edgeKey, edgeValue >::const_iterator map_it =
            myMesh.edgesMap.begin();
    cout << "Edges size=" << myMesh.edgesMap.size();
    cout << " contains:" << endl;
    while (map_it != myMesh.edgesMap.end()) {
        cout << "  " << map_it->first << " maps to " << map_it->second << endl;
        map_it++;
    }

    /*************************************
     *  TRIANGLES
     * ***********************************/

    vector< TriangleMeshTriangle* >::const_iterator tri_it =
            myMesh.triangles.begin();
    cout << "Triangles size=" << myMesh.triangles.size() << " contains:" << endl;
    while (tri_it != myMesh.triangles.end()) {
        cout << "  (" << (*tri_it)->getEdges()[0] << "," << (*tri_it)->getEdges()[1] << "," << (*tri_it)->getEdges()[2] << ")" << endl;
        tri_it++;
    }

    cout << "************************************" << endl;

}

bool testKeys() {
    bool retval = true;

    TriangleMesh myMesh;
    int v1 = myMesh.createVertex(0,0,0);
    int v4 = myMesh.createVertex(0,0,0);

    int v2 = myMesh.createVertex(10,0,0);
    int v5 = myMesh.createVertex(10,0,0);

    int v3 = myMesh.createVertex(0,10,0);
    int v6 = myMesh.createVertex(0,10,0);

    edgeKey e1 = edgeKey(myMesh.vertices[v1], myMesh.vertices[v2]);
    edgeKey e1p = edgeKey(myMesh.vertices[v2], myMesh.vertices[v1]);

    edgeKey e2 = edgeKey(myMesh.vertices[v1], myMesh.vertices[v3]);
    edgeKey e2p = edgeKey(myMesh.vertices[v3], myMesh.vertices[v1]);

    edgeKey e3 = edgeKey(myMesh.vertices[v2], myMesh.vertices[v3]);
    edgeKey e3p = edgeKey(myMesh.vertices[v3], myMesh.vertices[v2]);

    if ((myMesh.vertices[v1] < myMesh.vertices[v2])) {
        cout << __LINE__ << ": a < a" << endl;
        retval = false;
    }

    bool AlessthanB = false;

    if (myMesh.vertices[v1] < myMesh.vertices[v2])
        AlessthanB = true;
    else
        AlessthanB = false;

    if (myMesh.vertices[v2] < myMesh.vertices[v1]) {
        if (AlessthanB) {
            cout << __LINE__ << ": b<a and a<b" << endl;
            retval = false;
        }
    } else {
        if (!AlessthanB) {
            cout << __LINE__ << ": b!<a and a!<b" << endl;
            retval = false;
        }
    }

    if (!(e1 == e1p)) {
        cout << __LINE__ << ": (a,b) != (b,a)" << endl;
        retval = false;
    }

    if (!(e2 == e2p)) {
        cout << __LINE__ << ": (a,c) == (c,a)" << endl;
        retval = false;
    }

    if (!(e3 == e3p)) {
        cout << __LINE__ << ": (b,c) != (c,b)" << endl;
        retval = false;
    }

    /*******************************
     * Testing e1 e2 comparisons...
     *******************************/

    if (!(e1 != e2)) {
        cout << __LINE__ << ": (a,b) == (b,c)" << endl;
        retval = false;
    }
    if ((e1 == e2)) {
        cout << __LINE__ << ": (a,b) == (b,c)" << endl;
        retval = false;
    }
    if ((!(e2 < e1)) && (!(e1 < e2))) {
        cout << __LINE__ << ": LESS THAT COMPARISON IS WRONG!" << endl;
        retval = false;
    }

    /*******************************
     * Testing e2 e3 comparisons...
     *******************************/

    if (!(e2 != e3)) {
       cout << __LINE__ << ": (a,c) == (b,c)" << endl;
       retval = false;
    }
    if ((e2 == e3)) {
        cout << __LINE__ << ": (a,c) == (b,c)" << endl;
        retval = false;
    }
    if ((!(e2 < e3)) && (!(e3 < e2))) {
        cout << __LINE__ << ": LESS THAT COMPARISON IS WRONG!" << endl;
        retval = false;
    }

    /*******************************
     * Testing e1 e3 comparisons
     *******************************/
    if (!(e1 != e3)) {
       cout << __LINE__ << ": (a,b) == (b,c)" << endl;;
       retval = false;
    }
    if ((e1 == e3)) {
        cout << __LINE__ << ": (a,b) == (b,c)" << endl;;
        retval = false;
    }
    if ((!(e3 < e1)) && (!(e1 < e3))) {
        cout << __LINE__ << ": LESS THAT COMPARISON IS WRONG!" << endl;
        retval = false;
    }

    return retval;
}

int main() {
    cout << "Testing Mesh Methods:" << endl;

    /*******************************
     * Testing Map - keys!
     *******************************/
    if (!testKeys()) {
        cout << "\x1b[31m Error error! \x1b[0m" << endl;
    }


    TriangleMesh myMesh;
    int v1 = myMesh.createVertex(0,0,0);
    int v2 = myMesh.createVertex(10,0,0);
    int v3 = myMesh.createVertex(0,10,0);
    int t1 = myMesh.createTriangle(v1,v2,v3);

    TriangleMeshTriangle *triangle1 = myMesh.getTriangle(t1);

    int v4 = myMesh.createVertex(10,10,0);
    int t2 = myMesh.createTriangle(v2,v4,v3);

    TriangleMeshTriangle *triangle2 = myMesh.getTriangle(t2);

    int v5 = myMesh.createVertex(5,15,0);
    int t3 = myMesh.createTriangle(v3,v4,v5);

    TriangleMeshTriangle *triangle3 = myMesh.getTriangle(t3);

    printStateOfMesh(myMesh);

    TriangleMeshEdge* v2v3 =
        myMesh.getEdgeFromMap(myMesh.getVertex(v2),
                myMesh.getVertex(v3));

    if (v2v3->isPartOfTwoTriangles())
        cout << "v2v3 is a part of two triangles AS EXPECTED." << endl;

    if (v2v3->getParentTriangle(0) == triangle1) {
        cout << "v2v3 has t1 on side 0." << endl;

        if (!((triangle1->getEdges()[0] == v2v3) ||
                triangle1->getEdges()[1] == v2v3 ||
                triangle1->getEdges()[2] == v2v3))
            cout << "  BUT T1 DOES NOT HAVE V2V3 AS AN EDGE!" << endl;

        if (v2v3->getParentTriangle(1) == triangle2) {
            cout << "  and v2v3 has t2 on side 1." << endl;

            if (!((triangle2->getEdges()[0] == v2v3) ||
                    triangle2->getEdges()[1] == v2v3 ||
                    triangle2->getEdges()[2] == v2v3))
                cout << "  BUT T2 DOES NOT HAVE V2V3 AS AN EDGE!" << endl;

        } else {
            cout << "BUT V2V3 DOES NOT HAVE T2 ON SIDE 1..." << endl;
        }
    } else if (v2v3->getParentTriangle(1) == triangle1) {
        cout << "v2v3 has t1 on side 1." << endl;

        if (!((triangle1->getEdges()[0] == v2v3) ||
                triangle1->getEdges()[1] == v2v3 ||
                triangle1->getEdges()[2] == v2v3))
            cout << "  BUT T1 DOES NOT HAVE V2V3 AS AN EDGE!" << endl;

        if (v2v3->getParentTriangle(0) == triangle2) {
            cout << "  and v2v3 has t2 on side 0." << endl;

            if (!((triangle2->getEdges()[0] == v2v3) ||
                    triangle2->getEdges()[1] == v2v3 ||
                    triangle2->getEdges()[2] == v2v3))
                cout << "  BUT T2 DOES NOT HAVE V2V3 AS AN EDGE!" << endl;

        } else {
            cout << "BUT V2V3 DOES NOT HAVE T2 ON SIDE 0..." << endl;
        }
    }

}
