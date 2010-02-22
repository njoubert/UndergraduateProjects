#ifndef __OMP_REFERENCE__
#define __OMP_REFERENCE__

// This function contains the reference implementation of the algorithm needed
// to be implemented for CUDA
// The float that is returned is the total time required to execute this kernel
float openMPReferenceCleaner(float *real_image, float *imag_image, int size_x, int size_y);

#endif
