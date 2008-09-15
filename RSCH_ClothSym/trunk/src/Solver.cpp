/*
 * Solver.cpp
 *
 *  Created on: Sep 8, 2008
 *      Author: njoubert
 */

#include "Solver.h"

Solver::Solver() {
    // TODO Auto-generated constructor stub

}

Solver::~Solver() {
    // TODO Auto-generated destructor stub
}


void Solver::takeStep(System & sys, double h) {
    std::vector< std::vector < Particle > > * x = sys.evalDeriv();
    std::vector<int> dim = sys.getDim();
    for (int i = 0; i < dim[0]; i++) {
        for (int j = 0; j < dim[1]; j++) {
            if ((*x)[i][j].pinned)
                continue;
            (*x)[i][j].vx = (*x)[i][j].vx + h*((*x)[i][j].fx / (*x)[i][j].m);
            (*x)[i][j].vy = (*x)[i][j].vy + h*((*x)[i][j].fy / (*x)[i][j].m);
            //std::cout << "particle m = " << (*x)[i][j].m << ", fy="<< (*x)[i][j].fy <<", vy = " << (*x)[i][j].vy << std::endl;
            (*x)[i][j].x = (*x)[i][j].x + h*(*x)[i][j].vx;
            (*x)[i][j].y = (*x)[i][j].y + h*(*x)[i][j].vy;
        }
    }
    sys.setX(x);
    delete x;
}
