#My Code   -------------------------------

TARGET := main
SOURCES := $(wildcard ./src/*.cpp)

#Libraries -------------------------------

INCLUDE := -I./include/ -I/usr/X11R6/include
LIBRARY := -L./lib/ -L/usr/X11R6/lib \
	-lGL -lGLU -lglut -lXmu -lXi -lstdc++ -lm

#Basic Stuff -----------------------------

CC := gcc
CXX := g++
CXXFLAGS := -g -Wall -O2 -fmessage-length=0 $(INCLUDE) $(MACROS)
LDFLAGS := $(LIBRARY)

#-----------------------------------------
OBJECTS = $(SOURCES:.cpp=.o)

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $(TARGET)
	
default: $(TARGET)

clean:
	/bin/rm -f $(OBJECTS) $(TARGET)