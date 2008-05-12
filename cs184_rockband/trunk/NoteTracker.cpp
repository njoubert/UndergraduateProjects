#include "NoteTracker.h"

NoteTracker::NoteTracker(int string) {
	_string = string;
	size = -1;
}

void NoteTracker::initialize(int pSize) {
	if (size > 0)
		return;
	size = pSize;
	cursor = 0;
	start = 0;
	threshold = 0;		//appropriate threshold TBD...
	hitData.valid = false;
	data = new float[size];
	for (int i=0; i < size; i++) {
		data[i] = 0.0f;	
	}
}

NoteTracker::~NoteTracker() {
	delete [] data;
}

bool NoteTracker::add_notes(CvMat* notes) {
	if (notes->height != size)
		return false;
	
	for (int i=0; i < size; i++) {
		put(i, cvGet2D(notes, i, 0).val[0] + get(i));	//inverts image
	}
	return true;
}

bool NoteTracker::shift_add_invalidate(int steps, CvMat* notes, int estLength) {
	int hits = 0;
	//sweep start + cursor, doing peak detection to find note to hit.
	//invalidate values (set to zero) as you sweep
	assert(size == notes->height);
	start = (start+steps)%size;
	CvMat* copy = cvCreateMat(size, 1, notes->type);
	cvFlip(notes,copy,0);
	//run peak detection on data
	vector<int> peaks = PeakDetector::detectPeaksForTimer(copy, threshold, estLength, false, -1);
	for (int i=0; i < steps; i++) {
		for (unsigned int j=0; j < peaks.size(); j++) {
			if (cursor == peaks[j]) {
				hits++;
				hitData.valid = true;
				hitData.step = i;
				hitData.peak = data[cursor];
			}
		}
		data[cursor] = 0.0f;
		cursor = (cursor+1)%size;
	}
	//add given notes to current notes
	bool add_OK = add_notes(copy);
	
	plotMe();
	
	if (!add_OK)
		return false;
	if (hits != 1) {
		hitData.valid = false;
		return false;
	}
	return true;		//there was exactly one hit
}

void NoteTracker::put(int pos, float val) {
	data[(start + pos)%size] = val;
}

float NoteTracker::get(int pos) {
	return data[(start + pos)%size];
}

void NoteTracker::plotMe() {
	    IplImage* lPlot = cvCreateImage(cvSize(PLOT_WIDTH, size), IPL_DEPTH_8U, 1);
	    cvZero(lPlot);
	    
		CvMat *copy = cvCreateMat(size, 1, CV_32FC1);
		for (int i=0; i < size; i++) {
			cvmSet(copy, i, 0, get(cursor+i));
		}
		cvNormalize( copy, copy,0, PLOT_WIDTH, CV_MINMAX);
	    for (int i = 0; i < size - 1; i++) {
	        cvLine(lPlot, cvPoint(cvGet2D(copy, i, 0).val[0], i), cvPoint(cvGet2D(copy, i+1, 0).val[0], i+1), LINE_COLOR);
	    }
		char name[10];
	   sprintf(name, "Buffer %d", _string+1); 
	   cvFlip(lPlot, lPlot,0);
	   cvShowImage(name, lPlot);
       
}
