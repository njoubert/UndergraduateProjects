#OS Dependencies ----------------------


#Code   -------------------------------

TARGET := main
SOURCES := $(wildcard ./src/UCB/*.cpp) $(wildcard ./src/*.cpp)

#Libraries -------------------------------

#Check for OSX. Works on 10.5 (and 10.4 but untested)
ifeq ($(shell sw_vers | grep Mac | awk '{ print $$2}'),Mac)
	#Assume Mac
	INCLUDE := -I./include/ -I/usr/X11/include
	LIBRARY := -L./lib/ \
    	-L"/System/Library/Frameworks/OpenGL.framework/Libraries" \
    	-lGL -lGLU -lm -lstdc++
	FRAMEWORK := -framework GLUT -framework OpenGL
	MACROS := -DOSX
else
	#Assume X11
	FRAMEWORK := 
	INCLUDE := -I./include/ -I/usr/X11R6/include -I/sw/include -I/usr/sww/include -I/usr/sww/pkg/Mesa/include
	LIBRARY := -L./lib/ -L/usr/X11R6/lib -L/sw/lib -L/usr/sww/lib -L/usr/sww/bin -L/usr/sww/pkg/Mesa/lib -lglut -lGLU -lGL -lX11
	MACROS :=
endif


#Basic Stuff -----------------------------

CC := gcc
CXX := g++
CXXFLAGS := -g -Wall -O3 -fmessage-length=0 $(INCLUDE) $(MACROS)
LDFLAGS := $(FRAMEWORK) $(LIBRARY)

#-----------------------------------------
OBJECTS = $(SOURCES:.cpp=.o)

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $(TARGET)
	
default: $(TARGET) 

all: default

clean:
	/bin/rm -f $(OBJECTS) $(TARGET)


