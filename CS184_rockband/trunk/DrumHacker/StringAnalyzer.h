#ifndef STRINGANALYZER_H_
#define STRINGANALYZER_H_

#include "global.h"
#include "Convolver.h"
#include "DrumTimer.h"
#include "PeakDetector.h"
#include "utils.h"

class StringAnalyzer {
public:
	//Each StringAnalyzer is associated with a specific string through its StringNumber counter.
	StringAnalyzer(int);
	virtual ~StringAnalyzer();
	
	static bool startup;
	/**
	 * Takes a raw frame as parameter, processes it
	 * */
	vector<peak_t> analyzeFrame( IplImage*, int, double );

    static CvRect rectForStringInImageWithWidthAndHeight(int pString, int pWidth, int pHeight);
    static void debugSetStringToDisplay(int pString);
	static void startupDone();

private:
	int _stringNumber;
	vector<peak_t> storedPeaks;
	static double _threshold;
	static double _thresholdMax;
	 
    static int _debugStringToDisplay;
};

#endif /*STRINGANALYZER_H_*/