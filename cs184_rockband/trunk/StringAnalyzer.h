#ifndef STRINGANALYZER_H_
#define STRINGANALYZER_H_

#include "global.h"
#include "Convolver.h"
#include "NoteTracker.h"
#include "GuitarTimer.h"

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
private:
	int _stringNumber;	
	NoteTracker* _myTracker;
};

#endif /*STRINGANALYZER_H_*/
