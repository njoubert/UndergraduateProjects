/*
 * MeshTOLargeVector.h
 *
 *  Created on: Nov 6, 2008
 *      Author: njoubert
 */

#ifndef MESHTOLARGEVECTOR_H_
#define MESHTOLARGEVECTOR_H_

#include "../Mesh.h"
#include "../math/LargeMatrix.h"

class MeshTOLargeMatrix {
public:
    static vector<vector <int> > CalculateSparsityPattern(TriangleMesh* mesh);

    static vector<vector <int> > GenerateIdentityPattern(int dim);

};

#endif /* MESHTOLARGEVECTOR_H_ */
