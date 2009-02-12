#My Code   -------------------------------

TARGET := ClothSym
SOURCES := $(wildcard ./src/*.cpp) \
	$(wildcard ./src/util/*.cpp) \
    $(wildcard ./src/io/*.cpp) \
    $(wildcard ./src/math/*.cpp) \
    $(wildcard ./src/connectors/*.cpp) 
    
#Necessary declarations follow:
INCLUDE := 
LIBRARY := 
FRAMEWORK :=
MACROS :=
PLATFORM :=
    
#Options --------------------------------
    
USEFREEIMAGE := false
#ENABLE_OMP := 8	# enables and specifies the number of threads to use.    
MACROS += -DPERFORMANCEHAXX
MACROS += -DCATCHERRORS

#========================================================================
#========================================================================
    
    
#Platform Dependent Libs and Incs --------------------------------------------

#Check for OSX. Works on 10.5 (and 10.4 but untested)
#This line still kicks out an annoying error on Solaris.
ifeq ($(shell sw_vers 2>/dev/null | grep Mac | awk '{ print $$2}'),Mac)
	#Assume Mac
	
	PLATFORM := OSX
	INCLUDE := -I./include/ \
    -I/usr/X11/include
	LIBRARY := -L./lib/ \
    	-L"/System/Library/Frameworks/OpenGL.framework/Libraries" \
    	-lGL -lGLU -lm -lstdc++
	FRAMEWORK := -framework GLUT -framework OpenGL

else
	#Assume X11
	
	PLATFORM := _POSIX
	INCLUDE := -I./include/ -I/usr/X11R6/include -I/sw/include \
		-I/usr/sww/include -I/usr/sww/pkg/Mesa/include
	LIBRARY := -L./lib/ -L/usr/X11R6/lib -L/sw/lib -L/usr/sww/lib \
		-L/usr/sww/bin -L/usr/sww/pkg/Mesa/lib -lglut -lGLU -lGL -lX11

endif

ifeq ($(strip $(USEFREEIMAGE)),true)
	LIBRARY += -lfreeimage
	MACROS += -DFREEIMAGE -D'COMPLEX=std::complex<double>'
endif

#Basic Config -----------------------------------------------------------------

CC := gcc
CXX := g++
MACROS += -D$(PLATFORM)
CXXFLAGS := -Wall -O2 -fmessage-length=0 $(INCLUDE) $(MACROS)
LDFLAGS := $(FRAMEWORK) $(LIBRARY) $(MACROS)

ifdef ENABLE_OMP
	MACROS += -DENABLE_OMP=$(ENABLE_OMP)
	CXXFLAGS += -fopenmp
	LDFLAGS += -fopenmp
endif

#-----------------------------------------------------------------------------

%.o : %.cpp
	@echo "Compiling .cpp to .o for $(PLATFORM) Platform: $< " 
	@$(CXX) -c -Wall $(CXXFLAGS) $< -o $@
	
OBJECTS = $(SOURCES:.cpp=.o)

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $(TARGET)	
	
default: $(TARGET)

all: $(TARGET)
	
clean:
	@echo "Removing compiled files."
	/bin/rm -f $(OBJECTS) $(TARGET) $(TESTOBJECTS) $(TESTTARGETS) 