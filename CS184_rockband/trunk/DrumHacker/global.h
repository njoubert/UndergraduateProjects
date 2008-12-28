#include "cv.h"
#include "highgui.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <string>
#include <time.h>
#include <windows.h>

#include <NIDAQmx.h>

using namespace std;

#define NUM_STRINGS 4
#define PLOT_WIDTH 300
#define SIGNAL_LENGTH 1000/15

#define LINE_COLOR CV_RGB(192,192,192)

#ifndef GLOBALS_H_
#define GLOBALS_H_

typedef struct {
	int pos;
	int width;

} peak_t;


typedef struct {
	int pos;
	int timeToHit;
	bool hitRed;
	bool hitYellow;
	bool hitBlue;
	bool hitGreen;
	bool hitBass;
} noteHits_t;


const double STRING_THRESHOLD_RATIO = 29.0/30.0;
const double STDDEVWEIGHT = 0.50;

#endif /*GLOBALS_H_*/

