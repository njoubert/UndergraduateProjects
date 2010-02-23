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
  
PERFORMANCE RESULTS:
	
	Locally (2 x 2.26GHz Quad-Core Xeon HyperThreading, nVidia GT120):
		noisy_01.nsy:
		-------------
		
			As you can see, the 16 virtual cores of my machine scales up almost linearly.
			The low-end nVidia card still outperforms this easily.
			
		noisy_03.nsy:
		-------------
			TOTAL SPEEDUP FOR OPENMP: 13.636431
			
			Timing statistics for CUDA could not be recorded, since the kernel runs longer than the allotted time
			for a GPU that is the primary (and only) GPU of a machine that runs OSX's window manager.
				
	On Pup2 (Intel Core2 Duo CPU E6850 @ 3.16 GHz, nVidia GTX260):
	
		noisy_01.nsy:
		-------------
		
			The significantly faster GTX260 completely blows the poor little dual core
			machine out of the water. The OpenMP only runs 2 threads here, so
			it also gets massively outperformed.
		
		noisy_03.nsy:
		-------------
