/*
 * TimerCollection.h
 *
 *  Created on: Nov 7, 2008
 *      Author: njoubert
 */

#ifndef TIMERCOLLECTION_H_
#define TIMERCOLLECTION_H_

#include <vector>
#include <iostream>
#include "Timer.h"

class TimerCollection {
public:
    TimerCollection();
    TimerCollection(int);

    //Global timer control:
    void Start();
    float Elapsed();
    float Stop();
    float getRunningAverage();

    //Subtimer control:
    int addNewTimer();
    void switchToTimer(int i);
    float Elapsed(int i);
    float getRunningAverage(int i);

    //Misc
    void printStatistics();

private:
    std::vector<Timer> _timers;
    Timer _gTimer;
    int _currentSubtimer;
};

#endif /* TIMERCOLLECTION_H_ */
