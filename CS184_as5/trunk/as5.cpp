#include "as5.h"

using namespace std;

//****************************************************
// Some Classes
//****************************************************
class Viewport {
public:
	int w, h; // width and height
};


//****************************************************
// Global Variables
//****************************************************
Viewport	viewport;
vector<Patch*> bunchOPatches;
int numOfPatches;
double stepSize;
char divideType;
ifstream file;

//****************************************************
// reshape viewport if the window is resized
//****************************************************
void myReshape(int w, int h) {
	viewport.w = w;
	viewport.h = h;

	glViewport(0,0,viewport.w,viewport.h);// sets the rectangle that will be the window
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();				// loading the identity matrix for the screen

	//----------- setting the projection -------------------------
	// glOrtho sets left, right, bottom, top, zNear, zFar of the chord system


	// glOrtho(-1, 1 + (w-400)/200.0 , -1 -(h-400)/200.0, 1, 1, -1); // resize type = add
	// glOrtho(-w/400.0, w/400.0, -h/400.0, h/400.0, 1, -1); // resize type = center

	glOrtho(-1, 1, -1, 1, 1, -1);	// resize type = stretch

	//------------------------------------------------------------
}


//****************************************************
// sets the window up
//****************************************************
void initScene(){
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Clear to black, fully transparent
	
	myReshape(viewport.w,viewport.h);
}


//***************************************************
// function that does the actual drawing
//***************************************************
void myDisplay() {
	
	glClear(GL_COLOR_BUFFER_BIT);				// clear the color buffer (sets everything to black)
	
	glMatrixMode(GL_MODELVIEW);					// indicate we are specifying camera transformations
	glLoadIdentity();							// make sure transformation is "zero'd"
	
	//----------------------- code to draw objects --------------------------
							// Rectangle Code
	//glColor3f(red component, green component, blue component);
	glColor3f(1.0f,0.0f,0.0f);					// setting the color to pure red 90% for the rect
/*	
	glBegin(GL_POLYGON);						// draw rectangle 
		//glVertex3f(x val, y val, z val (won't change the point because of the projection type));
		glVertex3f(-0.8f, 0.0f, 0.0f);			// bottom left corner of rectangle
		glVertex3f(-0.8f, 0.5f, 0.0f);			// top left corner of rectangle
		glVertex3f( 0.0f, 0.5f, 0.0f);			// top right corner of rectangle
		glVertex3f( 0.0f, 0.0f, 0.0f);			// bottom right corner of rectangle
	glEnd();
							// Triangle Code
	glColor3f(1.0f,0.5f,0.0f);					// setting the color to orange for the triangle
	
	float basey = -sqrt(0.48f);					// height of triangle = sqrt(.8^2-.4^2)
	glBegin(GL_POLYGON);
		glVertex3f(0.5f,  0.0f, 0.0f);			// top tip of triangle
		glVertex3f(0.1f, basey, 0.0f);			// lower left corner of triangle
		glVertex3f(0.9f, basey, 0.0f);			// lower right corner of triangle
	glEnd();
	//-----------------------------------------------------------------------
	*/
	glBegin(GL_POINTS);
	
	glEnd();
	
	glFlush();
	glutSwapBuffers();					// swap buffers (we earlier set double buffer)
}


//****************************************************
// called by glut when there are no messages to handle
//****************************************************
void myFrameMove() {
	//nothing here for now
#ifdef _WIN32
	Sleep(10);						//give ~10ms back to OS (so as not to waste the CPU)
#endif
	glutPostRedisplay(); // forces glut to call the display function (myDisplay())
}


//****************************************************
// the file parser. Gets a patch file and stores the data
// into the vector of patches and number of patches.
//****************************************************

void getPatches(string filename) {
	file.open(filename.c_str(), ios::in);
	if (!file.is_open())
		return;
	// Get the number of patches
	file >> numOfPatches;
	printDebug(1, "Reading in " << numOfPatches << " patches...");
	
	double x, y, z; // Set up variables for coordinates
	// All right, start reading in patch data
	for (int i = 0; i<numOfPatches; i++) {
		Patch *newPatch;
		for (int u = 0; u<4; u++) {
			for (int v = 0; v<4; v++) {
				file >> x >> y >> z;
				newPatch->cP[u][v] = new Point(x, y, z);
			}
		}
		bunchOPatches.push_back(newPatch);
	}
	file.close();
}

//****************************************************
// the usual stuff, nothing exciting here
//****************************************************
void render() {
  	//This tells glut to use a double-buffered window with red, green, and blue channels 
  	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

  	// Initalize theviewport size
  	viewport.w = 400;
  	viewport.h = 400;

  	//The size and position of the window
  	glutInitWindowSize(viewport.w, viewport.h);
  	glutInitWindowPosition(0, 0);
  	glutCreateWindow("CS184!");

  	initScene();							// quick function to set up scene
  
  	glutDisplayFunc(myDisplay);				// function to run when its time to draw something
  	glutReshapeFunc(myReshape);				// function to run when the window gets resized
  	glutIdleFunc(myFrameMove);				// function to run when not handling any other task
  	glutMainLoop();							// infinite loop that will keep drawing and resizing and whatever else
  
}

int main(int argc, char *argv[]) {

	printInfo("Bezier Curve Renderer loading...");

  	//Read in the cmd args and store them
  	string filename = argv[1];
  	getPatches(filename);
  	stepSize = atof(argv[2]);
  	divideType = argv[3][1];
  	
  	printInfo("Initialized data!");
  	
  	//This initializes glut
  	glutInit(&argc, argv);
  	
  	render();
  	
  	return 0;
}








