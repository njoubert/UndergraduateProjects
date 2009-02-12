#My Code   -------------------------------

TARGET := testLargeMath
SOURCES := $(wildcard ../math/*.cpp) $(wildcard ../util/*.cpp) $(wildcard ../connectors/*.cpp) ../Mesh.cpp ../global.cpp testLargeMath.cpp

PLATFORM := OSX
#PLATFORM := _WIN32
#PLATFORM := _POSIX
#Libraries -------------------------------

FRAMEWORK := #-framework GLUT -framework OpenGL


        PLATFORM := _POSIX
        INCLUDE := -I./include/ -I/usr/X11R6/include -I/sw/include \
                -I/usr/sww/include -I/usr/sww/pkg/Mesa/include
        LIBRARY := -L./lib/ -L/usr/X11R6/lib -L/sw/lib -L/usr/sww/lib \
                -L/usr/sww/bin -L/usr/sww/pkg/Mesa/lib -lglut -lGLU -lGL -lX11

#Basic Stuff -----------------------------

CC := gcc
CXX := g++
MACROS := -D$(PLATFORM) -D'COMPLEX=std::complex<double>' #-DENABLE_OMP -DCATCH_ERRORS
CXXFLAGS := -O2 -fmessage-length=0 -fopenmp $(INCLUDE) $(MACROS)
LDFLAGS := $(FRAMEWORK) $(LIBRARY) $(MACROS) -fopenmp

#-----------------------------------------
OBJECTS = $(SOURCES:.cpp=.o)

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $(TARGET)	
	
default: $(TARGET)

all: $(TARGET)

clean:
	/bin/rm -f $(OBJECTS) $(TARGET) $(TESTOBJECTS) $(TESTTARGETS) 