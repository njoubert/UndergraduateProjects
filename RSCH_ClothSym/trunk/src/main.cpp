#include "main.h"

using namespace std;

World world;
ImageSaver imagesaver;

//-------------------------------------------------------------------------------
//
class Camera {
public:
    Camera() {
        _fovy = 30;
        _zNear = 1;
        _zFar = 10000;

        _zoom = 40.0f;
        _rotx = 10.0f;
        _roty = 0.0f;
        _tx = 0.0f;
        _ty = 0.0f;

        _lastx = _lasty = 0;

        _Buttons[0] = _Buttons[1] = _Buttons[2] = 0;

        wireFrame = false;
        showGrid = true;
        paused = true;
        inverseFPS = 1.0 / 30.0;
        lastTime = 0;
    }

    void setPerspective(int w, int h) {
        _w = w;
        _h = h;
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(_fovy, (float) w / h, _zNear, _zFar);

    }

    void setView() {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        //You can set the position of the camera qith gluLookAt
        gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

        //This order gives you Maya-like
        //mouse control of your camera
        glTranslatef(0, 0, -_zoom);
        glTranslatef(_tx, _ty, 0);
        glRotatef(_rotx, 1, 0, 0);
        glRotatef(_roty, 0, 1, 0);

    }

    void mousemotion(int x, int y) {
        int diffx = x - _lastx;
        int diffy = y - _lasty;
        _lastx = x;
        _lasty = y;

        if (_Buttons[2]) {
            _zoom -= (float) 0.01f * diffx * abs((_zoom + 1));
        } else if (_Buttons[0]) {
            _rotx += (float) 0.5f * diffy;
            _roty += (float) 0.5f * diffx;
        } else if (_Buttons[1]) {
            _tx += (float) 0.05f * diffx;
            _ty -= (float) 0.05f * diffy;
        }

        glutPostRedisplay();
    }

    void mousepress(int b, int s, int x, int y) {
        _lastx = x;
        _lasty = y;
        switch (b) {
        case GLUT_LEFT_BUTTON:
            _Buttons[0] = ((GLUT_DOWN == s) ? 1 : 0);
            break;
        case GLUT_MIDDLE_BUTTON:
            _Buttons[1] = ((GLUT_DOWN == s) ? 1 : 0);
            break;
        case GLUT_RIGHT_BUTTON:
            _Buttons[2] = ((GLUT_DOWN == s) ? 1 : 0);
            break;
        default:
            break;
        }
        glutPostRedisplay();
    }

    void keypress(int key, int x, int y) {
        switch (key) {
        case '=':
            _zoom -= 1.0;
            break;
        case '-':
            _zoom += 1.0;
            break;
        }
    }

    void specialkeypress(int key, int x, int y) {
        switch (key) {
        case GLUT_KEY_UP:

            break;
        case GLUT_KEY_DOWN:

            break;
        case GLUT_KEY_RIGHT:

            break;
        case GLUT_KEY_LEFT:

            break;
        }
    }

private:
    float _fovy, _zNear, _zFar;
    float _zoom, _rotx, _roty, _tx, _ty;
    int _lastx, _lasty;
    unsigned char _Buttons[3];
public:
    int _w, _h;
    bool wireFrame;
    bool showGrid;
    double lastTime;
    double inverseFPS;
    bool paused;
};

Camera cam;

//-------------------------------------------------------------------------------
//
void printUsage() {
    cout << "Usage: ";
    cout << " ClothSym " << endl;
    cout
            << "      {[-statobj input.obj] | [-simobj input.obj | [-aniobj inputXXXXXX.obj]}"
            << endl;
    cout << "      [-d i] " << endl;
    cout << "      [-img directory] " << endl;
}

//-------------------------------------------------------------------------------
//
int parseCommandLine(int argc, char *argv[]) {
    bool malformedArg;
    bool printUsage = false;
    bool hasOBJ = false;
    int i;
    for (i = 1; i < argc; i++) {
        malformedArg = false;

        if (strcmp(argv[i], "-d") == 0) {

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
                imagesaver.initialize(dirname, cam.inverseFPS);
            } else {
                malformedArg = true;
            }

        } else {
            malformedArg = true;
        }

        if (malformedArg) {
            cout << "Malformed input arg in parsing command \"" << argv[i]
                    << "\"" << endl;
            printUsage = true;
        }
    }
    if (printUsage || !hasOBJ)
        return 1;
    return 0;

}

//-------------------------------------------------------------------------------
//
void processSpecialKeys(int key, int x, int y) {
    cam.specialkeypress(key, x, y);
}

//-------------------------------------------------------------------------------
//
void processKeys(unsigned char key, int x, int y) {
    cam.keypress(key, x, y);
    switch (key) {
    case 'v':
        cam.wireFrame = !cam.wireFrame;
        break;
    case 'p':
        cam.paused = !cam.paused;
        break;
    case 'g':
        cam.showGrid = !cam.showGrid;
        break;
    case 27:
        exit(0);
        break;
    }
}

//-------------------------------------------------------------------------------
//
void init(void) {
    glEnable(GL_LIGHTING);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    glShadeModel(GL_SMOOTH);
    glClearColor(0.0, 0.0, 0.0, 0.0);

    GLfloat global_ambient[] = {.1f, .1f, .1f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

    //Lights:
    GLfloat ambient[] = { .1f, .1f, .1f };
    GLfloat diffuse[] = { .6f, .5f, .5f };
    GLfloat specular[]={0.0, 0.0, 0.0, 1.0};
    GLfloat light_position[] = { 0.0, 0.0, 10.0, 0.0 };
    GLfloat light_position2[] = { 5.0, 0.0, 20.0, 0.0 };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position2);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    //Material:
    GLfloat mat_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat mat_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat mat_specular[] = { 0.1, 0.1, 0.1, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

    glEnable(GL_DEPTH_TEST);
}

//-------------------------------------------------------------------------------
//All this shit is very much illegal in terms of memory usage! those are POINTERS...
void Green() {
    GLfloat mat_diffuse[] = { 0.0, 1.0, 0.0, 1.0 };
    GLfloat mat_ambient[] = { 0.0, 0.1, 0.0, 1.0 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
}
void Pink() {
    GLfloat mat_diffuse[] = { 0.8, 0.0, 0.4, 1.0 };
    GLfloat mat_ambient[] = { 0.8, 0.0, 0.4, 1.0 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
}
void White() {
    GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
}
void Grey() {
    GLfloat mat_diffuse[] = { 0.6, 0.0, 0.6, 1.0 };
    GLfloat mat_ambient[] = { 0.06, 0.06, 0.6, 1.0 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
}
//-------------------------------------------------------------------------------
// This function draws the actual world using OpenGL.
//
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    cam.setView();

    if (cam.wireFrame) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

	glPushMatrix();

    if (cam.showGrid) {
        White();
        // draw grid
        glBegin(GL_LINES);
        for (int i = -10; i <= 10; ++i) {
            glVertex3f(i, 0, -10);
            glVertex3f(i, 0, 10);

            glVertex3f(10, 0, i);
            glVertex3f(-10, 0, i);
        }
        glEnd();
    }
    Pink();

    //Draw the world!
    world.draw();

    glPopMatrix();

    glFlush();
    glutSwapBuffers();
}

//-------------------------------------------------------------------------------
//
void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    cam.setPerspective(w, h);
    cam.setView();
}

//-------------------------------------------------------------------------------
// Calculates the next frame of the world.
//
void myframemove() {
    imagesaver.saveFrame(world.getTime(), false, cam.inverseFPS, cam._w, cam._h);
    world.advance(cam.inverseFPS);
    glutPostRedisplay();
}

//-------------------------------------------------------------------------------
//
void myMousePress(int button, int state, int x, int y) {
    cam.mousepress(button, state, x, y);

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
            glReadPixels(x, view[3] - y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);

            if (GL_TRUE == gluUnProject(x, view[3] - y, z, modelview, proj,
                    view, &ox, &oy, &oz)) {

                //sys->enableMouseForce(vec3(ox,oy,0));

            }
        } else {
            //sys->disableMouseForce();
        }
    }
}

//-------------------------------------------------------------------------------
//
void myMouseMove(int x, int y) {
    cam.mousemotion(x, y);
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

//-------------------------------------------------------------------------------
//
int main(int argc, char *argv[]) {

    if (parseCommandLine(argc, argv)) {
        printUsage();
        exit(1);
    }

    cam._w = 500;
    cam._h = 500;

    //Create Window:
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(cam._w, cam._h);
    glutCreateWindow("ClothSym");

    //Initialize Settings:
    init();

    //Set up callbacks:
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
