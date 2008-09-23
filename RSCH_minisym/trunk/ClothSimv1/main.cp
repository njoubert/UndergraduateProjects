// Simple OpenGL example for CS184 sp08 by Trevor Standley, modified from sample code for CS184 on Sp06
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include "VectorTriple.h"

//#	include <sys/time.h>



#include <GLUT/glut.h>
#include <OpenGL/glu.h>

	//#include "Globals.h"
#include <time.h>
#include <math.h>
#include "algebra3.h"

/*#ifdef _WIN32
static DWORD lastTime;
#else
static struct timeval lastTime;
#endif*/

#define PI 3.14159265


using namespace std;

//****************************************************
// Global Variables
//****************************************************
//For tetrahedron
/*#define stepSize .005
#define k_s 10
#define k_d .5
#define Fgravity -1.1*/

#define stepSize .0005
#define k_s 15
#define k_d .1
#define k_b .1
#define CF 1
#define Fgravity -2.1

int width = 400;
int height = 400;
static float angle=0.0,ratio;
static float x=0.0f,y=0.0f,z=5.0f;
static float lx=0.0f,ly=0.0f,lz=-1.0f;

float groundPlane = -.5;
bool trigger = false;


//VectorTriple u[8];
VectorTriple u[16];
//VectorTriple initial_pos[4];
VectorTriple initial_pos[8];

void createBox(VectorTriple *u, float y){
	VectorTriple x1(-.5, y+1.0, -.5);
	VectorTriple x2(-.5, y, -.5);
	VectorTriple x3( .5, y, -.5);
	VectorTriple x4( .5, y+1.0, -.5);
	VectorTriple x5(-.5, y+1.0,  .5);
	VectorTriple x6(-.5, y,  .5);
	VectorTriple x7( .5, y,  .5);
	VectorTriple x8( .5, y+1.0,  .5);
	VectorTriple v1(0, 0, 0);
	VectorTriple v2(0, 0, 0);
	VectorTriple v3(0, 0, 0);
	VectorTriple v4(0, 0, 0);
	VectorTriple v5(0, 0, 0);
	VectorTriple v6(0, 0, 0);
	VectorTriple v7(0, 0, 0);
	VectorTriple v8(0, 0, 0);
	u[0] = x1, u[1] = x2, u[2] = x3, u[3] = x4, u[4] = x5, u[5] = x6, u[6] = x7, u[7] = x8,
	u[8] = v1; u[9] = v2, u[10] = v3, u[11] = v4, u[12] = v5; u[13] = v6, u[14] = v7, u[15] = v8;
}
void createTetrahedron(VectorTriple *u){
float a = sqrt(1*1 - .5*.5);
	float r = .5 * tan(3.1415/6);
	float b = sqrt(a*a - r*r);
	VectorTriple x1(0, .5, a/2);
	VectorTriple x2(-.5, .5, -a/2);
	VectorTriple x3(.5, .5, -a/2);
	VectorTriple x4(0, b+.5, -a/2 + r);
	VectorTriple v1(0, 0, 0);
	VectorTriple v2(0, 0, 0);
	VectorTriple v3(0, 0, 0);
	VectorTriple v4(0, 0, 0);
	u[0] = x1, u[1] = x2, u[2] = x3, u[3] = x4, u[4] = v1; u[5] = v2, u[6] = v3, u[7] = v4;
}
void createPatch(VectorTriple *u, float L, float y){
	float h = sqrt(L*L - (L/2)*(L/2));
	VectorTriple x1(-(L/2)*cos(PI/3), y, -(L/2)*sin(PI/3));
	VectorTriple x2(-h*cos(PI/6), y, h*sin(PI/6));
	VectorTriple x3( (L/2)*cos(PI/3), y,  (L/2)*sin(PI/3));
	VectorTriple x4( h*cos(PI/6), y, -h*sin(PI/6));
	VectorTriple v1(0, 0, 0);
	VectorTriple v2(0, 0, 0);
	VectorTriple v3(0, 0, 0);
	VectorTriple v4(0, 0, 0);
	u[0] = x1, u[1] = x2, u[2] = x3, u[3] = x4, u[4] = v1; u[5] = v2, u[6] = v3, u[7] = v4;
}
void changeColor(GLfloat R, GLfloat G, GLfloat B){
	GLfloat mat_ambient[] = { R, G, B, 1.0 };
	GLfloat mat_diffuse[] = { R, G, B, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
}
void drawTetrahedron(VectorTriple *u){
	changeColor(.1, .1, .9);
	glBegin(GL_POLYGON);
		glVertex3f(u[0].valueX(), u[0].valueY(), u[0].valueZ());
		glVertex3f(u[1].valueX(), u[1].valueY(), u[1].valueZ());			
		glVertex3f(u[3].valueX(), u[3].valueY(), u[3].valueZ());
	glEnd();
	changeColor(.1, .9, .1);
	glBegin(GL_POLYGON);
		glVertex3f(u[1].valueX(), u[1].valueY(), u[1].valueZ());			
		glVertex3f(u[2].valueX(), u[2].valueY(), u[2].valueZ());
		glVertex3f(u[3].valueX(), u[3].valueY(), u[3].valueZ());
	glEnd();
	changeColor(.9, .1, .1);
	glBegin(GL_POLYGON);
		glVertex3f(u[0].valueX(), u[0].valueY(), u[0].valueZ());
		glVertex3f(u[1].valueX(), u[1].valueY(), u[1].valueZ());			
		glVertex3f(u[2].valueX(), u[2].valueY(), u[2].valueZ());
	glEnd();
	changeColor(.1, .9, .9);	
	glBegin(GL_POLYGON);
		glVertex3f(u[0].valueX(), u[0].valueY(), u[0].valueZ());			
		glVertex3f(u[2].valueX(), u[2].valueY(), u[2].valueZ());
		glVertex3f(u[3].valueX(), u[3].valueY(), u[3].valueZ());
	glEnd();
	changeColor(.9, .1, .1);
	glBegin(GL_POLYGON);
		glVertex3f(-100, groundPlane, 100);			
		glVertex3f(-100, groundPlane, -100);
		glVertex3f(100, groundPlane, -100);
		glVertex3f(100, groundPlane, 100);
	glEnd();
}
void drawPatch(VectorTriple *u){
	changeColor(.1, .1, .9);
	glBegin(GL_POLYGON);
		glVertex3f(u[0].valueX(), u[0].valueY(), u[0].valueZ());
		glVertex3f(u[1].valueX(), u[1].valueY(), u[1].valueZ());			
		glVertex3f(u[2].valueX(), u[2].valueY(), u[2].valueZ());
	glEnd();
	changeColor(.1, .9, .1);
	glBegin(GL_POLYGON);
		glVertex3f(u[2].valueX(), u[2].valueY(), u[2].valueZ());			
		glVertex3f(u[3].valueX(), u[3].valueY(), u[3].valueZ());
		glVertex3f(u[0].valueX(), u[0].valueY(), u[0].valueZ());
	glEnd();

	changeColor(.9, .1, .1);
	glBegin(GL_POLYGON);
		glVertex3f(-100, groundPlane, 100);			
		glVertex3f(-100, groundPlane, -100);
		glVertex3f(100, groundPlane, -100);
		glVertex3f(100, groundPlane, 100);
	glEnd();
}
void drawBox(VectorTriple *u){
	changeColor(.1, .1, .9);
	glBegin(GL_POLYGON);
		glVertex3f(u[0].valueX(), u[0].valueY(), u[0].valueZ());
		glVertex3f(u[1].valueX(), u[1].valueY(), u[1].valueZ());			
		glVertex3f(u[2].valueX(), u[2].valueY(), u[2].valueZ());
		glVertex3f(u[3].valueX(), u[3].valueY(), u[3].valueZ());
	glEnd();
	changeColor(.1, .9, .1);
	glBegin(GL_POLYGON);
		glVertex3f(u[4].valueX(), u[4].valueY(), u[4].valueZ());			
		glVertex3f(u[5].valueX(), u[5].valueY(), u[5].valueZ());
		glVertex3f(u[6].valueX(), u[6].valueY(), u[6].valueZ());
		glVertex3f(u[7].valueX(), u[7].valueY(), u[7].valueZ());
	glEnd();
	changeColor(.9, .1, .1);
	glBegin(GL_POLYGON);
		glVertex3f(u[0].valueX(), u[0].valueY(), u[0].valueZ());
		glVertex3f(u[4].valueX(), u[4].valueY(), u[4].valueZ());			
		glVertex3f(u[7].valueX(), u[7].valueY(), u[7].valueZ());
		glVertex3f(u[3].valueX(), u[3].valueY(), u[3].valueZ());
	glEnd();
	changeColor(.1, .9, .9);
	glBegin(GL_POLYGON);
		glVertex3f(u[1].valueX(), u[1].valueY(), u[1].valueZ());			
		glVertex3f(u[5].valueX(), u[5].valueY(), u[5].valueZ());
		glVertex3f(u[6].valueX(), u[6].valueY(), u[6].valueZ());
		glVertex3f(u[2].valueX(), u[2].valueY(), u[2].valueZ());
	glEnd();
	changeColor(.9, .9, .1);
	glBegin(GL_POLYGON);
		glVertex3f(u[0].valueX(), u[0].valueY(), u[0].valueZ());
		glVertex3f(u[1].valueX(), u[1].valueY(), u[1].valueZ());			
		glVertex3f(u[5].valueX(), u[5].valueY(), u[5].valueZ());
		glVertex3f(u[4].valueX(), u[4].valueY(), u[4].valueZ());
	glEnd();
	changeColor(.9, .1, .9);
	glBegin(GL_POLYGON);
		glVertex3f(u[3].valueX(), u[3].valueY(), u[3].valueZ());			
		glVertex3f(u[7].valueX(), u[7].valueY(), u[7].valueZ());
		glVertex3f(u[6].valueX(), u[6].valueY(), u[6].valueZ());
		glVertex3f(u[2].valueX(), u[2].valueY(), u[2].valueZ());
	glEnd();

	changeColor(.9, .1, .1);
	glBegin(GL_POLYGON);
		glVertex3f(-100, groundPlane, 100);			
		glVertex3f(-100, groundPlane, -100);
		glVertex3f(100, groundPlane, -100);
		glVertex3f(100, groundPlane, 100);
	glEnd();
}
void f4(VectorTriple *u){
	VectorTriple Fnull(0,0,0);
	VectorTriple Flist[8];
	VectorTriple FlistSum[8];

	for(int k = 0; k < 8; k++)
		FlistSum[k] = Fnull;

	for(int i = 0; i < 8; i++)
		for(int j = 0; j < 8; j++) 
		FlistSum[i] = FlistSum[i] + Flist[j];

	VectorTriple Fexternal(0, Fgravity, 0);
	VectorTriple F;
	
	float M = 1;

	//BEND FORCES
	//VectorTriple Ffixbend(0,1,0);

	VectorTriple a = cross((u[3] - u[0]),(u[4] - u[0]));
	VectorTriple b = cross((u[4] - u[0]),(u[1] - u[0]));
	VectorTriple c = cross((u[1] - u[2]),(u[3] - u[2]));
	VectorTriple d = cross((u[6] - u[2]),(u[3] - u[2]));
	VectorTriple e = cross((u[7] - u[6]),(u[5] - u[6]));
	VectorTriple f = cross((u[1] - u[2]),(u[6] - u[2]));
	
	VectorTriple crosses[12];
	crosses[0] = cross(a.normalize(), b.normalize());
	crosses[1] = cross(a.normalize(), c.normalize());
	crosses[2] = cross(a.normalize(), d.normalize());
	crosses[3] = cross(a.normalize(), e.normalize());
	crosses[4] = cross(b.normalize(), c.normalize());
	crosses[5] = cross(b.normalize(), e.normalize());
	crosses[6] = cross(b.normalize(), f.normalize());
	crosses[7] = cross(c.normalize(), d.normalize());
	crosses[8] = cross(c.normalize(), f.normalize());
	crosses[9] = cross(d.normalize(), e.normalize());
	crosses[10] = cross(d.normalize(), f.normalize());
	crosses[11] = cross(e.normalize(), f.normalize());
	
	float theta[12];
	for(int i = 0; i < 12; i++){
		if((crosses[i].valueX() < 0) || (crosses[i].valueY() < 0) || (crosses[i].valueZ() < 0))
			theta[i] = - asin(dot(crosses[i], crosses[i]));
		else if((crosses[i].valueX() > 0) || (crosses[i].valueY() > 0) || (crosses[i].valueZ() > 0))
			theta[i] =  asin(dot(crosses[i], crosses[i]));
		else
			theta[i] = 0;
		}
	
	//float r = 1;
	//VectorTriple Fexternalb(r*cos(theta), r*cos(theta), 0);
	
	VectorTriple Fbend_a = a*-k_b*theta[0] + a*-k_b*theta[1] + a*-k_b*theta[2] + a*-k_b*theta[3];
	VectorTriple Fbend_b = b*-k_b*theta[0] + b*-k_b*theta[4] + b*-k_b*theta[5] + b*-k_b*theta[6];
	VectorTriple Fbend_c = c*-k_b*theta[1] + c*-k_b*theta[4] + c*-k_b*theta[7] + c*-k_b*theta[8];
	VectorTriple Fbend_d = d*-k_b*theta[2] + d*-k_b*theta[7] + d*-k_b*theta[9] + d*-k_b*theta[10];
	VectorTriple Fbend_e = e*-k_b*theta[3] + e*-k_b*theta[5] + e*-k_b*theta[9] + e*-k_b*theta[11];
	VectorTriple Fbend_f = f*-k_b*theta[6] + f*-k_b*theta[8] + f*-k_b*theta[10] + f*-k_b*theta[11];

	
	//cout<<"Theta: "<<theta<<endl;
	//cout<<"Bend Force: ";
//	Fbend.print();
	//cout<<endl;

	float r, l;
	VectorTriple R, L, v;
	for(int i = 0; i < 8; i++){
		
		for(int k = 0; k < 8; k++)		
		Flist[k] = Fnull;
		
		for(int j = 0; j < 8; j++){
			if((i != j)){// && (i != 0 && j != 6) && (i != 1 && j != 7) && (i != 2 && j != 4) && (i != 3 && j != 5) && (i != 4 && j != 2)){
				R = u[i]- u[j];
				r = sqrt(R.valueX()*R.valueX() + R.valueY()*R.valueY() + R.valueZ()*R.valueZ());
				L = initial_pos[i] - initial_pos[j];
				l = sqrt(L.valueX()*L.valueX() + L.valueY()*L.valueY() + L.valueZ()*L.valueZ());
				v = u[i+8] - u[j+8];

				Flist[j] = (R/r)*(-k_s*(r-l)) - v*k_d;
			}
		}

		for(int p = 0; p < 8; p++)
		FlistSum[i] = FlistSum[i] + Flist[p];
	}
	
	for(int i = 0; i < 8; i++)
		u[i] = u[i+8];
		
	VectorTriple CornerForce(0,CF,0);
	for(int m = 0; m < 8; m++)
		u[m+8] = FlistSum[m]+Fexternal;

	/*	u[8] = u[8] + Fbend_a + Fbend_b + Fbend_c;
		u[9] = u[9] + Fbend_b + Fbend_c + Fbend_f;
		u[10] = u[10] + Fbend_f + Fbend_d + Fbend_c;
		u[11] = u[11] + Fbend_a + Fbend_c + Fbend_d;
		u[12] = u[12] + Fbend_a + Fbend_b + Fbend_e;
		u[13] = u[13] + Fbend_b + Fbend_e + Fbend_f;
		u[14] = u[14] + Fbend_d + Fbend_e + Fbend_f;
		u[15] = u[15] + Fbend_a + Fbend_e + Fbend_d;
	*/
		/*u[4] = Fnull;
		u[6] = CornerForce+Fbend;;
		u[5] = Fnull;
		u[7] = Fnull;*/
		//u[6].print();
		//u[2] = u[2]*10;

		
		
		/*u[4].setX(0);
		u[4].setY(0);
		u[4].setZ(0);
		u[5].setX(0);
		u[5].setY(0);
		u[5].setZ(0);
		u[6].setX(0);
		u[6].setY(0);
		u[6].setZ(0);*/
	if(!trigger){
		u[8].setX(0);
		u[8].setY(0);
		u[8].setZ(0);
	}
}
void f3(VectorTriple *u){
	
	VectorTriple Fnull(0,0,0);
	VectorTriple Flist[4];

	VectorTriple FlistSum[4];
	for(int k = 0; k < 4; k++)
		FlistSum[k] = Fnull;

	for(int i = 0; i < 4; i++)
		FlistSum[i] = Flist[0] + Flist[1] + Flist[2] + Flist[3];


	VectorTriple Fexternal(0, Fgravity, 0);
	VectorTriple F;
	
	float M = 1;

	//BEND FORCES
	//VectorTriple Ffixbend(0,1,0);

	VectorTriple n1 = cross(u[2] - u[1], u[0] - u[1]);
	VectorTriple n2 = cross((u[0] - u[3]),(u[2] - u[3]));
	
	float n1DOTn2 = dot(n1.normalize(), n2.normalize());
	
	float theta = acos(n1DOTn2);
	
	//float r = 1;
	//VectorTriple Fexternalb(r*cos(theta), r*cos(theta), 0);
	
	VectorTriple Fbend = n1*-k_b*theta;

	
	/*cout<<"Theta: "<<theta<<endl;
	cout<<"Bend Force: ";
	Fbend.print();
	cout<<endl;*/

	float r, l;
	VectorTriple R, L, v;
	for(int i = 0; i < 4; i++){
		
			Flist[0] = Fnull;
			Flist[1] = Fnull;
			Flist[2] = Fnull;
			Flist[3] = Fnull;
		
		for(int j = 0; j < 4; j++){
			if(i != j){// && abs(i-j) == 1){
				R = u[i]- u[j];
				r = sqrt(R.valueX()*R.valueX() + R.valueY()*R.valueY() + R.valueZ()*R.valueZ());
				L = initial_pos[i] - initial_pos[j];
				l = sqrt(L.valueX()*L.valueX() + L.valueY()*L.valueY() + L.valueZ()*L.valueZ());

				v = u[i+4] - u[j+4];
				Flist[j] = (R/r)*(-k_s*(r-l))-v*k_d;
			}
		}
		FlistSum[i] = Flist[0] + Flist[1] + Flist[2] + Flist[3];
	}
	

	for(int i = 0; i < 4; i++)
		u[i] = u[i+4];
		
	VectorTriple CornerForce(0,CF,0);
		u[4] = FlistSum[0]+Fexternal;
		u[5] = FlistSum[1]+Fexternal+CornerForce+Fbend;
		u[6] = FlistSum[2]+Fexternal;
		u[7] = FlistSum[3]+Fexternal;
		
	
		/*u[4] = Fnull;
		u[6] = CornerForce+Fbend;;
		u[5] = Fnull;
		u[7] = Fnull;*/
		//u[6].print();
		//u[2] = u[2]*10;

		
		
		u[4].setX(0);
		u[4].setY(0);
		u[4].setZ(0);
		//u[5].setX(0);
		//u[5].setY(0);
		//u[5].setZ(0);
		u[6].setX(0);
		u[6].setY(0);
		u[6].setZ(0);
		u[7].setX(0);
		u[7].setY(0);
		u[7].setZ(0);
}
void Euler(VectorTriple *u_tnew, VectorTriple *u_t, float delta_t, void (*fhandle)(VectorTriple*) ){
	int uSize = 16;			//
	VectorTriple u_curr[16];//VectorTriple u_curr[8];
	for(int j = 0; j < uSize; j++)
	u_curr[j] = u_t[j];

	fhandle(u_t);									//u_t_dot = f(u)
	
	for(int i = 0; i < uSize; i++)							
		u_t[i] = u_curr[i] + u_t[i] * delta_t;		//u_tnew = u_t + delta_t * u_t_dot
}
inline mat3 dfdx_spring(const vec3 &X, float length, float rest, float k){
mat3 I;
I = identity2D();

return((I - ((X/length)*(X/length)))*-k*(rest/length) - I);
}
void Implicit(){
mat3 I;
I = identity2D();
}
void myReshape(int w, int h) {
	width = w;
	height = h;

		if(h == 0)
		h = 1;

	ratio = 1.0f * w / h;
	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// Set the viewport to be the entire window
    	glViewport(0, 0, w, h);

	// Set the clipping volume
	gluPerspective(45,ratio,1,1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(x, y, z, 
		  	x + lx,y + ly,z + lz,
			0.0f,1.0f,0.0f);
	

	/*
	glViewport(0,0,width,height);// sets the rectangle that will be the window
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();				// loading the identity matrix for the screen

	//----------- setting the projection -------------------------
	// glOrtho sets left, right, bottom, top, zNear, zFar of the chord system


	// glOrtho(-1, 1 + (w-400)/200.0 , -1 -(h-400)/200.0, 1, 1, -1); // resize type = add
	// glOrtho(-w/400.0, w/400.0, -h/400.0, h/400.0, 1, -1); // resize type = center

	glOrtho(-1, 1, -1, 1, 1, -1);	// resize type = stretch

	//------------------------------------------------------------
*/
}
void initScene(){
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Clear to black, fully transparent
	
	myReshape(width,height);
	
	GLfloat mat_ambient[] = { 0.4, 0.6, 0.8, 1.0 };
	GLfloat mat_diffuse[] = { 0.4, 0.6, 0.8, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };
    GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
	//GLfloat light_position[] = { -1.0, -1.0, -1.0, 0.0 };

    glShadeModel (GL_SMOOTH);
	//glShadeModel (GL_FLAT);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glClearDepth(1.0f);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glEnable(GL_DEPTH_TEST);

	/*
	VectorTriple x1(-.5, .9, 0);
	VectorTriple x2(.5, .9, 0);
	VectorTriple x3(0, .5, 0);
	VectorTriple v1(0, 0, 0);
	VectorTriple v2(0, 0, 0);
	VectorTriple v3(0, 0, 0);
	u[0] = x1, u[1] = x2, u[2] = x3, u[3] = v1, u[4] = v2; u[5] = v3;
*/

	//createTetrahedron(u);
	//createPatch(u, 1.0f, 0.5);
	createBox(u, 1);
	for (int i = 0; i < 8; i++){
		initial_pos[i] = u[i];
	}
}
void myDisplay() {
	
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glMatrixMode(GL_MODELVIEW);					// indicate we are specifying camera transformations
	//glLoadIdentity();							// make sure transformation is "zero'd"
	
	//----------------------- intialize state of triagle---------------------
	
	VectorTriple u_tnew[8];
	void (*fhandle)(VectorTriple*)  = NULL;
    fhandle = &f4;
	
	Euler(u_tnew, u, stepSize, fhandle);
	
	//GROUND CONSTRAINTS
	VectorTriple Fnull(0,0,0);
		for(int i = 0; i < 8; i++){
			//cout<<"vertex "<<i<<" ";
			//u[i].print();
			if(u[i].valueY() < groundPlane){
				u[i].setY(-.5);
				if(u[i+8].valueY() < 0){
					//u[i].print();
					//u[i+8].print();
					//cout<<endl;
					u[i+8].setY(u[i+8].valueY()*-.5);
					//u[i+8].print();
					//`cout<<endl;
				}
			}	
		}

	//drawTetrahedron(u);
	//drawPatch(u);
	drawBox(u);

	/*cout<<"Vertex #3 Pos: ";
		u[2].print();
		cout<<endl;*/
/*	glBegin(GL_POLYGON);
		glVertex3f(0,    25, 0);
		glVertex3f(25,  -25, 0);			
		glVertex3f(-25, -25, 0);
	glEnd();
*/
	//-----------------------------------------------------------------------
	
	glFlush();
	//glutSwapBuffers();					// swap buffers (we earlier set double buffer)
}
void keyboard(unsigned char key, int x, int y){
   switch (key) {
      case 27:
         exit(0);
         break;
   }
   switch (key) {
      case 32:
         exit(0);
         break;
   }
   switch (key) {
   case '=' : 
	    glTranslatef(0.0f, 0.0f, 0.1f);
		glutPostRedisplay();
		break;
   }
   switch (key) {
   case '-' : 
	    glTranslatef(0.0f, 0.0f, -0.1f);
		glutPostRedisplay();
		break;
   }
     switch (key) {
   case 't' : 
	    trigger = true;
		glutPostRedisplay();
		break;
   }
}
void processSpecialKeys(int key, int x, int y) {
int mod;
	switch (key) {
		case GLUT_KEY_LEFT : 
			mod = glutGetModifiers();
			if (mod == GLUT_ACTIVE_SHIFT){
			glTranslatef(-0.1f, 0.0f, 0.0f);
			glutPostRedisplay();
			}
			else if (mod == GLUT_ACTIVE_CTRL){
			glRotatef(1.0f, 0.0f, 0.0f, 1.0f);
			glutPostRedisplay();
			}
			else {
			glRotatef(-1.0f, 0.0f, 1.0f, 0.0f);
			glutPostRedisplay();
			}
	}
	switch (key) {
		case GLUT_KEY_RIGHT : 
			mod = glutGetModifiers();
			if (mod == GLUT_ACTIVE_SHIFT){
			glTranslatef(0.1f, 0.0f, 0.0f);
			glutPostRedisplay();
			}
			else if (mod == GLUT_ACTIVE_CTRL){
			glRotatef(-1.0f, 0.0f, 0.0f, 1.0f);
			glutPostRedisplay();
			}
			else {
			glRotatef(1.0f, 0.0f, 1.0f, 0.0f);
			glutPostRedisplay();
			}
	}
	switch (key) {
		case GLUT_KEY_UP : 
			mod = glutGetModifiers();
			if (mod == GLUT_ACTIVE_SHIFT){
			glTranslatef(0, 0.1f, 0);
			glutPostRedisplay();
			}
			else {
			glRotatef(-1.0f, 1.0f, 0.0f, 0.0f);
			glutPostRedisplay();
			}
	}
	switch (key) {
		case GLUT_KEY_DOWN : 
			mod = glutGetModifiers();
			if (mod == GLUT_ACTIVE_SHIFT){
			glTranslatef(0.0f, -0.01f, 0.0f);
			glutPostRedisplay();
			}
			else {
			glRotatef(1.0f, 1.0f, 0.0f, 0.0f);
			glutPostRedisplay();
			}
	}
	
}
void myFrameMove() {
	//nothing here for now
//#ifdef _WIN32
//	Sleep(10);						//give ~10ms back to OS (so as not to waste the CPU)
//#endif
	glutPostRedisplay(); // forces glut to call the display function (myDisplay())
}
int main(int argc, char *argv[]) {
  	//This initializes glut
  	glutInit(&argc, argv);
  
  	//This tells glut to use a double-buffered window with red, green, and blue channels 
  	//glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);

  	//The size and position of the window
  	glutInitWindowSize(width, height);
  	glutInitWindowPosition(0, 0);
  	glutCreateWindow("Sim");

  	initScene();							// quick function to set up scene
  
  	glutDisplayFunc(myDisplay);				// function to run when its time to draw something
  	glutReshapeFunc(myReshape);				// function to run when the window gets resized
  	glutIdleFunc(myFrameMove);				// function to run when not handling any other task
  	glutKeyboardFunc(keyboard);
	glutSpecialFunc(processSpecialKeys);
	glutMainLoop();							// infinite loop that will keep drawing and resizing and whatever else
  
  	return 0;
}