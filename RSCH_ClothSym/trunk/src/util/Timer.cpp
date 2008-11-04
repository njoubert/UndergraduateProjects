/*
 * Timer.cpp
 *
 *  Created on: Nov 3, 2008
 *      Author: njoubert
 */

#include "Timer.h"

Timer::Timer() {
    // TODO Auto-generated constructor stub

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
