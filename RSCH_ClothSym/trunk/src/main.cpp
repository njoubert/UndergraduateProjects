#include "main.h"

#define PI 3.14159265

using namespace std;

//****************************************************
// Some Classes
//****************************************************
TriangleMesh* myMesh;

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
	//glOrtho(0,viewport.w,0,viewport.h, 1, -1);
	glOrtho(-1*viewport.w/4,viewport.w/4,-1*viewport.h/4,viewport.h/4, 150, -150);

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

void initSystem(string filename) {
    Parser parser;
    myMesh = parser.parseOBJ(filename);
    cout << "Done Parsing .OBJ" << endl;
}

void myDisplay() {
    myReshape(viewport.w, viewport.h);
    glClear( GL_COLOR_BUFFER_BIT );
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3f(1.0f,0.0f,0.0f);

	vec3 a, b, c;

	TriangleMeshVertex** vertices;
    std::vector< TriangleMeshTriangle* >::const_iterator it =
        myMesh->triangles.begin();
    while (it != myMesh->triangles.end()) {
        vertices = (*it)->getVertices();
        a = vertices[0]->getX();
        b = vertices[1]->getX();
        c = vertices[2]->getX();
        glBegin(GL_POLYGON);
            //cout << "Drawing poly: " << a[0] << "," << b[0] << "," << c[0] << endl;
            glVertex3f(a[0],a[1],a[2]);
            glVertex3f(b[0],b[1],b[2]);
            glVertex3f(c[0],c[1],c[2]);
        glEnd();
        it++;
    }

	//sys.draw();

	glFlush();
	glutSwapBuffers();
}

void myFrameMove() {
	//Check time using lastTime?
	//solver.takeStep(sys, 2.0); //you get weird behavior on 4, crazy explosion on 12.
	glutPostRedisplay(); // forces glut to call the display function (myDisplay())
}

void myMousePress(int button, int state, int x, int y) {
/*
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
// */
}

void myMouseMove(int x, int y) {
/*
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
// */
}

int parseCommandLine(int argc, char *argv[]) {

    bool malformedArg;
    bool printUsage = false;
    bool hasOBJ = false;
    int i;
    for (i = 1; i < argc; i++) {
        malformedArg = false;

        if (strcmp(argv[i],"-d") == 0) {

            //TODO: Set up debug flags...
            if (isThereMore(i, argc, 1)) {
                ++i;
            } else {
                malformedArg = true;
            }

        } else if (!strcmp(argv[i], "-obj")) {

            if (isThereMore(i, argc, 1)) {
                std::string filename = std::string(argv[++i]);
                initSystem(filename);
                hasOBJ = true;
            } else {
                malformedArg = true;
            }

        } else {
            malformedArg = true;
        }

        if (malformedArg) {
            cout << "Malformed input arg in parsing command \"" << argv[i] << "\"";
            printUsage = true;
        }
    }
    if (printUsage || !hasOBJ)
        return 1;
    return 0;

}

void printUsage() {
    cout << "Usage: "<< endl;
    cout << "  ClothSym -obj filename [-d i]\\" << endl;
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);

    if (parseCommandLine(argc, argv)) {
        printUsage();
        exit(1);
    }

  	initScene();							// quick function to set up scene
    glutDisplayFunc(myDisplay);				// function to run when its time to draw something
  	glutReshapeFunc(myReshape);				// function to run when the window gets resized
  	glutIdleFunc(myFrameMove);				// function to run when not handling any other task
    glutMouseFunc(myMousePress);
    glutMotionFunc(myMouseMove);
  	glutMainLoop();							// infinite loop that will keep drawing and resizing and whatever else

  	return 0;
}
