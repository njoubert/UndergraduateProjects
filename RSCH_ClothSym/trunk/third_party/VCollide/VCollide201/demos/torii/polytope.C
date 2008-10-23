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

Polytope::Polytope() : id(0), num_tris(0), tris(NULL)
{
}
  

Polytope::~Polytope()
{
  delete[] tris;
}


//read in the polytope from an input file.
void Polytope::Read(char *filename)
{
  FILE *fp;
  
  if ( (fp = fopen(filename, "r")) == NULL )
    {
      fprintf(stderr, "Polytope::Read => Error opening %s\n", filename);
      exit(1);
    }
  
  fscanf(fp, "%d", &num_tris);
  
  tris = new Triangle[num_tris];
  
  int i;
  for (i=0; i<num_tris; i++)
    {
      fscanf(fp, "%lf %lf %lf", &((tris[i].p[0])[0]), &((tris[i].p[0])[1]), &((tris[i].p[0])[2]));
      fscanf(fp, "%lf %lf %lf", &((tris[i].p[1])[0]), &((tris[i].p[1])[1]), &((tris[i].p[1])[2]));
      fscanf(fp, "%lf %lf %lf", &((tris[i].p[2])[0]), &((tris[i].p[2])[1]), &((tris[i].p[2])[2]));
      fscanf(fp, "%lf %lf %lf", &(tris[i].normal[0]), &(tris[i].normal[1]), &(tris[i].normal[2]));
    }
  
  fclose(fp);
  
}




#ifdef OGL_GRAPHICS
//display the polytope using OpenGL and the given transformation matrix.
void Polytope::Display(double ogl_trans[])
{
  glPushMatrix();
  glLoadMatrixd(ogl_trans);
  
  glBegin(GL_TRIANGLES);
  
  int i;
  for (i=0; i<num_tris; i++)
    {
      glNormal3dv( tris[i].normal.v );
      glVertex3dv( tris[i].p[0].v );
      glVertex3dv( tris[i].p[1].v );
      glVertex3dv( tris[i].p[2].v );
      
    }
  
  glEnd();
  
  glPopMatrix();
  
}
#endif
