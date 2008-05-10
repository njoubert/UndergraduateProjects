#ifndef COLVOLVER_H_
#define COLVOLVER_H_

#include "global.h"

class Convolver {
public:
	static void accentuatePeaks(CvMat* input, int kernelWidth, CvMat* output);
	static CvMat* convolveWithFFTs(CvMat* input1, CvMat* input2);
	static CvMat* convolveWithFFTs2(CvMat* input1, CvMat* input2);
private:
	Convolver();
	virtual ~Convolver();
};

#endif /*COLVOLVER_H_*/
