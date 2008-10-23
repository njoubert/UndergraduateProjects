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

#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef OGL_GRAPHICS
#include <GL/glut.h>
#endif

#include "polytope.H"

//constructor: reads in the geometry from input file.
Polytope::Polytope(char *filename) 
{
  
  FILE *fp;
  
  if ( (fp = fopen(filename, "r")) == NULL )
    {
      fprintf(stderr, "Polytope::Polytope => Error opening %s\n", filename);
      exit(1);
    }
  
  char dummy_str[20];
  
  fscanf(fp, "%s", dummy_str);
  
  fscanf(fp, "%d", &num_vertices);
  fscanf(fp, "%d", &num_tris);
  int dummy_int;
  fscanf(fp, "%d", &dummy_int);
  
  vertex = new Vector[num_vertices];
  tris = new Triangle[num_tris];
  
  int i;
  for (i=0; i<num_vertices; i++)
    fscanf(fp, "%lf %lf %lf", &((vertex[i])[0]), &((vertex[i])[1]), &((vertex[i])[2]) );
  
  
  for (i=0; i<num_tris; i++)
    fscanf(fp, "%d %ld %ld %ld", &dummy_int, &((tris[i])[0]), &((tris[i])[1]), &((tris[i])[2]) );
  
  fclose(fp);
  
}


Polytope::~Polytope()
{

  delete[] vertex;
  delete[] tris;
}


#ifdef OGL_GRAPHICS

//Displays the polytope using OpenGL and the given transformation matrix.
void Polytope::Display(double ogl_trans[])
{
  glPushMatrix();
  glLoadMatrixd(ogl_trans);
  
  glBegin(GL_TRIANGLES);
  
  int i;
  for (i=0; i<num_tris; i++)
    {
      Vector normal = (vertex[(tris[i])[1]] - vertex[(tris[i])[0]]) * (vertex[(tris[i])[2]] - vertex[(tris[i])[0]]);
      normal.normalize();
      
      float n[3];
      n[0] = (float)normal[0];
      n[1] = (float)normal[1];
      n[2] = (float)normal[2];
      
      glNormal3fv(n);
      glVertex3dv( (vertex[(tris[i])[0]]).v );
      glVertex3dv( (vertex[(tris[i])[1]]).v );
      glVertex3dv( (vertex[(tris[i])[2]]).v );
      
    }
  
  glEnd();
  glPopMatrix();
}

#endif
