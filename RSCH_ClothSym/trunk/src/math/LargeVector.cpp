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
    for (int i = 0; i < v.size() - 1; i++) {
        s << v[i] << ",";
    }
    if (v.size() > 0)
        s << v[v.size() - 1];
    s << ")";
    return s;
}
