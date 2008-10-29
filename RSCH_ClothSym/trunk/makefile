#My Code   -------------------------------

TARGET := ClothSym
SOURCES := $(wildcard ./src/*.cpp) \
    $(wildcard ./src/io/*.cpp)
PLATFORM := OSX
#PLATFORM := _WIN32
#PLATFORM := _POSIX

#Libraries -------------------------------

FRAMEWORK := -framework GLUT -framework OpenGL

INCLUDE := -I./include/ \
    -I./include/mv \
    -I./include/SparseLib++ \
    -I/usr/X11/include
    
LIBRARY := -L./lib/ \
    -L"/System/Library/Frameworks/OpenGL.framework/Libraries" \
    -lGL -lGLU -lm -lstdc++ -lfreeimage -lsparse -lspblas -lmv
    
#Basic Stuff -----------------------------

CC := gcc
CXX := g++
MACROS := -D$(PLATFORM) -D'COMPLEX=std::complex<double>' 
CXXFLAGS := -g -Wall -O2 -fmessage-length=0 $(INCLUDE) $(MACROS)
LDFLAGS := $(FRAMEWORK) $(LIBRARY) $(MACROS)

#-----------------------------------------
OBJECTS = $(SOURCES:.cpp=.o)

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $(TARGET)
	
default: $(TARGET)

all: $(TARGET)

clean:
	/bin/rm -f $(OBJECTS) $(TARGET)