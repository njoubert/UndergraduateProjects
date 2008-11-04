/*
 * Timer.h
 *
 *  Created on: Nov 3, 2008
 *      Author: njoubert
 */

#ifndef TIMER_H_
#define TIMER_H_

#ifdef _WIN32
#   include <windows.h>
#else
#   include <sys/time.h>
#endif


class Timer {
public:

#ifdef _WIN32
    DWORD lastTime;
#else
    struct timeval lastTime;
#endif

    Timer();
    virtual ~Timer();

    void Start();
    float Stop();
};

#endif /* TIMER_H_ */
