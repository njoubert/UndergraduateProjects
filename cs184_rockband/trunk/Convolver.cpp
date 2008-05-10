#include "Convolver.h"

/**
 * This method convolves the input image with a rectangle of width kernelWidth
 * centered around 0. It assumes the input CvMat is a N-length row vector
 * It uses two FFT calculations, thus running in O(NlogN) time (where N is the length of
 * the input vector. It pads the input array to speed up FFT calculation.
 */
void Convolver::accentuatePeaks(CvMat* input, int kernelWidth, CvMat* output) {
	
	/**
	 * Initial tests:
	 */
	//cvNormalize(lPseudoRowLuminance, lPseudoRowLuminance, PLOT_WIDTH, 0, CV_MINMAX);
    //cvThreshold(lPseudoRowLuminance, lPseudoRowLuminance2, 60.0, 90.0, CV_THRESH_BINARY);
    //cvSmooth(input, output, CV_GAUSSIAN, kernelWidth);
    //cvSmooth(lPseudoRowLuminance, lPseudoRowLuminance2, CV_MEDIAN , 49);
      
    /**
     * For real:
     */  
        
	//pad the input signal to length(input) + kernelWidth + padding(best effiency)
	int paddedHeight = input->height + ceil(kernelWidth/(double)2.0);
	paddedHeight = 2*paddedHeight-1;
	paddedHeight = cvGetOptimalDFTSize(paddedHeight);
	
	if (0 >= paddedHeight) {	//This should be done cleaner
		printf("Could not compute Convolution since paddedHeight was too big!\n");
		exit(1);	
	}
	
	CvMat* padded_input = cvCreateMat(1, paddedHeight, CV_32F);
	cvZero(padded_input);
	int start = (paddedHeight-input->height);
	for (int i=start; i < paddedHeight; i++) {
		cvSet2D(padded_input, 0, i, cvGet2D(input, i - start, 0));	
	}

	//create kernel to same length as input signal.
	CvMat* kernel = cvCreateMat(1, paddedHeight, CV_32F);
	cvZero(kernel);
	
	//set kernel to have values of 1 over midpoint of width kernelWidth. (still 0 everywhere else)
	double kernelHeight = 1.0 / (double) kernelWidth;
	//double kernelHeight = 1.0;
	for (int i = 0; i < kernelWidth; i++) {
		cvSet2D(kernel, 0, i, cvScalar(kernelHeight));
	}
	
	//Convolve!
	CvMat* convolved = Convolver::convolveWithFFTs(padded_input, kernel);
	
	//truncate input
	for (int i=0; i < input->height; i++) {
		cvSet1D(output, i, cvScalar(cvmGet(convolved, 0, i))); //Is this row,col value right? currently row=0, col=i	
	}
	 cvReleaseMat(&convolved);
}

/**
 * Convolves the two inputs into the given output. Uses FFTs to accomplish this.
 * It will work best if input lengths are padded using cvGetOptimalDFTSize.
 * Only 32fC1, 32fC2, 64fC1 and 64fC2 formats are supported
 * 
 * THIS FUNCTION CHANGES THE INPUT VALUES!
 * 
 */
CvMat* Convolver::convolveWithFFTs(CvMat* input1, CvMat* input2) {
	//Take DTFs
	cvDFT( input1, input1, CV_DXT_FORWARD);
	cvDFT( input2, input2, CV_DXT_FORWARD);
    
	
	int length = std::max(input1->width, input2->width);
	CvMat* ret = cvCreateMat(1, length, CV_32F);
	
	cvMulSpectrums( input1, input2, ret, 0);

   	cvDFT( ret, ret, CV_DXT_INV_SCALE); // calculate only the top part
   	
   	IplImage* lPlot = cvCreateImage(cvSize(ret->width, PLOT_WIDTH), IPL_DEPTH_8U, 4);
    cvZero(lPlot);
    for (int i = 0; i < cvGetSize(ret).width - 1; i++) {
    	printf("%f ", cvGet2D(ret, 0, i).val[0]);
        cvLine(lPlot, cvPoint(i, cvGet2D(ret, 0, i).val[0]*100), cvPoint(i+1, cvGet2D(ret, 0, i+1).val[0]*100), LINE_COLOR);
    }
	cvShowImage("DFT", lPlot );

	return ret;
	
}

CvMat* Convolver::convolveWithFFTs2(CvMat* input1, CvMat* input2) {
	//Take DTFs
	cvDFT( input1, input1, CV_DXT_FORWARD);
	cvDFT( input2, input2, CV_DXT_FORWARD);
	
	
    IplImage* lPlot = cvCreateImage(cvSize(PLOT_WIDTH, input1->height), IPL_DEPTH_8U, 4);
    cvZero(lPlot);
    for (int i = 0; i < cvGetSize(input1).height - 1; i++) {
        cvLine(lPlot, cvPoint(cvGet2D(input1, i, 0).val[0], i), cvPoint(cvGet2D(input1, i+1, 0).val[0], i+1), LINE_COLOR);
    }
	cvShowImage("DFT", lPlot );
	
	int length = std::max(input1->width, input2->width);
	CvMat* ret = cvCreateMat(1, length, CV_32F);
	
	cvMulSpectrums( input1, input2, ret, 0);

   	cvDFT( ret, ret, CV_DXT_INV_SCALE); // calculate only the top part

	return ret;
	
}

Convolver::Convolver() {}

Convolver::~Convolver() {}
