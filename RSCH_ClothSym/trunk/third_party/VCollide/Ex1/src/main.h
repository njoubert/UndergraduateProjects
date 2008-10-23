#include <iostream>

//LINUX LIBRARIES
#ifdef _POSIX

#endif

//OSX LIBRARIES
#ifdef OSX
#	include <GLUT/glut.h>
#	include <OpenGL/glu.h>
#else
#	include <GL/glut.h>
#	include <GL/glu.h>
#endif

//WINDOWS ONLY LIBRARIES
#ifdef _WIN32
#   include <windows.h>
#else
#   include <sys/time.h>
#endif

#include <VCollide.H>

