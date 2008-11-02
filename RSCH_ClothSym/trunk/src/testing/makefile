#My Code   -------------------------------

TARGET := testLargeMath
SOURCES := $(wildcard ../math/*.cpp) testLargeMath.cpp
PLATFORM := OSX
#PLATFORM := _WIN32
#PLATFORM := _POSIX
#Libraries -------------------------------

FRAMEWORK := -framework GLUT -framework OpenGL

INCLUDE :=
    
LIBRARY := 
    
#Basic Stuff -----------------------------

CC := gcc
CXX := g++
MACROS := -D$(PLATFORM) -D'COMPLEX=std::complex<double>' -DCATCHERRORS
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