

/*
 * main.h
 *
 *  Created on: Sep 8, 2008
 *      Author: njoubert
 */

#ifndef MAIN_H_
#define MAIN_H_


#include "System.h"
#include "Solver.h"

#ifdef _WIN32
static DWORD lastTime;
#else
static struct timeval lastTime;
#endif


#endif