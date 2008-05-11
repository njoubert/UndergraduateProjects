#include "GuitarTimer.h"



GuitarTimer* GuitarTimer::sharedGuitarTimer = NULL;
vector<PeakList*> peaksList;
clock_t startTime,currentTime;
int currentString = -1;
bool calculate = false;
vector<int> temp;
int pixel_differences;
int pixeldiffcount;
int time_differences;
int timediffcount;

GuitarTimer* GuitarTimer::getInstance() {
	if (NULL == sharedGuitarTimer) {
		sharedGuitarTimer = new GuitarTimer();
	}
	return sharedGuitarTimer;	
}

/*
 *
 */
void GuitarTimer::frameArrived() {
	currentTime = clock () +  1* CLOCKS_PER_SEC ;
	//printf("start %d\n", startwait);
	calculate = true;
}

/*
 *
 */
void GuitarTimer::frameDone() {
	//printf("end %d\n", endwait-startwait);
	calculate = false;
}

/*
 *
 */
void GuitarTimer::startString(int string, int value) {
	assert(string < DEFAULT_AMOUNT_OF_STRINGS);
	peaksList[string]->peakCount += 1;
	peaksList[string]->absoluteTime += currentTime;
	peaksList[string]->peaks.push_back(value);
	vector<int>::iterator it;
	bool notstored = true;
	for ( it=peaksList[string]->peaks.begin() ; it < peaksList[string]->peaks.end(); it++ ){
		if(*it < value){
			//it has moved and this is it
			pixel_differences += (value - *it);
			pixeldiffcount += 1;
			//store in a new peaks
			temp.push_back(value);
			notstored = false;
			break;
		}
	}
	if(notstored){
		//add to new one
		temp.push_back(value);
	}
	
}
/* Note: what if one frame is completely different from another frame, how do you tackle deltas
 */


/*
 *
 */
void GuitarTimer::endString(int string) {
	assert(string < DEFAULT_AMOUNT_OF_STRINGS);
	assert(peaksList[string]->peakCount == peaksList[string]->peaks.size());
	peaksList[string]->peaks = temp;
	temp.empty();
}

/*
 *
 */
void peakDetected(int string, int value) {
	//Need a counter per string
	assert(calculate);
	if(currentString != string){
		if(currentString != -1){
			GuitarTimer::getInstance()->endString(currentString);
		}
		peaksList[string]->peakCount = 0;
		currentString = string;
	}
	GuitarTimer::getInstance()->startString(string,value);
	//peaks[string]->peakCount += 1;
	//Need to store EACH hit per string
}
/*
void peakDetected(int string, vector<int> peakList) {
	//Need a counter per string
	int i;
	for(i=0; i < peakList.size(); i++){
		peaksList[string]->peakCount += 1;
		peaksList[string]->absoluteTime += 1;
		peaksList[string]->peaks.push_back(peakList[i]);
		
	}
}*/
	
	//Need to store EACH hit per string


double GuitarTimer::getDeltaT() { return _deltaT; }

double GuitarTimer::getDeltaP() { return _deltaP; }

GuitarTimer::GuitarTimer() {
	_deltaT = 0.5; //Seed em!
	_deltaP = 20; //Seed em!
	peaksList.resize(DEFAULT_AMOUNT_OF_STRINGS);
	for (int i = 0; i < DEFAULT_AMOUNT_OF_STRINGS; i++) {
		peaksList[i] = new PeakList();
		peaksList[i]->peakCount = 0;
		peaksList[i]->absoluteTime = 0;
	}
	startTime = clock () +  1* CLOCKS_PER_SEC;
}

GuitarTimer::~GuitarTimer() {
	sharedGuitarTimer = NULL;
}


