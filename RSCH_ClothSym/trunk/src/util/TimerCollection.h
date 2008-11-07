/*
 * TimerCollection.h
 *
 *  Created on: Nov 7, 2008
 *      Author: njoubert
 */

#ifndef TIMERCOLLECTION_H_
#define TIMERCOLLECTION_H_

#include <vector>
#include "Timer.h"

class TimerCollection {
public:
    TimerCollection(int);
    virtual ~TimerCollection();

    void Start();
    float Stop();
    void switchToTimer(int i);
    float getTimerDuration(int i);

    void printStatistics();

private:
    std::vector<Timer> timers;
    Timer globalTimer;
};

#endif /* TIMERCOLLECTION_H_ */
