#include "NoteTracker.h"

NoteTracker::NoteTracker() { }

NoteTracker::NoteTracker(int pSize) {
	size = pSize;
	cursor = 0;
	start = 0;
	threshold = 0;		//Appropriate threshold TBD...
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
		put(i, cvmGet(notes, 1, i) + get(i));
	}
	return true;
}

bool NoteTracker::shift_add_invalidate(int steps, CvMat* notes) {
	int hits = 0;
	//sweep start + cursor, doing peak detection to find note to hit.
	//invalidate values (set to zero) as you sweep
	start = (start+steps)%size;
	for (int i=0; i < steps; i++) {
		if (data[cursor] > threshold) {
			hits++;
		}
		data[cursor] = 0.0f;
		cursor = (cursor+1)%size;
	}
	//add given notes to current notes
	bool add_OK = add_notes(notes);
	if (!add_OK)
		return false;
	if (hits != 1)
		return false;
	return true;		//There was exactly one hit
}

void NoteTracker::put(int pos, float val) {
	data[(start + pos)%size] = val;
}

float NoteTracker::get(int pos) {
	return data[(start + pos)%size];
}
