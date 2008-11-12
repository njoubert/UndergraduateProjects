#include "GuitarTimer.h"



GuitarTimer* GuitarTimer::sharedGuitarTimer = NULL;
bool GuitarTimer::autoTimer = true;

vector<PeakList*> peaksList;
clock_t startTime,currentTime;
int currentString = -1;
bool calculate = false;
vector<int> temp;
int pixel_differences=0;
int pixeldiffcount=0;
long time_differences;
const double THRESHOLD_RATIO = 99.0/100.0;

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
	currentTime = clock ();
	//printf("frame has arrived\n");
	calculate = true;
}

/*
 *
 */
void GuitarTimer::frameDone() {
	//printf("end %d\n", endwait-startwait);
	time_differences = (currentTime - startTime);
	_deltaT = (1.0 - THRESHOLD_RATIO)*time_differences + (THRESHOLD_RATIO)*_deltaT;
	
	startTime = currentTime;
	calculate = false;
	//printf("frame is done\n");
}

/*
 *
 */
void GuitarTimer::startString(int string, int value) {
	assert(string < DEFAULT_AMOUNT_OF_STRINGS);
	peaksList[string]->peakCount += 1;
	peaksList[string]->absoluteTime += currentTime;
	//peaksList[string]->peaks.push_back(value);
	vector<int>::iterator it;
	bool notstored = true;
	for ( it=peaksList[string]->peaks.begin() ; it < peaksList[string]->peaks.end(); it++ ){
		//printf("from list: %d compare to value %d\n",*it,value);
		if(*it < value){
			//it has moved and this is it
			pixel_differences += (value - *it);
			pixeldiffcount += 1;
			//store in a new peaks
			int ave_pixel;
			if(pixeldiffcount == 0){
				ave_pixel = pixel_differences/1;
			}else{
				ave_pixel = pixel_differences/pixeldiffcount;
			}
			_deltaP = (1.0 - THRESHOLD_RATIO)*ave_pixel + (THRESHOLD_RATIO)*_deltaP;
			
			peaksList[string]->peaks.erase(it);
			temp.push_back(value);
			notstored = false;
			break;
		}
	}
	//printf("end\n");
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
	//assert(peaksList[string]->peakCount == peaksList[string]->peaks.size());
	
	
	peaksList[string]->peaks = temp;
	temp.clear();
}

/*
 *
 */
void GuitarTimer::peakDetected(int string, int value) {
	//Need a counter per string
	assert(calculate);
	if(currentString != string){
		if(currentString != -1){
			endString(currentString);
		}
		peaksList[string]->peakCount = 0;
		currentString = string;
	}
	startString(string,value);
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

void GuitarTimer::setDeltaT(double t) { _deltaT = t; }

void GuitarTimer::setDeltaP(double p) { _deltaP = p; }


GuitarTimer::GuitarTimer() {
	_deltaT = 0.5; //Seed em!
	_deltaP = 20; //Seed em!
	peaksList.resize(DEFAULT_AMOUNT_OF_STRINGS);
	for (int i = 0; i < DEFAULT_AMOUNT_OF_STRINGS; i++) {
		peaksList[i] = new PeakList();
		peaksList[i]->peakCount = 0;
		peaksList[i]->absoluteTime = 0;
	}
	startTime = clock ();
}

GuitarTimer::~GuitarTimer() {
	sharedGuitarTimer = NULL;
}


