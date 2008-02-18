//============================================================================
// Name        : README for Shader
// Author      : Niels Joubert CS184-dv
// Platform    : Linux (freeglut3)
// Version     :
// Copyright   : 2008
// Description : A shader using the Phong Illumination Model
//============================================================================

#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
//#include <stdlib>

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

#define printDebug(A)	if (DEBUG) { cout << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": " << A << endl; }
#define isThereMore(CURR, MAX, WANT)	((MAX - CURR) > WANT)

const double PI = 3.14159265;
int DEBUG = 0;

using namespace std;

//****************************************************
// Some Classes
//****************************************************

class Viewport;
class Color;
class Vector3d;
class Light;
class PointLight;
class DirectionalLight;
class Object3d;

class Viewport {
public:
	int w, h; // width and height
};

class Color {
public:
	float r, g, b;	
	Color(float r, float g, float b) { r = 0; g = 0; b = 0; }
	Color & operator*=(const Color& other) {
		r *= other.r;
		g *= other.g;
		b *= other.b;
		return *this;
	}

	Color operator *(const Color& other) const {
		Color result = *this; //Make a copy of the current class
		result *= other;
		return result;
	}
};

class Vector3d {
public:
	float x, y, z; // width and height
};

class Light {
public:
	Light() {
		
	}
	//This creates a light with position (x,y,z) and color (r,g,b)
	//x, y, z are relative to sphere's center (0 is center, 1 is radius)
	//r, g, b are positive floats between [0.0, max_float]
	Light(float x, float y, float z, float r, float g, float b) {
		printDebug("Object3d::setAmbientColor(" << r << "," << g << "," << b << ");");
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
	Vector3d V_position;
	Vector3d V_luminosity;
};

class PointLight: public Light {
public:
	PointLight(float x, float y, float z, float r, float g, float b):Light(x,y,z,r,g,b){}
};

class DirectionalLight: public Light {
public:
	DirectionalLight(float x, float y, float z, float r, float g, float b):Light(x,y,z,r,g,b){}	
};

class Object3d {
public:
	Object3d() : C_ambient(0,0,0), C_diffuse(0,0,0), C_specular(0,0,0) {}
	virtual ~Object3d() {}
	void setAmbientColor(float r, float g, float b) {
		//Assume r g b values are [0.0,1.0]
		printDebug("Object3d::setAmbientColor(" << r << "," << g << "," << b << ");");
		C_ambient.r = r;
		C_ambient.g = g;
		C_ambient.b = b;
	}
	void setDiffuseColor(float r, float g, float b) {
		//Assume r g b values are [0.0,1.0]
		printDebug("Object3d.setDiffuseColor(" << r << "," << g << "," << b << ");");
		C_diffuse.r = r;
		C_diffuse.g = g;
		C_diffuse.b = b;
	}
	void setSpecularColor(float r, float g, float b) {
		//Assume r g b values are [0.0,1.0]
		printDebug("Object3d.setSpecularColor(" << r << "," << g << "," << b << ");");
		C_specular.r = r;
		C_specular.g = g;
		C_specular.b = b;
	}
	void setSpecularCoeff(float arg) {
		printDebug("Object3d.setSpecular(" << arg << ");");
		v = arg;
	}
	virtual void render(vector<Light*> lights) = 0;
private:
	float v;
	Color C_ambient;
	Color C_diffuse;
	Color C_specular;
};

class Sphere: public Object3d {
public:
	Sphere() {
		radius = 100;
	}
	~Sphere() {}
	
	void render(vector<Light*> lights) {
		
	}
private:
	float radius;
};


//****************************************************
// Global Variables
//****************************************************
Viewport	viewport;
int 		plotX = 400;
int 		plotY = 300;
Color bgColor(0,0,0);

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

int parseCommandLine(int argc, char *argv[], vector<Light*> & lights, Object3d * obj) {
	
	bool malformedArg;
	bool printUsage = false;
	float r=0, g=0, b=0, x=0, y=0, z=0, v=0;
	
	for (int i = 1; i < argc; i++) {
		
			malformedArg = false;
		
			if (strcmp(argv[i],"-d") == 0) {
				
				DEBUG = 1;
				
			} else if (!strcmp(argv[i], "-ka")) {
				
				if (isThereMore(i, argc, 3)) {
					r = atof(argv[++i]);
					g = atof(argv[++i]);
					b = atof(argv[++i]);
					obj->setAmbientColor(r,g,b);
				} else {
					malformedArg = true;
				}
				
			} else if (!strcmp(argv[i], "-kd")) {
				
				
				if (isThereMore(i, argc, 3)) {
					r = atof(argv[++i]);
					g = atof(argv[++i]);
					b = atof(argv[++i]);
					obj->setDiffuseColor(r,g,b);
				} else {
					malformedArg = true;
				}
				
			} else if (!strcmp(argv[i], "-ks")) {
				
				
				if (isThereMore(i, argc, 3)) {
					r = atof(argv[++i]);
					g = atof(argv[++i]);
					b = atof(argv[++i]);
					obj->setSpecularColor(r,g,b);
				} else {
					malformedArg = true;
				}
				
			} else if (!strcmp(argv[i], "-sp")) {
				
				
				if (isThereMore(i, argc, 1)) {
					v = atof(argv[++i]);
					obj->setSpecularCoeff(v);
				} else {
					malformedArg = true;
				}
				
			} else if (!strcmp(argv[i], "-pl")) {
				
				if (isThereMore(i, argc, 6)) {
					x = atof(argv[++i]);
					y = atof(argv[++i]);
					z = atof(argv[++i]);
					r = atof(argv[++i]);
					g = atof(argv[++i]);
					b = atof(argv[++i]);
					lights.push_back(new PointLight(x,y,z,r,g,b));
				} else {
					malformedArg = true;
				}
				
			} else if (!strcmp(argv[i], "-dl")) {
			
				if (isThereMore(i, argc, 6)) {
					x = atof(argv[++i]);
					y = atof(argv[++i]);
					z = atof(argv[++i]);
					r = atof(argv[++i]);
					g = atof(argv[++i]);
					b = atof(argv[++i]);
					lights.push_back(new DirectionalLight(x,y,z,r,g,b));
				} else {
					malformedArg = true;
				}
				
			} else if (!strcmp(argv[i], "-bg")) {
				
				if (isThereMore(i, argc, 3)) {
					bgColor.r = atof(argv[++i]);
					bgColor.g = atof(argv[++i]);
					bgColor.b = atof(argv[++i]);
				} else {
					malformedArg = true;
				}
				
			} else {
				malformedArg = true;
			}
			
			if (malformedArg) {
				printDebug("Malformed input arg in parsing command \"" << argv[i] << "\"");
				printUsage = true;
			}

		
	}
	if (printUsage)
		return 1;
	return 0;
	
}

void printUsage() {
	cout << "Usage: "<< endl;
	cout << "  shader -ka r g b -kd r g b -ks r g b -sp v \\" << endl;
	cout << "      [[-pl x y z r g b] ... [-pl x y z r g b]] \\" << endl;
	cout << "      [[-dl x y z r g b] ... [-dl x y z r g b] \\" << endl;
	cout << "      [-bg r g b]" << endl;
}

int main(int argc, char *argv[]) {

	
	vector<Light*> lights(10);
	Sphere sphr;
	Object3d *obj = &sphr;
	
	if (parseCommandLine(argc, argv, lights, obj)) {
		printUsage();
		return 1;
	}
	
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
