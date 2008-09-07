#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>

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

#include <time.h>
#include <math.h>

#ifdef _WIN32
static DWORD lastTime;
#else
static struct timeval lastTime;
#endif

#define PI 3.14159265

using namespace std;

//****************************************************
// Some Classes
//****************************************************
class Viewport {
public:
	int w, h; // width and height
};

Viewport	viewport;

class Particle {
public:
	double x, y, vx, vy;
public:
	Particle() { };
	Particle(double x, double y, double vx, double vy) {
		this->x = x; this->y = y; this->vx = vx; this->vy = vy;
	};
	void draw() {
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex2f(x, y);
	}
	void update() {
		x += vx;
		y += vy;
		vy -= ((rand() % 100) + 1000) / (double) 100000;
		//vx /= 1.004f;
		//vy /= 1.004f;
		if (x > viewport.w/2 || x < -viewport.w/2) {
			vx /= 1.6;
			//vx = -vx;
		}
		if (y > viewport.h/2 || y < -viewport.h/2) {
			y = -viewport.h/2;
			vy /= 1.6;
			vy = -vy;
		}

	}
};


//****************************************************
// Global Variables
//****************************************************

vector<Particle> particles;

void setPixel(float x, float y, GLfloat r, GLfloat g, GLfloat b) {
	glColor3f(r, g, b);
	glVertex2f(x, y);
}

void myReshape(int w, int h) {
	viewport.w = w;
	viewport.h = h;

	glViewport(0,0,viewport.w,viewport.h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1*viewport.w/2,viewport.w/2,-1*viewport.h/2,viewport.h/2, 1, -1);
}

void initScene(){
  	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  	viewport.w = 400;
  	viewport.h = 400;
  	glutInitWindowSize(viewport.w, viewport.h);
  	glutInitWindowPosition(0, 0);
  	glutCreateWindow("Cloth Sym");
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	for (int i = 0; i < 100; i++) {
		for (int j = 0; j < 100; j++) {
			float vx = ((rand() % 200) - 100) / 400.0f;
			float vy = ((rand() % 200) - 100) / 400.0f;
			particles.push_back(Particle(0, 0, vx, vy));
		}
	}

	myReshape(viewport.w,viewport.h);
}


//***************************************************
// function that does the actual drawing
//***************************************************
void myDisplay() {

	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(1.0f,1.0f,0.0f);

	for (int i = 0; i < 5; i++) {

		glBegin(GL_POINTS);
			for (vector<Particle>::iterator it = particles.begin();
					it!=particles.end(); ++it) {
				(*it).draw();
			}
		glEnd();

	}
	glFlush();
	glutSwapBuffers();
}


//****************************************************
// called by glut when there are no messages to handle
//****************************************************
void myFrameMove() {

	//Check time using lastTime.

	for (vector<Particle>::iterator it = particles.begin();
						it!=particles.end(); ++it) {
		(*it).update();
	}

	#ifdef _WIN32
		Sleep(10);						//give ~10ms back to OS (so as not to waste the CPU)
	#endif
	glutPostRedisplay(); // forces glut to call the display function (myDisplay())
}

int main(int argc, char *argv[]) {
  	glutInit(&argc, argv);

  	initScene();							// quick function to set up scene

  	glutDisplayFunc(myDisplay);				// function to run when its time to draw something
  	glutReshapeFunc(myReshape);				// function to run when the window gets resized
  	glutIdleFunc(myFrameMove);				// function to run when not handling any other task
  	glutMainLoop();							// infinite loop that will keep drawing and resizing and whatever else

  	return 0;
}
