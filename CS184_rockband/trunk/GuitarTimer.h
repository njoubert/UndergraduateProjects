#ifndef GUITARTIMER_H_
#define GUITARTIMER_H_

#include "global.h"

#define DEFAULT_AMOUNT_OF_STRINGS 5

using namespace std;

typedef struct PeakList {
		int peakCount;
		double absoluteTime;
		vector<int> peaks;	
} PeakList;

class GuitarTimer {
private:

	GuitarTimer();
	
	static GuitarTimer *sharedGuitarTimer;
	double _deltaT;	//average elapsed time
	double _deltaP;	//average moved pixels
	vector<PeakList*> peaksList; //This should contain a peak per string.
	
public:
	static bool autoTimer;
	static GuitarTimer* getInstance();
	virtual ~GuitarTimer();
	void frameArrived();	//call this when a frame arrives - it sets the internal current frame arrival time.
	void frameDone();		//call this when frame processing is completed  it resets string peak counters for next frame.
	void startString(int,int);
	void endString(int);
	void peakDetected(int,int);	//This function does ALL the work...
	double getDeltaT();
	double getDeltaP();
	void setDeltaT(double);
	void setDeltaP(double);

};

#endif /*GUITARTIMER_H_*/
