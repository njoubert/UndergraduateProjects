/*
 * CG.h
 *
 *  Created on: Nov 3, 2008
 *      Author: njoubert
 */

#ifndef CG_H_
#define CG_H_

#include "LargeMatrix.h"
#include "LargeVector.h"

#define RECALC_RESIDUE 50

class ConjugateGradient {
public:
    ConjugateGradient(int);
    ~ConjugateGradient();
    int simpleCG(LargeMat3Matrix & A, LargeVec3Vector & b, LargeVec3Vector & x,
            int imax, double e);

private:
    LargeVec3Vector r;
    LargeVec3Vector d;
    LargeVec3Vector q;
};


#endif /* CG_H_ */
