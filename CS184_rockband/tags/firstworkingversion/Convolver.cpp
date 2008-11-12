#include "Convolver.h"

/**
 * This method convolves the input image with a rectangle of width kernelWidth
 * centered around 0. It assumes the input CvMat is a N-length row vector
 * It uses two FFT calculations, thus running in O(NlogN) time (where N is the length of
 * the input vector. It pads the input array to speed up FFT calculation.
 */
void Convolver::accentuatePeaks(CvMat* input, int kernelWidth, CvMat* output) {

	//create kernel to same length as input signal.
	CvMat* kernel = cvCreateMat(kernelWidth, 1, CV_32F);
	double kernelHeight = 1.0 / (double) kernelWidth;
	for (int i = 0; i < kernelWidth; i++)
		cvSet2D(kernel, i, 0, cvScalar(kernelHeight));
	
	//By default the kernel is anchored in the middle. Keep this behaviour
	cvFilter2D(input, output, kernel);
	
	
}

Convolver::Convolver() {}

Convolver::~Convolver() {}
