#include "PeakDetector.h"

PeakDetector::PeakDetector() {
}

PeakDetector::~PeakDetector() {
}

/**
 * Assumes an image with N rows, 1 column.
 * Finds peaks, calls GuitarTimer with peak positions.
 * Clips data under threshold.
 * if writeBack == true, clips actual input.
 */
std::vector<int> PeakDetector::detectPeaksForTimer(CvMat * pLum, double pThreshold, int estLength, bool writeBack) {
	GuitarTimer* gTimer = GuitarTimer::getInstance();
	
	int peakPos;
	double peakHeight, temp;
	std::vector<int> ret;
	
	for (int start = pLum->height-1; start >= 0; start--) {
			peakPos = 0;
			temp = peakHeight = 0.0;
			
			if (cvGet2D(pLum, start, 0).val[0] < pThreshold) {
				if (writeBack)
					cvSet2D(pLum, start, 0, cvScalar(0));
				continue;
			}
			
			
			
			
			
			 //This is searching purely by max value...
			temp = cvGet2D(pLum, start, 0).val[0];
			int end = start-1;
			for (; end >= 0; end--) {
				temp = cvGet2D(pLum, end, 0).val[0];
				if (temp > peakHeight && (end-start) < estLength) {
					peakHeight = temp;
					peakPos = end;	
				} else if (temp < pThreshold) {
					break;	
				}	
			} 

			gTimer->peakDetected(peakPos);
			ret.push_back(peakPos);
			start = end;
    
    }
	return ret;
}
