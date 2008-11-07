#My Code   -------------------------------

TARGET := ClothSym
SOURCES := $(wildcard ./src/*.cpp) \
	$(wildcard ./src/util/*.cpp) \
    $(wildcard ./src/io/*.cpp) \
    $(wildcard ./src/math/*.cpp) \
    $(wildcard ./src/connectors/*.cpp) 
PLATFORM := OSX
MACROS := -DPERFORMANCEHAXX
MACROS += -DCATCHERRORS
#PLATFORM := _WIN32
#PLATFORM := _POSIX

#Libraries -------------------------------

FRAMEWORK := -framework GLUT -framework OpenGL

INCLUDE := -I./include/ \
    -I/usr/X11/include
    
LIBRARY := -L./lib/ \
    -L"/System/Library/Frameworks/OpenGL.framework/Libraries" \
    -lGL -lGLU -lm -lstdc++ -lfreeimage -lsparse -lspblas -lmv
    
#Basic Stuff -----------------------------

CC := gcc
CXX := g++
MACROS += -D$(PLATFORM) #-D'COMPLEX=std::complex<double>' 
CXXFLAGS := -g -Wall -O2 -fmessage-length=0 $(INCLUDE) $(MACROS)
LDFLAGS := $(FRAMEWORK) $(LIBRARY) $(MACROS)

#-----------------------------------------
OBJECTS = $(SOURCES:.cpp=.o)

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $(TARGET)	
	
default: $(TARGET)

all: $(TARGET)
	
clean:
	/bin/rm -f $(OBJECTS) $(TARGET) $(TESTOBJECTS) $(TESTTARGETS) 