CUDA Image Filtering
Niels Joubert
Suhaas Prasad
---------------------

EXTRA CREDIT: OpenMP implementation
  note that we implemented an OpenMP runtime alongside the reference runtime.
  thus, our program will run three (rather than two) versions of the filtering algorithm.
  1) the reference implementation provided as part of the project	
  2) our OpenMP version of the reference implementation
  3) our custom CUDA version of the reference implementation
  We also generate two output files - the _out.jpg from CUDA and _omp_out.jpg from OpenMP
  The makefile has also been modified to link against openmp.
  

CORRECTNESS & IMPLEMENTATION:

	Our CUDA implementation follows the general outline of the reference implementation.
	We have 5 different kernels, 2 for ffts, 1 for filtering, and 2 for inverse ffts.
	Each of the fft kernels make the following observation:
		A thread can be responsible for a single output value and not conflict with other threads.
	
	We break the image up into a grid of threadblocks, where each thread in a threadblock calculates
	the resulting value vir the FFT at that location in the image, thus accessing an entire row or column.
	
	Consider the x dimension for the fft. The grid is of such dimensions that each row has
	a threadblock, and each set of columns the size of BLOCK_SIZE (256 on my local machine) falls in this threadblock.
	Since the treadblock all access the same row, we can first load that row into shared memory, which is what we do.
	Each thread is responsible for loading the value in the input image that corresponds to its output position
	(properly repeated if there are multiple threadblocks per row).
	We can now do the FFT calculation, and each thread writes its value back.
	
	Rather than working on an in-place image, we modified the kernels to read from one image and write to
	a second image. This means we don't have to worry about synchronization between threadblocks when
	you have multiple threadblocks per row (or column in the case of the y fft), which happens in the 1024x1024 case.
	
	The combination of shared memory usage, memory separation and massively parallel execution gave us
	huge performance increases.
	
	
	The OpenMP implementation divides up the image into chunks, where a single OpenMP thread is responsible for
	calculating the FFT over only a horizontal (for the x fft) or vertical (for the y fft) strip of the image.
	It thus closely resembles the reference implementation, and scales nearly linearly with core count.
  
  
PERFORMANCE RESULTS:
	
	====================================================================
	Locally (2 x 2.26GHz Quad-Core Xeon HyperThreading, nVidia GT120):
	====================================================================
		noisy_01.nsy:
		-------------
			REFERENCE IMPLEMENTATION STATISTICS:
  				Reference Kernel Execution Time: 78340.000000 ms
			OPENMP IMPLEMENTATION STATISTICS:
  				OpenMP Kernel Execution Time: 5602.992188 ms over 16 threads
			CUDA IMPLEMENTATION STATISTICS:
  				Host to Device Transfer Time: 1.098240 ms
  				Kernel(s) Execution Time: 892.577026 ms
  				Device to Host Transfer Time: 1.130752 ms
  				Total CUDA Execution Time: 894.806030 ms
  				
			TOTAL SPEEDUP FOR OPENMP: 13.981815
			TOTAL SPEEDUP FOR CUDA: 87.549698
			TOTAL SPEEDUP FOR CUDA over OPENMP: 6.261683
			
			As you can see, the 16 virtual cores of my machine scales up almost linearly with OpenMP.
			The low-end nVidia card still outperforms this easily.
			
		noisy_03.nsy:
		-------------
			REFERENCE IMPLEMENTATION STATISTICS:
  				Reference Kernel Execution Time: 662036.187500 ms
  			OPENMP IMPLEMENTATION STATISTICS:
  				OpenMP Kernel Execution Time: 47967.359375 ms over 16 threads
			CUDA IMPLEMENTATION STATISTICS:
  				Host to Device Transfer Time: 3.179104 ms
  				Kernel(s) Execution Time: 7131.184082 ms
  				Device to Host Transfer Time: 4.393824 ms
  				Total CUDA Execution Time: 7138.757324 ms
  				
			TOTAL SPEEDUP FOR OPENMP: 13.801806
			TOTAL SPEEDUP FOR CUDA: 92.738297
			TOTAL SPEEDUP FOR CUDA over OPENMP: 6.719287

			Again, CUDA shines with a massive two orders of magnitude increase in speed.
	
	====================================================================			
	On Pup2 (Intel Core2 Duo CPU E6850 @ 3.16 GHz, nVidia GTX260):
	====================================================================
	
		noisy_01.nsy:
		-------------
			REFERENCE IMPLEMENTATION STATISTICS:
  				Reference Kernel Execution Time: 94138.273438 ms
			OPENMP IMPLEMENTATION STATISTICS:
  				OpenMP Kernel Execution Time: 48368.179688 ms over 2 threads
			CUDA IMPLEMENTATION STATISTICS:
  				Host to Device Transfer Time: 1.535520 ms
  				Kernel(s) Execution Time: 140.818237 ms
  				Device to Host Transfer Time: 1.351648 ms
  				Total CUDA Execution Time: 143.705414 ms
  				
			TOTAL SPEEDUP FOR OPENMP: 1.946285
			TOTAL SPEEDUP FOR CUDA: 655.078125
			TOTAL SPEEDUP FOR CUDA over OPENMP: 336.578674
			
			The significantly faster GTX260 completely blows the poor little dual core
			machine out of the water. The OpenMP only runs 2 threads here, so
			it also gets massively outperformed.
		
		noisy_03.nsy:
		-------------
		
			REFERENCE IMPLEMENTATION STATISTICS:
  				Reference Kernel Execution Time: 856339.500000 ms
			OPENMP IMPLEMENTATION STATISTICS:
  				OpenMP Kernel Execution Time: 482818.812500 ms over 2 threads
			CUDA IMPLEMENTATION STATISTICS:
  				Host to Device Transfer Time: 3.929856 ms
  				Kernel(s) Execution Time: 1178.917969 ms
  				Device to Host Transfer Time: 3.889248 ms
  				Total CUDA Execution Time: 1186.737061 ms

			TOTAL SPEEDUP FOR OPENMP: 1.773625
			TOTAL SPEEDUP FOR CUDA: 721.591614
			TOTAL SPEEDUP FOR CUDA over OPENMP: 406.845642
		