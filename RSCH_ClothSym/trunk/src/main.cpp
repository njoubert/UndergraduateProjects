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

bool mouseSelectedItem = false;
InputForce* mouseForce;


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
	//glOrtho(-1*viewport.w/2,viewport.w/2,-1*viewport.h/2,viewport.h/2, 1, -1);
	glOrtho(0,viewport.w,0,viewport.h, 1, -1);
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
    double border = 100;
    int gridWidth = 10;
    vector< vector < Particle> > x(step, vector< Particle >(step));
    for (int i = 0; i < step; i++) {
        for (int j = 0; j < step; j++) {
            //x[i][j].x = (((double) viewport.w - border*2) / (double) step) * (double) i - ((viewport.w - border*2) / 2.0);
            //x[i][j].y = (((double) viewport.h - border*2) / (double) step) * (double) j - ((viewport.h - border*2) / 2.0);
            //x[i][j].m = (rand() % 200) + 100;
            x[i][j].x = border + i*gridWidth;
            x[i][j].y = border + j*gridWidth;
            x[i][j].z = 0;
            x[i][j].m = 10; //Make this 2 to see it blow up.
        }
    }
    x[step-1][step-1].pinned = true;
    x[0][step-1].pinned = true;
    /**
    for (int i = 0; i < step; i++) {
        x[i][step-1].pinned = true;
    }
    */
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

    double ks = 0.01;
    double kd = 0.82;
    //VERTICAL SPRINGS
    SpringForce * f2;
    for (int i = 0; i < step-1; i++) {
        for (int j = 0; j < step; j++) {
            f2 = new SpringForce();
            f2->u1 = i;
            f2->v1 = j;
            f2->u2 = i+1;
            f2->v2 = j;
            f2->ks = ks;
            f2->kd = kd;
            f2->r = gridWidth;
            sys.addForce(f2);
        }
    }
    //HORIZONTAL SPRINGS
    //**
    for (int i = 0; i < step; i++) {
        for (int j = 0; j < step-1; j++) {
            f2 = new SpringForce();
            f2->u1 = i;
            f2->v1 = j;
            f2->u2 = i;
            f2->v2 = j+1;
            f2->ks = ks;
            f2->kd = kd;
            f2->r = gridWidth;
            sys.addForce(f2);
        }
    }
    // */

    //MOUSE
    mouseForce = new InputForce();
    sys.addForce(mouseForce);
}

void myDisplay() {
    myReshape(viewport.w, viewport.h);
    glClear( GL_COLOR_BUFFER_BIT );
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3f(1.0f,1.0f,0.0f);

	sys.draw();

	glFlush();
	glutSwapBuffers();
}

void myFrameMove() {
	//Check time using lastTime?
	solver.takeStep(sys, 2.0); //you get weird behavior on 4, crazy explosion on 12.
	glutPostRedisplay(); // forces glut to call the display function (myDisplay())
}

void myMousePress(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            //Find the point
            mouseSelectedItem = true;
            float z = 0.5;
            double ox, oy, oz;
            GLdouble modelview[16];
            GLdouble proj[16];
            GLint view[4];
            glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
            glGetDoublev(GL_PROJECTION_MATRIX, proj);
            glGetIntegerv(GL_VIEWPORT, view);
            glReadPixels( x, view[3]-y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z );

            if (GL_TRUE == gluUnProject(x, view[3]-y, z, modelview, proj, view, &ox, &oy, &oz)) {
                int u1, v1;
                sys.getClosestParticle(ox,oy, &u1, &v1);
                std::vector< std::vector< Particle > > * particles = sys.getX();
                if ((*particles)[u1][v1].pinned) {
                    (*particles)[u1][v1].x = ox;
                    (*particles)[u1][v1].y = oy;
                    sys.setX(particles);
                } else {
                    mouseForce->u1 = u1;
                    mouseForce->v1 = v1;
                    mouseForce->xi = ox;
                    mouseForce->yi = oy;
                    mouseForce->zi = oz;
                    mouseForce->enabled = true;
                }
            }
        } else {
            mouseSelectedItem = false;
            mouseForce->enabled = false;
        }
    }
}

void myMouseMove(int x, int y) {
    if (mouseSelectedItem) {
        float z;
        double ox, oy, oz;
        GLdouble modelview[16];
        GLdouble proj[16];
        GLint view[4];
        glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
        glGetDoublev(GL_PROJECTION_MATRIX, proj);
        glGetIntegerv(GL_VIEWPORT, view);
        glReadPixels( x, view[3]-y, 1, 1,
                 GL_DEPTH_COMPONENT, GL_FLOAT, &z );
        gluUnProject(x, view[3]-y, z, modelview, proj, view, &ox, &oy, &oz);
        mouseForce->xi = ox;
        mouseForce->yi = oy;
        mouseForce->zi = oz;

    }

}

/**
int main(int argc, char *argv[]) {
  	glutInit(&argc, argv);

  	initScene();							// quick function to set up scene
    initSystem();
    glutDisplayFunc(myDisplay);				// function to run when its time to draw something
  	glutReshapeFunc(myReshape);				// function to run when the window gets resized
  	glutIdleFunc(myFrameMove);				// function to run when not handling any other task
    glutMouseFunc(myMousePress);
    glutMotionFunc(myMouseMove);
  	glutMainLoop();							// infinite loop that will keep drawing and resizing and whatever else

  	return 0;
}
// */
