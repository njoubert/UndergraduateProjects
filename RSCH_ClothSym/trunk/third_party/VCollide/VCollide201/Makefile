############################################################################
#
# Top Level Makefile for V-Collide.
#
#
############################################################################

#Choose the C++ compiler you want
CC = g++

# If you want the demos to be compiled with a graphical display of 
# the simulation, then uncomment the following lines and provide
# the correct locations of the libraries and include files.

GRAPHICS_CFLAGS   = -DOGL_GRAPHICS
#GRAPHICS_INCLUDES = -I/afs/unc/project/walk/include
GRAPHICS_INCLUDES = -L"/System/Library/Frameworks/OpenGL.framework/Libraries" -I/usr/X11/include
#GRAPHICS_LDFLAGS  = -L/afs/unc/project/walk/lib/`hw_os`
GRAPHICS_LDFLAGS  = -lGL -lGLU -lm -lstdc++ -framework GLUT -framework OpenGL -L"/System/Library/Frameworks/OpenGL.framework/Libraries" 

GRAPHICS_LIBS     = -lGLU -lGL

all : 
	cd RAPID; \
	$(MAKE) CC="$(CC)"
	cd src; \
	$(MAKE) CC="$(CC)" install
	cd demos; \
	$(MAKE) CC="$(CC)" GRAPHICS_CFLAGS="$(GRAPHICS_CFLAGS)" GRAPHICS_INCLUDES="$(GRAPHICS_INCLUDES)" GRAPHICS_LDFLAGS="$(GRAPHICS_LDFLAGS)" GRAPHICS_LIBS="$(GRAPHICS_LIBS)"

clean : 
	cd RAPID; \
	$(MAKE) clean
	cd src; \
	$(MAKE) clean
	cd lib; \
	rm -f libVCollide.a
	cd demos; \
	$(MAKE) clean

