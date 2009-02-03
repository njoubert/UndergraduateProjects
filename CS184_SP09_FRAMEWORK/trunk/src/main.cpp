#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

//Include our math library
#include <algebra3.h>
//Include our Image Saving library.
#include "UCB/ImageSaver.h"

#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

using namespace std;


//****************************************************
// Some Classes
//****************************************************
class Viewport {
public:
	int w, h; // width and height
};

class Polygon {
public:
	Polygon() {
		//Constructor
	}

private:
};

class Vertex {
public:
	Vertex(): pos(0.0,0.0) {
		//Constuctor
	}
private:
	vec2 pos;
};

//****************************************************
// Global Variables
//****************************************************
Viewport viewport;
Polygon polygon;
Vertex * tempVertex;
UCB::ImageSaver * imgSaver;

float offs = 0.2;

//-------------------------------------------------------------------------------
void display()
{

	//Clear Buffers
    glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);					// indicate we are specifying camera transformations
	glLoadIdentity();							// make sure transformation is "zero'd"

	// draw some lines
    glBegin(GL_LINE_STRIP);
	glVertex2f(-offs,offs);
	glVertex2f(-offs,-offs);
	glVertex2f(offs,-offs);
	glVertex2f(offs,offs);
	glVertex2f(-offs,offs);
    glEnd();

	//Now that we've drawn on the buffer, swap the drawing buffer and the displaying buffer.
	glutSwapBuffers();
}

//-------------------------------------------------------------------------------
/// \brief	Called when the screen gets resized
///
void reshape(int w, int h)
{
	//Set up the viewport to ignore any size change.
	glViewport(0,0,viewport.w,viewport.h);

	//Set up the PROJECTION transformationstack to be a simple orthographic [-1, +1] projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, 1, -1);	// resize type = stretch

	//Set the MODELVIEW transformation stack to the identity.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//-------------------------------------------------------------------------------
///
void myKeyboardFunc (unsigned char key, int x, int y) {
	switch (key) {
		case 27:			// Escape key
			exit(0);
			break;
	}
}

//-------------------------------------------------------------------------------
///
void myMouseFunc( int button, int state, int x, int y ) {
	if ( button==GLUT_LEFT_BUTTON && state==GLUT_DOWN ) {
		float xPos = ((float)x - viewport.w/2)/((float)(viewport.w/2));
		float yPos = ((float)y - viewport.h/2)/((float)(viewport.h/2));

		//Flip the values to get the correct position relative to our coordinate axis.
		yPos = -yPos;

		cout << "Mouseclick at " << xPos << "," << yPos << "." << endl;

		offs *= 1.05;
		
		glutPostRedisplay();
	}

	if ( button==GLUT_RIGHT_BUTTON && state==GLUT_DOWN ) {
	    //Save a window capture to disk
	    imgSaver->saveFrame(viewport.w, viewport.h);

	}
}

//-------------------------------------------------------------------------------
///
int main(int argc,char** argv)
{
	//Initialize OpenGL
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);

	//Set up global variables
	viewport.w = 600;
	viewport.h = 600;

	imgSaver = new UCB::ImageSaver("./", "morph");

	//Create OpenGL Window
	glutInitWindowSize(viewport.w,viewport.h);
	glutInitWindowPosition(0,0);
	glutCreateWindow("CS184!");

	//Register event handlers with OpenGL.
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(myKeyboardFunc);
	glutMouseFunc(myMouseFunc);

	//And Go!
	glutMainLoop();
}
