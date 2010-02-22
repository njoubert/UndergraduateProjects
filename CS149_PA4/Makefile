DEBUG = 1
# You can un-comment the following line to run in device emulation mode on the CPU
# Just remember that only one thread block runs at a time and you can only launch
# as many threads as your operating system supports. 
#DEVICE_EMULATION = 1

MACROS 			:= 
NVCCFLAGS   	= -arch sm_13
LDFLAGS 		:= -lcudart  -fopenmp

###
# This is to make things run on my own machine:
HP_64 =	$(shell uname -m | grep 64)
ifeq "$(strip $(HP_64))" ""
	BMPNOTJPEG = 1
	FORCE32BIT = 1
	MACROS += -DSKIPREFERENCE	
endif
###


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

ifdef FORCE32BIT
GCCFLAGS	+= -m32
LDFLAGS		+= -arch i386 -L/usr/local/cuda/lib
else
LDFLAGS		+= -L/usr/local/cuda/lib64
endif

GCCFLAGS += -fopenmp

ifeq ($(BMPNOTJPEG),1)
	MACROS += -DBMPNOTJPEG
	IMAGEOBJ := BMPWriter.o
	IMGLIB :=
else
	IMAGEOBJ := JPEGWriter.o
	IMGLIB := -ljpeg
endif

all: main.o ImageCleaner.o $(IMAGEOBJ) CpuReference.o OpenMPReference.o
	g++ $(LDFLAGS) $(IMGLIB) $(MACROS) -o ImageCleaner main.o ImageCleaner.o CpuReference.o OpenMPReference.o $(IMAGEOBJ)

main.o: main.cc
	g++ -I /usr/local/cuda/include -c -o main.o main.cc $(GCCFLAGS) $(MACROS)

ImageCleaner.o: ImageCleaner.cu
	nvcc -c -o ImageCleaner.o ImageCleaner.cu $(NVCCFLAGS) $(MACROS)

BMPWriter.o: BMPWriter.cc
	g++ -c -o BMPWriter.o BMPWriter.cc $(GCCFLAGS) $(MACROS)

JPEGWriter.o: JPEGWriter.cc
	g++ -c -o JPEGWriter.o JPEGWriter.cc $(GCCFLAGS) $(MACROS)

CpuReference.o: CpuReference.cc
	g++ -c -o CpuReference.o CpuReference.cc $(GCCFLAGS) $(MACROS)

OpenMPReference.o: OpenMPReference.cc
	g++ -c -o OpenMPReference.o OpenMPReference.cc $(GCCFLAGS) $(MACROS)

clean:
	rm -f *~ *.o *.linkinfo ImageCleaner
