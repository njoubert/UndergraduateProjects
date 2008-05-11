#include "NoteTracker.h"

NoteTracker::NoteTracker() { }

NoteTracker::NoteTracker(int pSize) {
	size = pSize;
	cursor = 0;
	start = 0;
	threshold = 0;		//appropriate threshold TBD...
	estLenth = 1; 		//TBD...
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
	if (notes->cols != size)
		return false;
	
	for (int i=0; i < size; i++) {
		put(i, cvmGet(notes, 1, size-i-1) + get(i));	//inverts image
	}
	return true;
}

bool NoteTracker::shift_add_invalidate(int steps, CvMat* notes) {
	int hits = 0;
	//sweep start + cursor, doing peak detection to find note to hit.
	//invalidate values (set to zero) as you sweep
	start = (start+steps)%size;
	CvMat *copy = cvCreateMat(size, 1, CV_32FC1);
	for (int i=0; i < size; i++) {
		cvmSet(copy, i, 1, get(cursor+i));
	}
	//run peak detection on data
	vector<int> peaks = PeakDetector::detectPeaksForTimer(copy, threshold, estLength, false, -1);
	for (int i=0; i < steps; i++) {
		for (int j=0; j < peaks.size; j++) {
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
	bool add_OK = add_notes(notes);
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
