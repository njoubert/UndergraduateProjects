#ifndef AS5_H_
#define AS5_H_

// Simple OpenGL example for CS184 sp08 by Trevor Standley, modified from sample code for CS184 on Sp06
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

#ifdef _WIN32
#	include <windows.h>
#else
#	include <sys/time.h>
#endif

#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#define GLUT_DISABLE_ATEXIT_HACK
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include <time.h>
#include <math.h>

#define PI 3.14159265

//****** Add libraries ******
//#include "cml/cml.h"

//****** Add our headers ******
#include "Algebra.cpp"

#include "types.h"
#include "bezier.h"
#include "patch.h"
#include "Parser.h"

#include "NielsDebug.h"

#define isThereMore(CURR, MAX, WANT)	((MAX - CURR) >= WANT)

#endif /*AS5_H_*/
