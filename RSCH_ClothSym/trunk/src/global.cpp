/*
 * global.cpp
 *
 *  Created on: Nov 7, 2008
 *      Author: njoubert
 */

#include "global.h"

Timer fpstimer;
Profiler profiler;

double TIME = 0.0;

double INTERSTEPTIME = 0.0;

bool DEBUG = false;

float GAMMA = DEFAULT_GAMMA;
float BETA = DEFAULT_BETA;

int MAX_CG_ITER = DEFAULT_MAX_CG_ITER;
float MAX_CG_ERR = DEFAULT_MAX_CG_ERR;

double TIMESTEP = DEFAULT_TIMESTEP;
float MASS = DEFAULT_MASS;
int MESHSIZE;

float Ke = DEFAULT_KS;
float Ks_comp = DEFAULT_KS;
float Kd = DEFAULT_KD;
float KBe = DEFAULT_KBE;
float KBd = DEFAULT_KBD;
float Kcoll = DEFAULT_KCOLL;
float Kcr = 0;
bool USECOLLJACOBIAN = true;

bool BEND_FORCES = false;
bool FRICTION_FORCES = false;
bool COLLISIONS = false;

bool STATIC_CONSTRAINTS = false;
bool DYNAMIC_CONSTRAINTS = false;
bool OVERRIDE_DYNAMIC_CONSTRAINTS = false;
int FOLLOW1 = -1;
int FOLLOW2 = -1;
int FOLLOW3 = -1;
int FOLLOW4 = -1;
int LEAD1 = -1;
int LEAD2 = -1;
int LEAD3 = -1;
int LEAD4 = -1;
int HIERARCHY1 = -1;
int HIERARCHY2 = -1;
int HIERARCHY3 = -1;
int HIERARCHY4 = -1;

float MUs = DEFAULT_MUS;
float MUd = DEFAULT_MUD;

vector<bool> DRAWMODELS;

int LOWQINDEX = -1;
int HIGHQINDEX = -1;
double SYNCSTEP = -1;
double EDAMP = 1;

vec3 WIND(0);
double KDRAG;
bool isWIND = false;

float COLL_VEL_HAXX = 1; //Multiplies the velocity term, higher than one will result in cloth being ahead of lead object

bool DRAWELLIPSOIDS = false;
bool WIREFRAME = false;
bool HIDDEN = false;
bool DRAWNORMALS = true;
bool DRAWMESHDIFF = true;

bool PLAYALLFRAMES = false;
double BIGGESTTIMESTEP = 0.01;
bool WRITEALLFRAMES = false;

bool WRITEFILEANDEXIT = false;
bool DRAWCOLLISIONS = false;

bool EXITONLASTFRAME = false;
