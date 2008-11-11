

/*
 * main.h
 *
 *  Created on: Sep 8, 2008
 *      Author: njoubert
 */

#ifndef MAIN_H_
#define MAIN_H_



#include "global.h"
#include "World.h"
#include "io/ImageSaver.h"
#include <string.h>

#define isThereMore(CURR, MAX, WANT)    ((MAX - CURR) > WANT)

//TODO: Timing... to be done in the future!
#ifdef _WIN32
static DWORD lastTime;
#else
//static struct timeval lastTime;
#endif

void closeMe(int code);

#endif
