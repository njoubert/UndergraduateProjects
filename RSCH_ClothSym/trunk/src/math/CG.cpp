/*
 * CG.cpp
 *
 *  Created on: Nov 3, 2008
 *      Author: njoubert
 */

#include "CG.h"

ConjugateGradient::ConjugateGradient(int size):
        r(size), d(size), q(size){

}
ConjugateGradient::~ConjugateGradient() {

}

int ConjugateGradient::simpleCG(LargeMat3Matrix & A, LargeVec3Vector & b, LargeVec3Vector & x,
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
    return i;
}
