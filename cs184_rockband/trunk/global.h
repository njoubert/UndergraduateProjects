#include "cv.h"
#include "highgui.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>
#include <iostream>
#include <vector>

#define PLOT_WIDTH 300
#define LINE_COLOR CV_RGB(192,192,192)

#define NUM_STRINGS 5

#define CONTROL_POINT_FILE_NAME "tmp_controlPoints.yaml"
#define FRAME_LUMINANCE_FILE_NAME "frameLum.yaml"

#ifndef GLOBALS_H_
#define GLOBALS_H_

const double STRING_THRESHOLD_RATIO = 29.0/30.0;
const double STDDEVWEIGHT = 2.0;

#endif /*GLOBALS_H_*/

