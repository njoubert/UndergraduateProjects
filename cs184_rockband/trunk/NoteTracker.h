#ifndef NOTETRACKER_H_
#define NOTETRACKER_H_

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
	int position, size;
	float *cursor;
	float *data;

public:
	NoteTracker();
	NoteTracker(CvMat* notes);
	virtual ~NoteTracker();

	void increment_cursor();
	void set_cursor_value(float val);
	float get_cursor_value();
};

#endif /*NOTETRACKER_H_*/
