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
std::vector<int> PeakDetector::detectPeaksForTimer(CvMat * pLum, double pThreshold, int estLength, bool writeBack, int string) {
	GuitarTimer* gTimer = GuitarTimer::getInstance();
	
	int peakPos;
	double peakHeight, tempHeight;
	std::vector<int> ret;
	
	for (int start = pLum->height-1; start >= 1; start--) {
			peakPos = 0;
			tempHeight = peakHeight = 0.0;
			
			if (cvGet2D(pLum, start, 0).val[0] < pThreshold) {
				if (writeBack)
					cvSet2D(pLum, start, 0, cvScalar(0));
				continue;
			}
			
			 //This is searching by max value and increasing derivative...
			 
			double startDerivative = derivative(pLum, start, -1);
			if (startDerivative < 2.0)
				continue;
			 
			tempHeight = cvGet2D(pLum, start, 0).val[0];
			int end = start-1;
			for (; end >= 1 && (end-start) < estLength/2; end--) {
				tempHeight = cvGet2D(pLum, end, 0).val[0];
				if (tempHeight > peakHeight) {	//found a possible hit
					peakHeight = tempHeight;
					peakPos = end;	
				} else if (derivative(pLum, end, -1) <= 0) {	//derivative decreased - must have a possibly hit by now
					break;	
				} else if (tempHeight < pThreshold) {
					break;	
				} 
			}
			ret.push_back(peakPos);
			//if (string > -1)
			//	gTimer->peakDetected(string, peakPos);
			start = end - (estLength/2.2);
			
    
    }
	return ret;
}

/**
 * Simple derivative function. calculates [ f(x+step) - f(x) ] / step
 */
inline double PeakDetector::derivative( CvMat* array, int x, int step) {
	return (cvGet2D(array, x+step, 0).val[0] - cvGet2D(array, x, 0).val[0])/abs(step);
}

