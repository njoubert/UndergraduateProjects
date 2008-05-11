#include "NoteTracker.h"

NoteTracker::NoteTracker() { }

NoteTracker::NoteTracker(int pSize) {
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
	//run peak detection on data
	//create copy of data with everything but peaks zero'd out where initially copy[0] corresponds to data[cursor]
	for (int i=0; i < steps; i++) {
		if (data[cursor] > threshold) {	//replace with peak detection (copy[i])
			hits++;
			hitData.valid = true;
			hitData.step = i;
			hitData.peak = data[cursor];
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
