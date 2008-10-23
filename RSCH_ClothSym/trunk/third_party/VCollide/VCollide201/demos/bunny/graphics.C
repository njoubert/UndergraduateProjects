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

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include "polytope.H"
#include "polyObject.H"
#include "graphics.H"

extern polyObject *list;
extern int WallDist;

extern void UpdatePolytope(int id, double trans[4][4]);
extern void CollisionTestReportAndRespond(void);

const int WIDTH=600;
const int HEIGHT=600;

const GLfloat cube_specular[] = {0.0, 0.0, 0.0, 1.0};
const GLfloat cube_diffuse[] = {0.7, 0.4, 0.4, 1.0};
const GLfloat cube_shininess[] = {0.0};
  
const GLfloat bunny_specular[] = {0.7, 0.4, 0.7, 1.0};
const GLfloat bunny_diffuse[] = {0.3, 0.0, 0.3, 1.0};
const GLfloat bunny_shininess[] = {50.0};
  
void DrawCube(void)
{
  
  float normal[3];
  
  
  glBegin(GL_QUADS);
  
  normal[0] = 0.0;
  normal[1] = 1.0;
  normal[2] = 0.0;
  glNormal3fv(normal);
  glVertex3i(WallDist, -WallDist, -WallDist);
  glVertex3i(-WallDist, -WallDist, -WallDist);
  glVertex3i(-WallDist, -WallDist, WallDist);
  glVertex3i(WallDist, -WallDist, WallDist);  

  normal[0] = 1.0;
  normal[1] = 0.0;
  normal[2] = 0.0;
  glNormal3fv(normal);
  glVertex3i(-WallDist, -WallDist, -WallDist);
  glVertex3i(-WallDist, WallDist, -WallDist);
  glVertex3i(-WallDist, WallDist, WallDist);
  glVertex3i(-WallDist, -WallDist, WallDist);
  
  normal[0] = 0.0;
  normal[1] = -1.0;
  normal[2] = 0.0;
  glNormal3fv(normal);
  glVertex3i(-WallDist, WallDist, -WallDist);
  glVertex3i(WallDist, WallDist, -WallDist);
  glVertex3i(WallDist, WallDist, WallDist);
  glVertex3i(-WallDist, WallDist, WallDist);
  
  normal[0] = 0.0;
  normal[1] = 0.0;
  normal[2] = 1.0;
  glNormal3fv(normal);
  glVertex3i(-WallDist, -WallDist, -WallDist);
  glVertex3i(WallDist, -WallDist, -WallDist);
  glVertex3i(WallDist, WallDist, -WallDist);
  glVertex3i(-WallDist, WallDist, -WallDist);

  normal[0] = 0.0;
  normal[1] = 0.0;
  normal[2] = -1.0;
  glNormal3fv(normal);
  glVertex3i(-WallDist, -WallDist, WallDist);
  glVertex3i(-WallDist, WallDist, WallDist);
  glVertex3i(WallDist, WallDist, WallDist);
  glVertex3i(WallDist, -WallDist, WallDist);
  
  glEnd();
}


void DisplayFunc(void)
{
  static int step=0;
  step++;

  cout<<"Step no: "<<step<<"\n";
  
  glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );


  glMaterialfv(GL_FRONT, GL_SPECULAR, cube_specular);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, cube_diffuse);
  glMaterialfv(GL_FRONT, GL_SHININESS, cube_shininess);
  DrawCube();

  glMaterialfv(GL_FRONT, GL_SPECULAR, bunny_specular);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, bunny_diffuse);
  glMaterialfv(GL_FRONT, GL_SHININESS, bunny_shininess);
  
  polyObject *curr;
  for (curr=list; curr != NULL; curr = curr->next)
    {
      double ogl_trans[16];
      curr->UpdateOneStep(ogl_trans);

      double trans[4][4];
      
      int i,j;
      for (i=0; i<4; i++)
	for (j=0; j<4; j++)
	  trans[i][j] = ogl_trans[4*j+i];
      
      UpdatePolytope(curr->id, trans);
      curr->p->Display(ogl_trans);
    }
  
  glutSwapBuffers();
  CollisionTestReportAndRespond();
  
}

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

void grInit(int *argc, char **argv)
{

  glutInit(argc, argv);
  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
  glutInitWindowSize( WIDTH, HEIGHT);
  
  glutCreateWindow( "Bunny simulation" );
  
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glDisable(GL_DITHER);
  glClearColor( 0.0, 0.0, 0.0, 0.0 );
  
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  GLfloat light_position[] = {2.732*WallDist, 0.0, 0.0, 1.0};
  
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 0.0);
  glPopMatrix();
  
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  
  
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity( );
  gluPerspective(60.0, ((float)(WIDTH)) / ((float)(HEIGHT)), 0.1, 10000.0);
  gluLookAt(2.732*WallDist, 0, 0, 0, 0, 0, 0, 0, 1);
  glMatrixMode( GL_MODELVIEW );
  
  glutDisplayFunc(DisplayFunc);
  glutIdleFunc(DisplayFunc);
  glutKeyboardFunc(KeyboardFunc);
  
  glutMainLoop();
  
}

#endif
