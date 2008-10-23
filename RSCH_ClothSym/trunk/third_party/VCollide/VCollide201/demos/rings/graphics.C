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

#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include "graphics.H"
#include "polytope.H"

const GLfloat sph_specular[] = {0.7, 0.4, 0.7, 1.0};
const GLfloat sph_diffuse[] = {0.3, 0.0, 0.3, 1.0};
const GLfloat sph_shininess[] = {50.0};

extern FILE *fp;
extern int num_of_polytopes;
extern Polytope **polytope;
extern void UpdatePolytope(int i, double ogl_trans[]);
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

void DisplayFunc(void)
{
  static int done=0;
  static int step=1;
  
  if (!done)
    {
      glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
      
      
      char t;
      double time;
      
      fscanf(fp, "%c", &t);
      fscanf(fp, "t %lf", &time);
      
      double ogl_trans[16];
      
      if (time <5.57)
	{
	  cout<<"step = "<<step<<"\n";
      step++;
	  
	  int i;
	  for (i=0; i<num_of_polytopes; i++)
	    {
	      UpdatePolytope(i, ogl_trans);
	      polytope[i]->Display(ogl_trans);
	    }
	  
	  glutSwapBuffers();
	  CollisionTestAndReport();
	}
      else
	{
	  done=1;
	}
      
    }
}

void grInit(int *argc, char **argv)
{

  glutInit(argc, argv);
  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
  glutInitWindowSize( WIDTH, HEIGHT);
  
  glutCreateWindow( "Rings simulation" );
  
  glEnable(GL_DEPTH_TEST);
  //glEnable(GL_CULL_FACE);
  //glCullFace(GL_BACK);
  glDisable(GL_DITHER);
  glClearColor( 0.0, 0.0, 0.0, 0.0 );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glutSwapBuffers();
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glutSwapBuffers();
  
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
  gluLookAt(15, 0, -5, 0, 0, -5, 0, 0, 1);
  glMatrixMode( GL_MODELVIEW );

   
  glMaterialfv(GL_FRONT, GL_SPECULAR, sph_specular);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, sph_diffuse);
  glMaterialfv(GL_FRONT, GL_SHININESS, sph_shininess);
  
  glutKeyboardFunc(KeyboardFunc);
  glutDisplayFunc(DisplayFunc);
  glutIdleFunc(DisplayFunc);
  
  glutMainLoop();
  
}

#endif
