#My Code   -------------------------------

TARGET := main
SOURCES := $(wildcard ./src/*.cpp)
PLATFORM := OSX
#PLATFORM := _WIN32
#PLATFORM := _POSIX

#Libraries -------------------------------

INCLUDE := -I./include/
LIBRARY := -L./lib/ 

#Basic Stuff -----------------------------

CC := gcc
CXX := g++
MACROS := -D$(PLATFORM)
CXXFLAGS := -g -Wall -O2 -fmessage-length=0 $(INCLUDE) $(MACROS)
LDFLAGS := $(LIBRARY) $(MACROS)

#-----------------------------------------
OBJECTS = $(SOURCES:.cpp=.o)

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $(TARGET)
	
default: $(TARGET)

clean:
	/bin/rm -f $(OBJECTS) $(TARGET)