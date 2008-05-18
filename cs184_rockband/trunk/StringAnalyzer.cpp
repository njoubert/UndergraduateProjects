#include "StringAnalyzer.h"


double StringAnalyzer::_threshold = 0;
double StringAnalyzer::_thresholdMax = 0;

StringAnalyzer::StringAnalyzer(int pStringNumber, NoteTracker* pMyTracker) {
	_myTracker = pMyTracker;
	_stringNumber = pStringNumber;
	_threshold = 0;
}

StringAnalyzer::~StringAnalyzer() { }

CvRect StringAnalyzer::rectForStringInImageWithWidthAndHeight(int pString, int pWidth, int pHeight) {
    int lStringWidth = pWidth/5;
    return cvRect(lStringWidth*pString, 0, lStringWidth-1, pHeight-1);
}

int StringAnalyzer::_debugStringToDisplay = 3;

void StringAnalyzer::debugSetStringToDisplay(int pString) {
    _debugStringToDisplay = pString;
}

/**
 * Expects as input a image containing all strings.
 */
void StringAnalyzer::analyzeFrame( IplImage* pImage, int estimatedNoteLength ) {
	
		GuitarTimer* guitarTimer = GuitarTimer::getInstance();
	
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
        }
        
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
		
		std::vector<int> peaks = PeakDetector::detectPeaksForTimer(lPseudoRowLuminance2, _threshold, estimatedNoteLength*2, false, _stringNumber);
        
        //done with string peaks
        _myTracker->initialize(lClippedStringImage->height);
        _myTracker->shift_add_invalidate(round(guitarTimer->getDeltaP()),lPseudoRowLuminance2, estimatedNoteLength*2);
        
        /**
        
        IplImage* lPlot = cvCreateImage(cvSize(PLOT_WIDTH, pImage->height), IPL_DEPTH_8U, pImage->nChannels);
        cvZero(lPlot);
        for (int i = 0; i < cvGetSize(lPseudoRowLuminance2).height - 1; i++) {
            cvLine(lPlot, cvPoint(cvGet2D(lPseudoRowLuminance2, i, 0).val[0], i), cvPoint(cvGet2D(lPseudoRowLuminance2, i+1, 0).val[0], i+1), LINE_COLOR);
        }
        
        for (unsigned int i = 0; i < peaks.size(); i++) {
        	cvLine(lPlot, cvPoint(0, peaks[i]), cvPoint(255, peaks[i]), CV_RGB(192,192,192));
        }
        cvLine(lPlot, cvPoint(_threshold, 0), cvPoint(_threshold, cvGetSize(lPseudoRowLuminance2).height-1), CV_RGB(192,192,192));
        
		
        
        CvFont font;
        cvInitFont( &font, CV_FONT_HERSHEY_PLAIN, 1.0f, 1.0f);
        char text[500];
        int pos = 20;
        sprintf(text, "avg: %f", cvMean.val[0]);
        cvPutText( lPlot, text, cvPoint(5,pos), &font, cvScalar(255.0,255.0,255.0,0.0) );
        pos += 15;
        
        sprintf(text, "stdd: %f", cvStddev.val[0]);
        cvPutText( lPlot, text, cvPoint(5,pos), &font, cvScalar(255.0,255.0,255.0,0.0) );
        pos += 15;
        sprintf(text, "thresh: %f", _threshold);
        cvPutText( lPlot, text, cvPoint(5,pos), &font, cvScalar(255.0,255.0,255.0,0.0) );
        pos += 15;
        sprintf(text, "stddevWeight: %f", STDDEVWEIGHT);
        cvPutText( lPlot, text, cvPoint(5,pos), &font, cvScalar(255.0,255.0,255.0,0.0) );
        pos += 15;
        sprintf(text, "deltaP: %f", guitarTimer->getDeltaP());
        cvPutText( lPlot, text, cvPoint(5,pos), &font, cvScalar(255.0,255.0,255.0,0.0) );
        pos += 15;
        sprintf(text, "deltaT: %f", guitarTimer->getDeltaT()*((double)1000/CLOCKS_PER_SEC));
        cvPutText( lPlot, text, cvPoint(5,pos), &font, cvScalar(255.0,255.0,255.0,0.0) );
        pos += 15;
        sprintf(text, "demandedHWF: %f", PeakDetector::demandedHalfWidthFactor);
        cvPutText( lPlot, text, cvPoint(5,pos), &font, cvScalar(255.0,255.0,255.0,0.0) );
       
       char name[10];
       sprintf(name, "Buffer %d", _stringNumber+1); 
       cvShowImage(name, lPlot);
       cvReleaseImage(&lPlot);
       
       // */
       
		
       
        
        cvReleaseMat(&lPseudoRowLuminance);
        cvReleaseMat(&lPseudoRowLuminance2);
        cvReleaseMat(&lTempHeader);
		cvReleaseImage( &lClippedStringImage );

}
