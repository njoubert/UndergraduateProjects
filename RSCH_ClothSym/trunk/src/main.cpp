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

		_zoom = 10.0f;
		_rotx = 10.0f;
		_roty = 0.0f;
		_tx = 0.0f;
		_ty = 0.0f;

		_lastx = _lasty = 0;

		_Buttons[0] = _Buttons[1] = _Buttons[2] = 0;

		wireFrame = false;
		showGrid = true;
		paused = true;
		dynamicConstraints = false;
		collisions = false;
		follow = false;
		inverseFPS = 1.0 / 24.0;
		lastTime = 0;
	}

	float getZoom() {
		return _zoom;
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

		//You can set the position of the camera with gluLookAt
		gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

		//This order gives you Maya-like
		//mouse control of your camera
		glTranslatef(0, 0, -_zoom);
		glTranslatef(_tx, _ty, 0);
		glRotatef(_rotx, 1, 0, 0);
		glRotatef(_roty, 0, 1, 0);

	}

	void setFocusedView(vec3 focusPt) {
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		//cout<<"focusPt "<<focusPt<<endl;
		//You can set the position of the camera qith gluLookAt
		gluLookAt(0.0, 0.0, 1.0, focusPt[0] / _zoom, 0, 0, 0.0, 1.0, 0.0);

		//gluLookAt(0.0,0.0,1.0,
		//.1,0,0,
		//0.0,1.0,0.0);

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
			_zoom -= (float) 0.01f * diffx * (abs(_zoom) + 1);
		} else if (_Buttons[0]) {
			_rotx += (float) 0.5f * diffy;
			_roty += (float) 0.5f * diffx;
		} else if (_Buttons[1]) {
			_tx += (float) 0.0005f * diffx * (abs(_zoom) + 1);
			_ty -= (float) 0.0005f * diffy * (abs(_zoom) + 1);
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
	bool dynamicConstraints;
	bool collisions;
	bool follow;
};

Camera cam;
//-------------------------------------------------------------------------------
//
void printUsage() {
	cout << "Usage: ";
	cout << " ClothSym " << endl;
	cout
			<< "      {[-statobj input.obj] | [-simobj input.obj timestep mass] | [-simOFF input.obj timestep mass] | [-aniobj inputXXX.obj] | [-elliobj input framecount]}"
			<< endl;
	cout
			<< "      {[-dcons4 FollowPoint1 FollowPoint2 FollowPoint3 FollowPoint4 LeadEllipse1 LeadEllipse2 LeadEllipse3 LeadEllipse4 HierarchyEllipse1 HierarchyEllipse2 HierarchyEllipse3 HierarchyEllipse4] |"
			<< endl;
	cout
			<< "      [-dcons3 FollowPoint1 FollowPoint2 FollowPoint3  LeadEllipse1 LeadEllipse2 LeadEllipse3 HierarchyEllipse1 HierarchyEllipse2 HierarchyEllipse3] |"
			<< endl;
	cout
			<< "      [-dcons2 FollowPoint1 FollowPoint2 LeadEllipse1 LeadEllipse2 HierarchyEllipse1 HierarchyEllipse2] |"
			<< endl;
	cout << "      [-dcons1 FollowPoint1  LeadEllipse1 HierarchyEllipse1] }"
			<< endl;
	cout << "      [-coll]" << endl;
	cout
			<< "      [-cmesh lowQualityMeshIndexNumber highQualityMeshIndexNumber]"
			<< endl;
	cout << "      [-kbe KBe(float)]" << endl;
	cout << "      [-kbd KBd(float)]" << endl;
	cout << "      [-ke Ke(float)]" << endl;
	cout << "      [-kd Kd(float)]" << endl;
	cout << "      [-mus MUs(float)]" << endl;
	cout << "      [-mud MUd(float)]" << endl;
	cout << "      [-d i] " << endl;
	cout << "      [-img directory] " << endl;
	cout << "      [-cg max_iter(float) err_res(float)] " << endl;
	cout << "      [-gamma value(float)] " << endl;
	cout << "ORDER MATTERS: -elliobj, -simobj..." << endl;
}

void printStats() {
	profiler.print();
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
				DRAWMODELS.push_back(true);
			} else {
				malformedArg = true;
			}

		} else if (!strcmp(argv[i], "-simobj")) {

			if (isThereMore(i, argc, 3)) {
				std::string filename = std::string(argv[++i]);
				TIMESTEP = atof(argv[++i]);
				MASS = atof(argv[++i]);
				OBJParser parser;
				TriangleMesh* mesh = parser.parseOBJ(filename);
				world.loadSimModel(mesh, TIMESTEP, MASS, "default");
				hasOBJ = true;
				DYNAMIC_CONSTRAINTS = false;
				STATIC_CONSTRAINTS = true;
				cam.dynamicConstraints = false;
				DRAWMODELS.push_back(true);
			} else {
				malformedArg = true;
			}

		} else if (!strcmp(argv[i], "-simobj_new")) {

			if (isThereMore(i, argc, 3)) {
				std::string filename = std::string(argv[++i]);
				TIMESTEP = atof(argv[++i]);
				MASS = atof(argv[++i]);
				OBJParser parser;
				TriangleMesh* mesh = parser.parseOBJ(filename);
				world.loadSimModel(mesh, TIMESTEP, MASS, "newmark");
				hasOBJ = true;
				DYNAMIC_CONSTRAINTS = false;
				STATIC_CONSTRAINTS = true;
				cam.dynamicConstraints = false;
				DRAWMODELS.push_back(true);
			} else {
				malformedArg = true;
			}

		} else if (!strcmp(argv[i], "-simobj_exp")) {

			if (isThereMore(i, argc, 3)) {
				std::string filename = std::string(argv[++i]);
				TIMESTEP = atof(argv[++i]);
				MASS = atof(argv[++i]);
				OBJParser parser;
				TriangleMesh* mesh = parser.parseOBJ(filename);
				world.loadSimModel(mesh, TIMESTEP, MASS, "explicit");
				hasOBJ = true;
				DYNAMIC_CONSTRAINTS = false;
				STATIC_CONSTRAINTS = true;
				cam.dynamicConstraints = false;
				DRAWMODELS.push_back(true);
			} else {
				malformedArg = true;
			}

		} else if (!strcmp(argv[i], "-simOFF")) {

			if (isThereMore(i, argc, 3)) {
				std::string filename = std::string(argv[++i]);
				TIMESTEP = atof(argv[++i]);
				MASS = atof(argv[++i]);
				OFFParser parser;
				TriangleMesh* mesh = parser.parse(filename);
				string objfilename = filename + ".obj";
				mesh->exportAsOBJ(objfilename);
				world.loadSimModel(mesh, TIMESTEP, MASS, "default");
				hasOBJ = true;
				DRAWMODELS.push_back(true);
			}
			/*
			 else if (isThereMore(i, argc, 2)) {
			 std::string filename = std::string(argv[++i]);
			 double timeStep = atof(argv[++i]);
			 world.loadSimModel(filename, TIMESTEP, MASS);
			 hasOBJ = true;
			 } else if (isThereMore(i, argc, 1)) {
			 std::string filename = std::string(argv[++i]);
			 world.loadSimModel(filename, TIMESTEP, MASS);
			 hasOBJ = true;
			 }
			 //*/
			else {
				malformedArg = true;
			}

		} else if (!strcmp(argv[i], "-aniobj")) {

			if (isThereMore(i, argc, 1)) {
				std::string filename = std::string(argv[++i]);
				world.loadAniModel(filename);
				hasOBJ = true;
				DRAWMODELS.push_back(true);
			} else {
				malformedArg = true;
			}

		} else if (!strcmp(argv[i], "-elliobj")) {

			if (isThereMore(i, argc, 2)) {
				std::string filename = std::string(argv[++i]);
				int numFrames = atoi(argv[++i]);
				world.loadEllipseModel(filename, numFrames);
				hasOBJ = true;
				DRAWMODELS.push_back(true);
			} else {
				malformedArg = true;
			}

		} else if (!strcmp(argv[i], "-coll")) {
			COLLISIONS = true;
			cam.collisions = true;

		} else if (!strcmp(argv[i], "-cmesh")) {
			if (isThereMore(i, argc, 3)) {
				LOWQINDEX = atoi(argv[++i]);
				HIGHQINDEX = atoi(argv[++i]);
				SYNCSTEP = atof(argv[++i]);
				//cout<<"SYNC STEP HAS BEEN SET TO: "<<SYNCSTEP<<endl;
				world.initializeMeshSyncing(LOWQINDEX, HIGHQINDEX);
			} else {
				malformedArg = true;
			}

		} else if (!strcmp(argv[i], "-edamp")) {

			if (isThereMore(i, argc, 1)) {
				EDAMP = atof(argv[++i]);
			} else {
				malformedArg = true;
			}
		} else if (!strcmp(argv[i], "-dcons1")) {
			if (isThereMore(i, argc, 3)) {
				FOLLOW1 = atoi(argv[++i]);
				LEAD1 = atoi(argv[++i]);
				HIERARCHY1 = atoi(argv[++i]);
				DYNAMIC_CONSTRAINTS = true;
				STATIC_CONSTRAINTS = false;
				cam.dynamicConstraints = true;
			} else {
				malformedArg = true;
			}

		} else if (!strcmp(argv[i], "-dcons2")) {
			if (isThereMore(i, argc, 6)) {
				FOLLOW1 = atoi(argv[++i]);
				FOLLOW2 = atoi(argv[++i]);
				LEAD1 = atoi(argv[++i]);
				LEAD2 = atoi(argv[++i]);
				HIERARCHY1 = atoi(argv[++i]);
				HIERARCHY2 = atoi(argv[++i]);
				STATIC_CONSTRAINTS = false;
				DYNAMIC_CONSTRAINTS = true;
				cam.dynamicConstraints = true;
			} else {
				malformedArg = true;
			}

		} else if (!strcmp(argv[i], "-dcons3")) {
			if (isThereMore(i, argc, 9)) {
				FOLLOW1 = atoi(argv[++i]);
				FOLLOW2 = atoi(argv[++i]);
				FOLLOW3 = atoi(argv[++i]);
				LEAD1 = atoi(argv[++i]);
				LEAD2 = atoi(argv[++i]);
				LEAD3 = atoi(argv[++i]);
				HIERARCHY1 = atoi(argv[++i]);
				HIERARCHY2 = atoi(argv[++i]);
				HIERARCHY3 = atoi(argv[++i]);
				STATIC_CONSTRAINTS = false;
				DYNAMIC_CONSTRAINTS = true;
				cam.dynamicConstraints = true;
			} else {
				malformedArg = true;
			}

		} else if (!strcmp(argv[i], "-dcons4")) {
			if (isThereMore(i, argc, 12)) {
				FOLLOW1 = atoi(argv[++i]);
				FOLLOW2 = atoi(argv[++i]);
				FOLLOW3 = atoi(argv[++i]);
				FOLLOW4 = atoi(argv[++i]);
				LEAD1 = atoi(argv[++i]);
				LEAD2 = atoi(argv[++i]);
				LEAD3 = atoi(argv[++i]);
				LEAD4 = atoi(argv[++i]);
				HIERARCHY1 = atoi(argv[++i]);
				HIERARCHY2 = atoi(argv[++i]);
				HIERARCHY3 = atoi(argv[++i]);
				HIERARCHY4 = atoi(argv[++i]);
				STATIC_CONSTRAINTS = false;
				DYNAMIC_CONSTRAINTS = true;
				cam.dynamicConstraints = true;
			} else {
				malformedArg = true;
			}

		} else if (!strcmp(argv[i], "-discons")) {
			DYNAMIC_CONSTRAINTS = false;
			cam.dynamicConstraints = false;
			STATIC_CONSTRAINTS = false;
		} else if (!strcmp(argv[i], "-ke")) {

			if (isThereMore(i, argc, 1)) {
				Ke = atof(argv[++i]);
			} else {
				malformedArg = true;
			}
		} else if (!strcmp(argv[i], "-kd")) {

			if (isThereMore(i, argc, 1)) {
				Kd = atof(argv[++i]);
			} else {
				malformedArg = true;
			}
		} else if (!strcmp(argv[i], "-kbe")) {

			if (isThereMore(i, argc, 1)) {
				KBe = atof(argv[++i]);
				BEND_FORCES = true;
			} else {
				malformedArg = true;
			}
		} else if (!strcmp(argv[i], "-kbd")) {

			if (isThereMore(i, argc, 1)) {
				KBd = atof(argv[++i]);
				BEND_FORCES = true;
			} else {
				malformedArg = true;
			}
		} else if (!strcmp(argv[i], "-mus")) {

			if (isThereMore(i, argc, 1)) {
				MUs = atof(argv[++i]);
				FRICTION_FORCES = true;
			} else {
				malformedArg = true;
			}
		} else if (!strcmp(argv[i], "-mud")) {

			if (isThereMore(i, argc, 1)) {
				MUd = atof(argv[++i]);
				FRICTION_FORCES = true;
			} else {
				malformedArg = true;
			}
		} else if (!strcmp(argv[i], "-wind")) {

			if (isThereMore(i, argc, 3)) {
				WIND[0] = atof(argv[++i]);
				WIND[1] = atof(argv[++i]);
				WIND[2] = atof(argv[++i]);
				isWIND = true;
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

		} else if (!strcmp(argv[i], "-obj")) {

			if (isThereMore(i, argc, 1)) {
				std::string dirname = std::string(argv[++i]);
				world.inializeExportSim(dirname, cam.inverseFPS);
			} else {
				malformedArg = true;
			}

		} else if (!strcmp(argv[i], "-kcoll")) {

			if (isThereMore(i, argc, 1)) {
				Kcoll = atof(argv[++i]);
			} else {
				malformedArg = true;
			}

		} else if (!strcmp(argv[i], "-cg")) {

			if (isThereMore(i, argc, 2)) {
				MAX_CG_ITER = atoi(argv[++i]);
				MAX_CG_ERR = atof(argv[++i]);
			} else {
				malformedArg = true;
			}

		} else if (!strcmp(argv[i], "-gamma")) {

			if (isThereMore(i, argc, 1)) {
				GAMMA = atof(argv[++i]);
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
	case 'f':
		cam.follow = !cam.follow;
		break;
	case 'd':
		DYNAMIC_CONSTRAINTS = !DYNAMIC_CONSTRAINTS;
		break;
	case 'e':
		DRAWELLIPSOIDS = !DRAWELLIPSOIDS;
		break;
	case '1':
		DRAWMODELS[0] = !DRAWMODELS[0];
		cout << "Model 1: " << DRAWMODELS[0] << endl;
		break;
	case '2':
		DRAWMODELS[1] = !DRAWMODELS[1];
		cout << "Model 2: " << DRAWMODELS[1] << endl;
		break;
	case '3':
		DRAWMODELS[2] = !DRAWMODELS[2];
		cout << "Model 3: " << DRAWMODELS[2] << endl;
		break;
	case '4':
		DRAWMODELS[3] = !DRAWMODELS[3];
		cout << "Model 4: " << DRAWMODELS[3] << endl;
		break;
	case 27:
		closeMe(0);
		break;
	}
}

//-------------------------------------------------------------------------------
//
void init(void) {
	cout<<"Simulation with Display at: "<<1/cam.inverseFPS<<"FPS."<<endl;
	if (DYNAMIC_CONSTRAINTS)
		world.createVertexToAnimatedEllipseContraint();
	else
		world.createFixedVertexContraints();

	if (cam.collisions)
		world.createVertexToAnimatedEllipseCollisions();

	glEnable(GL_LIGHTING);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	glShadeModel(GL_SMOOTH);
	glClearColor(0.7, 0.7, 0.7, 0.0);

	GLfloat global_ambient[] = { .1f, .1f, .1f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

	glEnable(GL_NORMALIZE);

	//Lights:
	GLfloat ambient[] = { .1f, .1f, .1f };
	GLfloat diffuse[] = { .6f, .5f, .5f };
	GLfloat specular[] = { 0.0, 0.0, 0.0, 1.0 };
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
	GLfloat mat_ambient[] = { 0.0, 0.1, 0.6, 1.0 };
	GLfloat mat_diffuse[] = { 0.0, 0.1, 0.6, 1.0 };
	GLfloat mat_specular[] = { 0.0, 0.0, 0.0, 1.0 };
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
// STUFF FOR RENDERING BITMAPS FOR OPENGL FRAMERATE
//
char s[30];
int* Font = (int*) GLUT_BITMAP_8_BY_13;
void setOrthographicProjection() {
	// switch to projection mode
	glMatrixMode(GL_PROJECTION);
	// save previous matrix which contains the
	//settings for the perspective projection
	glPushMatrix();
	// reset matrix
	glLoadIdentity();
	// set a 2D orthographic projection
	gluOrtho2D(0, cam._w, 0, cam._h);
	// invert the y axis, down is positive
	glScalef(1, -1, 1);
	// mover the origin from the bottom left corner
	// to the upper left corner
	glTranslatef(0, -cam._h, 0);
	glMatrixMode(GL_MODELVIEW);
}
void resetPerspectiveProjection() {
	// set the current matrix to GL_PROJECTION
	glMatrixMode(GL_PROJECTION);
	// restore previous settings
	glPopMatrix();
	// get back to GL_MODELVIEW matrix
	glMatrixMode(GL_MODELVIEW);
}
void renderBitmapString(float x, float y, void *font, char *string) {
	char *c;
	// set position to start drawing fonts
	glRasterPos2f(x, y);
	// loop all the characters in the string
	for (c = string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}
void drawParameters() {
	//glColor3f(1.0f,1.0f,1.0f);
	char s2[50];
	sprintf(s2, "Time Step (s): %4.5f", TIMESTEP);
	renderBitmapString(5, 15, (void *) Font, s2);
	sprintf(s2, "Mass (kg): %4.2f", MASS);
	renderBitmapString(5, 30, (void *) Font, s2);
	sprintf(s2, "# of Vertices: %i", MESHSIZE);
	renderBitmapString(5, 45, (void *) Font, s2);
	sprintf(s2, "Ke (Planar): %4.2f", Ke);
	renderBitmapString(5, 60, (void *) Font, s2);
	sprintf(s2, "Kd (Planar): %4.2f", Kd);
	renderBitmapString(5, 75, (void *) Font, s2);
	if (BEND_FORCES) {
		sprintf(s2, "Bend Forces: Enabled");
		renderBitmapString(5, cam._h - 75, (void *) Font, s2);
		sprintf(s2, "Ke (Bend): %4.4f", KBe);
		renderBitmapString(5, cam._h - 60, (void *) Font, s2);
		sprintf(s2, "Kd (Bend): %4.4f", KBd);
		renderBitmapString(5, cam._h - 45, (void *) Font, s2);
	} else {
		sprintf(s2, "Bend Forces: Disabled");
		renderBitmapString(5, cam._h - 75, (void *) Font, s2);
	}
	if (FRICTION_FORCES) {
		sprintf(s2, "Friction Forces: Enabled");
		renderBitmapString(300, cam._h - 75, (void *) Font, s2);
		sprintf(s2, "Mu Static: %4.2f", MUs);
		renderBitmapString(300, cam._h - 60, (void *) Font, s2);
		sprintf(s2, "Mu Dynamic: %4.2f", MUd);
		renderBitmapString(300, cam._h - 45, (void *) Font, s2);
	} else {
		sprintf(s2, "Friction Forces: Disabled");
		renderBitmapString(300, cam._h - 75, (void *) Font, s2);
	}
	if (isWIND) {
		sprintf(s2, "Wind Forces: Enabled");
		renderBitmapString(5, cam._h - 30, (void *) Font, s2);
		sprintf(s2, "Velocity: %4.4f", WIND[0]);
		renderBitmapString(5, cam._h - 15, (void *) Font, s2);
		sprintf(s2, " %4.4f", WIND[1]);
		renderBitmapString(140, cam._h - 15, (void *) Font, s2);
		sprintf(s2, " %4.4f", WIND[2]);
		renderBitmapString(200, cam._h - 15, (void *) Font, s2);
	} else {
		sprintf(s2, "Wind Forces: Disabled");
		renderBitmapString(5, cam._h - 30, (void *) Font, s2);
	}
	if (COLLISIONS) {
		sprintf(s2, "Collisions: Enabled");
		renderBitmapString(300, cam._h - 30, (void *) Font, s2);
	} else {
		sprintf(s2, "Collisions: Disabled");
		renderBitmapString(300, cam._h - 30, (void *) Font, s2);
	}
	if (DYNAMIC_CONSTRAINTS) {
		sprintf(s2, "Moving Constraints: Enabled");
		renderBitmapString(300, cam._h - 15, (void *) Font, s2);
	} else {
		sprintf(s2, "Moving Constraints: Disabled");
		renderBitmapString(300, cam._h - 15, (void *) Font, s2);
	}
}
//FPS CALCULATION VARIABLES
int GLframe = 0, GLtime, GLtimebase = 0;

//-------------------------------------------------------------------------------
// This function draws the actual world using OpenGL.
//
void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (cam.follow)
		cam.setFocusedView(world.getFocusPoint());
	else
		cam.setView();

	if (cam.wireFrame) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glPushMatrix();

	glDisable(GL_LIGHTING);
	White();
	glColor3f(1.0f, 1.0f, 1.0f);
	if (cam.showGrid) {
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
	glEnable(GL_LIGHTING);

	//Draw the world!
	world.draw();

	glPopMatrix();

	glDisable(GL_LIGHTING);
	//---------OPENGL FRAMERATE-----------------//
	//*
	//FPS CALCULATOR
	GLframe++;
	GLtime = glutGet(GLUT_ELAPSED_TIME);
	if (GLtime - GLtimebase > 1000) {
		sprintf(s, "FPS:%4.2f", GLframe * 1000.0 / (GLtime - GLtimebase));
		//cout<<"FPS: "<<GLframe*1000.0/(GLtime-GLtimebase)<<endl;
		GLtimebase = GLtime;
		GLframe = 0;
	}

	//CODE TO RENDER A BITMAP
	glColor3f(1.0f, 1.0f, 1.0f);
	setOrthographicProjection();
	glPushMatrix();
	glLoadIdentity();
	renderBitmapString(cam._w - 100, 15, (void *) Font, s);
	drawParameters();
	glPopMatrix();
	resetPerspectiveProjection();
	//*/
	//-----------------------------------------------
	glEnable(GL_LIGHTING);

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

	if (!cam.paused) {
		if (fpstimer.Elapsed() < cam.inverseFPS) {
			return;
		} else {
			fpstimer.Stop();
			fpstimer.Start();
		}

		profiler.frametimers.Start();

		imagesaver.saveFrame(world.getTime(), false, cam.inverseFPS, cam._w,
				cam._h);
		world.exportSim(1, world.getTime(), true, cam.inverseFPS);
		world.advance(cam.inverseFPS);

		profiler.frametimers.switchToTimer("postRedisplay");
		glutPostRedisplay();

		profiler.frametimers.Stop();
	}
}

//-------------------------------------------------------------------------------
//
void myMousePress(int button, int state, int x, int y) {

	//*
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && glutGetModifiers()
			== GLUT_ACTIVE_SHIFT) {
		//Find the point
		float z = 0.5;
		double ox, oy, oz;
		GLdouble modelview[16];
		GLdouble proj[16];
		GLint view[4];
		glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
		glGetDoublev(GL_PROJECTION_MATRIX, proj);
		glGetIntegerv(GL_VIEWPORT, view);
		// glReadPixels( x, view[3]-y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z );
		//cout<<"mouse position Screen Space-> x: "<<x<<"   y: "<<y<<endl;
		if (GL_TRUE == gluUnProject(x, view[3] - y, z, modelview, proj, view,
				&ox, &oy, &oz)) {
			//cout<<"mouse Position World Space-> x: "<<ox*(1/cam.getZoom())<<"   y: "<<oy*(1/cam.getZoom())<<"   z: "<<0<<endl;
			world.enableMouseForce(vec3(ox * 5, oy * 5, 0));
			//sys->enableMouseForce(vec3(ox,oy,0));
		} else
			cout << "gluUnProject Returned False" << endl;
	} else {
		cam.mousepress(button, state, x, y);
		world.disableMouseForce();
	}
	//*/
}

//-------------------------------------------------------------------------------
//
void myMouseMove(int x, int y) {

	//*
	if (world.isMouseEnabled()) {
		//Find the point
		float z = 0.5;
		double ox, oy, oz;
		GLdouble modelview[16];
		GLdouble proj[16];
		GLint view[4];
		glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
		glGetDoublev(GL_PROJECTION_MATRIX, proj);
		glGetIntegerv(GL_VIEWPORT, view);
		// glReadPixels( x, view[3]-y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z );
		//cout<<"mouse position Screen Space-> x: "<<x<<"   y: "<<y<<endl;
		if (GL_TRUE == gluUnProject(x, view[3] - y, z, modelview, proj, view,
				&ox, &oy, &oz)) {
			//cout<<"mouse Position World Space-> x: "<<ox<<"   y: "<<oy<<"   z: "<<oz<<endl;
			world.updateMouseForce(vec3(ox * 5, 5 * oy, 0));
		} else
			cout << "gluUnProject Returned False" << endl;
	} else
		cam.mousemotion(x, y);
	//*/
}

//-------------------------------------------------------------------------------
//
void closeMe(int code) {
	printStats();
	exit(code);
}

//-------------------------------------------------------------------------------
//
int main(int argc, char *argv[]) {

	if (parseCommandLine(argc, argv)) {
		printUsage();
		exit(1);
	}

	cam._w = 1280;
	cam._h = 720;

	//Create Window:
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(cam._w, cam._h);
	glutCreateWindow("ClothSym");

	//Initialize Settings:
	init();

	fpstimer.Start();

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
