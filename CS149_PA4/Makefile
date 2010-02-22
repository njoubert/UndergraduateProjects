DEBUG = 1
# You can un-comment the following line to run in device emulation mode on the CPU
# Just remember that only one thread block runs at a time and you can only launch
# as many threads as your operating system supports. 
#DEVICE_EMULATION = 1

NVCCFLAGS   = -arch sm_13
ifdef DEBUG
NVCCFLAGS   += -g #-G
else
NVCCFLAGS   +=
endif

ifdef DEVICE_EMULATION
NVCCFLAGS   += -deviceemu
endif

ifdef DEBUG
GCCFLAGS    += -g
else
GCCFLAGS    +=
endif

all: main.o ImageCleaner.o JPEGWriter.o CpuReference.o
	nvcc -L /usr/local/cuda/lib -lcudart -ljpeg -o ImageCleaner main.o ImageCleaner.o JPEGWriter.o CpuReference.o

main.o: main.cc
	g++ -I /usr/local/cuda/include -c -o main.o main.cc $(GCCFLAGS)

ImageCleaner.o: ImageCleaner.cu
	nvcc -c -o ImageCleaner.o ImageCleaner.cu $(NVCCFLAGS)

JPEGWriter.o: JPEGWriter.cc
	g++ -c -o JPEGWriter.o JPEGWriter.cc $(GCCFLAGS)

CpuReference.o: CpuReference.cc
	g++ -c -o CpuReference.o CpuReference.cc $(GCCFLAGS)

clean:
	rm -f *~ *.o *.linkinfo ImageCleaner
