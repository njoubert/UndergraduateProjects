#My Code   -------------------------------

TARGET := main
SOURCES := $(wildcard ./src/UCB/*.cpp) $(wildcard ./src/*.cpp)

#Libraries -------------------------------

INCLUDE := -I./include/ -I/usr/X11R6/include -I/sw/include -I/usr/sww/include -I/usr/sww/pkg/Mesa/include
LIBRARY := -L./lib/ -L/usr/X11R6/lib -L/sw/lib -L/usr/sww/lib -L/usr/sww/bin -L/usr/sww/pkg/Mesa/lib -lglut -lGLU -lGL -lX11

#Basic Stuff -----------------------------

CC := gcc
CXX := g++
CXXFLAGS := -g -Wall -O3 -fmessage-length=0 $(INCLUDE) $(MACROS)
LDFLAGS := $(LIBRARY)

#-----------------------------------------
OBJECTS = $(SOURCES:.cpp=.o)

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $(TARGET)
	
default: $(TARGET)

clean:
	/bin/rm -f $(OBJECTS) $(TARGET)


