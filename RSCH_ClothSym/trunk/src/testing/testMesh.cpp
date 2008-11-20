/*
 * testMesh.cpp
 *
 *  Created on: Sep 20, 2008
 *      Author: njoubert
 */

#include <iostream>
#include "../Mesh.h"
#include "../io/OBJParser.h"

using namespace std;

void printStateOfMesh(TriangleMesh &myMesh, bool showInside) {

    cout << "************************************" << endl;

    /*************************************
     *  VERTICES
     * ***********************************/

    std::vector< std::pair < TriangleMeshVertex*, std::vector< std::pair< int, TriangleMeshEdge* > > * > >::const_iterator vert_it =
        myMesh._vertices.begin();

    cout << "Vertices size=" << myMesh._vertices.size();
    if (showInside)
        cout << " contains:" << endl;
    int eCount = 0;
    while (vert_it != myMesh._vertices.end()) {
        if (showInside)
            cout << " " << (*vert_it).first;

        std::vector<TriangleMeshEdge*>::const_iterator vert_ed_it = (*vert_it).first->getEdgesBeginIterator();
        while (vert_ed_it != (*vert_it).first->getEdgesEndIterator()) {
            eCount++;
            vert_ed_it++;
        }
        vert_it++;
    }
    cout << endl;
    cout << eCount << " edges counted for all vertices." << endl;

    /*************************************
     *  EDGES
     * ***********************************/

    EdgesIterator edg_it = myMesh.getEdgesIterator();
    int count = 0;
    if (showInside)
        cout << "TriangleMesh contains edges: " << endl;
    do {
        count++;
        if (showInside)
            cout << "  edge " << (*edg_it) << endl;
    } while (edg_it.next());
    cout << count << " edges detected in mesh." << endl;



    /*************************************
     *  TRIANGLES
     * ***********************************/

    vector< TriangleMeshTriangle* >::const_iterator tri_it =
            myMesh._triangles.begin();
    cout << "Triangles size=" << myMesh._triangles.size();
    if (showInside) {
        cout << " contains:" << endl;
        while (tri_it != myMesh._triangles.end()) {
            cout << "  triangle (" << (*tri_it)->getEdges()[0] << "," << (*tri_it)->getEdges()[1] << "," << (*tri_it)->getEdges()[2] << ")" << endl;
            tri_it++;
        }
    }
    cout << endl;
    cout << "************************************" << endl;

}

/*
int main() {
    cout << "Testing Mesh Methods:" << endl;

    TriangleMesh myMesh;
    int v1 = myMesh.createVertex(0,0,0);
    int v2 = myMesh.createVertex(10,0,0);
    int v3 = myMesh.createVertex(0,10,0);
    cout << "Three vertices created: " << v1 << ", " << v2 << ", " << v3 << "." << endl;
    int t1 = myMesh.createTriangle(v1,v2,v3);

    TriangleMeshTriangle *triangle1 = myMesh.getTriangle(t1);

    int v4 = myMesh.createVertex(10,10,0);
    int t2 = myMesh.createTriangle(v2,v4,v3);

    TriangleMeshTriangle *triangle2 = myMesh.getTriangle(t2);

    int v5 = myMesh.createVertex(5,15,0);
    int t3 = myMesh.createTriangle(v3,v4,v5);

    TriangleMeshTriangle *triangle3 = myMesh.getTriangle(t3);

    //State of the mesh at the moment:
    //      *
    //     / \
    //    /   \
    //   *-----*
    //   |\    |
    //   |  \  |
    //   |    \|
    //   *-----*
    // 5 vertices, 3 triangles, 7 total edges.
    printStateOfMesh(myMesh, true);

    TriangleMeshEdge* v2v3 =
        myMesh.getEdgeBetweenVertices(v2,v3);

    TriangleMeshEdge* v3v2 =
        myMesh.getEdgeBetweenVertices(v3,v2);

    assert( v2v3 == v3v2 );

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

    int wee1 = 1;
    int wee2 = 2;
    TriangleMeshVertex* vert1 = myMesh.getVertex(v1);
    TriangleMeshVertex* vert2 = myMesh.getVertex(v2);

    cout << "Testing (v1, v2)" << endl;
    cout << "(" << wee1 << "," << wee2 << "):" << vert1 << " | " << vert2 << endl;
    myMesh.applyNaturalOrdering(&vert1, &vert2, &wee1,&wee2);
    cout << "(" << wee1 << "," << wee2 << "):" << vert1 << " | " << vert2 << endl;

    cout << "Testing (v2,v1)" << endl;
    cout << "(" << wee1 << "," << wee2 << "):" << vert2 << " | " << vert1 << endl;
    myMesh.applyNaturalOrdering(&vert2, &vert1, &wee2,&wee1);
    cout << "(" << wee1 << "," << wee2 << "):" << vert2 << " | " << vert1 << endl;

    vert2 = myMesh.getVertex(v2);
    TriangleMeshVertex* vert3 = myMesh.getVertex(v3);
    wee2 = 2;
    int wee3 = 3;

    cout << "Testing (v2,v3)" << endl;
    cout << "(" << wee2 << "," << wee3 << "):" << vert2 << " | " << vert3 << endl;
    myMesh.applyNaturalOrdering(&vert2, &vert3, &wee2,&wee3);
    cout << "(" << wee2 << "," << wee3 << "):" << vert2 << " | " << vert3 << endl;

    cout << "Testing (v3,v2)" << endl;
    cout << "(" << wee2 << "," << wee3 << "):" << vert3 << " | " << vert2 << endl;
    myMesh.applyNaturalOrdering(&vert3, &vert2, &wee2,&wee3);
    cout << "(" << wee2 << "," << wee3 << "):" << vert3 << " | " << vert2 << endl;




    cout << "Reading in file..." << endl;
    Parser parser;
    TriangleMesh* mesh2 = parser.parseOBJ("/Users/njoubert/Code/workspace1/ClothSym/data/face1.obj");
    if (mesh2 == NULL)
        return 0;
    cout << "Done reading file..." << endl;

    printStateOfMesh(*mesh2, false);


}

// */
