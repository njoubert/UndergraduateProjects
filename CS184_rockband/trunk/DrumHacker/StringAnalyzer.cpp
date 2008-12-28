#include "StringAnalyzer.h"


double StringAnalyzer::_threshold = 0;
double StringAnalyzer::_thresholdMax = 0;
bool StringAnalyzer::startup = true;

StringAnalyzer::StringAnalyzer(int pStringNumber) {
	_stringNumber = pStringNumber;
	_threshold = 0;
}

void StringAnalyzer::startupDone() {
	startup = false;
}

StringAnalyzer::~StringAnalyzer() { }

CvRect StringAnalyzer::rectForStringInImageWithWidthAndHeight(int pString, int pWidth, int pHeight) {
    int lStringWidth = pWidth/NUM_STRINGS;
    return cvRect(lStringWidth*pString, 0, lStringWidth-1, pHeight-1);
}

int StringAnalyzer::_debugStringToDisplay = 3;

void StringAnalyzer::debugSetStringToDisplay(int pString) {
    _debugStringToDisplay = pString;
}

/**
 * Expects as input a image containing all strings.
 */
vector<peak_t> StringAnalyzer::analyzeFrame( IplImage* pImage, int estimatedNoteLength, double estimatedNoteWidth ) {
	
		DrumTimer* DrumTimer = DrumTimer::getInstance();
	
		CvRect lClippedStringImageRect = insetRect(rectForStringInImageWithWidthAndHeight(_stringNumber, pImage->width, pImage->height), 9, 0);
        IplImage* lClippedStringImage = cvCreateImage(cvSize(lClippedStringImageRect.width, lClippedStringImageRect.height), IPL_DEPTH_8U, pImage->nChannels);

        CvRect lSavedROI = cvGetImageROI(pImage);
        
        cvSetImageROI(pImage, lClippedStringImageRect);
        //cout << "ROI: " << lClippedStringImageRect.x << " " << lClippedStringImageRect.y << " " << lClippedStringImageRect.width << " " << lClippedStringImageRect.height << endl, 
        cvCopy(pImage, lClippedStringImage);
        
        cvSetImageROI(pImage, lSavedROI);
        
        CvMat *lPseudoRowLuminance = cvCreateMat(lClippedStringImage->height, 1, CV_8UC1);
        CvMat *lPseudoRowLuminance2 = cvCreateMat(lClippedStringImage->height, 1, CV_8UC1);
        cvZero(lPseudoRowLuminance);
        cvZero(lPseudoRowLuminance2);
        CvMat *lTempHeader = cvCreateMatHeader(1, lClippedStringImage->width, CV_32FC1);
        
        //Calculate the luminance values
        for (int i = 0; i < lClippedStringImage->height; i++) {
            cvSet2D(lPseudoRowLuminance, i, 0, cvScalar(cvAvg(cvGetRow(lClippedStringImage, lTempHeader, i)).val[0]));
			//cvSet2D(lPseudoRowLuminance, i, 0,cvGet2D(lClippedStringImage,i,(lClippedStringImage->width)/2));	// get a luma val from the middle of the track instead of averaging
        }
		//lPseudoRowLuminance2 = lPseudoRowLuminance;

        //We Fourier Filter the luminance plot to convert note rectangles into triangles
        Convolver::accentuatePeaks(lPseudoRowLuminance, estimatedNoteLength, lPseudoRowLuminance2);
        
        //We do a peak detection for timing
        CvScalar cvMean, cvStddev;
        cvAvgSdv(lPseudoRowLuminance2,&cvMean,&cvStddev);

		double lThreshold = cvMean.val[0] + STDDEVWEIGHT*cvStddev.val[0];
		double newThreshold = (1.0 - STRING_THRESHOLD_RATIO)*lThreshold + (STRING_THRESHOLD_RATIO)*_threshold;	
		if (newThreshold > _thresholdMax)
			_thresholdMax = newThreshold;
		if (newThreshold < 60.0/100.0*_thresholdMax) {
			_thresholdMax = _thresholdMax - 0.5;
			_threshold = 60.0/100.0*_thresholdMax;
		} else {
			_threshold = newThreshold;
		}
		if (_threshold < 115.0 && startup == false)
			_threshold = 115.0;
		
		vector<peak_t> peaks;

		//PeakDetector::detectPeaks(lPseudoRowLuminance2, _threshold, estimatedNoteLength, &peaks, _stringNumber);
		PeakDetector::detectWidths(lPseudoRowLuminance2, _threshold, estimatedNoteWidth, &peaks, _stringNumber);

		DrumTimer->calculateDeltaP(peaks, storedPeaks);
		storedPeaks = peaks;
		

		//cout << peaks.size() << endl;
		
		//std::vector<int> peaks2 = PeakDetector::detectPeaksForTimer(lPseudoRowLuminance2, _threshold, estimatedNoteLength*2, false, _stringNumber);
        
        //done with string peaks
        //_myTracker->initialize(lClippedStringImage->height);
        //_myTracker->shift_add_invalidate(DrumTimer->getDeltaP(),lPseudoRowLuminance2, estimatedNoteLength);
        
        
        
        IplImage* lPlot = cvCreateImage(cvSize(PLOT_WIDTH, pImage->height), IPL_DEPTH_8U, pImage->nChannels);
        cvZero(lPlot);
        for (int i = 0; i < cvGetSize(lPseudoRowLuminance2).height - 1; i++) {
            cvLine(lPlot, cvPoint((int)cvGet2D(lPseudoRowLuminance2, i, 0).val[0], i), cvPoint((int)cvGet2D(lPseudoRowLuminance2, i+1, 0).val[0], i+1), LINE_COLOR);
        }
        
        for (unsigned int i = 0; i < peaks.size(); i++) {
			cvLine(lPlot, cvPoint(0, peaks[i].pos), cvPoint(255, peaks[i].pos), LINE_COLOR);
			cvLine(lPlot, cvPoint(255, peaks[i].pos+peaks[i].width/2), cvPoint(255, peaks[i].pos-peaks[i].width/2), LINE_COLOR);
        }
        cvLine(lPlot, cvPoint(_threshold, 0), cvPoint(_threshold, cvGetSize(lPseudoRowLuminance2).height-1), CV_RGB(192,192,192));
        
		
        
        CvFont font;
        cvInitFont( &font, CV_FONT_HERSHEY_PLAIN, 1.0f, 1.0f);
        char text[500];
        int pos = 20;
        /*
		sprintf(text, "avg: %f", cvMean.val[0]);
        cvPutText( lPlot, text, cvPoint(5,pos), &font, cvScalar(255.0,255.0,255.0,0.0) );
        pos += 15;
        
        sprintf(text, "stdd: %f", cvStddev.val[0]);
        cvPutText( lPlot, text, cvPoint(5,pos), &font, cvScalar(255.0,255.0,255.0,0.0) );
        pos += 15;*/
        sprintf(text, "thresh: %f", _threshold);
        cvPutText( lPlot, text, cvPoint(5,pos), &font, cvScalar(255.0,255.0,255.0,0.0) );
        pos += 15;
        sprintf(text, "stddevWeight: %f", STDDEVWEIGHT);
        cvPutText( lPlot, text, cvPoint(5,pos), &font, cvScalar(255.0,255.0,255.0,0.0) );
        pos += 15;
        sprintf(text, "deltaP: %f", DrumTimer->getDeltaP());
        cvPutText( lPlot, text, cvPoint(5,pos), &font, cvScalar(255.0,255.0,255.0,0.0) );
        pos += 15;
        sprintf(text, "deltaT: %f", DrumTimer->getDeltaT()*((double)1000/CLOCKS_PER_SEC));
        cvPutText( lPlot, text, cvPoint(5,pos), &font, cvScalar(255.0,255.0,255.0,0.0) );
        pos += 15;/*
        sprintf(text, "demandedHWF: %f", PeakDetector::demandedHalfWidthFactor);
        cvPutText( lPlot, text, cvPoint(5,pos), &font, cvScalar(255.0,255.0,255.0,0.0) );
       */
       char name[10];
       sprintf(name, "Buffer %d", _stringNumber+1); 
       cvShowImage(name, lPlot);
       cvReleaseImage(&lPlot);
       
        
       cvReleaseMat(&lPseudoRowLuminance);
       cvReleaseMat(&lPseudoRowLuminance2);
       cvReleaseMat(&lTempHeader);
	   cvReleaseImage( &lClippedStringImage );

	   return peaks;
}
