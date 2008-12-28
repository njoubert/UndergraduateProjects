#include "global.h"
#include "PeakDetector.h"

/*Stores relevant data if a hit is detected.*/
struct hit_data
{
	bool valid;  //true if data is relevant to current hit
	int step;	 //point at which hit was detected
	float peak;  //the value of the peak corresponding to the hit
};

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
	static int cursor;
	int start;
	int _string;
	float *data;
	hit_data hitData;
	void put(int pos, float val);
	float get(int pos);


public:
	double _threshold;
	static double _thresholdMax;
	NoteTracker(int string);
	virtual ~NoteTracker();
	void initialize(int);
	bool add_notes(CvMat*);			//add current matrix to the buffer
	bool shift_add_invalidate(int, CvMat*, int);
	bool hit();
	void plotMe(vector<int>, int);
	static void incCursor();
	static void decCursor();
	static int getCursor();
	static void setCursor(int);
};
