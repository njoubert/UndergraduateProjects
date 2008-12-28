#ifndef DRUMTIMER_H_
#define DRUMTIMER_H_

#include "global.h"

#define DEFAULT_AMOUNT_OF_STRINGS 4
#define DELTAP_MAX 30

typedef struct PeakList {
		int peakCount;
		double absoluteTime;
		vector<int> peaks;	
} PeakList;

class DrumTimer {
private:

	DrumTimer();
	static double thresh_ratio;
	static DrumTimer *sharedDrumTimer;
	double _deltaT;	//average elapsed time
	double _deltaP;	//average moved pixels
	vector<PeakList*> peaksList; //This should contain a peak per string.
	
public:
	static bool autoTimer;
	static DrumTimer* getInstance();
	virtual ~DrumTimer();
	void frameArrived();	//call this when a frame arrives - it sets the internal current frame arrival time.
	void frameDone();		//call this when frame processing is completed  it resets string peak counters for next frame.
	void startString(int,int);
	void endString(int);
	void peakDetected(int,int);
	void calculateDeltaP(vector<peak_t>, vector<peak_t>);
	double getDeltaT();
	double getDeltaP();
	void setDeltaT(double);
	void setDeltaP(double);

};

#endif /*DRUMTIMER_H_*/
