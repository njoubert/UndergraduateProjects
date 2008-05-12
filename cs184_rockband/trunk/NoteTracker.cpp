#include "NoteTracker.h"

int NoteTracker::cursor = 100;

NoteTracker::NoteTracker(int string) {
	_string = string;
	size = -1;
}

void NoteTracker::initialize(int pSize) {
	if (size > 0)
		return;
	size = pSize;
	start = 0;
	_thresholdMax = _threshold = 20.0;		//appropriate threshold TBD...
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
	CvMat* notes_normalized = cvCreateMat(notes->rows, notes->cols, notes->type);
	//cvNormalize(notes, notes_normalized, 0.0, 1.0,CV_C);
	cvFlip(notes, notes_normalized, 0);
	if (notes->height != size)
		return false;
	
	double newVal;
	for (int i=0; i < size; i++) {
		newVal = ((double)cvGet2D(notes_normalized, i, 0).val[0] + (double)get(i))/2.0;
		put(i, newVal);	//inverts image
	}
	return true;
}

bool NoteTracker::shift_add_invalidate(int steps, CvMat* notes, int estLength) {
	int hits = 0;
	//sweep start + cursor, doing peak detection to find note to hit.
	//invalidate values (set to zero) as you sweep
	assert(size == notes->height);
	CvMat* copy = cvCreateMat(size, 1, notes->type);
	for (int i=0; i<size; i++) {
		cvSet2D(copy, i, 0, cvScalar(get(i)));
	}
	cvFlip(copy,copy,0);
	
    CvScalar cvMean, cvStddev;
    cvAvgSdv(copy,&cvMean,&cvStddev);
    double lThreshold = cvMean.val[0] + 1.3*cvStddev.val[0];
	double newThreshold = (1.0 - STRING_THRESHOLD_RATIO)*lThreshold + (STRING_THRESHOLD_RATIO)*_threshold;
	_threshold = newThreshold;
	if (newThreshold > _thresholdMax)
		_thresholdMax = newThreshold;
	if (newThreshold < 65.0/100.0*_thresholdMax) {
		_threshold = 65.0/100.0*_thresholdMax;
	} else {
		_threshold = newThreshold;
	}
	
	//run peak detection on data
	vector<int> peaks = PeakDetector::detectPeaksForTimer(copy, _threshold, estLength, false, -1);
	for (unsigned int i = 0; i < peaks.size(); i++) {
		peaks[i] = copy->height - peaks[i];
	}
	
	for (int i=0; i < steps; i++) {
		for (unsigned int j=0; j < peaks.size(); j++) {
			if ((cursor+i) == peaks[j]) {
				hits++;
				hitData.valid = true;
				hitData.step = i;
				hitData.peak = get(cursor);
			}
		}
		put(cursor+i, 0.0f);
	}
	
	plotMe(peaks, hits);
	start = (start+steps)%size;
	//add given notes to current notes
	
	bool add_OK = add_notes(notes);
	
	if (!add_OK)
		return false;
	//if (hits != 1) {
	//	hitData.valid = false;
	//	return false;
	//}
	return true;		//there was exactly one hit
}

bool NoteTracker::hit() {
	bool temp = hitData.valid; 
	hitData.valid = false;
	return temp;
}

inline void NoteTracker::put(int pos, float val) {
	data[(start + pos)%size] = val;
}

inline float NoteTracker::get(int pos) {
	return data[(start + pos)%size];
}

void NoteTracker::plotMe(vector<int> peaks, int numHitsUnderCursor) {
	    IplImage* lPlot = cvCreateImage(cvSize(PLOT_WIDTH, size), IPL_DEPTH_8U, 1);
	    cvZero(lPlot);
	    
		CvMat *copy = cvCreateMat(size, 1, CV_32FC1);
		for (int i=0; i < size; i++) {
			cvmSet(copy, i, 0, get(i));
		}

	    for (int i = 0; i < size - 1; i++) {
	        cvLine(lPlot, cvPoint(cvGet2D(copy, i, 0).val[0], i), cvPoint(cvGet2D(copy, i+1, 0).val[0], i+1), LINE_COLOR);
	    }
	    for (unsigned int i = 0; i < peaks.size(); i++) {
        	cvLine(lPlot, cvPoint(0, peaks[i]), cvPoint(255, peaks[i]), CV_RGB(192,192,192));
        }
	    cvLine(lPlot, cvPoint(_threshold, 0), cvPoint(_threshold, cvGetSize(lPlot).height-1), CV_RGB(192,192,192));
	    cvLine(lPlot, cvPoint(0, cursor), cvPoint(300, cursor), CV_RGB(192,192,192));
	    cvFlip(lPlot, lPlot,0);
	    
	    CvFont font;
        cvInitFont( &font, CV_FONT_HERSHEY_PLAIN, 1.0f, 1.0f);
        char text[500];
        int pos = 20;
        sprintf(text, "start: %d", start);
        cvPutText( lPlot, text, cvPoint(5,pos), &font, cvScalar(255.0,255.0,255.0,0.0) );
        pos += 15;
        sprintf(text, "cursor: %d", cursor);
        cvPutText( lPlot, text, cvPoint(5,pos), &font, cvScalar(255.0,255.0,255.0,0.0) );
        pos += 15;
        sprintf(text, "size: %d", size);
        cvPutText( lPlot, text, cvPoint(5,pos), &font, cvScalar(255.0,255.0,255.0,0.0) );
        pos += 15;
        sprintf(text, "hits: %d", numHitsUnderCursor);
        cvPutText( lPlot, text, cvPoint(5,pos), &font, cvScalar(255.0,255.0,255.0,0.0) );
        pos += 15;
 	    
		char name[10];
	   sprintf(name, "Buffer %d", _string+1); 
	   
	   cvShowImage(name, lPlot);
       
}

void NoteTracker::incCursor() {
	cursor += 1;
}
void NoteTracker::decCursor() {
	cursor -= 1;	
}
int NoteTracker::getCursor() {
	return cursor;
}
void NoteTracker::setCursor(int pCursor) {
	cursor = pCursor;	
}
