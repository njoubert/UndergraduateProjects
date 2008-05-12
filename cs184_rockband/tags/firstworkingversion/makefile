#Libraries/Includes ----------------------
INCLUDES := -I/usr/local/include/opencv
LIBS := -lcv -lcvaux -lcxcore -lhighgui -lml
#-----------------------------------------
#Compiler -- -----------------------------
CXX          = g++ $(INCLUDES) -g -Wall
cc          = gcc -g -Wall -fmessage-length=0
#-----------------------------------------
#Optimization ----------------------------
OPT   = -O2
#-----------------------------------------

FILES := \
global.h \
utils.o \
Guitar.o \
Convolver.o \
PeakDetector.o \
GuitarTimer.o \
NoteTracker.o \
StringAnalyzer.o \
rockbandcv.o

TARGET := rockbandcv

all: $(FILES)
	$(CXX) $(LIBS) $(FILES) -o $(TARGET)

clean:
	/bin/rm -f *.o $(TARGETS)