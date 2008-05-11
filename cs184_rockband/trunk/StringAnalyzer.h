#ifndef STRINGANALYZER_H_
#define STRINGANALYZER_H_

#include "global.h"
#include "Convolver.h"
#include "NoteTracker.h"
#include "GuitarTimer.h"
#include "PeakDetector.h"
#include "utils.h"

const double STRING_THRESHOLD_RATIO = 19.0/20.0;

class StringAnalyzer {
public:
	//Each StringAnalyzer is associated with a specific string through its StringNumber counter.
	StringAnalyzer(int, NoteTracker*);
	virtual ~StringAnalyzer();
	
	/**
	 * Takes a raw frame as parameter, processes it, and notifies 
	 * both GuitarTimer and NoteTracker of the results.
	 * */
	void analyzeFrame( IplImage*, int );
    
    static CvRect rectForStringInImageWithWidthAndHeight(int pString, int pWidth, int pHeight);
    static void debugSetStringToDisplay(int pString);

private:
	int _stringNumber;	
	static double _threshold;
	 
	NoteTracker* _myTracker;
    static int _debugStringToDisplay;
};

#endif /*STRINGANALYZER_H_*/
