#include "main.h"


#define PI 3.14159265

using namespace std;

//****************************************************
// Some Classes
//****************************************************
System sys;
Solver solver;

class Viewport {
public:
	int w, h; // width and height
};

Viewport	viewport;

//****************************************************
// Global Variables
//****************************************************

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

	myReshape(viewport.w,viewport.h);
}

void initSystem() {
    int step = 20;
    double border = 50;
    vector< vector < Particle> > x(step, vector< Particle >(step));
    for (int i = 0; i < step; i++) {
        for (int j = 0; j < step; j++) {
            x[i][j].x = (((double) viewport.w - border*2) / (double) step) * (double) i - ((viewport.w - border*2) / 2.0);
            x[i][j].y = (((double) viewport.h - border*2) / (double) step) * (double) j - ((viewport.h - border*2) / 2.0);
            //x[i][j].m = (rand() % 200) + 100;
            x[i][j].m = 100;
        }
    }
    for (int i = 0; i < step; i++) {
        x[i][step-1].pinned = true;

    }
    sys.setDim(step, step);
    sys.setX(&x);
    Force * f;
    for (int i = 0; i < step; i++) {
        for (int j = 0; j < step-1; j++) {
            f = new GravityForce();
            f->u1 = i;
            f->v1 = j;
            sys.addForce(f);
        }
    }
    SpringForce * f2;
    for (int i = 0; i < step-1; i++) {
        for (int j = 0; j < step-1; j++) {
            f2 = new SpringForce();
            f2->u1 = i;
            f2->v1 = j;
            f2->u2 = i+1;
            f2->v2 = j;
            f2->ks = 0.01;
            f2->kd = 0.82;
            f2->r = 10;
            sys.addForce(f2);
        }
    }
    for (int i = 0; i < step-1; i++) {
        for (int j = 0; j < step-1; j++) {
            f2 = new SpringForce();
            f2->u1 = i;
            f2->v1 = j;
            f2->u2 = i;
            f2->v2 = j+1;
            f2->ks = 0.01;
            f2->kd = 0.12;
            f2->r = 20;
            sys.addForce(f2);
        }
    }
    // */
}

void myDisplay() {
    myReshape(viewport.w, viewport.h);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3f(1.0f,1.0f,0.0f);

	sys.draw();

	glFlush();
	glutSwapBuffers();
}

void myFrameMove() {
	//Check time using lastTime?
	solver.takeStep(sys, 1.0);
	glutPostRedisplay(); // forces glut to call the display function (myDisplay())
}

int main(int argc, char *argv[]) {
  	glutInit(&argc, argv);

  	initScene();							// quick function to set up scene
    initSystem();

  	glutDisplayFunc(myDisplay);				// function to run when its time to draw something
  	glutReshapeFunc(myReshape);				// function to run when the window gets resized
  	glutIdleFunc(myFrameMove);				// function to run when not handling any other task
  	glutMainLoop();							// infinite loop that will keep drawing and resizing and whatever else

  	return 0;
}
