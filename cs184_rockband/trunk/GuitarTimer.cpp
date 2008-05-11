#include "GuitarTimer.h"

GuitarTimer* GuitarTimer::sharedGuitarTimer = NULL;

GuitarTimer* GuitarTimer::getInstance() {
	if (NULL == sharedGuitarTimer) {
		sharedGuitarTimer = new GuitarTimer();
	}
	return sharedGuitarTimer;	
}

void GuitarTimer::frameArrived() {
	
}	
void GuitarTimer::frameDone() {
	
}

void GuitarTimer::startString(int string) {
	assert(string < DEFAULT_AMOUNT_OF_STRINGS);
	
	
	
}
void GuitarTimer::endString(int string) {
	assert(string < DEFAULT_AMOUNT_OF_STRINGS);
}

void GuitarTimer::peakDetected(int string) {
	//Need a counter per string
	
	//Need to store EACH hit per string
}

double GuitarTimer::getDeltaT() { return _deltaT; }

double GuitarTimer::getDeltaP() { return _deltaP; }

GuitarTimer::GuitarTimer() {
	_deltaT = 0.5; //Seed em!
	_deltaP = 20; //Seed em!
	peaks.resize(DEFAULT_AMOUNT_OF_STRINGS);
	for (int i = 0; i < DEFAULT_AMOUNT_OF_STRINGS; i++) {
		peaks[i] = new PeakList();
		peaks[i]->peakCount = 0;
		peaks[i]->absoluteTime = 0;
	}
}

GuitarTimer::~GuitarTimer() {
	sharedGuitarTimer = NULL;
}
