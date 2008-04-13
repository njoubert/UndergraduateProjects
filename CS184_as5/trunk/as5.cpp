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
Point* furthestPoint;
int numOfPatches;

//Display Settings:
double stepSize;
bool wireframe;
bool adaptive;
bool smoothShading;

int NIELSDEBUG=0;
int NIELSINFO=1;

static double anglelr=0.0,angleud=0.0,ratio=0.0;
static double x=0.0,y=0.0,z=0.0;
static double lx=0.0,ly=0.0,lz=0.0;

//****************************************************
// reshape viewport if the window is resized
//****************************************************
void myReshape(int w, int h) {
	viewport.w = w;
	viewport.h = h;

	glViewport(0,0,viewport.w,viewport.h);// sets the rectangle that will be the window
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();				// loading the identity matrix for the screen
	//glTranslated(0.0, 0.0, -4.0);
	//----------- setting the projection -------------------------
	// glOrtho sets left, right, bottom, top, zNear, zFar of the chord system


	// glOrtho(-1, 1 + (w-400)/200.0 , -1 -(h-400)/200.0, 1, 1, -1); // resize type = add
	// glOrtho(-w/400.0, w/400.0, -h/400.0, h/400.0, 1, -1); // resize type = center

	float maxv = (*furthestPoint)[0];
	if ((*furthestPoint)[1] > maxv)
		maxv = (*furthestPoint)[1]; 

	glOrtho(-1*maxv, maxv, -1*maxv, maxv, maxv, -1*maxv);	// resize type = stretch

	//------------------------------------------------------------
}


//****************************************************
// sets the window up
//****************************************************
void initScene(){
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f); // Clear to black, fully transparent
	
	myReshape(viewport.w,viewport.h);
}


//***************************************************
// function that does the actual drawing
//***************************************************
void myDisplay() {
	
	glClear(GL_COLOR_BUFFER_BIT);				// clear the color buffer (sets everything to black)
	
	glMatrixMode(GL_MODELVIEW);					// indicate we are specifying camera transformations
	//glLoadIdentity();							// make sure transformation is "zero'd"
	
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
	
	//glLoadIdentity();
	//glTranslatef(0.0f, 0.0f, -3.7f);
	//glRotatef(1.0f, 0.0f, 0.0f, 0.0f);
	//Drawing Points:
	
	
	if (wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	if (!adaptive)	{

		for (int p = 0; p < numOfPatches; p++) {
			
			for (unsigned int u = 0; u < bunchOPatches[p]->bezpoints.size()-1; u++) {
				for (unsigned int v = 0; v < bunchOPatches[p]->bezpoints[u].size()-1; v++) {
					glBegin(GL_QUADS);
					glVertex3dv(bunchOPatches[p]->bezpoints[u][v]->p.data());
					glVertex3dv(bunchOPatches[p]->bezpoints[u+1][v]->p.data());
					glVertex3dv(bunchOPatches[p]->bezpoints[u+1][v+1]->p.data());
					glVertex3dv(bunchOPatches[p]->bezpoints[u][v+1]->p.data());					
					glEnd();
//					glBegin(GL_TRIANGLES);
//					glVertex3dv(bunchOPatches[p]->bezpoints[u][v]->p.data());
//					glVertex3dv(bunchOPatches[p]->bezpoints[u+1][v]->p.data());
//					glVertex3dv(bunchOPatches[p]->bezpoints[u+1][v+1]->p.data());
//					glEnd();
//					glBegin(GL_TRIANGLES);
//					glVertex3dv(bunchOPatches[p]->bezpoints[u][v]->p.data());
//					glVertex3dv(bunchOPatches[p]->bezpoints[u][v+1]->p.data());
//					glVertex3dv(bunchOPatches[p]->bezpoints[u+1][v+1]->p.data());
//					glEnd();
				}	
			}
			
		}
		
	}
	
	
	
	glFlush();
	glutSwapBuffers();					// swap buffers (we earlier set double buffer)
}


//**********Transformation functions***********
void rotateLeftRight(float ang) {
	lx = sin(ang);
	lz = -cos(ang);
	glMatrixMode(GL_MODELVIEW);	
	glLoadIdentity();
	gluLookAt(x, y, z, 
		      x + lx,y + ly,z + lz,
			  0.0f,1.0f,0.0f);
	
}

void rotateUpDown(float ang) {
	ly = sin(ang);
	lz = -cos(ang);
	glMatrixMode(GL_MODELVIEW);	
	glLoadIdentity();
	gluLookAt(x, y, z, 
		      x + lx,y + ly,z + lz,
			  0.0f,1.0f,0.0f);
}

void zoomInOut(int direction) {
//	x = x + direction*(lx)*0.1;
//	z = z + direction*(lz)*0.1;
	x = x + direction*0.1;
	z = z + direction*0.1;

	glMatrixMode(GL_MODELVIEW);	
	glLoadIdentity();
	gluLookAt(x, y, z, 
		      x + lx,y + ly,z + lz,
			  0.0f,1.0f,0.0f);
}

void shiftUDLR(double x, double y) {
	glMatrixMode(GL_MODELVIEW);	
	glLoadIdentity();
	glTranslated(x,y,0.0);
	gluLookAt(x,y,z,
				x+lx, y+ly, z+lz,
				0.0f, 1.0f, 0.0f);
}

//***********Toggle functions***********
void toggleFlatSmooth() {
	printDebug(1, "Toggling smooth shading.");
	smoothShading = !smoothShading;
}

void toggleWireframe() {
	printDebug(1, "Toggling wireframe display.");
	wireframe = !wireframe;
	glutPostRedisplay();
}

//******Keyboard Input Processing*********
void processSpecialKeys(int key, int x, int y) {
	printDebug(2, "Processing special key "<<key);
	switch(key) {
	case GLUT_KEY_UP: 
		if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
			shiftUDLR(0.0, 0.1);
		} else {
			angleud -=0.01;
			rotateUpDown(angleud);
			break;
		}
	case GLUT_KEY_DOWN:
		if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
			shiftUDLR(0.0, -0.1);
		} else {
			angleud +=0.01;
			rotateUpDown(angleud);

		}break;
	case GLUT_KEY_RIGHT:
		if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
			shiftUDLR(0.1, 0.0);
		} else {
			anglelr +=0.01;
			rotateLeftRight(anglelr);
		}
		break;

	case GLUT_KEY_LEFT:
		if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
			shiftUDLR(-0.1, 0.0);
		} else {
			anglelr -= 0.01;
			rotateLeftRight(anglelr);

		}break;

	}
}

void processKeys(unsigned char key, int x, int y) {
	printDebug(2, "Processing normal key "<<key);
	switch (key) {
	case '=':
		zoomInOut(1);
		break;
	case '-':
		zoomInOut(-1);
		break;
	case 's':
		toggleFlatSmooth();
		break;
	case 'w':
		toggleWireframe(); //toggle filled and wireframe
	case 'h':
		break; //optional: filled and hidden-line
	}
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

bool getPatches(char* filename) {
	ifstream file(filename, ifstream::in);
	if (!file) {
		printError("Could not open file!");
		return false;
	}
	// Get the number of patches
	file >> numOfPatches;
	
	double x, y, z; // Set up variables for coordinates
	Patch * newPatch;
	// All right, start reading in patch data
	for (int i = 0; i<numOfPatches; i++) {
		newPatch = new Patch();
		for (int u = 0; u<4; u++) {
			for (int v = 0; v<4; v++) {
				file >> x >> y >> z;
				newPatch->cP[u][v] = new Point(x, y, z);
			}
		}
		bunchOPatches.push_back(newPatch);
	}
	file.close();
	return true;
}

//****************************************************
// the usual stuff, nothing exciting here
//****************************************************
void render() {
	printInfo("Rendering Scene!");
	
  	//This tells glut to use a double-buffered window with red, green, and blue channels 
  	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

  	// Initalize theviewport size
  	viewport.w = 800;
  	viewport.h = 800;

  	//The size and position of the window
  	glutInitWindowSize(viewport.w, viewport.h);
  	glutInitWindowPosition(0, 0);
  	glutCreateWindow("CS184!");

  	initScene();							// quick function to set up scene
  
  
	glutDisplayFunc(myDisplay);				// function to run when its time to draw something
	glutReshapeFunc(myReshape);				// function to run when the window gets resized
	glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);
	glutIdleFunc(myFrameMove);				// function to run when not handling any other task
	glutMainLoop();							// infinite loop that will keep drawing and resizing and whatever else
  
}

void process() {
	printInfo("Processing Data!");
	Point* point;
	furthestPoint = new Point(0,0,0);
	for (int p = 0; p < numOfPatches; p++) {
		point = bunchOPatches[p]->subdividepatch(stepSize);
		
		printDebug(5, "Biggest point in patch " <<p<<": ("<<(*point)[0]<<","<<(*point)[1]<<","<<(*point)[2]<<")");
		
		if (abs((*point)[0]) > (*furthestPoint)[0])
			(*furthestPoint)[0] = abs((*point)[0]);
		if (abs((*point)[1]) > (*furthestPoint)[1])
			(*furthestPoint)[1] = abs((*point)[1]);
		if (abs((*point)[2]) > (*furthestPoint)[2])
			(*furthestPoint)[2] = abs((*point)[2]);	
		delete point;
	}
	printDebug(2,"Largest point found ("<<(*furthestPoint)[0]<<","<<(*furthestPoint)[1]<<","<<(*furthestPoint)[2]<<")");
	
	//run adaptive tessellation algorithm here
	if (adaptive) {
		
		
	}
	
}

//Set up default settings
void setDefaults() {
	printDebug(1, "Setting up defaults:");
	printDebug(1, "Uniform Subdivision, Filled Polygons, Flat Shading");
    adaptive=false;
    wireframe=false;
    smoothShading=false;
}

//Return 0 if no error
int parseCommandLine(int argc, char *argv[]) {

    bool malformedArg;
    bool printUsage = false;
    int i = 1;
    
    setDefaults();
    
    printDebug(1, "Reading in "<<argc<<" arguments.");
    
    //Read in filename and parse patches
    if (isThereMore(i, argc, 1)) {
    	if (!getPatches(argv[i]))
    		printUsage = true;
    } else {
    	printError("No filename given!");
    	printUsage = true;
    }
    
    i++;
    
    //Read in stepsize
	if (isThereMore(i, argc, 1)) {
		stepSize = atof(argv[i]);		
	} else {
    	printError("No stepSize!");
    	printUsage = true;			
	}
	
	i++;
	
	//Read the rest of the arguments
    for (; i < argc; i++) {

        malformedArg = false;

        if (strcmp(argv[i],"-d") == 0) {

                if (isThereMore(i, argc, 1)) {
                    if (atoi(argv[i + 1]) < 6 && atoi(argv[i + 1]) > 0)
                        NIELSDEBUG = atoi(argv[++i]);
                } else {
                    NIELSDEBUG = 1;
                }

        }  else if (!strcmp(argv[i], "-q")) {
    
                NIELSINFO = 0;

        } else if (!strcmp(argv[i], "-a")) {
            
            adaptive=true;

        }  else {
            malformedArg = true;
        }

        if (malformedArg) {
            printError("Malformed input arg in parsing command \"" << argv[i] << "\"");
            printUsage = true;
        }
    }
    
    printInfo("Read " << numOfPatches << " patches from file.");
    
    if (printUsage)
        return 1;
    return 0;

}

void printUsage() {
    cout << "Usage: "<< endl;
    cout << "  cs184_as5 inputfile.bez stepSize\n   [-a]\n   [-d n]\n   [-q]" << endl;
}

int main(int argc, char *argv[]) {
	
	if (parseCommandLine(argc, argv) != 0) {
        printUsage();
        exit(1);
    }
    
    printInfo("Curve Renderer Started");
  	
  	//This initializes glut
  	glutInit(&argc, argv);
  	process();
  	render();
  	
  	return 0;
}








