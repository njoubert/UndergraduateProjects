/*
 * Solver.h
 *
 *  Created on: Sep 8, 2008
 *      Author: njoubert
 */

#include "global.h"
#include "System.h"

#ifndef SOLVER_H_
#define SOLVER_H_

class Solver {
public:
    Solver();
    virtual ~Solver();

    void takeStep(System & sys, double h);
};

#endif /* SOLVER_H_ */
