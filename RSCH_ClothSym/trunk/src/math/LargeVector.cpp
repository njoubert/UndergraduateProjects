/*
 * LargeVector.cpp
 *
 *  Created on: Nov 1, 2008
 *      Author: njoubert
 */

#include "LargeVector.h"


//-----------------------------------------------------------------------------
ostream & operator <<(ostream & s, const LargeVec3Vector &v) {
    s << "(";
    for (int i = 0; i < v.getSize() - 1; i++) {
        s << v[i] << ",";
    }
    if (v.getSize() > 1)
        s << v[v.getSize() - 1];
    s << ")";
    return s;
}
