#ifndef STRINGANALYZER_H_
#define STRINGANALYZER_H_

#include "cv.h"

class StringAnalyzer
{
public:
	//Each StringAnalyzer is associated with a specific string through its StringNumber counter.
	StringAnalyzer(int pStringNumber);
	virtual ~StringAnalyzer();
	
	/**
	 * Takes a raw frame as parameter, processes it, and notifies 
	 * both GuitarTimer and NoteTracker of the results.
	 * */
	void analyzeFrame( IplImage* pImage );
private:
	int _stringNumber;	
};

#endif /*STRINGANALYZER_H_*/
