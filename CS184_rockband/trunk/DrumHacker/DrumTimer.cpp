#include "DrumTimer.h"


DrumTimer* DrumTimer::sharedDrumTimer = NULL;
bool DrumTimer::autoTimer = true;

vector<PeakList*> peaksList;
clock_t startTime,currentTime;
int currentString = -1;
bool calculate = false;
vector<int> temp;
int pixel_differences=0;
int pixeldiffcount=0;
long time_differences;
const double THRESHOLD_RATIO = 99.0/100.0;

double DrumTimer::thresh_ratio = 99.0/100.0;


DrumTimer* DrumTimer::getInstance() {
	if (NULL == sharedDrumTimer) {
		sharedDrumTimer = new DrumTimer();
	}
	return sharedDrumTimer;	
}


void DrumTimer::calculateDeltaP(vector<peak_t> peaks, vector<peak_t> storedPeaks) {
	if (peaks.size() > 0) {
		if (peaks.size() == storedPeaks.size()) {//do timing
			int pixelDiff = 0;
			int diff;
			unsigned int i;
			for (i = 0; i < peaks.size(); i++) {
				diff = peaks[i].pos - storedPeaks[i].pos;
				if (diff > 0) {
					pixelDiff += diff;
				} else {
					break;
				}
			}
			if (pixelDiff != 0) {
				_deltaP = (1.0 - thresh_ratio)*pixelDiff/i + (thresh_ratio)*_deltaP;
				//cout << _deltaP << endl;
			}
		}
	}
}
/*
 *
 */
void DrumTimer::frameArrived() {
	currentTime = clock ();
	//printf("frame has arrived\n");
	calculate = true;
}

/*
 *
 */
void DrumTimer::frameDone() {
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
void DrumTimer::startString(int string, int value) {
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

			if (_deltaP > DELTAP_MAX)  //added upper limit to deltaP
				_deltaP = DELTAP_MAX;
			
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
void DrumTimer::endString(int string) {
	assert(string < DEFAULT_AMOUNT_OF_STRINGS);
	//assert(peaksList[string]->peakCount == peaksList[string]->peaks.size());
	
	
	peaksList[string]->peaks = temp;
	temp.clear();
}

/*
 *
 */
void DrumTimer::peakDetected(int string, int value) {
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


double DrumTimer::getDeltaT() { return _deltaT; }

double DrumTimer::getDeltaP() { return _deltaP; }

void DrumTimer::setDeltaT(double t) { _deltaT = t; }

void DrumTimer::setDeltaP(double p) { _deltaP = p; }


DrumTimer::DrumTimer() {
	_deltaT = 0.5; //Seed em!
	_deltaP = 7; //Seed em!
	peaksList.resize(DEFAULT_AMOUNT_OF_STRINGS);
	for (int i = 0; i < DEFAULT_AMOUNT_OF_STRINGS; i++) {
		peaksList[i] = new PeakList();
		peaksList[i]->peakCount = 0;
		peaksList[i]->absoluteTime = 0;
	}
	startTime = clock ();
}

DrumTimer::~DrumTimer() {
	sharedDrumTimer = NULL;
}


