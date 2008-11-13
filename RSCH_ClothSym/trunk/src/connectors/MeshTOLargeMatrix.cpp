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

    EdgesIterator iter = mesh->getEdgesIterator();
    int index1, index2;
    do {
        index1 = iter->getVertex(0)->getIndex();
        index2 = iter->getVertex(1)->getIndex();

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
