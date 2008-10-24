#include "main.h"

using namespace std;

World world;
ImageSaver imagesaver;

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
        paused = true;
        inverseFPS = 1.0 / 25.0;
        lastTime = 0;
    }
	int w, h; // width and height
	float rotateX;
	float rotateY;
	float rotateZ;
	float translateX;
	float translateY;
	float translateZ;
	bool wireFrame;
	double lastTime;
    double inverseFPS;
    bool paused;
};

Viewport    viewport;

void printUsage() {
    cout << "Usage: ";
    cout << " ClothSym " << endl;
    cout << "      {[-statobj input.obj] | [-simobj input.obj | [-aniobj inputXXXXXX.obj]}" << endl;
    cout << "      [-d i] " << endl;
    cout << "      [-img directory] " << endl;
}

int parseCommandLine(int argc, char *argv[]) {

    bool malformedArg;
    bool printUsage = false;
    bool hasOBJ = false;
    int i;
    for (i = 1; i < argc; i++) {
        malformedArg = false;

        if (strcmp(argv[i],"-d") == 0) {

            if (isThereMore(i, argc, 1)) {
                ++i;
                Debug::DEBUG = atoi(argv[i]);
            } else {
                malformedArg = true;
            }

        } else if (!strcmp(argv[i], "-statobj")) {

            if (isThereMore(i, argc, 1)) {
                std::string filename = std::string(argv[++i]);
                world.loadStatModel(filename);
                hasOBJ = true;
            } else {
                malformedArg = true;
            }

        } else if (!strcmp(argv[i], "-simobj")) {

				if (isThereMore(i, argc, 1)) {
					std::string filename = std::string(argv[++i]);
					world.loadSimModel(filename);
					hasOBJ = true;
				} else {
					malformedArg = true;
				}

        } else if (!strcmp(argv[i], "-aniobj")) {

				if (isThereMore(i, argc, 1)) {
					std::string filename = std::string(argv[++i]);
					world.loadAniModel(filename);
					hasOBJ = true;
				} else {
					malformedArg = true;
				}

        } else if (!strcmp(argv[i], "-img")) {

            if (isThereMore(i, argc, 1)) {
                std::string dirname = std::string(argv[++i]);
                imagesaver.initialize(dirname, viewport.inverseFPS);
            } else {
                malformedArg = true;
            }

        } else {
            malformedArg = true;
        }

        if (malformedArg) {
            cout << "Malformed input arg in parsing command \"" << argv[i] << "\"" << endl;
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
        viewport.translateY -= .1f;
        break;
    case GLUT_KEY_DOWN:
        viewport.translateY += .1f;
        break;
    case GLUT_KEY_RIGHT:
        viewport.translateX -= .1f;
        break;
    case GLUT_KEY_LEFT:
        viewport.translateX += .1f;
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
    case 'p':
        viewport.paused = !viewport.paused;
        break;
    case 27:
        exit(0);
        break;
    }
}

void init(void)
{
   GLfloat mat_diffuse[] = { 0.8, 0.0, 0.8, 1.0 };
   GLfloat mat_ambient[] = { 0.6, 0.0, 0.6, 1.0 };
   GLfloat mat_specular[] = { 0.1, 0.1, 0.1, 1.0 };
   GLfloat mat_shininess[] = { 50.0 };
   GLfloat light_position[] = { 0.0, 0.0, 10.0, 0.0 };
   GLfloat light_position2[] = {5.0, 0.0, 20.0, 0.0 };

   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_SMOOTH);

   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
   glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);
   glLightfv(GL_LIGHT1, GL_POSITION, light_position2);

   glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_LIGHT1);
   glEnable(GL_DEPTH_TEST);
}

void Blue () {
	GLfloat mat_diffuse[] = { 0.0, 1.0, 0.0, 1.0 };
	GLfloat mat_ambient[] = { 0.0, 0.1, 0.0, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
}

void Pink () {
	GLfloat mat_diffuse[] = { 0.8, 0.0, 0.8, 1.0 };
	GLfloat mat_ambient[] = { 0.6, 0.0, 0.6, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_ambient);
}

/**
 * This function draws the actual world using OpenGL.
 */
void display(void)
{
   glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glLoadIdentity();

   //Apply the camera transformations
   glTranslatef(viewport.translateX,
           viewport.translateY,
           viewport.translateZ);

   glRotatef(viewport.rotateX, 1.0f, 0.0f, 0.0f);
   glRotatef(viewport.rotateY, 0.0f, 1.0f, 0.0f);
   glRotatef(viewport.rotateZ, 0.0f, 0.0f, 1.0f);

   if (viewport.wireFrame) {
       glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   } else {
       glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   }

   //Draw the world!
   world.draw();

   glFlush ();
   glutSwapBuffers();
}

void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity();
   viewport.w = w;
   viewport.h = h;
   /*
   if (w <= h)
      glOrtho (-1*x, x, -1*x*(GLfloat)h/(GLfloat)w,
         x*(GLfloat)h/(GLfloat)w, -400.0, 400.0);
   else
      glOrtho (-1*x*(GLfloat)w/(GLfloat)h,
         x*(GLfloat)w/(GLfloat)h, -1*x, x, -400.0, 400.0);
   */
   glOrtho (-200, 200, -200, 200, -400.0, 400.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}


/**
 * Calculates the next frame of the world.
 */
void myframemove() {
    imagesaver.saveFrame(world.getTime(), false, viewport.inverseFPS, viewport.w, viewport.h);
    world.advance(viewport.inverseFPS);

    //REMOVE THIS:
    Blue();
    glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);
        glVertex3f(-10, 10, -3);
        glVertex3f(-10, -10, -3);
        glVertex3f(10, -10, -3);
        glVertex3f(10, 10, -3);
    glEnd();
    Pink();

    glutPostRedisplay();
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

                //sys->enableMouseForce(vec3(ox,oy,0));

            }
        } else {
            //sys->disableMouseForce();
        }
    }
}

void myMouseMove(int x, int y) {
	/*
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
        sys->updateMouseForce(vec3(ox,oy,0));
    }
	*/
}

int main(int argc, char *argv[]) {

    if (parseCommandLine(argc, argv)) {
        printUsage();
        exit(1);
    }

    viewport.w = 500;
    viewport.h = 500;
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (viewport.w, viewport.h);
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
