#ifndef CONVOLVER_H_
#define CONVOLVER_H_

#include "global.h"

class Convolver {
public:
	static void accentuatePeaks(CvMat* input, int kernelWidth, CvMat* output);
private:
	Convolver();
	virtual ~Convolver();
};

#endif /*CONVOLVER_H_*/