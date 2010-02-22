#ifndef __IMAGE_CLEANER__
#define __IMAGE_CLEANER__

#include <stdio.h>
#include <stdlib.h>

// This is the entry point function that has to be filled out for this assignment
// The float value that it returns is the total time taken to transfer the data
// to the device, execute all kernels, and transfer the data back from the device
float filterImage(float *real_image, float *imag_image, int size_x, int size_y);

#endif
