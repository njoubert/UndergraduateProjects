#include "CpuReference.h"
#include <math.h>
#include <sys/time.h>
#include <stdio.h>

/**************************************************/
/* CS149: DO NOT MODIFY THIS FILE!                */
/* This is the reference implementation of the    */
/* two dimensional FFT, filter, and inverse two   */
/* dimensional FFT.                               */
/**************************************************/

#define PI	3.14159256

void cpu_fftx(float *real_image, float *imag_image, int size_x, int size_y) {
	for (unsigned int x = 0; x < size_x; x++) {

		//This runs row by row on the image:

		// Create some space for storing temporary values
		float *realOutBuffer = new float[size_y];
		float *imagOutBuffer = new float[size_y];

		// This runs over each pixel in the row: (cols in row. y is the column)

		for (unsigned int y = 0; y < size_y; y++) {

			//-----------------------------------------------------------------------------------------------
			//						THIS CALCULATES A SINGLE OUTPUT VALUE
			//-----------------------------------------------------------------------------------------------


			// Compute the local values
			float *fft_real = new float[size_y];
			float *fft_imag = new float[size_y];
			for (unsigned int n = 0; n < size_y; n++) { // here we calculate the frequency component that this term represents, just as a speedup.
				float term = -2 * PI * y * n / size_y;
				fft_real[n] = cos(term);
				fft_imag[n] = sin(term);
			}

			// Compute the value for this index
			float real_value = 0;
			float imag_value = 0;
			for (unsigned int n = 0; n < size_y; n++) { 				//(a+bi)(c+di) = (ac-bd) + (bc+ad)i
				real_value += (real_image[x * size_x + n] * fft_real[n])
						- (imag_image[x * size_x + n] * fft_imag[n]);
				imag_value += (imag_image[x * size_x + n] * fft_real[n])
						+ (real_image[x * size_x + n] * fft_imag[n]);
			}

			// Write the values back into the temporary buffer
			realOutBuffer[y] = real_value;
			imagOutBuffer[y] = imag_value;

			// Reclaim memory
			delete[] fft_real;
			delete[] fft_imag;

			//-----------------------------------------------------------------------------------------------
			//						END END END END END END END END END END
			//-----------------------------------------------------------------------------------------------

		}
		// Write the buffer back to were the original values were
		for (unsigned int y = 0; y < size_y; y++) {
			real_image[x * size_x + y] = realOutBuffer[y];
			imag_image[x * size_x + y] = imagOutBuffer[y];
		}
		// Reclaim the memory
		delete[] realOutBuffer;
		delete[] imagOutBuffer;
	}
}

// This is the same as the thing above, except it has a scaling factor added to it
void cpu_ifftx(float *real_image, float *imag_image, int size_x, int size_y) {
	for (unsigned int x = 0; x < size_x; x++) {
		// Create some space for storing temporary values
		float *realOutBuffer = new float[size_y];
		float *imagOutBuffer = new float[size_y];
		for (unsigned int y = 0; y < size_y; y++) {


			//-----------------------------------------------------------------------------------------------
			//						THIS CALCULATES A SINGLE OUTPUT VALUE
			//-----------------------------------------------------------------------------------------------


			// Compute the local values
			float *fft_real = new float[size_y];
			float *fft_imag = new float[size_y];
			for (unsigned int n = 0; n < size_y; n++) {
				// Note that the negative sign goes away for the term here
				float term = 2 * PI * y * n / size_y;
				fft_real[n] = cos(term);
				fft_imag[n] = sin(term);
			}

			// Compute the value for this index
			float real_value = 0;
			float imag_value = 0;
			for (unsigned int n = 0; n < size_y; n++) {
				real_value += (real_image[x * size_x + n] * fft_real[n])
						- (imag_image[x * size_x + n] * fft_imag[n]);
				imag_value += (imag_image[x * size_x + n] * fft_real[n])
						+ (real_image[x * size_x + n] * fft_imag[n]);
			}

			// Write the values back into the temporary buffer
			// Incoporate the scaling factor here
			realOutBuffer[y] = real_value / size_y;
			imagOutBuffer[y] = imag_value / size_y;

			// Reclaim memory
			delete[] fft_real;
			delete[] fft_imag;


			//-----------------------------------------------------------------------------------------------
			//						END END END END END END END END END END
			//-----------------------------------------------------------------------------------------------

		}

		// Write the buffer back to were the original values were
		for (unsigned int y = 0; y < size_y; y++) {
			real_image[x * size_x + y] = realOutBuffer[y];
			imag_image[x * size_x + y] = imagOutBuffer[y];
		}
		// Reclaim the memory
		delete[] realOutBuffer;
		delete[] imagOutBuffer;
	}
}

void cpu_ffty(float *real_image, float *imag_image, int size_x, int size_y) {
	for (unsigned int y = 0; y < size_y; y++) {
		// Create some space for storing temporary values
		float *realOutBuffer = new float[size_x];
		float *imagOutBuffer = new float[size_x];
		for (unsigned int x = 0; x < size_x; x++) {

			//-----------------------------------------------------------------------------------------------
			//						THIS CALCULATES A SINGLE OUTPUT VALUE
			//-----------------------------------------------------------------------------------------------


			// Compute the local values
			float *fft_real = new float[size_x];
			float *fft_imag = new float[size_x];
			for (unsigned int n = 0; n < size_y; n++) {
				float term = -2 * PI * x * n / size_x;
				fft_real[n] = cos(term);
				fft_imag[n] = sin(term);
			}

			// Compute the value for this index
			float real_value = 0;
			float imag_value = 0;
			for (unsigned int n = 0; n < size_x; n++) {
				real_value += (real_image[n * size_x + y] * fft_real[n])
						- (imag_image[n * size_x + y] * fft_imag[n]);
				imag_value += (imag_image[n * size_x + y] * fft_real[n])
						+ (real_image[n * size_x + y] * fft_imag[n]);
			}

			// Write the values back into the temporary buffer
			realOutBuffer[x] = real_value;
			imagOutBuffer[x] = imag_value;

			// Reclaim memory
			delete[] fft_real;
			delete[] fft_imag;


			//-----------------------------------------------------------------------------------------------
			//						END END END END END END END END END END
			//-----------------------------------------------------------------------------------------------


		}
		// Write the buffer back to were the original values were
		for (unsigned int x = 0; x < size_x; x++) {
			real_image[x * size_x + y] = realOutBuffer[x];
			imag_image[x * size_x + y] = imagOutBuffer[x];
		}
		// Reclaim the memory
		delete[] realOutBuffer;
		delete[] imagOutBuffer;
	}
}

// This is the same as the thing about it, but it includes a scaling factor
void cpu_iffty(float *real_image, float *imag_image, int size_x, int size_y) {
	for (unsigned int y = 0; y < size_y; y++) {
		// Create some space for storing temporary values
		float *realOutBuffer = new float[size_x];
		float *imagOutBuffer = new float[size_x];
		for (unsigned int x = 0; x < size_x; x++) {


			//-----------------------------------------------------------------------------------------------
			//						THIS CALCULATES A SINGLE OUTPUT VALUE
			//-----------------------------------------------------------------------------------------------



			// Compute the local values
			float *fft_real = new float[size_x];
			float *fft_imag = new float[size_x];
			for (unsigned int n = 0; n < size_y; n++) {
				// Note that the negative sign goes away for the term
				float term = 2 * PI * x * n / size_x;
				fft_real[n] = cos(term);
				fft_imag[n] = sin(term);
			}

			// Compute the value for this index
			float real_value = 0;
			float imag_value = 0;
			for (unsigned int n = 0; n < size_x; n++) {
				real_value += (real_image[n * size_x + y] * fft_real[n])
						- (imag_image[n * size_x + y] * fft_imag[n]);
				imag_value += (imag_image[n * size_x + y] * fft_real[n])
						+ (real_image[n * size_x + y] * fft_imag[n]);
			}

			// Write the values back into the temporary buffer
			// Incorporate the scaling factor here
			realOutBuffer[x] = (real_value / size_x);
			imagOutBuffer[x] = (imag_value / size_x);

			// Reclaim memory
			delete[] fft_real;
			delete[] fft_imag;


			//-----------------------------------------------------------------------------------------------
			//						END END END END END END END END END END
			//-----------------------------------------------------------------------------------------------


		}
		// Write the buffer back to were the original values were
		for (unsigned int x = 0; x < size_x; x++) {
			real_image[x * size_x + y] = realOutBuffer[x];
			imag_image[x * size_x + y] = imagOutBuffer[x];
		}
		// Reclaim the memory
		delete[] realOutBuffer;
		delete[] imagOutBuffer;
	}
}

void cpu_filter(float *real_image, float *imag_image, int size_x, int size_y) {
	int eightX = size_x / 8;
	int eight7X = size_x - eightX;
	int eightY = size_y / 8;
	int eight7Y = size_y - eightY;
	for (unsigned int x = 0; x < size_x; x++) {
		for (unsigned int y = 0; y < size_y; y++) {
			if (!(x < eightX && y < eightY) && !(x < eightX && y >= eight7Y)
					&& !(x >= eight7Y && y < eightY) && !(x >= eight7Y && y
					>= eight7Y)) {
				// Zero out these values
				real_image[y * size_x + x] = 0;
				imag_image[y * size_x + x] = 0;
			}
		}
	}
}

float referenceCleaner(float *real_image, float *imag_image, int size_x,
		int size_y) {
	// These are used for timing
	struct timeval tv1, tv2;
	struct timezone tz1, tz2;

	// Start timing
	gettimeofday(&tv1, &tz1);

	cpu_fftx(real_image, imag_image, size_x, size_y);
	cpu_ffty(real_image, imag_image, size_x, size_y);

	cpu_filter(real_image, imag_image, size_x, size_y);

	cpu_ifftx(real_image, imag_image, size_x, size_y);
	cpu_iffty(real_image, imag_image, size_x, size_y);

	// End timing
	gettimeofday(&tv2, &tz2);

	// Compute the time difference in micro-seconds
	float execution = ((tv2.tv_sec - tv1.tv_sec) * 1000000 + (tv2.tv_usec
			- tv1.tv_usec));
	// Convert to milli-seconds
	execution /= 1000;
	// Print some output
	printf("REFERENCE IMPLEMENTATION STATISTICS:\n");
	printf("  Reference Kernel Execution Time: %f ms\n\n", execution);
	return execution;
}
