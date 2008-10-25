#include <GL/glut.h>
#include <iostream>

float zoom = 15.0f;
float rotx = 0;
float roty = 0.001f;
float tx = 0;
float ty = 0;
int lastx=0;
int lasty=0;
unsigned char Buttons[3] = {0};

void display()
{
	
	//****** SET THE SYSTEM UP TO BE A 6X6X18 BOX ********************************//     
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0,10.0,0.0,10.0,0.0,25.0);
	
	//****** CLEAR BUFFERS **************************************************    *****//     
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
	//****** SETUP THE CAMERA ANGLE*********************************************    **// 
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(10.0,-10.0,10.0,0.0,0.0,0.0,0.0,1.0,0.0);
	
	
	//	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
	//	glLoadIdentity();
	
	//	glTranslatef(0,0,-zoom);
	//	glTranslatef(tx,ty,0);
	//	glRotatef(rotx,1,0,0);
	//	glRotatef(roty,0,1,0);	
	
	// draw grid
	glBegin(GL_LINES);
	for(int i=-10;i<=10;++i) {
		glVertex3f(i,0,-10);
		glVertex3f(i,0,10);
		
		glVertex3f(10,0,i);
		glVertex3f(-10,0,i);
	}
	glEnd();
	
	glutSwapBuffers();
}

//-------------------------------------------------------------------------------
/// \brief	Called when the screen gets resized
/// \param	w	-	the new width
/// \param	h	-	the new height
/// 
void reshape(int w, int h)
{
	// prevent divide by 0 error when minimised
	if(w==0) 
		h = 1;
	
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45,(float)w/h,0.1,100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//-------------------------------------------------------------------------------
///
int main(int argc,char** argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowSize(640,480);
	glutInitWindowPosition(100,100);
	glutCreateWindow("TEMPLATE X11R6 OPENGL LIBRARIES");
	
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glEnable(GL_DEPTH_TEST);
	
	glutMainLoop();
}