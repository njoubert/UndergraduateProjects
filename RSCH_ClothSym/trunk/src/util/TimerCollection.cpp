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
        _timers(i, Timer()), _names(i, ""), _gTimer() {
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
float TimerCollection::getRunningTotal() {
    return _gTimer.getRunningTotal();
}
int TimerCollection::getTimer(string name) {
    for (unsigned int i = 0; i < _names.size(); i++) {
        if (_names[i] == name)
            return i;
    }
    return addNewTimer(name);
}
int TimerCollection::addNewTimer(string name) {
    _timers.push_back(Timer());
    _names.push_back(name);
    return _timers.size() - 1;
}

void TimerCollection::switchToTimer(string name) {
    int i = getTimer(name);
    switchToGlobal();
    _currentSubtimer = i;
    _timers[i].Start();
}
void TimerCollection::switchToGlobal() {
    if (_currentSubtimer >= 0) {
        _timers[_currentSubtimer].Stop();
        _currentSubtimer = -1;
    }

}
float TimerCollection::getRunningAverage(int i) {
    return _timers[i].getRunningAverage();
}
float TimerCollection::getRunningTotal(int i) {
    return _timers[i].getRunningTotal();
}
void TimerCollection::printStatistics() {
    std::cout << "  global duration: " << _gTimer.getRunningTotal() << "s" << std::endl;
    std::cout << "  subtimers: " << std::endl;
    for (unsigned int i = 0; i < _timers.size(); i++) {
        std::cout << "    " << i << " - \"" << _names[i] << "\" duration: " << getRunningTotal(i) << "s, " << 100 * getRunningTotal(i) / getRunningTotal() << "%" << std::endl;
    }
}
