#ifndef PEAKDETECTOR_H_
#define PEAKDETECTOR_H_

#include "global.h"
#include "GuitarTimer.h"

class PeakDetector
{
public:
	PeakDetector();
	virtual ~PeakDetector();
	static std::vector<int> detectPeaksForTimer( CvMat *, double, int, bool, int );
	static double derivative( CvMat*, int, int);
	static double demandedHalfWidthFactor;
	
};

#endif /*PEAKDETECTOR_H_*/
