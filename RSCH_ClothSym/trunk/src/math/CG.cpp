/*
 * CG.cpp
 *
 *  Created on: Nov 3, 2008
 *      Author: njoubert
 */

#include "CG.h"

#define RECALC_RESIDUE 50

int simpleCG(LargeMat3Matrix & A, LargeVec3Vector & b, LargeVec3Vector & x,
        int imax, double e) {

    int i = 0;                                  //Steps so far
    LargeVec3Vector r(b.size());            //Residue
    A.postMultiply(x, r);
    r *= -1;
    r += b;
    LargeVec3Vector d(r);
    double deln = r.Dot(r);
    double del0 = deln;

    double esquare = e*e;
    double alpha = 0, beta = 0;
    LargeVec3Vector q(r.size());
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
        del0 = deln;
        deln = r.Dot(r);
        beta = deln / del0;
        d *= beta;
        d += r;
        i++;
        //cout << "deln = " << deln << " del0 = " << del0 << endl;
    }
    return i;
}
