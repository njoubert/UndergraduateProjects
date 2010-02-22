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

__global__ void exampleKernel(float *real_image, float *imag_image, int size_x, int size_y)
{
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
  exampleKernel<<<1,128,0,filterStream>>>(device_real,device_imag,size_x,size_y);







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

