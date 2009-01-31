/*
 * MeshTOLargeVector.cpp
 *
 *  Created on: Nov 6, 2008
 *      Author: njoubert
 */

#include "MeshTOLargeMatrix.h"

/**
 * Calculates the sparsity pattern used in LargeMatrix from a TriangleMesh
 */
vector<vector <int> > MeshTOLargeMatrix::CalculateSparsityPattern(TriangleMesh* mesh) {

    vector<vector<int> > sparsePattern(mesh->countVertices());

	TriangleMeshVertex *a, *b;
	TriangleMeshTriangle *A, *B;

    EdgesIterator iter = mesh->getEdgesIterator();
    int index1, index2, ai=0, bi=0, index3, index4;
    do {
        index1 = iter->getVertex(0)->getIndex();
        index2 = iter->getVertex(1)->getIndex();
/*
        a = (*iter)->getVertex(0);
		b = (*iter)->getVertex(1);
		A = (*iter)->getParentTriangle(0);
		B = (*iter)->getParentTriangle(1);
		TriangleMeshVertex** aList = A->getVertices();
		TriangleMeshVertex** bList = B->getVertices();

		if (A != NULL && B != NULL) {
				assert(A != B);
				for (int i = 0; i < 3; i++) {
					if (aList[i] != a && aList[i] != b)
						ai = i;

					if (bList[i] != a && bList[i] != b)
						bi = i;
				}
		index3 = aList[ai]->getIndex();
		index4 = bList[bi]->getIndex();

		sparsePattern[index3].push_back(index3);
		sparsePattern[index3].push_back(index4);
		sparsePattern[index4].push_back(index3);
		sparsePattern[index4].push_back(index4);

		//cout<<"Put Entry Into Sparse Matrix point: "<<index3<<" and point: "<<index4<<endl;
		}
//*/
        sparsePattern[index1].push_back(index1);
        sparsePattern[index1].push_back(index2);
        sparsePattern[index2].push_back(index1);
        sparsePattern[index2].push_back(index2);
    } while (iter.next());

    return sparsePattern;

}

vector<vector <int> > MeshTOLargeMatrix::GenerateIdentityPattern(int dim) {
    vector<vector<int> > sparsePattern(dim);
    for (int i = 0; i < dim; i++) {
        sparsePattern[i].push_back(i);
    }
    return sparsePattern;
}

vector<vector <int> > MeshTOLargeMatrix::GenerateNByOnePattern(int dim) {
    vector<vector<int> > sparsePattern(dim);
    for (int i = 0; i < dim; i++) {
        sparsePattern[i].push_back(0);
    }
    return sparsePattern;
}
