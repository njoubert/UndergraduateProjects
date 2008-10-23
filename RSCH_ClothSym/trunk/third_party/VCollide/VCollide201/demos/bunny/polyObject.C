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

#include <stdlib.h>
#include "polyObject.H"

#define M_PI 3.14159265358979323846

extern int WallDist;


inline double RAD2DEG(double x)
  {
    return (180.0*(x) / M_PI);
  }



/* 
 * This function has been borrowed from OpenGL source code.
 * Generate a 4x4 transformation matrix from glRotate parameters.
 */
void Rotated( double angle, double x, double y, double z, double m[] )
{
  double mag, s, c;
  double xx, yy, zz, xy, yz, zx, xs, ys, zs, one_c;

  s = sin( angle * (M_PI / 180.0) );
  c = cos( angle * (M_PI / 180.0) );

  mag = sqrt( x*x + y*y + z*z );

  if (mag == 0.0) {
    /* generate an identity matrix and return */
    for (int i=0; i<4; i++)
      for (int j=0; j<4; j++)
	m[4*j+i] = ( (i==j) ? 1.0 : 0.0);
    
    return;
  }
  
  x /= mag;
  y /= mag;
  z /= mag;

#define M(row,col)  m[col*4+row]

  xx = x * x;
  yy = y * y;
  zz = z * z;
  xy = x * y;
  yz = y * z;
  zx = z * x;
  xs = x * s;
  ys = y * s;
  zs = z * s;
  one_c = 1.0F - c;
  
  M(0,0) = (one_c * xx) + c;
  M(0,1) = (one_c * xy) - zs;
  M(0,2) = (one_c * zx) + ys;
  M(0,3) = 0.0F;
  
  M(1,0) = (one_c * xy) + zs;
  M(1,1) = (one_c * yy) + c;
  M(1,2) = (one_c * yz) - xs;
  M(1,3) = 0.0F;
  
  M(2,0) = (one_c * zx) - ys;
  M(2,1) = (one_c * yz) + xs;
  M(2,2) = (one_c * zz) + c;
  M(2,3) = 0.0F;
  
  M(3,0) = 0.0F;
  M(3,1) = 0.0F;
  M(3,2) = 0.0F;
  M(3,3) = 1.0F;
  
#undef M
}

polyObject::polyObject(Polytope *polytope)
{
  
  position[0] = (((double)rand())/RAND_MAX - 0.5) * 2.0 * WallDist;
  position[1] = (((double)rand())/RAND_MAX - 0.5) * 2.0 * WallDist;
  position[2] = (((double)rand())/RAND_MAX - 0.5) * 2.0 * WallDist;
  
  orientation[0] = orientation[1] = orientation[2] = 0.0;
  
  v[0] = ((double)rand())/RAND_MAX * 5.0;
  v[1] = ((double)rand())/RAND_MAX * 5.0;
  v[2] = ((double)rand())/RAND_MAX * 5.0;
  
  omega[0] = ((double)rand())/RAND_MAX / 4.0;
  omega[1] = ((double)rand())/RAND_MAX / 4.0;
  omega[2] = ((double)rand())/RAND_MAX / 4.0;
  
  p = polytope;
  
}


polyObject::~polyObject()
{
  p=NULL;
}


void polyObject::UpdateOneStep(double ogl_trans[16])
{
  
  position  = position + v;
  orientation = orientation + omega;
  
  
  if ( (position[0] > WallDist) && (v[0] > 0) )
    {
      v[0] = -v[0];
    }
  if ( (position[0] < -WallDist) && (v[0] < 0) )
    {
      v[0] = -v[0];
    }
  
  if ( (position[1] > WallDist) && (v[1] > 0) )
    {
      v[1] = -v[1];
    }
  if ( (position[1] < -WallDist) && (v[1] < 0) )
    {
      v[1] = -v[1];
    }
  
  if ( (position[2] > WallDist) && (v[2] > 0) )
    {
      v[2] = -v[2];
    }
  if ( (position[2] < -WallDist) && (v[2] < 0) )
    {
      v[2] = -v[2];
    }
  
  
  double theta_len = orientation.length();
  Vector theta = orientation;
  theta.normalize();
  
  Rotated(RAD2DEG(theta_len), theta[0], theta[1], theta[2], ogl_trans);
  ogl_trans[12] = position[0];
  ogl_trans[13] = position[1];
  ogl_trans[14] = position[2];
  
}

