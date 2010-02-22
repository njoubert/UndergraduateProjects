#ifndef __CPU_REFERENCE__
#define __CPU_REFERENCE__

// This function contains the reference implementation of the algorithm needed
// to be implemented for CUDA
// The float that is returned is the total time required to execute this kernel
float referenceCleaner(float *real_image, float *imag_image, int size_x, int size_y);

#endif
