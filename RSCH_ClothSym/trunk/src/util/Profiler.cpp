/*
 * Profiler.cpp
 *
 *  Created on: Nov 9, 2008
 *      Author: njoubert
 */

#include "Profiler.h"

Profiler::Profiler() {
    _CGlaps = 1;
    _avgCGIterations = 0;
    _maxCGIterations = 0;
}

Profiler::~Profiler() {
    // TODO Auto-generated destructor stub
}

void Profiler::profileNewCGIteration(int count) {
    _avgCGIterations = _avgCGIterations * ((float)_CGlaps / (float) (_CGlaps + 1)) +
            (float) count / (float) (_CGlaps + 1);
    _CGlaps++;
    if (_maxCGIterations < count)
        _maxCGIterations = count;
}

void Profiler::print() {
    cout << "=== STATISTICS ===" << endl;
    cout << "FRAMERATE:" << endl;
    cout << "  Average framerate: " << 1.0 / fpstimer.getRunningAverage() << endl;
    cout << "  The maximum framerate we could have achieved: " << 1.0 / frametimers.getRunningAverage() << endl;
    cout << "CG:" << endl;
    cout << "  Average CG Iterations: " << _avgCGIterations << ", Max CG Iterations: " << _maxCGIterations << endl;
    cout << "FRAMETIMER: " << endl;
    frametimers.printStatistics();
    cout << "==================" << endl;
}
