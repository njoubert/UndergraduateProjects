/*
 * util.h
 *
 *  Created on: Sep 8, 2008
 *      Author: njoubert
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <cstdlib>

#ifdef _WIN32
#   include <windows.h>
#else
#   include <sys/time.h>
#endif

#ifdef OSX

#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include <time.h>
#include <math.h>

#include "Debug.h"

#include "math/algebra3.h"

#include "DEFAULTS.h"

#define PI 3.14159265

#endif /* UTIL_H_ */
