#ifndef PEAKDETECTOR_H_
#define PEAKDETECTOR_H_


#include "global.h"
#include "DrumTimer.h"


class PeakDetector
{
public:
	PeakDetector();
	virtual ~PeakDetector();
	static void detectPeaks(CvMat*, double, int, vector<peak_t> *, int);
	static void detectWidths(CvMat*, double, double, vector<peak_t> *, int);
	static std::vector<int> detectPeaksForTimer( CvMat *, double, int, bool, int );
	static double derivative( CvMat*, int, int);
	static double demandedHalfWidthFactor;
	
};

#endif /*PEAKDETECTOR_H_*/