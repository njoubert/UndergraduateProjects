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
#include <math.h>
#include <cmath>
#include <cstdlib>

#include "Debug.cpp"
#include "Color.cpp"
#include "Vector3d.cpp"
#include "Position.cpp"
#include "Light.cpp"

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

#define isThereMore(CURR, MAX, WANT)	((MAX - CURR) > WANT)

const double PI = 3.14159265;

int ANI = 0;
int BM = 0;

using namespace std;

//****************************************************
// Some Classes
//****************************************************

void setPixel(float x, float y, GLfloat r, GLfloat g, GLfloat b) {
	glColor3f(r, g, b);
	glVertex2f(x, y);
}

class Viewport;
class Viewport {
public:
	int w, h; // width and height
};

class Sphere;

class Sphere {
public:
	Sphere() { specularCoeff=1; center.setPositionValues(0,0,0);}
	~Sphere() { }

	void setAmbientColorValues(float r, float g, float b) { ambient.setColor(r,g,b);}
	void setDiffuseColorValues(float r, float g, float b) { diffuse.setColor(r,g,b);}
	void setSpecularColorValues(float r, float g, float b) { specular.setColor(r,g,b);}
	void setSpecularCoeff(float f) {specularCoeff = f;}
	void setAbsRadiusValue(int f) {
		printDebug("Sphere radius set to " << f << " absolute.");
		this->radius = f;
	}
	void setCenter(float x, float y) {
		center.x = x;
		center.y = y;
	}

	void render(vector<Light*> & lights, Vector3d & view) {
		AbsPosition3d point;
		Vector3d normal,incidence,reflectance;
		Color point_ambient,point_diffuse,point_specular,point_total;
		float x=0,y=0,z=0,width=0;

		printDebug("Rendering sphere of radius " << radius);

		glBegin(GL_POINTS);

		for (y = -radius; y <= radius; y += 0.25) {
			width = sqrt(radius*radius - y*y);
			for (x = -width; x <= width; x += 0.25) {

					z = sqrt(radius*radius - x*x - y*y);
					point.setPositionValues(center.x+x,center.y+y,center.z+z);
					normal.calculateFromPositions(&center,&point);
					normal.normalize();

					if (BM) {
						//normal = normal + U*horiz + V*vert

						//We need to find the two vectors that are tangent to
						//the sphere at this point, and multiply them by
						//the gradients to positon the texture correctly
//						normal.setX(normal.getX() +
//								bmap.getHorizontalGradient((int)point.x, (int)point.y) +
//								bmap.getVerticalGradient((int)point.x, (int)point.y));
//						normal.setY(normal.getY() +
//								bmap.getHorizontalGradient((int)point.x, (int)point.y) +
//								bmap.getVerticalGradient((int)point.x, (int)point.y));
					}


					point_diffuse.setColor(0,0,0);
					point_ambient = ambient;
					point_specular.setColor(0,0,0);


					for (unsigned int i = 0; i < lights.size(); i++) {

							incidence = lights[i]->getIncidence(point,radius);
							incidence.normalize();
							reflectance.calculateReflective(incidence,normal);
							reflectance.normalize();

							point_diffuse += (diffuse * lights[i]->getColor()) *
											max(incidence.dot(&normal),0.0f);
							point_specular += (specular * lights[i]->getColor()) *
											pow(max(reflectance.dot(&view),0.0f),specularCoeff);

					}

					point_total = point_ambient + point_diffuse + point_specular;
					setPixel(point.x,
							point.y,
							point_total.getGlR(0,1),
							point_total.getGlG(0,1),
							point_total.getGlB(0,1));

			}
		}

		glEnd();

	}

private:

	Color ambient;
	Color diffuse;
	Color specular;
	float specularCoeff;
	float radius;
	AbsPosition3d center;
};

//****************************************************
// Global Variables
//****************************************************
Color 		bgColor(0,0,0);
Viewport	viewport;
vector<Light*> lights(0);
Sphere		sphr;

//========================================
//
// OpenGL Callbacks Follow
//
//========================================



/*
 * Callback function for window reshape event
 */
void myReshape(int w, int h) {
	viewport.w = w;
	viewport.h = h;
	printDebug("Reshaping viewport to " << w << "x" << h);
	sphr.setAbsRadiusValue(min(w,h)/2 - min(w,h)/10);

	glViewport (0,0,viewport.w,viewport.h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1*viewport.w/2,viewport.w/2,-1*viewport.h/2,viewport.h/2, 1, -1);
}

/*
 * Callback function to draw scene
 */
void myDisplay() {
	printDebug("Drawing Scene");
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	Vector3d view(0,0,1);
	sphr.render(lights, view);

	glFlush();
	glutSwapBuffers();
}

/*
 * Initializes Viewport
 */
void initScene(){
	printDebug("Initializing Scene, background color of ("<<bgColor.r<<","<<bgColor.g<<","<<bgColor.b<<")");
	glClearColor(bgColor.getGlR(0,255),bgColor.getGlG(0,255),bgColor.getGlB(0,255), 0.0f);
	myReshape(800,600);
}


void processNormalKeys(unsigned char key, int x, int y) {
	printDebug("Key pressed: " << key);
	if (key == 32)
		exit(0);
	if (key == 'f')
		ANI += 2;
	if (key == 'b')
		ANI += -2;
	if (key == 's')
		ANI = 0;
}

//========================================
//
// Setup and Teardown Follows
//
//========================================

int parseCommandLine(int argc, char *argv[], vector<Light*> & lights, Sphere * sphere) {

	bool malformedArg;
	bool printUsage = false;
	float r=0, g=0, b=0, x=0, y=0, z=0, v=0;
	int i;
	for (i = 1; i < argc; i++) {

			malformedArg = false;

			if (strcmp(argv[i],"-d") == 0) {

				DEBUG = 1;

			} else if (!strcmp(argv[i], "-ka")) {

				if (isThereMore(i, argc, 3)) {
					r = atof(argv[++i]);
					g = atof(argv[++i]);
					b = atof(argv[++i]);
					sphere->setAmbientColorValues(r,g,b);
				} else {
					malformedArg = true;
				}

			} else if (!strcmp(argv[i], "-kd")) {


				if (isThereMore(i, argc, 3)) {
					r = atof(argv[++i]);
					g = atof(argv[++i]);
					b = atof(argv[++i]);
					sphere->setDiffuseColorValues(r,g,b);
				} else {
					malformedArg = true;
				}

			} else if (!strcmp(argv[i], "-ks")) {


				if (isThereMore(i, argc, 3)) {
					r = atof(argv[++i]);
					g = atof(argv[++i]);
					b = atof(argv[++i]);
					sphere->setSpecularColorValues(r,g,b);
				} else {
					malformedArg = true;
				}

			} else if (!strcmp(argv[i], "-sp")) {


				if (isThereMore(i, argc, 1)) {
					v = atof(argv[++i]);
					sphere->setSpecularCoeff(v);
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
					r = atof(argv[++i]);
					g = atof(argv[++i]);
					b = atof(argv[++i]);
					bgColor.setColor(r,g,b);
				} else {
					malformedArg = true;
				}

			} else if (!strcmp(argv[i], "-ani")) {

				ANI = 1;

			} else if (!strcmp(argv[i], "-bumpmap")) {

				if (isThereMore(i, argc, 1)) {
					BM = atoi(argv[++i]);
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
	cout << "      [-ani]" << endl;
	cout << "      [-bumpmap extremity]" << endl;
}

void move() {
	if (ANI) {
	lights[0]->setPosition(lights[0]->getPosition().x + (float)ANI/100,
			lights[0]->getPosition().y + (float)ANI/100,
			lights[0]->getPosition().z + (float)ANI/100);
	glutPostRedisplay();
	}
}

int main(int argc, char *argv[]) {

	if (parseCommandLine(argc, argv, lights, &sphr)) {
		printUsage();
		return 1;
	}

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(800,600);
	glutInitWindowPosition(0,0);
	glutCreateWindow(argv[0]);

	initScene();

	glutDisplayFunc(myDisplay);					// function to run when its time to draw something
	glutReshapeFunc(myReshape);					// function to run when the window gets resized
	glutKeyboardFunc(processNormalKeys);
	glutIdleFunc(move);
	glutMainLoop();							// infinite loop that will keep drawing and resizing and whatever else

	//cleanUp();

	return 0;

}
