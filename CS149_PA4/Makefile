###
# This is to make things run on my own machine:
BMPNOTJPEG = 1
FORCE32BIT = 1
###

DEBUG = 1
# You can un-comment the following line to run in device emulation mode on the CPU
# Just remember that only one thread block runs at a time and you can only launch
# as many threads as your operating system supports. 
#DEVICE_EMULATION = 1

MACROS := 
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
ifdef FORCE32BIT
GCCFLAGS	+= -m32
endif

ifeq ($(BMPNOTJPEG),1)
	MACROS += -DBMPNOTJPEG
	IMAGEOBJ := BMPWriter.o
	IMGLIB :=
else
	IMAGEOBJ := JPEGWriter.o
	IMGLIB := -ljpeg
endif

all: main.o ImageCleaner.o $(IMAGEOBJ) CpuReference.o
	nvcc -L /usr/local/cuda/lib -lcudart $(IMGLIB)  $(MACROS) -o ImageCleaner main.o ImageCleaner.o BMPWriter.o CpuReference.o

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

clean:
	rm -f *~ *.o *.linkinfo ImageCleaner
