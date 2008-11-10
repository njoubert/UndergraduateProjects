/*
 * Profiler.h
 *
 *  Created on: Nov 9, 2008
 *      Author: njoubert
 */

#ifndef PROFILER_H_
#define PROFILER_H_

#include <iostream>
#include <vector>

class Profiler;

#include "../global.h"

#include "Timer.h"
#include "TimerCollection.h"

using namespace std;

class Profiler {
public:
    Profiler();
    virtual ~Profiler();
    void print();
    TimerCollection frametimers;

    void profileNewCGIteration(int count);
private:
    float _avgCGIterations;
    int _maxCGIterations;
    int _CGlaps;

};

#endif /* PROFILER_H_ */
