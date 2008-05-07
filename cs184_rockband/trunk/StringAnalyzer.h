#ifndef STRINGANALYZER_H_
#define STRINGANALYZER_H_

#include "cv.h"

class StringAnalyzer
{
public:
	StringAnalyzer();
	virtual ~StringAnalyzer();
	
	/**
	 * Takes a raw frame as parameter - 
	 * */
	void analyzeFrame( IplImage* pImage );
	
	
};

#endif /*STRINGANALYZER_H_*/
