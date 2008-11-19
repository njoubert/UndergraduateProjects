/*
 * CG.cpp
 *
 *  Created on: Nov 3, 2008
 *      Author: njoubert
 */

#include "CG.h"

ConjugateGradient::ConjugateGradient(int size) {

}
ConjugateGradient::~ConjugateGradient() {

}

SimpleCG::SimpleCG(int size) :
    ConjugateGradient(size), r(size), d(size), q(size) {

}

SimpleCG::~SimpleCG() {

}

int SimpleCG::solve(LargeMat3Matrix & A, LargeVec3Vector & b, LargeVec3Vector & x,
        int imax, double e) {

    int i = 0;                                  //Steps so far
    A.postMultiply(x, r);                       //Residue
    r *= -1;
    r = b;
    d = r;
    double deln = r.Dot(r);
    double del0 = deln;
    double delo;

    double esquare = e*e;
    double alpha = 0, beta = 0;
    while (i < imax && deln > esquare*del0) {
        A.postMultiply(d, q);
        alpha = deln / d.Dot(q);
        d *= alpha;
        x += d;
        d /= alpha;

#ifdef PERFORMANCEHAXX
        if ((i % RECALC_RESIDUE) == 0) {
#endif
            A.postMultiply(x, r);
            r *= -1;
            r += b;
#ifdef PERFORMANCEHAXX
        } else {
            q *= alpha;
            r -= q;
            q /= alpha;
        }
#endif
        delo = deln;
        deln = r.Dot(r);
        beta = deln / delo;
        d *= beta;
        d += r;
        i++;
    }
    //cout<<"Conjugate Gradient Converged at iteration "<<i<<" with a residual of "<<deln<<endl;


    //Calculate statistics on efficiency.
    profiler.profileNewCGIteration(i);
    return i;
}

ModifiedCG::ModifiedCG(int size) :
    ConjugateGradient(size), r(size), d(size), q(size) {

}

ModifiedCG::~ModifiedCG() {

}

//LargeVec3Vector* ModifiedCG::filter(LargeVec3Vector* a, ) {

//}

int ModifiedCG::solve(LargeMat3Matrix & A, LargeVec3Vector & b, LargeVec3Vector & x,
        int imax, double e) {

    int i = 0;                                  //Steps so far
    A.postMultiply(x, r);                       //Residue
    r *= -1;
    r = b;
    d = r;
    double deln = r.Dot(r);
    double del0 = deln;
    double delo;

    double esquare = e*e;
    double alpha = 0, beta = 0;
    while (i < imax && deln > esquare*del0) {
        A.postMultiply(d, q);
        alpha = deln / d.Dot(q);
        d *= alpha;
        x += d;
        d /= alpha;

#ifdef PERFORMANCEHAXX
        if ((i % RECALC_RESIDUE) == 0) {
#endif
            A.postMultiply(x, r);
            r *= -1;
            r += b;
#ifdef PERFORMANCEHAXX
        } else {
            q *= alpha;
            r -= q;
            q /= alpha;
        }
#endif
        delo = deln;
        deln = r.Dot(r);
        beta = deln / delo;
        d *= beta;
        d += r;
        i++;
    }
    //cout<<"Conjugate Gradient Converged at iteration "<<i<<" with a residual of "<<deln<<endl;


    //Calculate statistics on efficiency.
    profiler.profileNewCGIteration(i);
    return i;
}

PreconditionedCG::PreconditionedCG(int size) :
    ConjugateGradient(size), r(size), d(size), s(size), q(size),
    Minv(size,size,MeshTOLargeMatrix::GenerateIdentityPattern(size), true) {

}

PreconditionedCG::~PreconditionedCG() {

}

void PreconditionedCG::computePreconditioner(LargeMat3Matrix & A) {
    for (int i = 0; i < A.getColSize(); i++) {
        Minv(i,i) = A(i,i).inverse();
    }
}

int PreconditionedCG::solve(LargeMat3Matrix & A, LargeVec3Vector & b, LargeVec3Vector & x,
        int imax, double e) {

    computePreconditioner(A);

    int i = 0;                                  //Steps so far
    A.postMultiply(x, r);                       //Residue
    r *= -1;
    r = b;
    Minv.postMultiply(r, d);
    double deln = r.Dot(d);
    double del0 = deln;
    double delo;

    double esquare = e*e;
    double alpha = 0, beta = 0;
    while (i < imax && deln > esquare*del0) {
        A.postMultiply(d, q);
        alpha = deln / d.Dot(q);
        d *= alpha;
        x += d;
        d /= alpha;

#ifdef PERFORMANCEHAXX
        if ((i % RECALC_RESIDUE) == 0) {
#endif
            A.postMultiply(x, r);
            r *= -1;
            r += b;
#ifdef PERFORMANCEHAXX
        } else {
            q *= alpha;
            r -= q;
            q /= alpha;
        }
#endif
        Minv.postMultiply(r, s);
        delo = deln;
        deln = r.Dot(s);
        beta = deln / delo;
        d *= beta;
        d += s;
        i++;
    }
    //cout<<"Conjugate Gradient Converged at iteration "<<i<<" with a residual of "<<deln<<endl;


    //Calculate statistics on efficiency.
    profiler.profileNewCGIteration(i);
    return i;
}
