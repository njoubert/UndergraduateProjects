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

void simpleCG(LargeMat3Matrix & A, LargeVec3Vector & b, LargeVec3Vector & x,
        int imax, double e);

#endif /* CG_H_ */
