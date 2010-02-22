#include <assert.h>
#include "cuda.h"
#include "cuda_runtime.h"
#include <stdio.h>

/*****************************************************/
/* CS149: ALL OF YOUR CODE SHOULD GO IN THIS FILE    */
/*****************************************************/

// You can modify these parameters to match the image input size
#define SIZEX    512
#define SIZEY    512

// no need to modify this:
#define BLOCK_SIZE 512
#define PI	3.14159256

__global__ void gpu_fftx(float *dReal, float *dImag, int size_x, int size_y) {

	//responsible for a single output cell in the FFT'd image.
	//runs in 512-wide threadblocks all working together

	int myRow = blockIdx.y; //processed by the whole threadblock
	int myCol = threadIdx.x + blockIdx.x * blockDim.x; //i calculate this result

	// Compute the value for this index
	float real_value = 0;
	float imag_value = 0;
	float term, realTerm, imagTerm;
	for (unsigned int n = 0; n < size_y; n++) { 				//(a+bi)(c+di) = (ac-bd) + (bc+ad)i
		term = -2 * PI * myCol * n / size_y;
		realTerm = term;//cos(term);
		imagTerm = term;//sin(term);

		real_value += (dReal[myRow * size_x + n] * realTerm)
				- (dImag[myRow * size_x + n] * imagTerm);

		imag_value += (dImag[myRow * size_x + n] * realTerm)
				+ (dReal[myRow * size_x + n] * imagTerm);

	}

	// Write the values back into the temporary buffer
	dReal[myRow * size_x + myCol] = real_value;
	dImag[myRow * size_x + myCol] = imag_value;

}
__global__ void gpu_ifftx(float *real_image, float *imag_image, int size_x, int size_y) {
  // Currently does nothing
}
__global__ void gpu_ffty(float *real_image, float *imag_image, int size_x, int size_y) {
  // Currently does nothing
}
__global__ void gpu_iffty(float *real_image, float *imag_image, int size_x, int size_y) {
  // Currently does nothing
}
__global__ void gpu_filter(float *real_image, float *imag_image, int size_x, int size_y) {
  // Currently does nothing
}

__host__ float filterImage(float *real_image, float *imag_image, int size_x, int size_y)
{
  // check that the sizes match up
  assert(size_x == SIZEX);
  assert(size_y == SIZEY);

  int matSize = size_x * size_y * sizeof(float);

  // These variables are for timing purposes
  float transferDown = 0, transferUp = 0, execution = 0;
  cudaEvent_t start,stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);

  // Create a stream and initialize it
  cudaStream_t filterStream;
  cudaStreamCreate(&filterStream);

  // Alloc space on the device
  float *device_real, *device_imag;
  cudaMalloc((void**)&device_real, matSize);
  cudaMalloc((void**)&device_imag, matSize);

  // Start timing for transfer down
  cudaEventRecord(start,filterStream);
  
  //-----------------------------------------------------------------
  // HERE IS WHERE WE COPY THE MATRICES DOWN TO THE DEVICE
  //-----------------------------------------------------------------
  cudaMemcpy(device_real,real_image,matSize,cudaMemcpyHostToDevice);
  cudaMemcpy(device_imag,imag_image,matSize,cudaMemcpyHostToDevice);
  
  // Stop timing for transfer down
  cudaEventRecord(stop,filterStream);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&transferDown,start,stop);

  // Start timing for the execution
  cudaEventRecord(start,filterStream);



 
  //----------------------------------------------------------------
  // CS149: YOU SHOULD PLACE ALL YOUR KERNEL EXECUTIONS
  //        HERE BETWEEN THE CALLS FOR STARTING AND
  //        FINISHING TIMING FOR THE EXECUTION PHASE
  //----------------------------------------------------------------

  // This is an example kernel call, you should feel free to create
  // as many kernel calls as you feel are needed for your program
  // Each of the parameters are as follows:
  //    1. Number of thread blocks, can be either int or dim3 (see CUDA manual)
  //    2. Number of threads per thread block, can be either int or dim3 (see CUDA manual)
  //    3. Always should be '0' unless you read the CUDA manual
  //    4. Stream to execute kernel on, should always be 'filterStream' for this project
  //
  // Also note that you pass the pointers to the device memory to the kernel call

  // we have to run kernels that does an fft.
  //	ffty:
  //		for each row in the image
  // 			for each cell in the row
  //				access all the other cells in the row


  //Here is how we split this up:
  // 	each threadblock is responsible for 512 elements in a row.
  //	threadblocks are arranged in a grid of 512-groups elements by all the rows.
  //	input image is SIZEX by SIZEY
  //	block this into a (SIZEX/512) by SIZEY grid
  //	each threadblock consists of 512 threads for a row in the grid.




  dim3 dimBlock(BLOCK_SIZE);
  dim3 dimGrid(ceil((float)SIZEX/BLOCK_SIZE),SIZEY);
  printf("  Launching kernel with %d threads per block arranged in a grid of %dx%d.\n", dimBlock.x, dimGrid.x, dimGrid.y);
  gpu_fftx<<<dimGrid, dimBlock>>>(device_real,device_imag,size_x,size_y);







  // Finish timimg for the execution 
  cudaEventRecord(stop,filterStream);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&execution,start,stop);

  // Check for any errors in kernel launch
  cudaError_t kernelResult = cudaGetLastError();
  if( kernelResult != cudaSuccess )
  {
    printf("WARNING: CUDA KERNEL CALL FAILED!  %s\n", cudaGetErrorString(kernelResult));
  }

  // Start timing for the transfer up
  cudaEventRecord(start,filterStream);

  //----------------------------------------------------------------
  // HERE IS WHERE WE COPY THE MATRICES BACK FROM THE DEVICE
  //----------------------------------------------------------------
  cudaMemcpy(real_image,device_real,matSize,cudaMemcpyDeviceToHost);
  cudaMemcpy(imag_image,device_imag,matSize,cudaMemcpyDeviceToHost);

  // Finish timing for transfer up
  cudaEventRecord(stop,filterStream);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&transferUp,start,stop);

  // Synchronize the stream
  cudaStreamSynchronize(filterStream);
  // Destroy the stream
  cudaStreamDestroy(filterStream);
  // Destroy the events
  cudaEventDestroy(start);
  cudaEventDestroy(stop);

  // Free the memory
  cudaFree(device_real);
  cudaFree(device_imag);

  // Dump some usage statistics
  printf("CUDA IMPLEMENTATION STATISTICS:\n");
  printf("  Host to Device Transfer Time: %f ms\n", transferDown);
  printf("  Kernel(s) Execution Time: %f ms\n", execution);
  printf("  Device to Host Transfer Time: %f ms\n", transferUp);
  float totalTime = transferDown + execution + transferUp;
  printf("  Total CUDA Execution Time: %f ms\n\n", totalTime);
  // Return the total time to transfer and execute
  return totalTime;
}

