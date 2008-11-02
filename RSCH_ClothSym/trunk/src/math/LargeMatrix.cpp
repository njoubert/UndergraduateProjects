/*
 * LargeMatrix.cpp
 *
 *  Created on: Nov 1, 2008
 *      Author: njoubert
 */

#include "LargeMatrix.h"

ostream & operator <<(ostream & s, const LargeMat3Matrix &m) {
    s << "LargeMat3Matrix(" << m._rowCount << "," << m._rowCount << ") = " << endl;
    for (int i = 0; i < m._rowCount; i++) {
        for (unsigned int j = 0; j < m._rowData[i]->_sparseIndices.size(); j++) {
            s << "(" << i << "," << m._rowData[i]->_sparseIndices[j] << ") = " << endl;
            s << m._rowData[i]->_sparseElements[j] << endl;
        }
    }
    return s;
}

