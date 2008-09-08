#include "main.h"


#define PI 3.14159265

using namespace std;

//****************************************************
// Some Classes
//****************************************************
System sys;

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

	myReshape(viewport.w,viewport.h);
}

void initParticles() {

    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            float vx = ((rand() % 200) - 100) / 400.0f;
            float vy = ((rand() % 200) - 100) / 400.0f;
            particles.push_back(Particle(0, 0, vx, vy));
        }
    }

}

void initSystem() {
    int step = 20;
    double border = 50;
    vector< vector < Point> > x(step, vector< Point >(step));
    vector<int> dim(2,step);
    for (int i = 0; i < step; i++) {
        for (int j = 0; j < step; j++) {
            x[i][j].x = (((double) viewport.w - border*2) / (double) step) * (double) i - ((viewport.w - border*2) / 2.0);
            x[i][j].y = (((double) viewport.h - border*2) / (double) step) * (double)j - ((viewport.h - border*2) / 2.0);
            x[i][j].vx = -1.0;
            x[i][j].vy = 0.0;
        }
    }
    sys.setX(&x, dim);
}

//***************************************************
// function that does the actual drawing
//***************************************************
void myDisplay() {

    myReshape(viewport.w, viewport.h);
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(1.0f,1.0f,0.0f);


		glBegin(GL_POINTS);
			for (vector<Particle>::iterator it = particles.begin();
					it!=particles.end(); ++it) {
				(*it).draw();
			}
		glEnd();

		sys.draw();

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

//  	initParticles();
  	initScene();							// quick function to set up scene
    initSystem();

  	glutDisplayFunc(myDisplay);				// function to run when its time to draw something
  	glutReshapeFunc(myReshape);				// function to run when the window gets resized
  	glutIdleFunc(myFrameMove);				// function to run when not handling any other task
  	glutMainLoop();							// infinite loop that will keep drawing and resizing and whatever else

  	return 0;
}
