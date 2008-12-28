#ifndef NOTEDETECTOR_H_
#define NOTEDETECTOR_H_

#include "global.h"

class NoteDetector {
private:
	static const double noteCountRatio;

public:
	static double noteWidth;
	static int noteCount;

	static noteHits_t detectNotes(vector<peak_t>, vector<peak_t>, vector<peak_t>, vector<peak_t>, int, double);
};

#endif /*NOTEDETECTOR_H_*/