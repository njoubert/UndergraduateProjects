//============================================================================
// Name        : shader.cpp
// Author      : Niels Joubert CS184-dv
// Version     :
// Copyright   : 2008
// Description : A shader using the Phong Illumination Model
//============================================================================

#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

#ifdef _WIN32
#	include <windows.h>
#else
#	include <sys/time.h>
#endif

#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include <math.h>

const double PI = 3.14159265;

using namespace std;

//****************************************************
// Some Classes
//****************************************************

class Viewport;
class Vector3d;
class Light;

class Viewport {
public:
	int w, h; // width and height
};


class Vector3d {
public:
	float x, y, z; // width and height
};

class Light {
public:
	Light() {
		
	}
	Light(Vector3d pos) : position(pos) {
		
	}
	void setPosition(float x, float y, float z) {
		
	}
	void setPosition(Vector3d v) {
		
	}
	void setLuminosity(float r, float g, float b) {
		
	}
	void setLuminosity(Vector3d v) {
		
	}
private:
	Vector3d position;
	Vector3d luminosity;
};


//****************************************************
// Global Variables
//****************************************************
Viewport	viewport;
int 		plotX = 400;
int 		plotY = 300;



void setPixel(int x, int y, GLfloat r, GLfloat g, GLfloat b) {
	glColor3f(r, g, b);
	glVertex2f(x, y);
}

void circle(int x, int y, int radius) {
	glBegin(GL_POINTS);

	for (int i = -radius; i <= radius; i++) {
		int width = (int)(sqrt((float)(radius*radius-i*i)) + 0.5f);
		for (int j = -width; j <= width; j++) {

			setPixel(x + j, y + i, 1.0f, 1.0f, 1.0f);
		}
	}
	
	glEnd();

}

//========================================
//
// OpenGL Callbacks Follow
//
//========================================

/*
 * Initializes Viewport
 */
void initScene(){
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glViewport (0,0,viewport.w,viewport.h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,viewport.w, 0, viewport.h);
}

/*
 * Callback function to draw scene 
 */
void myDisplay() {

	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//circle(plotX, plotY, min(viewport.w, viewport.h) / 4);

	glFlush();
	glutSwapBuffers();
	
}

/*
 * Callback function for window reshape event
 */
void myReshape(int w, int h) {
	viewport.w = w;
	viewport.h = h;

	glViewport (0,0,viewport.w,viewport.h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, viewport.w, 0, viewport.h);

}

void parseCommandLine(int argc, char *argv[], vector<Light> & lights) {
	Light * l = new Light();
	l->setLuminosity(1.0, 1.0, 1.0);
	l->setPosition(50, 50, 0);
	
	lights.push_back(l);
}



int main(int argc, char *argv[]) {
	
	vector<Light> lights;
	
	parseCommandLine(argc, argv, lights);
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	viewport.w = 800;
	viewport.h = 600;
	glutInitWindowSize(viewport.w, viewport.h);
	glutInitWindowPosition(0,0);
	glutCreateWindow(argv[0]);

	initScene();

	glutDisplayFunc(myDisplay);					// function to run when its time to draw something
	glutReshapeFunc(myReshape);					// function to run when the window gets resized
	//glutIdleFunc(myFrameMove); 		
	glutMainLoop();							// infinite loop that will keep drawing and resizing and whatever else

	//cleanUp();
	
	return 0;
	
}