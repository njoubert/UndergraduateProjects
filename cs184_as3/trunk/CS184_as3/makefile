#!gmake 

#-----------------------------------------
#Basic Stuff -----------------------------
CC          = g++ -g -Wall -fmessage-length=0
cc          = gcc -g -Wall -fmessage-length=0

#-----------------------------------------
#Optimization ----------------------------
OPT   = -O3

#-----------------------------------------
# X       --------------------------------
X_INC  =   -I/usr/X11R6/include -I/sw/include -I/usr/sww/include -I/usr/sww/pkg/Mesa/include
X_LIB  =   -L/usr/X11R6/lib -L/sw/lib -L/usr/sww/lib -L/usr/sww/bin -L/usr/sww/pkg/Mesa/lib

#-----------------------------------------

#-----------------------------------------
# GL      --------------------------------
GL_LIB  =   -lglut -lGLU -lGL -lX11

#-----------------------------------------

TARGETS = shader

OBJECTS = shader.o

#-----------------------------------------

FRAMEWORK = -framework GLUT
FRAMEWORK += -framework OpenGL
MACLIBS = -lGL -lGLU -lm -lstdc++ 
MACINCS = -L"/System/Library/Frameworks/OpenGL.framework/Libraries"

CCOPTSMAC = $(MACINCS) -DOSX
LDOPTSMAC = $(FRAMEWORK) $(MACINCS) $(MACLIBS) -DOSX

LIBS = $(X_LIB) $(GL_LIB)
INCS = $(X_INC)

#CCOPTS = $(OPT) $(INCS)
#LDOPTS = $(OPT) $(INCS) $(LIBS)

CCOPTS = $(CCOPTSMAC)
LDOPTS = $(LDOPTSMAC)
#-----------------------------------------
#-----------------------------------------

default: $(TARGETS)

clean:
	/bin/rm -f *.o $(TARGETS)

#-----------------------------------------
#-----------------------------------------

shader: shader.o
	$(CC) shader.o $(LDOPTS) -o shader

shader.o: Debug.cpp Color.cpp Vector3d.cpp Position.cpp Light.cpp shader.cpp
	$(CC) Debug.cpp Color.cpp Vector3d.cpp Position.cpp Light.cpp shader.cpp -c $(CCOPTS)


#export LD_LIBRARY_PATH="/usr/sww/pkg/Mesa/lib:/usr/lib:/usr/sww/lib"

