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

//vector<Triangle*> bunchOTriangles;

Point* furthestPoint;
int numOfPatches;

//Display Settings:
double stepSize;
bool wireframe;
bool adaptive;
bool smoothShading;

int NIELSDEBUG=0;
int NIELSINFO=1;

static double moveStep=0.1;
double anglelr=0.0,angleud=0.0,anglesp=0.0;
double anglelrd=0.0,angleudd=0.0,anglespd=0.0;
double objPosX=0.0,objPosY=0.0,eyePosZ=0.0;
double lx=0.0,ly=0.0,lz=0.0;

//****************************************************
// reshape viewport if the window is resized
//****************************************************
void myReshape(int w, int h) {
	viewport.w = w;
	viewport.h = h;

	glViewport(0,0,viewport.w,viewport.h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	float maxv = abs((*furthestPoint)[0]);
	if (abs((*furthestPoint)[1]) > maxv)
		maxv = abs((*furthestPoint)[1]); 
		
	//This should be slightly bigger than -1*sqrt(3)*maxv offset 
	//from the largest point to fit the whole thing.
	//Its trig: straight distance = orthogonal distance / tan(angle) 
	eyePosZ = maxv + 1.5*maxv; 
	gluPerspective(60.0, 1.0, 0.0, 2*maxv); //We set up our perspective for a natural feel (60 degrees) and a far clipping plane.

}


//****************************************************
// sets the window up
//****************************************************
void initScene(){
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f); // Clear to black, fully transparent
	
	//glShadeModel(GL_SMOOTH);
  	
  	//glDepthFunc(GL_LESS);
  	//glEnable(GL_DEPTH_TEST);

	myReshape(viewport.w,viewport.h);
}


//***************************************************
// function that does the actual drawing
//***************************************************
void myDisplay() {
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			// clear the color buffer (sets everything to black)
	
	glMatrixMode(GL_MODELVIEW);					// indicate we are specifying camera transformations
	glLoadIdentity();							// make sure transformation is "zero'd"
	
	if (wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


//	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
//	glEnable(GL_COLOR_MATERIAL);
	
	GLfloat light_ambient[] = {0.0, 0.0, 0.0, 0.0};
	GLfloat light_diffuse[] = {0.3, 0.3, 0.3, 0.0};
	GLfloat light_specular[] = {0.3, 0.3, 0.3, 0.0};
	GLfloat light_position[] = {5.0, 5.0, 0.0, 0.0};
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	GLfloat al[] = {0.2, 0.2, 0.2, 1.0};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, al);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat mat_d[] = {0.1f, 0.5f, 0.8f, 1.0f};
	GLfloat mat_s[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat low_sh[] = {0.0f};
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_d);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_d);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_s);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, low_sh);
	
	glTranslated(objPosX,objPosY,0);
	gluLookAt(0,0,eyePosZ,
		0,0,0,
		0,1,0);
	glRotated(angleud, 1.0f, 0.0f, 0.0f);
	glRotated(anglelr, 0.0f, 1.0f, 0.0f);	
	glRotated(anglesp, 0.0f, 0.0f, 1.0f);
	
	if (!adaptive)	{

		for (int p = 0; p < numOfPatches; p++) {
			
			for (unsigned int u = 0; u < bunchOPatches[p]->bezpoints.size()-1; u++) {
				for (unsigned int v = 0; v < bunchOPatches[p]->bezpoints[u].size()-1; v++) {
					glColor4f(1.0, 0.0, 0.0, 1.0);
//					glBegin(GL_QUADS);
//					glVertex3dv(bunchOPatches[p]->bezpoints[u][v]->p.data());
//					glVertex3dv(bunchOPatches[p]->bezpoints[u+1][v]->p.data());
//					glVertex3dv(bunchOPatches[p]->bezpoints[u+1][v+1]->p.data());
//					glVertex3dv(bunchOPatches[p]->bezpoints[u][v+1]->p.data());					
//					glEnd();
					glBegin(GL_TRIANGLES);
					glNormal3dv(bunchOPatches[p]->bezpoints[u][v]->n.data());
					glVertex3dv(bunchOPatches[p]->bezpoints[u][v]->p.data());
					glNormal3dv(bunchOPatches[p]->bezpoints[u+1][v]->n.data());
					glVertex3dv(bunchOPatches[p]->bezpoints[u+1][v]->p.data());
					glNormal3dv(bunchOPatches[p]->bezpoints[u+1][v+1]->n.data());
					glVertex3dv(bunchOPatches[p]->bezpoints[u+1][v+1]->p.data());
					
					glEnd();
					glBegin(GL_TRIANGLES);
					glNormal3dv(bunchOPatches[p]->bezpoints[u][v]->n.data());
					glVertex3dv(bunchOPatches[p]->bezpoints[u][v]->p.data());
					glNormal3dv(bunchOPatches[p]->bezpoints[u+1][v+1]->n.data());
					glVertex3dv(bunchOPatches[p]->bezpoints[u+1][v+1]->p.data());
					glNormal3dv(bunchOPatches[p]->bezpoints[u][v+1]->n.data());
					glVertex3dv(bunchOPatches[p]->bezpoints[u][v+1]->p.data());
					
					
					glEnd();
				}	
			}
			
		}
		
	}
	
	glDisable(GL_COLOR_MATERIAL);
	
	
	glFlush();
	glutSwapBuffers();					// swap buffers (we earlier set double buffer)
}


//**********Transformation functions***********

void zoomInOut(int direction) {
	eyePosZ += direction*moveStep;
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
			printDebug(2, "Up key! " <<objPosY);
			objPosY += moveStep;
			printDebug(2, "Up key! " <<objPosY);
		} else if (glutGetModifiers() == GLUT_ACTIVE_CTRL) {
			angleudd -= moveStep;
		} else {
			angleudd = 0.0;
			angleud -= 10*moveStep;
		}
		break;
	case GLUT_KEY_DOWN:
		if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
			objPosY -= moveStep;
		} else if (glutGetModifiers() == GLUT_ACTIVE_CTRL) {
			angleudd += moveStep;
		} else {
			angleudd = 0.0;
			angleud += 10*moveStep;
		}
		break;
	case GLUT_KEY_RIGHT:
		if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
			objPosX += moveStep;
		} else if (glutGetModifiers() == GLUT_ACTIVE_CTRL) {
			anglelrd += moveStep;
		} else {
			anglelrd = 0.0;
			anglelr += 10*moveStep;
		}
		break;

	case GLUT_KEY_LEFT:
		if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
			objPosX -= moveStep;
		} else if (glutGetModifiers() == GLUT_ACTIVE_CTRL) {
			anglelrd -= moveStep;
		} else {
			anglelrd = 0.0;
			anglelr -= 10*moveStep;
		}
		break;

	}
}

void processKeys(unsigned char key, int x, int y) {
	printDebug(2, "Processing normal key "<<key);
	switch (key) {
	case '=':
		zoomInOut(-1);
		break;
	case '-':
		zoomInOut(1);
		break;
	case 's':
		toggleFlatSmooth();
		break;
	case ',':
		if (glutGetModifiers() == GLUT_ACTIVE_CTRL) {
			anglespd += moveStep;
		} else {
			anglespd = 0.0;
			anglesp += 10*moveStep;
		}
		break;
	case '.':
		if (glutGetModifiers() == GLUT_ACTIVE_CTRL) {
			anglespd -= moveStep;
		} else {
			anglespd = 0.0;
			anglesp -= 10*moveStep;
		}
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
#ifdef _WIN32
	Sleep(10);						//give ~10ms back to OS (so as not to waste the CPU)
#endif
	
		anglesp += anglespd;
		anglelr += anglelrd;
		angleud += angleudd;
	
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
		
		printDebug(2, "Subdividing patch "<<p);
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
  	
  	//This initializes glut
  	glutInit(&argc, argv);
  	process();
  	render();
  	
  	return 0;
}








