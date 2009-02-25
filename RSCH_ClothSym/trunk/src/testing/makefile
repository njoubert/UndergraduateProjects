#My Code   -------------------------------

TARGET := testLargeMath
SOURCES := $(wildcard ../math/*.cpp) $(wildcard ../util/*.cpp) $(wildcard ../connectors/*.cpp) ../Mesh.cpp ../global.cpp testLargeMath.cpp

PLATFORM := OSX
#PLATFORM := _WIN32
#PLATFORM := _POSIX
#Libraries -------------------------------
FRAMEWORK := -framework GLUT -framework OpenGL

INCLUDE := -I./include/ \
    -I/usr/X11/include
    
LIBRARY := -L./lib/ \
    -L"/System/Library/Frameworks/OpenGL.framework/Libraries" \
    -lGL -lGLU -lm -lstdc++

#Basic Stuff -----------------------------

CC := gcc
CXX := g++
MACROS := -D$(PLATFORM) -D'COMPLEX=std::complex<double>' -DCATCH_ERRORS #-DENABLE_OMP=8
CXXFLAGS := -O2 -fmessage-length=0 $(INCLUDE) $(MACROS)
LDFLAGS := $(FRAMEWORK) $(LIBRARY) $(MACROS) #-fopenmp

#-----------------------------------------
OBJECTS = $(SOURCES:.cpp=.o)

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $(TARGET)	
	
default: $(TARGET)

all: $(TARGET)

clean:
	/bin/rm -f $(OBJECTS) $(TARGET) $(TESTOBJECTS) $(TESTTARGETS) 