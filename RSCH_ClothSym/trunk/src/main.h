

/*
 * main.h
 *
 *  Created on: Sep 8, 2008
 *      Author: njoubert
 */

#ifndef MAIN_H_
#define MAIN_H_



#include "global.h"
#include "Model.h"
#include "io/OBJParser.h"
#include "io/ImageSaver.h"

#include <FreeImage.h>

#define isThereMore(CURR, MAX, WANT)    ((MAX - CURR) > WANT)

#ifdef _WIN32
static DWORD lastTime;
#else
//static struct timeval lastTime;
#endif


#endif
