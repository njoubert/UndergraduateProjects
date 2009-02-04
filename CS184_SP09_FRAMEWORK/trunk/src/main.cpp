#include "main.h"

using namespace std;

//****************************************************
// Some Classes
//****************************************************
class Viewport {
public:
    Viewport(): mousePos(0.0,0.0) { /* empty */ };
	int w, h; // width and height
	vec2 mousePos;
};

//****************************************************
// Global Variables
//****************************************************
Viewport viewport;
Polygon * polygon;
Vertex * tempVertex;
UCB::ImageSaver * imgSaver;


//-------------------------------------------------------------------------------
/// You will be calling all of your drawing-related code from this function.
/// Nowhere else in your code should you use glBegin(...) and glEnd() except code
/// called from this method.
///
/// To force a redraw of the screen (eg. after mouse events or the like) simply call
/// glutPostRedisplay();
void display() {

	//Clear Buffers
    glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);					// indicate we are specifying camera transformations
	glLoadIdentity();							// make sure transformation is "zero'd"

	// draw cursor
    glBegin(GL_LINES);
        glVertex2f(viewport.mousePos[0]-0.03, viewport.mousePos[1]);
        glVertex2f(viewport.mousePos[0]+0.03, viewport.mousePos[1]);
        glVertex2f(viewport.mousePos[0], viewport.mousePos[1]-0.03);
        glVertex2f(viewport.mousePos[0], viewport.mousePos[1]+0.03);
    glEnd();

    polygon->draw();

	//Now that we've drawn on the buffer, swap the drawing buffer and the displaying buffer.
	glutSwapBuffers();

}


//-------------------------------------------------------------------------------
/// \brief	Called when the screen gets resized.
/// This gives you the opportunity to set up all the relevant transforms.
///
void reshape(int w, int h) {
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
/// Called to handle keyboard events.
void myKeyboardFunc (unsigned char key, int x, int y) {
	switch (key) {
		case 27:			// Escape key
			exit(0);
			break;
	}
}


//-------------------------------------------------------------------------------
/// This function is used to convert the pixel coordinates on the screen to the
/// coordinates of your view volume. Thus, it converts a pixel location to a coordinate
/// that can be used for picking and selection.
vec2 inverseViewportTransform(vec2 screenCoords) {
    //Create a vec2 on the local stack. See algebra3.h
    vec2 viewCoords(0.0,0.0);

    viewCoords[0] = ((float)screenCoords[0] - viewport.w/2)/((float)(viewport.w/2));
    viewCoords[1] = ((float)screenCoords[1] - viewport.h/2)/((float)(viewport.h/2));
    //Flip the values to get the correct position relative to our coordinate axis.
    viewCoords[1] = -viewCoords[1];

    //C++ will copy the whole vec2 to the calling function.
    return viewCoords;
}


//-------------------------------------------------------------------------------
///
void myMouseFunc( int button, int state, int x, int y ) {
    //Convert the pixel coordinates to view coordinates.
    vec2 screenCoords((double) x, (double) y);
    vec2 viewCoords = inverseViewportTransform(screenCoords);

	if ( button==GLUT_LEFT_BUTTON && state==GLUT_DOWN ) {
		cout << "Mouseclick at " << viewCoords[0] << "," << viewCoords[1] << "." << endl;

		//YOUR CODE HERE
		//possible do something with the tempVertex?

	}

	if ( button==GLUT_LEFT_BUTTON && state==GLUT_UP ) {

	   //YOUR CODE HERE
	    //Possibly do something with the tempVertex?

	}

	if ( button==GLUT_RIGHT_BUTTON && state==GLUT_DOWN ) {
	    //Save a window capture to disk as a BITMAP file.

	    //YOUR CODE HERE

	    //Move this somewhere more convenient!
	    imgSaver->saveFrame(viewport.w, viewport.h);
	}

	//Force a redraw of the window
	glutPostRedisplay();

}


//-------------------------------------------------------------------------------
/// Called whenever the mouse moves while a button is pressed
void myActiveMotionFunc(int x, int y) {
    //Record the mouse location for drawing crosshairs
    viewport.mousePos = inverseViewportTransform(vec2((double)x,(double)y));

    if (tempVertex != NULL)
        tempVertex->setStartPos(viewport.mousePos);

    //Force a redraw of the window.
    glutPostRedisplay();
}


//-------------------------------------------------------------------------------
/// Called whenever the mouse moves without any buttons pressed.
void myPassiveMotionFunc(int x, int y) {
    //Record the mouse location for drawing crosshairs
    viewport.mousePos = inverseViewportTransform(vec2((double)x,(double)y));

    //Force a redraw of the window.
    glutPostRedisplay();
}


//-------------------------------------------------------------------------------
/// Initialize the environment
int main(int argc,char** argv) {
	//Initialize OpenGL
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);

	//Set up global variables
	viewport.w = 600;
	viewport.h = 600;

	if (argc < 2) {
	    cout << "USAGE: morph poly.obj" << endl;
	    exit(1);
	}

	//Initialize the screen capture class to save BMP captures
	//in the current directory, with the prefix "morph"
	imgSaver = new UCB::ImageSaver("./", "morph");

	//Parse the OBJ file.
	OBJParser parser;
	polygon = parser.parseOBJ(argv[1]);

	//Create OpenGL Window
	glutInitWindowSize(viewport.w,viewport.h);
	glutInitWindowPosition(0,0);
	glutCreateWindow("CS184 Framework");

	//Register event handlers with OpenGL.
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(myKeyboardFunc);
	glutMouseFunc(myMouseFunc);
	glutMotionFunc(myActiveMotionFunc);
	glutPassiveMotionFunc(myPassiveMotionFunc);

	//And Go!
	glutMainLoop();
}
