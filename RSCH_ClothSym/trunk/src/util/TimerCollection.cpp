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
    std::cout << fixed << setprecision(2);
    float netPercentage = 0, percentage;
    for (unsigned int i = 0; i < _timers.size(); i++) {
        percentage = 100 * getRunningTotal(i) / getRunningTotal();
        std::cout << setfill('.');
        std::cout << "    " << i << " - " << setw(30) << left << _names[i].substr(0,30);
        std::cout << setfill(' ');
        std::cout << right << " | dur: " << setw(8) << getRunningTotal(i)*1000 << "ms | avg: " << setw(8) << getRunningAverage(i)*1000 << "ms | per: " << setw(8) << percentage << "%" << std::endl;
        netPercentage += percentage;
    }
    std::cout << "  Accounted for " << netPercentage << "% of total time, runtime of " << _gTimer.getRunningTotal() << "s" << endl;
    std::cout << scientific << setprecision(6);
}
