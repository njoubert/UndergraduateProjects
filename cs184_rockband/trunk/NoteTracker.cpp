#include "NoteTracker.h"

NoteTracker::NoteTracker()
{
}

NoteTracker::NoteTracker(CvMat *notes)
{
	size = notes->cols;
	position = 0;
	data = new float[size];
	for (int i=0; i < size; i++) {
		data[i] = cvmGet(notes, 1, i);
	}
	cursor = data[position];
}

NoteTracker::~NoteTracker()
{
	delete[size] data;
}

void NoteTracker::increment_cursor()
{
	if (position+1 >= size) {
		position = 0;
	} else {
		position++;
		cursor = data[position];
	}
}

void NoteTracker::set_cursor_value(float val)
{
	data[position] = val;
	cursor = data[position];
}

float NoteTracker::get_cursor_value()
{
	return *cursor;
}