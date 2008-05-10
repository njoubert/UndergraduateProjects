#ifndef NOTETRACKER_H_
#define NOTETRACKER_H_

#include "global.h"

/**
 * NOTETRACKER CLASS
 * 
 * Tracks Notes On A Single String
 * 
 * 	This class implements a circular buffer with a movable cursor.
 * 	
 * The buffer itself stored distributions of point luminance values
 * in correspondence with where we believe notes are.
 * 
 * The cursor points to the current "lowest" point in our buffer, and thus
 * in time. The point where the cursor is corresponds to the current state
 * the guitar needs to be in.
 * 
 * The buffer can be advanced by a certain amount of points.
 * The cursor then sweeps over these points, possibly finding a note and
 * emitting it. (This is intentionally left vague...)
 * 
 * 
 */
class NoteTracker
{
private:
	int size;
	int cursor;
	int start;
	float threshold;
	float *data;
	
	void put(int pos, float val);
	float get(int pos);

public:
	NoteTracker();
	NoteTracker(int size);
	virtual ~NoteTracker();
	
	bool add_notes(CvMat*);			//add current matrix to the buffer
	bool shift_add_invalidate(int, CvMat*);
	
};

#endif /*NOTETRACKER_H_*/
