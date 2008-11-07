/*
 * Timer.cpp
 *
 *  Created on: Nov 3, 2008
 *      Author: njoubert
 */

#include "Timer.h"

Timer::Timer() {
    runningTotal = 0;
    runningLaps = 1;
}

Timer::~Timer() {
    // TODO Auto-generated destructor stub
}

void Timer::Start() {
#ifdef _WIN32
    lastTime = GetTickCount();
#else
    gettimeofday(&lastTime, NULL);
#endif
}

float Timer::Stop() {
    float deltaT;
#ifdef _WIN32
    DWORD currentTime = GetTickCount();
    deltaT = (float)(currentTime - lastTime)*0.001f;
#else
    timeval currentTime;
    gettimeofday(&currentTime, NULL);
    deltaT = (float) ((currentTime.tv_sec - lastTime.tv_sec) + 1e-6
            * (currentTime.tv_usec - lastTime.tv_usec));
#endif
    return deltaT;
}

void Timer::updateRunningTotal() {
    float lap = Stop();
    runningTotal = runningTotal*((float)runningLaps / ((float)runningLaps+1)) + lap/(runningLaps+1);
    runningLaps++;
}
float Timer::getRunningTotal() {
    return runningTotal;
}
