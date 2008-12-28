#include "PeakDetector.h"

double PeakDetector::demandedHalfWidthFactor = 4;


PeakDetector::PeakDetector() {
}

PeakDetector::~PeakDetector() {
}

void PeakDetector::detectPeaks(CvMat *lum, double threshold, int estLength, vector<peak_t> *peaks, int string) {
	DrumTimer* dTimer = DrumTimer::getInstance();

	double peakHeight, tempHeight;
	for (int start = lum->height-1; start >= 1; start--) {
		tempHeight = peakHeight = 0.0;
		int halfPeakWidth = 0;
		int peakPos;

		if (derivative(lum, start, -1) < 2.0)
			continue;

		tempHeight = cvGet2D(lum, start, 0).val[0];
		int end = start-1;
		for (; end >= 1 && (end-start) < estLength/2; end--) {
			halfPeakWidth++;
			tempHeight = cvGet2D(lum, end, 0).val[0];
			if (tempHeight < threshold) {
				break;	
			} 
			if (tempHeight > peakHeight) {					//found a possible hit
				peakHeight = tempHeight;
				peakPos = end;	
			} else if (derivative(lum, end, -1) < 0 && halfPeakWidth > estLength/demandedHalfWidthFactor) {	//derivative decreased - must have a possibly hit by now
				peak_t peak;
				peakPos = start-halfPeakWidth;
				peak.pos = peakPos;
				peak.width = halfPeakWidth*2;
				//cout << "PeakWidth: " << peak.width << endl;
				peaks->push_back(peak);
				//cout << peaks.size() << endl;
				if (string > -1 && DrumTimer::autoTimer)
					dTimer->peakDetected(string, peakPos);
				break;
			}
		}
		start = end - (estLength/3);
	}
}

void PeakDetector::detectWidths(CvMat *lum, double threshold, double noteWidth, vector<peak_t> *peaks, int string) {
	DrumTimer* dTimer = DrumTimer::getInstance();

	double height;
	int halfPeakWidth, halfPeakWidthFinal;
	bool halfPeakWidthDone, halfPeakWidthOK;

	for (int start = lum->height-1; start >= 1; start--) {
		halfPeakWidth = 0;
		halfPeakWidthDone = false;
		halfPeakWidthOK = false;
		height = cvGet2D(lum, start, 0).val[0];

		if (height < threshold || derivative(lum, start, -1) < 2.0)
			continue;

		int end = start-1;
		for (; end >= 1 && start-end < noteWidth*1.5; end--) {
			halfPeakWidth++;
			if (derivative(lum, end, -1) <= 0 && halfPeakWidth*2 > noteWidth*0.5) {
				int zeroCnt;
				for (zeroCnt = 0; end >= 1 && derivative(lum, end, -1) == 0; zeroCnt++, end--);
				if (!halfPeakWidthDone) {
					halfPeakWidthOK = true;
					halfPeakWidthDone = true;
					halfPeakWidthFinal = halfPeakWidth+zeroCnt/2;
				}
			}
			height = cvGet2D(lum, end, 0).val[0];
			if (height < threshold) {
				halfPeakWidthOK = false;
				break;
			}
		}
		peak_t peak;
		if (halfPeakWidthOK) {
			peak.width = halfPeakWidthFinal*2;
		} else {
			peak.width = start-end;
		}
		peak.pos = start-peak.width/2;
		peaks->push_back(peak);
		//if (string > -1 && DrumTimer::autoTimer)
		//	dTimer->peakDetected(string, peak.pos);

		start = start-peak.width - 1;
	}
}


/**
 * Assumes an image with N rows, 1 column.
 * Finds peaks, calls DrumTimer with peak positions.
 * Clips data under threshold.
 * if writeBack == true, clips actual input.
 */
std::vector<int> PeakDetector::detectPeaksForTimer(CvMat * pLum, double pThreshold, int estLength, bool writeBack, int string) {
	DrumTimer* dTimer = DrumTimer::getInstance();
	
	int peakPos;
	double peakHeight, halfPeakWidth, tempHeight;
	std::vector<int> ret;
	
	for (int start = pLum->height-1; start >= 1; start--) {
			peakPos = 0;
			tempHeight = peakHeight = halfPeakWidth = 0.0;
			
			if (cvGet2D(pLum, start, 0).val[0] < pThreshold) {
				if (writeBack)
					cvSet2D(pLum, start, 0, cvScalar(0));
				continue;
			}
			
			 //This is searching by max value and increasing derivative...
			if (derivative(pLum, start, -1) < 2.0)
				continue;
			 
			tempHeight = cvGet2D(pLum, start, 0).val[0];
			int end = start-1;
			for (; end >= 1 && (end-start) < estLength/2; end--) {
				halfPeakWidth++;
				tempHeight = cvGet2D(pLum, end, 0).val[0];
				if (tempHeight > peakHeight) {					//found a possible hit
					peakHeight = tempHeight;
					peakPos = end;	
				} else if (derivative(pLum, end, -1) < 0 && halfPeakWidth > estLength/demandedHalfWidthFactor) {	//derivative decreased - must have a possibly hit by now
					ret.push_back(peakPos);
					if (string > -1 && DrumTimer::autoTimer)
						dTimer->peakDetected(string, peakPos);
					break;
				} else if (tempHeight < pThreshold) {
					break;	
				} 
			}
			start = end - (estLength/3);							//Offsets forward for no double-peaking
			/*
			//Search on until we go below the threshold
			for (; end >= 1; end--) {
				tempHeight = cvGet2D(pLum, end, 0).val[0];
				if (tempHeight < (120.0/100.0*pThreshold))
					break;
			}
			start = end;
			*/
    }
    if (writeBack && cvGet2D(pLum, 0, 0).val[0] < pThreshold)
		cvSet2D(pLum, 0, 0, cvScalar(0));
	return ret;
}

/**
 * Simple derivative function. calculates [ f(x+step) - f(x) ] / step
 */
inline double PeakDetector::derivative( CvMat* array, int x, int step) {
	return (cvGet2D(array, x+step, 0).val[0] - cvGet2D(array, x, 0).val[0])/abs(step);
}
