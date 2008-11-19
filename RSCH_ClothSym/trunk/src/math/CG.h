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
#include "../connectors/MeshTOLargeMatrix.h"
#include "../global.h"

#define RECALC_RESIDUE 50

class ConjugateGradient {
public:
    ConjugateGradient(int);
    virtual ~ConjugateGradient();
    virtual int solve(LargeMat3Matrix & A, LargeVec3Vector & b, LargeVec3Vector & x,
            int imax, double e) = 0;
};

class SimpleCG : public ConjugateGradient {
public:
    SimpleCG(int);
    ~SimpleCG();
    int solve(LargeMat3Matrix & A, LargeVec3Vector & b, LargeVec3Vector & x,
                int imax, double e);
private:
    LargeVec3Vector r;
    LargeVec3Vector d;
    LargeVec3Vector q;
};

class ModifiedCG : public ConjugateGradient {
public:
	ModifiedCG(int);
    ~ModifiedCG();
    int solve(LargeMat3Matrix & A, LargeVec3Vector & b, LargeVec3Vector & x,
                int imax, double e);
    LargeVec3Vector* filter(LargeVec3Vector*);
private:
    LargeVec3Vector r;
    LargeVec3Vector d;
    LargeVec3Vector q;
};

class PreconditionedCG : public ConjugateGradient {
public:
    PreconditionedCG(int);
    ~PreconditionedCG();
    int solve(LargeMat3Matrix & A, LargeVec3Vector & b, LargeVec3Vector & x,
                int imax, double e);
private:
    void computePreconditioner(LargeMat3Matrix & A);
    LargeVec3Vector r;
    LargeVec3Vector d;
    LargeVec3Vector s;
    LargeVec3Vector q;
    LargeMat3Matrix Minv;
};


#endif /* CG_H_ */
