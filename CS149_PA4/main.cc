#include "CpuReference.h"
#include "OpenMPReference.h"

#include "ImageCleaner.h"

#ifdef BMPNOTJPEG
	#include "BMPWriter.h"
#else
	#include "JPEGWriter.h"
#endif
#include <iostream>
#include <fstream>
#include <assert.h>
#include <string>

int main(int argc, char **argv)
{
  if(argc != 2)
  {
    printf("Usage: ImageCleaner noisy_image_file.nsy\n");
    return 1;	
  }
  printf("Opening input image...\n");

  // Read in the noisy image file
  char *fileName = argv[1];
  std::ifstream ifs;
  ifs.open(fileName, std::ios::in);
  
  // Get the size of the image
  int size_x, size_y;
  ifs >> size_x >> size_y;
  assert((size_x > 0) && (size_y > 0));
  
  // Allocate space for the real and imaginary components of the image
  float *real_image = new float[size_x * size_y];
  float *imag_image = new float[size_x * size_y];
  // Also allocate space for the reference implementation
  float *real_image_ref = new float[size_x * size_y];
  float *imag_image_ref = new float[size_x * size_y];
  // Also allocate space for the openmp implementation
  float *real_image_omp = new float[size_x * size_y];
  float *imag_image_omp = new float[size_x * size_y];

  // Read in the components
  for(unsigned int i=0; i< size_x; i++)
  {
    for(unsigned int j=0; j<size_y; j++)
    {
      float real, imag;
      // Read in both the real and imaginary parts
      ifs >> real >> imag;
      // Write them into the arrays
      real_image[i*size_x + j] = real;
      imag_image[i*size_x + j] = imag;
      // Do the same for the reference
      real_image_ref[i*size_x + j] = real;
      imag_image_ref[i*size_x + j] = imag;
      // Do the same for the omp
      real_image_omp[i*size_x + j] = real;
      imag_image_omp[i*size_x + j] = imag;
    }
  }
  // Close the file since we're done with it now
  ifs.close();
#ifdef SKIPREFERENCE
  printf("Skipping reference implementation...\n");
  float referenceTime = 1;
#else
  printf("Running reference implementation...\n");
  float referenceTime = referenceCleaner(real_image_ref, imag_image_ref, size_x, size_y);
#endif
  printf("Running cuda implementation...\n");
  float cudaTime = filterImage(real_image, imag_image, size_x, size_y);
  printf("Running openMP implementation...\n");
  float openMPTime = openMPReferenceCleaner(real_image_omp, imag_image_omp, size_x, size_y);

#ifndef SKIPREFERENCE
  printf("TOTAL SPEEDUP FOR OPENMP: %f\n\n", (referenceTime/openMPTime));
  printf("TOTAL SPEEDUP FOR CUDA: %f\n\n", (referenceTime/cudaTime));
#endif
  printf("TOTAL SPEEDUP FOR CUDA over OPENMP: %f\n\n", (openMPTime/cudaTime));


  // Dump the image to jpeg format for the cuda implementation
  {
    std::string tempFileName(fileName);
    // Take off the '.nsy' postfix and post-pend '_out.jpg'
    int index = tempFileName.find(".nsy");
    // Better have found it
    assert(index >= 0);
    std::string outFileName = tempFileName.substr(0,index);
    std::string outFileNameRef = tempFileName.substr(0,index);
    std::string outFileNameOmp = tempFileName.substr(0,index);
    printf("Writing out CUDA generated image to %s\n\n", outFileName.c_str());

#ifdef BMPNOTJPEG
    outFileName.append("_out.bmp");
    write_bmp(outFileName.c_str(), real_image, imag_image, size_x, size_y);
    outFileNameRef.append("_refout.bmp");
    write_bmp(outFileNameRef.c_str(), real_image_ref, imag_image_ref, size_x, size_y);
    outFileNameOmp.append("_ompout.bmp");
    write_bmp(outFileNameOmp.c_str(), real_image_omp, imag_image_omp, size_x, size_y);
#else
    outFileName.append("_out.jpg");
    write_jpeg(outFileName.c_str(), real_image, imag_image, size_x, size_y); 
    outFileName.append("_omp_out.jpg");
    write_jpeg(outFileName.c_str(), real_image_omp, imag_image_omp, size_x, size_y);
#endif
  }
  // Clean up the memory
  delete [] real_image;
  delete [] imag_image;
  delete [] real_image_ref;
  delete [] imag_image_ref; 

  return 0; 
}
