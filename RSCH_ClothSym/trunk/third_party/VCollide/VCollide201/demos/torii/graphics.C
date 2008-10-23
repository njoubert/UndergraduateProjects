/************************************************************************\

  Copyright 1997 The University of North Carolina at Chapel Hill.
  All Rights Reserved.

  Permission to use, copy, modify and distribute this software
  and its documentation for educational, research and non-profit
  purposes, without fee, and without a written agreement is
  hereby granted, provided that the above copyright notice and
  the following three paragraphs appear in all copies.

  IN NO EVENT SHALL THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL
  HILL BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL,
  INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS,
  ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
  EVEN IF THE UNIVERSITY OF NORTH CAROLINA HAVE BEEN ADVISED OF
  THE POSSIBILITY OF SUCH DAMAGES.


  Permission to use, copy, modify and distribute this software
  and its documentation for educational, research and non-profit
  purposes, without fee, and without a written agreement is
  hereby granted, provided that the above copyright notice and
  the following three paragraphs appear in all copies.

  THE UNIVERSITY OF NORTH CAROLINA SPECIFICALLY DISCLAIM ANY
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
  BASIS, AND THE UNIVERSITY OF NORTH CAROLINA HAS NO OBLIGATION
  TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
  MODIFICATIONS.


   --------------------------------- 
  |Please send all BUG REPORTS to:  |
  |                                 |
  |   geom@cs.unc.edu               |
  |                                 |
   ---------------------------------
  
     
  The authors may be contacted via:

  US Mail:  A. Pattekar/J. Cohen/T. Hudson/S. Gottschalk/M. Lin/D. Manocha
            Department of Computer Science
            Sitterson Hall, CB #3175
            University of N. Carolina
            Chapel Hill, NC 27599-3175
	    
  Phone:    (919)962-1749
	    
  EMail:    geom@cs.unc.edu

\************************************************************************/
#ifdef OGL_GRAPHICS

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include "graphics.H"
#include "polytope.H"

const GLfloat sph_specular[] = {0.7, 0.4, 0.7, 1.0};
const GLfloat sph_diffuse[] = {0.3, 0.0, 0.3, 1.0};
const GLfloat sph_shininess[] = {50.0};
      

extern FILE *fp[];
extern Polytope polytope[];
extern int number_of_steps;
extern void UpdatePolytope(int i, double trans[4][4]);
extern void CollisionTestAndReport(void);

void KeyboardFunc(unsigned char key, int x, int y)
{
  
  switch (key)
    {
    case 'q':
      {
        exit(1);
        break;
      }
    default:
      {
        fprintf(stderr, "Unknown key-press: --%c--\n", key);
        break;
      }
    }
}


//the display function and idle function used by the event loop.
void DisplayFunc(void)
{
  static int step=0;
  step++;
  

  if (step < number_of_steps)
    {
      cout<<"Step no: "<<step<<"\n";
      glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
      
      int i;
      for (i=0; i<2; i++)
	{
	  double trans[4][4];
	  
	  UpdatePolytope(i, trans); //read in the transformation matrix
                                    //and apply it to the corresponding
                                    //object in the library.
	  
	  double ogl_trans[16];
	  int j,k;
	  for (j=0; j<4; j++)
	    for (k=0; k<4; k++)
	      ogl_trans[4*k+j] = trans[j][k];
	  polytope[i].Display(ogl_trans); //display the polytope.
	}
      glutSwapBuffers();
      CollisionTestAndReport();  //test for collisions and report results.
      
    }
  else
    {
      step--;
    }
}


//initializes OpenGL and enters the event loop.
void grInit(int *argc, char **argv)
{

  glutInit(argc, argv);
  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
  glutInitWindowSize( WIDTH, HEIGHT);
  
  glutCreateWindow( "Torii simulation" );
  
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glDisable(GL_DITHER);
  glClearColor( 0.0, 0.0, 0.0, 0.0 );
    
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  GLfloat light_position[] = {15.0, 0.0, -5.0, 1.0};
  
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 0.0);
  glPopMatrix();
  
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  
  
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity( );
  gluPerspective(60.0, ((float)(WIDTH)) / ((float)(HEIGHT)), 0.1, 100.0);
  gluLookAt(6, 0, 0, 0, 0, 0, 0, 0, 1);
  glMatrixMode( GL_MODELVIEW );

  glMaterialfv(GL_FRONT, GL_SPECULAR, sph_specular);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, sph_diffuse);
  glMaterialfv(GL_FRONT, GL_SHININESS, sph_shininess);
  
  glutDisplayFunc(DisplayFunc);
  glutIdleFunc(DisplayFunc);
  glutKeyboardFunc(KeyboardFunc);

  glutMainLoop();
  
}

#endif
