#Basic Stuff -----------------------------

CC := gcc
CXX := g++
CXXFLAGS := -g -Wall -O2 -fmessage-length=0 -I./

#Libraries -------------------------------

LDFLAGS := -L./
#eg: LDFLAGS := -L./ -lfreeimage

#My Code   -------------------------------

SOURCES = main.cpp
TARGET = main

#-----------------------------------------
OBJECTS = $(SOURCES:.cpp=.o)

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $(TARGET)
	
default: $(TARGET)

clean:
	/bin/rm -f $(OBJECTS) $(TARGET)