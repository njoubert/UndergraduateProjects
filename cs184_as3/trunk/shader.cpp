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
#include <limits.h>
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

class Color;
class Position3d;
class Vector3d;
class Light;
class PointLight;
class DirectionalLight;
class Object3d;

class Viewport;

class Viewport {
public:
	int w, h; // width and height
};

class Color {
public:
	
	Color() {}
	Color(float r, float g, float b) { setColor(r,g,b); }
	
	void setColor(float r, float g, float b) { this->r = r; this->g = g; this->b = b; }
	float getNormalizedR(float m) { return r / m; }
	float getNormalizedG(float m) { return g / m; }
	float getNormalizedB(float m) { return b / m; }
	
	Color & operator*=(const Color& other) {
		r *= other.r;
		g *= other.g;
		b *= other.b;
		return *this;
	}
	Color & operator*=(const Color* other) {
		r *= other->r;
		g *= other->g;
		b *= other->b;
		return *this;
	}
	Color & operator *=(float v) {
		r *= v;
		g *= v;
		b *= v;
		return *this;
	}	
	Color operator *(const Color & other) const {
		Color result = *this; //Make a copy of the current class
		result *= other;
		return result;
	}
	Color operator *(const Color * other) const {
		Color result = *this; //Make a copy of the current class
		result *= other;
		return result;
	}
	Color operator +=(const Color& other) {
		r += other.r;
		g += other.g;
		b += other.b;
		return *this;
	}
	
	Color operator +(const Color& other) const {
		Color result = *this;
		result += other;
		return result;
	}
	
	void debugMe() { printDebug("(" << r << "," << g << "," << b << ")"); }
	
	float getMaxVal() {
		return max(max(r,g),b);
	}
	
	void normalizeTo(float m) {
		if (m == 0)
			m = 1;
		//printDebug("Normalizing color by dividing with " << m);
		r = r / m;
		g = g / m;
		b = b / m;
	}
	
private:
	float r, g, b;	
};

class Position3d {
public:
	Position3d() { inputx = 0; inputy = 0; inputz = 0; }
	float getX() { return inputx; }
	float getY() { return inputy; }
	float getZ() { return inputz; }
	void setX(float x) { this->inputx = x; }
	void setY(float y) { this->inputy = y; }
	void setZ(float z) { this->inputz = z; }	
	void setPos(float x, float y, float z) {
		setX(x); setY(y); setZ(z);
	}
	void debugMe() { printDebug("(" << inputx << "," << inputy << "," << inputz << ")"); }
private:	
	float inputx, inputy, inputz; // width and height
};

class Vector3d {
public:
	
	Vector3d() {}
	
	void calculateFromPositions(Position3d * start, Position3d * end) {
		setX(end->getX() - start->getX());
		setY(end->getY() - start->getY());
		setZ(end->getZ() - start->getZ());
	}
	
	void calculateReflective(Vector3d * in, Vector3d * normal) {
		Vector3d temp = *normal;
		float scale = 2.0f*(in->dot(normal));
		*this = *in; //Copy this
		temp = temp*scale;
		*this += temp;
	}
	
	float dot(Vector3d * other) {
		return x*other->x + y*other->y + z*other->z;
	}
	
	Vector3d normalize() {
		Vector3d result = *this; //Make a copy of the current class
		float l = sqrt(x*x + y*y + z*z);
		result.x = result.x/l;
		result.y = result.y/l;
		result.z = result.z/l;
		return result;
	}
	Vector3d normalize(float length) {
		Vector3d result = *this; //Make a copy of the current class
		result.x = result.x/length;
		result.y = result.y/length;
		result.z = result.z/length;
		return result;
	}
	void debugMe() { printDebug("(" << x << "," << y << "," << z << ")"); }
	
	Vector3d & operator*=(float v) {
		x *= v;
		y *= v;
		z *= v;
		return *this;
	}
	Vector3d operator*(float v) {
		Vector3d result = *this; //Make a copy of the current class
		result *= v;
		return result;
	}
	Vector3d & operator+=(float v) {
		x += v;
		y += v;
		z += v;
		return *this;
	}
	Vector3d operator+(float v) const {
		Vector3d result = *this; //Make a copy of the current class
		result += v;
		return result;
	}
	
	Vector3d & operator+=(Vector3d & other) {
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}
	Vector3d operator +(Vector3d & other) const {
		Vector3d result = *this; //Make a copy of the current class
		result += other;
		return result;
	}

	float getX() { return (int) x; }
	float getY() { return (int) y; }
	float getZ() { return (int) z; }
	void setX(float x) { this->x = x; }
	void setY(float y) { this->y = y; }
	void setZ(float z) { this->z = z; }		
private:	
	float x, y, z; // width and height
};

class Light {
public:
	Light() {
		printDebug("Creating a light...");
	}
	//This creates a light with position (x,y,z) and color (r,g,b)
	//x, y, z are relative to sphere's center (0 is center, 1 is radius)
	//r, g, b are positive floats between [0.0, max_float]
	Light(float x, float y, float z, float r, float g, float b) {
		printDebug("Creating Light at " << "(" << x << "," << y << "," << z << ") with color (" << r << "," << g << "," << b << ");");
		setPosition(x,y,z);
		setLuminosity(r,g,b);
	}
	void setPosition(float x, float y, float z) {
		P_relPosition.setX(x);
		P_relPosition.setY(y);
		P_relPosition.setZ(z);
	}
	Position3d * getPosition() {
		return &P_relPosition;
	}
	
	void setLuminosity(float r, float g, float b) {
		C_lum.setColor(r,g,b);	
	}
	
	virtual Vector3d getLightingVector(Position3d pos) = 0;
	
	Color * getLuminosity() {
		return &C_lum;
	}
private:
	Position3d P_relPosition;
	Color C_lum;
};

class PointLight: public Light {
public:
	PointLight(float x, float y, float z, float r, float g, float b):Light(x,y,z,r,g,b){}
	Vector3d getLightingVector(Position3d pos) {
		Vector3d temp;
		return temp;
	}
};

class DirectionalLight: public Light {
public:
	DirectionalLight(float x, float y, float z, float r, float g, float b):Light(x,y,z,r,g,b){}	
	Vector3d getLightingVector(Position3d pos) {
		Vector3d temp;
		return temp;
	}
};

class Object3d {
public:
	Object3d() : C_ambient(0,0,0), C_diffuse(0,0,0), C_specular(0,0,0) {}
	virtual ~Object3d() {}
	void setAmbientColor(float r, float g, float b) {
		//Assume r g b values are [0.0,1.0]
		printDebug("Object3d::setAmbientColor(" << r << "," << g << "," << b << ");");
		C_ambient.setColor(r,g,b);
	}
	void setDiffuseColor(float r, float g, float b) {
		//Assume r g b values are [0.0,1.0]
		printDebug("Object3d.setDiffuseColor(" << r << "," << g << "," << b << ");");
		C_diffuse.setColor(r,g,b);
	}
	void setSpecularColor(float r, float g, float b) {
		//Assume r g b values are [0.0,1.0]
		printDebug("Object3d.setSpecularColor(" << r << "," << g << "," << b << ");");
		C_specular.setColor(r,g,b);
	}
	void setSpecularCoeff(float arg) {
		printDebug("Object3d.setSpecular(" << arg << ");");
		v = arg;
	}
	virtual void render(vector<Light*> & lights, Vector3d & view) = 0;
protected:
	float v;
	Color C_ambient;
	Color C_diffuse;
	Color C_specular;
};

class Sphere: public Object3d {
public:
	Sphere() {
		radius = 100;
		center.setPos(0,0,0);
	}
	~Sphere() {}
	
	void setRadiusfromViewport(Viewport & v) {
		radius = (int) (0.4 * min(v.w, v.h));
	}
	
	void setRadius(float x) {
		radius = x;
	}
	
	int getRadius() {
		return (int) radius;
	}
	
	void render(vector<Light*> & lights, Vector3d & view) {
		Position3d point;
		Vector3d normal;
		Vector3d normalcp;
		Vector3d incidence;
		Vector3d reflectance;
		Color C_pixel;
		Color C_tempDiff;
		Color C_tempSpec;
		Light* l;
		
		printDebug("Shading a sphere...");
		
		glBegin(GL_POINTS);
			
		for (float i = -radius; i <= radius; i += 1) {
			float width = (sqrt((float)(radius*radius-i*i)) - 0.5f);
			for (float j = -width; j <= width; j += 1) {
				
		/*

1) compute the xyz location of the point on the sphere.  Since you know the xy location of the pixel and radius of the sphere, you can figure out the z coordinate.  Call this location "p".  Compute the normal for the point also, "n".

2) if the pixel is outside the radius of the sphere set it black on move on

3) set the initial color for the pixel to ka

4) for each light

	4a) compute l = l_pos_i-p
	4b) v = [0,0,1]
	4c) r = reflect(v,n) (see text)
	4c) c = kd * l_i * ( n . l ) + ks * l_i * (r.l)^q
	4d) pixel color += c

5) draw pixel
				
*/				
				C_pixel = C_ambient;
				
				point.setPos(center.getX() + i, center.getY() + j,0);
				calculateZ(point);
				
				normal.calculateFromPositions(&center,&point);
				normal = normal.normalize(radius);
				
				for (unsigned int i = 0; i < lights.size(); i++) {
						
					l = lights[i];
					incidence.calculateFromPositions(l->getPosition(),&point);
					incidence.normalize();

					reflectance.calculateReflective(&incidence, &normal);
					
					C_tempDiff = C_diffuse * ((l->getLuminosity()));
					C_tempDiff *= max(0.0f, incidence.dot(&normal));
					
					C_tempSpec = C_specular * ((l->getLuminosity()));
					C_tempSpec *= pow(max(0.0f, view.dot(&reflectance)), v);
					
					C_pixel += C_tempDiff;
					C_pixel += C_tempSpec;
					
				}
				
				C_pixel.normalizeTo(C_pixel.getMaxVal());
				setPixel(point.getX(), point.getY(), C_pixel.getNormalizedR(1), C_pixel.getNormalizedG(1), C_pixel.getNormalizedB(1));
				
				
			}
		}
		
		glEnd();
	}
private:
	float radius;
	Position3d center;
	
	//Uses the x and y coordinates to calculate z on a sphere.
	void calculateZ(Position3d & pos) {
		//Use pythoagoras' theorem in 3D:
		float x = pos.getX();
		float y = pos.getY();
		pos.setZ(sqrt(radius*radius - x*x - y*y));
	}
	
	void setPixel(float x, float y, GLfloat r, GLfloat g, GLfloat b) {
		glColor3f(r, g, b);
		glVertex2f(x, y);
	}
	
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
	
	sphr.setRadiusfromViewport(viewport);
	
	glViewport (0,0,viewport.w,viewport.h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1*viewport.w/2,viewport.w/2,-1*viewport.h/2,viewport.h/2, 1, -1);
	//gluOrtho2D(-1*viewport.x*100,viewport.x*100, -1*viewport.y*100, viewport.y*100);

}


/*
 * Callback function to draw scene 
 */
void myDisplay() {

	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	Vector3d view;
	view.setX(0);
	view.setY(0);
	view.setZ(1);
	sphr.render(lights, view);

	glFlush();
	glutSwapBuffers();
	
}

/*
 * Initializes Viewport
 */
void initScene(){
	glClearColor(bgColor.getNormalizedR(1), bgColor.getNormalizedG(1), bgColor.getNormalizedB(1), 0.0f);
	myReshape(viewport.w,viewport.h);
}


void processNormalKeys(unsigned char key, int x, int y) {
	printDebug("Key pressed: " << key);
	if (key == 32) 
		exit(0);
}

//========================================
//
// Setup and Teardown Follows
//
//========================================

int parseCommandLine(int argc, char *argv[], vector<Light*> & lights, Object3d * obj) {
	
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
					r = atof(argv[++i]);
					g = atof(argv[++i]);
					b = atof(argv[++i]);
					bgColor.setColor(r,g,b);
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

	Object3d *obj = &sphr;
		
	//Set some default values. changes as the person drags the window around
	viewport.w = 800;
	viewport.h = 600;
	
	sphr.setRadiusfromViewport(viewport);
	
	if (parseCommandLine(argc, argv, lights, obj)) {
		printUsage();
		return 1;
	}
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(viewport.w,viewport.h);
	glutInitWindowPosition(0,0);
	glutCreateWindow(argv[0]);

	initScene();

	glutDisplayFunc(myDisplay);					// function to run when its time to draw something
	glutReshapeFunc(myReshape);					// function to run when the window gets resized
	glutKeyboardFunc(processNormalKeys);	
	glutMainLoop();							// infinite loop that will keep drawing and resizing and whatever else

	//cleanUp();
	
	return 0;
	
}
