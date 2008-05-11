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
			
			
			//Do it with derivative... but noise fucks you over!
			double startDerivative = derivative(pLum, start, -1);
			if (startDerivative < 1.0)
				continue;
			
			int end = start;
			for (; end >= 1; end--) {
				double endDerivative = derivative(pLum, end, -1);
				if (endDerivative <= 0) {
					peakHeight = cvGet2D(pLum, end, 0).val[0];
					peakPos = end;	
					gTimer->peakDetected(peakPos);
					ret.push_back(peakPos);
					break;
				}	
			}
			
			start = end;//n - estLength/2;
			
			/**
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
			*/
    
    }
	return ret;
}

/**
 * Simple derivative function. calculates [ f(x+step) - f(x) ] / step
 */
inline double PeakDetector::derivative( CvMat* array, int x, int step) {
	return (cvGet2D(array, x+step, 0).val[0] - cvGet2D(array, x, 0).val[0])/abs(step);
}

