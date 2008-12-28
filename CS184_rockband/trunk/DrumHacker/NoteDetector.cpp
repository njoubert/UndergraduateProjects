#include "NoteDetector.h"

double NoteDetector::noteWidth = 30.0;  //fix
const double NoteDetector::noteCountRatio = 49.0/50.0;

double calculateSD(int x1, int x2, int x3, int x4, double avg) {
	double retval = 0.0;
	double cnt = 0.0;
	if (x1 != 0) {
		retval += pow((x1-avg), 2);
		cnt++;
	}
	if (x2 != 0) {
		retval += pow((x2-avg), 2);
		cnt++;
	}
	if (x3 != 0) {
		retval += pow((x3-avg), 2);
		cnt++;
	}
	if (x4 != 0) {
		retval += pow((x4-avg), 2);
		cnt++;
	}
	return sqrt(retval/cnt);
}

peak_t findClosest(vector<peak_t> peaks, int center, int regionSize) {
	peak_t closest;
	closest.pos = -1;
	closest.width = 0;

	if (peaks.size() == 0)
		return closest;

	int start = peaks.size()-1;
	for (int i=start; i >=0; i--) {
		if (peaks[i].pos > center || peaks[i].pos < center-regionSize)
		//if (peaks[i].pos != detIndex)
			continue;
		if (abs(peaks[i].pos-center) < abs(closest.pos-center))
			closest = peaks[i];
	}
	return closest;
}

noteHits_t NoteDetector::detectNotes(vector<peak_t> redPeaks, vector<peak_t> yellowPeaks, vector<peak_t> bluePeaks, vector<peak_t> greenPeaks, int detIndex, double noteSpeed) {
	noteHits_t notesToHit;

	peak_t closestNotes[NUM_STRINGS];

	closestNotes[0] = findClosest(redPeaks, detIndex, noteSpeed*2);
	closestNotes[1] = findClosest(yellowPeaks, detIndex, noteSpeed*2);
	closestNotes[2] = findClosest(bluePeaks, detIndex, noteSpeed*2);
	closestNotes[3] = findClosest(greenPeaks, detIndex, noteSpeed*2);

	//cout << closestNotes[0].width << endl;

	notesToHit.hitRed = closestNotes[0].pos >= 0;
	notesToHit.hitYellow = closestNotes[1].pos >= 0;
	notesToHit.hitBlue = closestNotes[2].pos >= 0;
	notesToHit.hitGreen = closestNotes[3].pos >= 0;
	notesToHit.hitBass = false;

	int noteSum = (int) notesToHit.hitRed + (int) notesToHit.hitYellow + (int) notesToHit.hitBlue + (int) notesToHit.hitGreen;

	if (noteSum == 0) {
		notesToHit.pos = -1;
		return notesToHit;
	}

	int maxPos = 0;
	int minPos = 1000;

	for (int i=0; i < NUM_STRINGS; i++) {
		if (closestNotes[i].pos > maxPos)
			maxPos = closestNotes[i].pos;
		if (closestNotes[i].pos < minPos)
			minPos = closestNotes[i].pos;
	}

	if (!notesToHit.hitRed) {
		//closestNotes[0] = findClosest(redPeaks, minPos, noteSpeed/2);
		closestNotes[0] = findClosest(redPeaks, maxPos+noteSpeed/2, noteSpeed/2);
		notesToHit.hitRed = closestNotes[0].pos >= 0;
	}
	if (!notesToHit.hitYellow) {
		closestNotes[1] = findClosest(yellowPeaks, minPos, noteSpeed/2);
		notesToHit.hitYellow = closestNotes[1].pos >= 0;
	}
	if (!notesToHit.hitBlue) {
		closestNotes[2] = findClosest(bluePeaks, minPos, noteSpeed/2);
		notesToHit.hitBlue = closestNotes[2].pos >= 0;
	}
	if (!notesToHit.hitGreen) {
		//closestNotes[3] = findClosest(greenPeaks, minPos, noteSpeed/2);
		closestNotes[3] = findClosest(greenPeaks, maxPos+noteSpeed/2, noteSpeed/2);
		notesToHit.hitGreen = closestNotes[3].pos >= 0;
	}

	noteSum = (int) notesToHit.hitRed + (int) notesToHit.hitYellow + (int) notesToHit.hitBlue + (int) notesToHit.hitGreen;

	int posSum = 0;

	if (notesToHit.hitRed) {
		posSum += closestNotes[0].pos;
	}
	if (notesToHit.hitYellow) {
		posSum += closestNotes[1].pos;
	}
	if (notesToHit.hitBlue) {
		posSum += closestNotes[2].pos;
	}
	if (notesToHit.hitGreen) {
		posSum += closestNotes[3].pos;
	}
	notesToHit.pos = posSum/noteSum;

	//cout << closestRed.pos << ", " << closestRed.width << " -- " << closestYellow.pos << ", " << closestYellow.width << " -- "
	//	<< closestBlue.pos << ", " << closestBlue.width << " -- " << closestGreen.pos << ", " << closestGreen.width << endl;

	int widths[4] = {closestNotes[0].width, closestNotes[1].width, closestNotes[2].width, closestNotes[3].width};
	//compare widths
	double avgWidth = (widths[0]+widths[1]+widths[2]+widths[3])/((double)noteSum);
	double stddev = calculateSD(widths[0], widths[1], widths[2], widths[3], avgWidth);
	//cout << widths[0] << " " << widths[1] << " " << widths[2] << " " << widths[3] << " - " << noteWidth << " | " << avgWidth << " | " << stddev << " - ";

	int wThresh = noteWidth*0.8;
	if (closestNotes[0].width < wThresh)
		notesToHit.hitRed = false;
	if (closestNotes[1].width < wThresh)
		notesToHit.hitYellow = false;
	if (closestNotes[2].width < wThresh)
		notesToHit.hitBlue = false;
	if (closestNotes[3].width < wThresh)
		notesToHit.hitGreen = false;

	wThresh = noteWidth*0.7;
	if (closestNotes[0].width >= avgWidth+stddev && closestNotes[0].width > wThresh)
		notesToHit.hitRed = true;
	if (closestNotes[1].width >= avgWidth+stddev && closestNotes[1].width > wThresh)
		notesToHit.hitYellow = true;
	if (closestNotes[2].width >= avgWidth+stddev && closestNotes[2].width > wThresh)
		notesToHit.hitBlue = true;
	if (closestNotes[3].width >= avgWidth+stddev && closestNotes[3].width > wThresh)
		notesToHit.hitGreen = true;

	if (noteSum > 2) {
		//if (noteSum == 4)
		notesToHit.hitBass = true;
		/*
		int wThresh = noteWidth*0.9;
		if (closestNotes[0].width < avgWidth+0.5*stddev || closestNotes[0].width < wThresh)
			notesToHit.hitRed = false;
		if (closestNotes[1].width < avgWidth+0.5*stddev || closestNotes[1].width < wThresh)
			notesToHit.hitYellow = false;
		if (closestNotes[2].width < avgWidth+0.5*stddev || closestNotes[2].width < wThresh)
			notesToHit.hitBlue = false;
		if (closestNotes[3].width < avgWidth+0.5*stddev || closestNotes[3].width < wThresh)
			notesToHit.hitGreen = false;
		*/
		noteSum = (int) notesToHit.hitRed + (int) notesToHit.hitYellow + (int) notesToHit.hitBlue + (int) notesToHit.hitGreen;
		if (noteSum > 2) {
			//int widths[4] = {closestRed.width, closestYellow.width, closestBlue.width, closestGreen.width};
			//cout << "Here" << endl;
			int maxWidth = 0;
			int hit1, hit2;
			for (int i=0; i < sizeof(widths); i++) {
				if (widths[i] > maxWidth) {
					maxWidth = widths[i];
					hit1 = i;
				}
			}
			widths[hit1] = 0;
			maxWidth = 0;
			for (int i=0; i < sizeof(widths); i++) {
				if (widths[i] > maxWidth) {
					maxWidth = widths[i];
					hit2 = i;
				}
			}
			if (hit1 != 0 && hit2 != 0)
				notesToHit.hitRed = false;
			if (hit1 != 1 && hit2 != 1)
				notesToHit.hitYellow = false;
			if (hit1 != 2 && hit2 != 2)
				notesToHit.hitBlue = false;
			if (hit1 != 3 && hit2 != 3)
				notesToHit.hitGreen = false;
		}
	}

	//sanity check
	noteSum = (int) notesToHit.hitRed + (int) notesToHit.hitYellow + (int) notesToHit.hitBlue + (int) notesToHit.hitGreen;
	//if (noteSum > 2)
	//	cout << "Sanity check failed: detected more than 2 notes to hit!\n";

	if (noteSum > 0) {
		int wSum = 0;

		if (notesToHit.hitRed) {
			wSum += closestNotes[0].width;
		}
		if (notesToHit.hitYellow) {
			wSum += closestNotes[1].width;
		}
		if (notesToHit.hitBlue) {
			wSum += closestNotes[2].width;
		}
		if (notesToHit.hitGreen) {
			wSum += closestNotes[3].width;
		}

		double wAvg = wSum/noteSum;
		//cout << wAvg << endl;

		//if (noteCount <= 100)
		//	noteCount++;
		
		noteWidth = (1.0 - noteCountRatio)*wAvg + (noteCountRatio)*noteWidth;
		//noteWidth = wAvg;
		//cout << noteWidth << endl;
	} else if (!notesToHit.hitBass){
		notesToHit.pos = -1;
	}

	//cout << noteWidth << endl;
	//cout << notesToHit.hitRed << endl;

	//cout << notesToHit.hitRed << " " << notesToHit.hitYellow << " " << notesToHit.hitBlue << " " << notesToHit.hitGreen << " " << notesToHit.hitBass << " -- " << notesToHit.pos << endl;
	
	return notesToHit;
	
}