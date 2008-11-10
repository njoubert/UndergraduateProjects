/*
 * global.cpp
 *
 *  Created on: Nov 7, 2008
 *      Author: njoubert
 */

#include "global.h"

Timer fpstimer;
TimerCollection frametimers;

float GAMMA = DEFAULT_GAMMA;

int MAX_CG_ITER = DEFAULT_MAX_CG_ITER;
float MAX_CG_ERR = DEFUALT_MAX_CG_ERR;

double TIMESTEP = DEFAULT_TIMESTEP;
float MASS = DEFAULT_MASS;
int MESHSIZE;

float Ke = DEFAULT_KS;
float Kd = DEFAULT_KD;
float KBe = DEFAULT_KBE;
float KBd = DEFAULT_KBD;

bool BEND_FORCES = false;
bool FRICTION_FORCES = false;
bool COLLISIONS = false;

bool STATIC_CONSTRAINTS = false;
bool DYNAMIC_CONSTRAINTS = false;
int FOLLOW1 = -1;
int FOLLOW2 = -1;
int FOLLOW3 = -1;
int FOLLOW4 = -1;
int LEAD1 = -1;
int LEAD2 = -1;
int LEAD3 = -1;
int LEAD4 = -1;

float MUs = DEFAULT_MUS;
float MUd = DEFAULT_MUD;

float COLL_VEL_HAXX = 1; //Multiplies the velocity term, higher than one will result in cloth being ahead of lead object

bool DRAWELLIPSOIDS = false;
