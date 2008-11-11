/*
 * Timer.cpp
 *
 *  Created on: Nov 3, 2008
 *      Author: njoubert
 */

#include "Timer.h"

Timer::Timer() {
    running = false;
    lastLap = 0;
    runningAverage = 0;
    runningTotal = 0;
    lapsCount = 1;
}

Timer::~Timer() {
    // TODO Auto-generated destructor stub
}

void Timer::Start() {
#ifdef _WIN32
    lastTime = GetTickCount();
#else
    gettimeofday(&lastTime, 0);
#endif
    running = true;
}

float Timer::Elapsed() {
    if (!running)
        return lastLap;

    float deltaT;
#ifdef _WIN32
    DWORD currentTime = GetTickCount();
    deltaT = (float)(currentTime - lastTime)*0.001f;
#else
    timeval currentTime;
    gettimeofday(&currentTime, 0);
    deltaT = (float) ((currentTime.tv_sec - lastTime.tv_sec) + 1e-6
            * (currentTime.tv_usec - lastTime.tv_usec));
#endif

    return deltaT;
}

float Timer::Stop() {
    if (!running)
        return lastLap;
    lastLap = Elapsed();
    runningTotal += lastLap;
    running = false;

    runningAverage = runningAverage * ((float) lapsCount / ((float) lapsCount + 1))
            + lastLap / (float) (lapsCount + 1);
    lapsCount++;

    return lastLap;
}

float Timer::getRunningAverage() {
    return runningAverage;
}
float Timer::getRunningTotal() {
    return runningTotal;
}
