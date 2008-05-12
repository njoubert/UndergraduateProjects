#ifndef COLVOLVER_H_
#define COLVOLVER_H_

#include "global.h"

class Convolver {
public:
	static void accentuatePeaks(CvMat* input, int kernelWidth, CvMat* output);
private:
	Convolver();
	virtual ~Convolver();
};

#endif /*COLVOLVER_H_*/
