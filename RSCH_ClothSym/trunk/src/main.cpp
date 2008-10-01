#include "main.h"

using namespace std;

//****************************************************
// Some Classes
//****************************************************
TriangleMesh* myMesh;
System* sys;
//Solver* solver = new ImplicitSolver();
Solver* solver = new ExplicitSolver();

double timeStep = 0.01;

class Viewport {
public:
    Viewport() {
        rotateX = rotateY = rotateZ = translateX = translateY = translateZ = 0.0f;
        rotateX = 0;
        rotateY = 0;
        rotateZ = 0;
        translateX = 0;
        translateY = 0;
        translateZ = 0;
        wireFrame = false;
    }
	int w, h; // width and height
	float rotateX;
	float rotateY;
	float rotateZ;
	float translateX;
	float translateY;
	float translateZ;
	bool wireFrame;
};

Viewport	viewport;

//****************************************************
// Global Variables
//****************************************************


void initSystem(string filename) {
    Parser parser;
    myMesh = parser.parseOBJ(filename);
    sys = new System(myMesh);
    cout << "Done Parsing .OBJ" << endl;
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

        } else if (!strcmp(argv[i], "-timestep")) {

            if (isThereMore(i, argc, 1)) {
                timeStep = atof(argv[++i]);
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

void processSpecialKeys(int key, int x, int y) {
    switch(key) {
    case GLUT_KEY_UP:
        viewport.translateY -= 2.5f;
        break;
    case GLUT_KEY_DOWN:
        viewport.translateY += 2.5f;
        break;
    case GLUT_KEY_RIGHT:
        viewport.translateX -= 2.5f;
        break;
    case GLUT_KEY_LEFT:
        viewport.translateX += 2.5f;
        break;
    }
}

void processKeys(unsigned char key, int x, int y) {
    switch (key) {
    case 'w':
        viewport.rotateY -= 1.5f;
        break;
    case 's':
        viewport.rotateY += 1.5f;
        break;
    case 'a':
        viewport.rotateZ -= 1.5f;
        break;
    case 'd':
        viewport.rotateZ += 1.5f;
        break;
    case 'q':
        viewport.rotateX -= 1.5f;
        break;
    case 'e':
        viewport.rotateX += 1.5f;
        break;
    case '-':
        viewport.translateZ -= 2.5f;
        break;
    case '=':
        viewport.translateZ += 2.5f;
        break;
    case 'v':
        viewport.wireFrame = !viewport.wireFrame;
        break;
    case 27:
        exit(0);
        break;
    }
}

void printUsage() {
    cout << "Usage: "<< endl;
    cout << "  ClothSym -obj filename [-d i] [-timestep i]\\" << endl;
}

void init(void)
{
    GLfloat mat_diffuse[] = { 0.8, 0.0, 0.8, 1.0 };
    GLfloat mat_specular[] = { 0.1, 0.1, 0.1, 1.0 };
   GLfloat mat_shininess[] = { 50.0 };
   GLfloat light_position[] = { 0.0, 0.0, 150.0, 0.0 };

   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_SMOOTH);

   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
   glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);

   glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);
}

void display(void)
{
   glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   //glutSolidSphere (1.0, 20, 16);
   glLoadIdentity();

   glTranslatef(viewport.translateX,
           viewport.translateY,
           viewport.translateZ);

   glRotatef(viewport.rotateX, 1.0f, 0.0f, 0.0f);
   glRotatef(viewport.rotateY, 0.0f, 1.0f, 0.0f);
   glRotatef(viewport.rotateZ, 0.0f, 0.0f, 1.0f);

   vec3 a, b, c, na, nb, nc;

   if (viewport.wireFrame) {
       glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   } else {
       glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   }

   TriangleMeshVertex** vertices;
   std::vector< TriangleMeshTriangle* >::const_iterator it =
       myMesh->triangles.begin();
   while (it != myMesh->triangles.end()) {
       vertices = (*it)->getVertices();

       a = vertices[0]->getX();
       na = vertices[0]->getNormal();
       b = vertices[1]->getX();
       nb = vertices[1]->getNormal();
       c = vertices[2]->getX();
       nc = vertices[2]->getNormal();

       glBegin(GL_TRIANGLES);
           glNormal3f( na[0], na[1], na[2]);
           glVertex3f(a[0],a[1],a[2]);
           glNormal3f( nb[0], nb[1], nb[2]);
           glVertex3f(b[0],b[1],b[2]);
           glNormal3f( nc[0], nc[1], nc[2]);
           glVertex3f(c[0],c[1],c[2]);
       glEnd();

       it++;
   }

   glFlush ();
   glutSwapBuffers();
}

void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity();
   double x = 300;
   if (w <= h)
      glOrtho (-1*x, x, -1*x*(GLfloat)h/(GLfloat)w,
         x*(GLfloat)h/(GLfloat)w, -400.0, 400.0);
   else
      glOrtho (-1*x*(GLfloat)w/(GLfloat)h,
         x*(GLfloat)w/(GLfloat)h, -1*x, x, -400.0, 400.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void myframemove() {
    sys->takeStep(solver, timeStep);
    //cout << "We're at time " << sys->getT() << endl;
    //exit(1);
    glutPostRedisplay(); // forces glut to call the display function (myDisplay())
}

void myMousePress(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            //Find the point
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

                sys->enableMouseForce(vec3(ox,oy,oz));

            }
        } else {
            sys->disableMouseForce();
        }
    }
}

void myMouseMove(int x, int y) {
    if (sys->isMouseEnabled()) {
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
        sys->updateMouseForce(vec3(ox,oy,oz));
    }

}

//*
int main(int argc, char *argv[]) {

    if (parseCommandLine(argc, argv)) {
        printUsage();
        exit(1);
    }
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (500, 500);
    glutInitWindowPosition (100, 100);
    glutCreateWindow (argv[0]);
    init ();
    glutIdleFunc(myframemove);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(processKeys);
    glutSpecialFunc(processSpecialKeys);
    glutMouseFunc(myMousePress);
    glutMotionFunc(myMouseMove);
    glutMainLoop();
    return 0;
}
// */
