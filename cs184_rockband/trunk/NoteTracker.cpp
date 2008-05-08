#include "NoteTracker.h"

NoteTracker::NoteTracker() { }

NoteTracker::NoteTracker(int pSize) {
	size = pSize;
	cursor = 0;
	start = 0;
	data = new float[size];
	for (int i=0; i<size; i++) {
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

bool NoteTracker::increment_cursor() {
	if (cursor+1 >= size)
		return false; 		//can't sweet cursor further than we know.
	cursor++;
	return true;
}

void NoteTracker::set_cursor_value(float val)
{
	data[cursor] = val;
}

float NoteTracker::get_cursor_value()
{
	return data[cursor];
}

bool NoteTracker::shift_add_invalidate(int steps, CvMat* notes) {
	//sweep start + cursor, doing peak detection to find note to hit.
	
	//invalidate values (set to zero) as you sweep
	
	//add given notes to current notes
	
}
void NoteTracker::put(int pos, float val) {
	data[(start + pos)%size] = val;
}
float NoteTracker::get(int pos) {
	return data[(start + pos)%size];
}
