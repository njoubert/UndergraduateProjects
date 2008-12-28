#ifndef UTILS_H_
#define UTILS_H_

#include "global.h"

bool pointInRect(CvPoint pPoint, CvRect pRect);
CvRect insetRect(CvRect pRect, int pDx, int pDy);

#endif /*UTILS_H_*/