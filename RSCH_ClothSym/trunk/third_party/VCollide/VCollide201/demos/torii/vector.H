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
#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>

#define EPS 0.0001

class Vector{
public:
  double v[3];

  Vector()
    {
    }
  
  
  Vector(double v0, double v1, double v2)
    {
      v[0] = v0;
      v[1] = v1;
      v[2] = v2;
    }
  
  
  Vector(const Vector& p)
    {
      v[0] = p.v[0];
      v[1] = p.v[1];
      v[2] = p.v[2];
    }

  Vector& operator=(const Vector &rhs)
    {
      v[0] = rhs.v[0];
      v[1] = rhs.v[1];
      v[2] = rhs.v[2];

      return *this;
    }

  double& operator[](int i)
    {
      return v[i];
    }

  Vector operator+(const Vector& b)
    {
      Vector sum;

      sum[0] = v[0]+b.v[0];
      sum[1] = v[1]+b.v[1];
      sum[2] = v[2]+b.v[2];

      return sum;
    }

  Vector operator-(const Vector& b)
    {
      Vector diff;

      diff[0] = v[0]-b.v[0];
      diff[1] = v[1]-b.v[1];
      diff[2] = v[2]-b.v[2];

      return diff;

    }
  

  Vector operator*(double term)
  {
    Vector result;
    
    result[0] = v[0] * term;
    result[1] = v[1] * term;
    result[2] = v[2] * term;
    
    return result;
    
  }
  

  Vector operator/(double denom)
    {
      Vector result;

      result[0] = v[0] / denom;
      result[1] = v[1] / denom;
      result[2] = v[2] / denom;

      return result;

    }

  
  Vector operator*(const Vector& b) /* cross product */
    {
      Vector result;
    
      result[0] = v[1]*b.v[2] - b.v[1]*v[2];
      result[1] = b.v[0]*v[2] - v[0]*b.v[2];
      result[2] = v[0]*b.v[1] - b.v[0]*v[1];
    
      return result;

    }

  double operator/(const Vector& b) /* dot product */
    {
      double result;

      result = v[0]*b.v[0] + v[1]*b.v[1] + v[2]*b.v[2];
      return result;
    }

  void normalize(void)
    {
      
      double length = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
      
      if (length > EPS)
	{
	  v[0] /= length;
	  v[1] /= length;
	  v[2] /= length;
	}
      
    }

  double length()
    {
      return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    }
  
};

#endif
