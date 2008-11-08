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
#include <string>
#include <iomanip>
#include "Timer.h"

using namespace std;

class TimerCollection {
public:
    TimerCollection();
    TimerCollection(int);

    //Global timer control:
    void Start();
    float Elapsed();
    float Stop();
    float getRunningAverage();
    float getRunningTotal();


    //Subtimer control:
private:
    int getTimer(string);
    int addNewTimer(string);
    float getRunningAverage(int i);
    float getRunningTotal(int i);
public:
    void switchToTimer(string);
    void switchToGlobal();

    //Misc
    void printStatistics();

private:
    vector<Timer> _timers;
    vector<string> _names;
    Timer _gTimer;
    int _currentSubtimer;
};

#endif /* TIMERCOLLECTION_H_ */
