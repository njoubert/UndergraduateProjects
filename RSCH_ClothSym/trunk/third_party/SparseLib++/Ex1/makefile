#My Code   -------------------------------

TARGET := main
SOURCES := $(wildcard ./src/*.cpp)

#Libraries -------------------------------

INCLUDE := -I./include/ -I./mv/include/
LIBRARY := -L./lib/ -lm ./lib/libspblas.a ./lib/libsparse.a ./lib/libmv.a

#Basic Stuff -----------------------------

CC := gcc
CXX := g++
CXXFLAGS := -g -Wall -O2 -fmessage-length=0 \
	-DMV_VECTOR_BOUNDS_CHECK '-DCOMPLEX=std::complex<double>' \
	$(INCLUDE)
LDFLAGS := $(LIBRARY)

#-----------------------------------------
OBJECTS = $(SOURCES:.cpp=.o)

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $(TARGET)
	
default: $(TARGET)

clean:
	/bin/rm -f $(OBJECTS) $(TARGET)