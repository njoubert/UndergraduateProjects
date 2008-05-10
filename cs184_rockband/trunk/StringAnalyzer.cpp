#include "StringAnalyzer.h"

StringAnalyzer::StringAnalyzer(int pStringNumber) {
	
}

StringAnalyzer::~StringAnalyzer() {
}

/**
 * Expects as input a image containing ONE string.
 */
void StringAnalyzer::analyzeFrame( IplImage* pImage, int estimatedNoteLength ) {
	
		//GuitarTimer.frameStarted(pStringNumber);
	
		//Create the 3 channels of the images
        CvMat *lRed = cvCreateMat(pImage->height, pImage->width, CV_8UC1);
        CvMat *lGreen = cvCreateMat(pImage->height, pImage->width, CV_8UC1);
        CvMat *lBlueAndGray = cvCreateMat(pImage->height, pImage->width, CV_8UC1);
        CvMat *lTemp = cvCreateMat(pImage->height, pImage->width, CV_8UC1);
        cvSplit(pImage, lRed, lGreen, lBlueAndGray, NULL);
        
        //collapse the 3 channels into one matrix of luminance values
        cvMax(lBlueAndGray, lGreen, lTemp);
        cvMax(lRed, lTemp, lBlueAndGray);
        CvMat *lPseudoRowLuminance = cvCreateMat(pImage->height, 1, CV_8UC1);
        CvMat *lPseudoRowLuminance2 = cvCreateMat(pImage->height, 1, CV_8UC1);
        CvMat *lTempHeader = cvCreateMatHeader(1, pImage->width, CV_32FC1);
        
        for (int i = 0; i < pImage->height; i++) {
            cvSet2D(lPseudoRowLuminance, i, 0, cvScalar(cvAvg(cvGetRow(lBlueAndGray, lTempHeader, i)).val[0]));
        }
        
        Convolver::accentuatePeaks(lPseudoRowLuminance, estimatedNoteLength, lPseudoRowLuminance2);
        
        IplImage* lPlot = cvCreateImage(cvSize(PLOT_WIDTH, pImage->height), IPL_DEPTH_8U, pImage->nChannels);
        cvZero(lPlot);
        for (int i = 0; i < cvGetSize(lPseudoRowLuminance2).height - 1; i++) {
            cvLine(lPlot, cvPoint(cvGet2D(lPseudoRowLuminance2, i, 0).val[0], i), cvPoint(cvGet2D(lPseudoRowLuminance2, i+1, 0).val[0], i+1), LINE_COLOR);
        }
        
        cvShowImage("Plot", lPlot );
        
        
        
        cvReleaseMat(&lPseudoRowLuminance);
        cvReleaseMat(&lPseudoRowLuminance2);
        cvReleaseMat(&lTempHeader);
        cvReleaseMat(&lRed);
        cvReleaseMat(&lGreen);
        cvReleaseMat(&lBlueAndGray);
        cvReleaseMat(&lTemp);
        cvReleaseImage(&lPlot);
	
	
	/**
	 * Convolution
	 */
	
	
	/**
	 * Peak Detection
	 */
	 
	 
	 
	 //for each peak
	 
	 //GuitarTimer.peakFound(_stringNumber, position-from-top);
	
	
	
	
	/**
	 * Done
	 */
		//GuitarTimer.frameDone();
	
}
