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

#include <math.h>
#include "util/Timer.h"
#include "util/TimerCollection.h"
#include "util/Profiler.h"

#include "Debug.h"

#include "math/algebra3.h"

#include "DEFAULTS.h"

extern Timer fpstimer;
extern Profiler profiler;

extern bool DEBUG;

extern double TIME;
extern double INTERSTEPTIME;

extern float GAMMA;
extern float BETA;

extern int MAX_CG_ITER;
extern float MAX_CG_ERR;

extern double TIMESTEP;
extern float MASS;
extern int MESHSIZE;

extern float Ke;
extern float Kd;

extern bool BEND_FORCES;
extern bool FRICTION_FORCES;
extern bool COLLISIONS;

extern bool STATIC_CONSTRAINTS;
extern bool DYNAMIC_CONSTRAINTS;
extern bool OVERRIDE_DYNAMIC_CONSTRAINTS;
extern int FOLLOW1;
extern int FOLLOW2;
extern int FOLLOW3;
extern int FOLLOW4;
extern int LEAD1;
extern int LEAD2;
extern int LEAD3;
extern int LEAD4;
extern int HIERARCHY1;
extern int HIERARCHY2;
extern int HIERARCHY3;
extern int HIERARCHY4;

extern float KBe;
extern float KBd;

extern float Kcoll;
extern float Kcr;

extern float MUs;
extern float MUd;

extern vector<bool> DRAWMODELS;

extern bool EXITONLASTFRAME;

extern int LOWQINDEX;
extern int HIGHQINDEX;
extern double SYNCSTEP;
extern double EDAMP;

extern vec3 WIND;
extern bool isWIND;

extern float COLL_VEL_HAXX;

extern bool DRAWELLIPSOIDS;
extern bool WIREFRAME;
extern bool HIDDEN;
extern bool DRAWNORMALS;
extern bool DRAWMESHDIFF;
extern bool USECOLLJACOBIAN;
extern bool PLAYALLFRAMES;
extern double BIGGESTTIMESTEP;
extern bool WRITEALLFRAMES;
extern bool WRITEFILEANDEXIT;

extern bool DRAWCOLLISIONS;

#define PI 3.14159265

#endif /* UTIL_H_ */
