#My Code   -------------------------------

TARGET := main
SOURCES := $(wildcard ./src/*.cpp)

#Libraries -------------------------------

INCLUDE := -I./include/
LIBRARY := -L./lib/

#Basic Stuff -----------------------------

CC := gcc
CXX := g++
CXXFLAGS := -g -Wall -O2 -fmessage-length=0 $(INCLUDE)
LDFLAGS := $(LIBRARY)

#-----------------------------------------
OBJECTS = $(SOURCES:.cpp=.o)

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $(TARGET)
	
default: $(TARGET)

clean:
	/bin/rm -f $(OBJECTS) $(TARGET)