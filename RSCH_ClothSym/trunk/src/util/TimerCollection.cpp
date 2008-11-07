/*
 * TimerCollection.cpp
 *
 *  Created on: Nov 7, 2008
 *      Author: njoubert
 */

#include "TimerCollection.h"

TimerCollection::TimerCollection():
    _timers(), _gTimer() {
    _currentSubtimer = -1;
}

TimerCollection::TimerCollection(int i):
        _timers(i, Timer()), _gTimer() {
    _currentSubtimer = -1;
}

void TimerCollection::Start() {
    _gTimer.Start();
}

float TimerCollection::Elapsed() {
    return _gTimer.Elapsed();
}
float TimerCollection::Stop() {
    if (_currentSubtimer >= 0)
        _timers[_currentSubtimer].Stop();
    return _gTimer.Stop();
}
float TimerCollection::getRunningAverage() {
    return _gTimer.getRunningAverage();
}

int TimerCollection::addNewTimer() {
    _timers.push_back(Timer());
    return _timers.size() - 1;
}

void TimerCollection::switchToTimer(int i) {
    if (_currentSubtimer >= 0)
        _timers[_currentSubtimer].Stop();
    if (i >= 0 && i < (int)_timers.size()) {
        _currentSubtimer = i;
        _timers[i].Start();
    }
}

float TimerCollection::Elapsed(int i) {
    if (_currentSubtimer >= 0)
        return _timers[_currentSubtimer].Elapsed();
    else
        return Elapsed();
}
float TimerCollection::getRunningAverage(int i) {
    if (_currentSubtimer >= 0)
        return _timers[_currentSubtimer].getRunningAverage();
    else
        return getRunningAverage();
}

void TimerCollection::printStatistics() {
    std::cout << "  global average: " << _gTimer.getRunningAverage() << " s" << std::endl;
    std::cout << "  subtimers: " << std::endl;
    for (unsigned int i = 0; i < _timers.size(); i++) {
        std::cout << "    timer " << i << " average: " << getRunningAverage(i) << " s, " << getRunningAverage(i) / getRunningAverage() << "%" << std::endl;
    }
}
