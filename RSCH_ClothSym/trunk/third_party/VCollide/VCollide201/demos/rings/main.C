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

#include "quat.h"
#include "VCollide.H"
#include "polytope.H"

#ifdef OGL_GRAPHICS
#include "graphics.H"
#endif

Polytope **polytope;
FILE *fp;
int num_of_polytopes;
VCollide vc;


//reads in the transformation and applies it the the corresponding
//object from the library.
void UpdatePolytope(int i, double ogl_trans[])
{
  q_xyz_quat_struct q;
  
  fscanf(fp, "%lf %lf %lf", &(q.xyz[0]), &(q.xyz[1]), &(q.xyz[2]) );
  fscanf(fp, "%lf %lf %lf %lf", &(q.quat[3]), &(q.quat[0]), &(q.quat[1]), &(q.quat[2]) );
  
  q_xyz_quat_to_ogl_matrix(ogl_trans, &q); //convert from quaternion to 
                                           //OpenGL transformation matrix.
  
  double trans[4][4];
  int j,k;
  for (j=0; j<4; j++)
    for (k=0; k<4; k++)
      {
	trans[j][k] = ogl_trans[4*k+j];  //convert the standard notation.
      }
  
  vc.UpdateTrans(polytope[i]->id, trans); //update the transformation of 
                                          //corresponding object from the
                                          //library.
  
}

//test for collisions and report the results.
void CollisionTestAndReport(void)
{
    VCReport report;

    vc.Collide( &report );  //perform collision test.
                            //default is VC_FIRST_CONTACT
    int j;

    for (j = 0; j < report.numObjPairs(); j++)
    {
      cout << "Detected collision between objects "
           << report.obj1ID(j) <<" and "<< report.obj2ID(j) <<"\n";
    }
  
}


//the simulation_loop...
void simulation_loop(int *argc, char *argv[])
{
#ifdef OGL_GRAPHICS
  grInit(argc, argv);
#else
  for(;;)
    {
      static int step=1;
      
      cout<<"step = "<<step<<"\n";
      step++;
      
      char t;
      double time;
      
      fscanf(fp, "%c", &t);
      fscanf(fp, "t %lf", &time);
      
      double ogl_trans[16];
      
      if (time <5.57)
	{
	  int i;
	  for (i=0; i<num_of_polytopes; i++)
	    {
	      UpdatePolytope(i, ogl_trans);
	    }
	  CollisionTestAndReport();
	}
      else
	{
	  break;
	}
      
    }
#endif
}


int main(int argc, char *argv[])
{
  if (argc != 2)
    {
      fprintf(stderr, "USAGE: %s <simulation-file>\n", argv[0]);
      exit(1);
    }

  if ( (fp = fopen(argv[1], "r")) == NULL )
    {
      fprintf(stderr, "%s: Error opening file %s\n", argv[0], argv[1]);
      exit(1);
    }
  
  //read in the geometry from input file.
  fscanf(fp, "%d", &num_of_polytopes);

  polytope = new Polytope*[num_of_polytopes];

  char name[10], filename[20];
  int color_index;
  
  int i;
  for (i=0; i<num_of_polytopes; i++)
    {
      fscanf(fp, "%s", name);
      fscanf(fp, "%s", filename);
      fscanf(fp, "%d", &color_index);

      cout<<"Reading in polytope from file "<<filename<<"\n";

      polytope[i] = new Polytope(filename);
    }


  //add the object to the library.
  for (i=0; i<num_of_polytopes; i++)
    {
      Polytope *p = polytope[i];

      vc.NewObject(&(p->id));
      
      int j;
      for (j=0; j<p->num_tris; j++)
	vc.AddTri(p->vertex[(p->tris[j])[0]].v, p->vertex[(p->tris[j])[1]].v, p->vertex[(p->tris[j])[2]].v );
      
      vc.EndObject();
    }
  
  //perform the simulation.
  simulation_loop(&argc, argv);

  return 0;
}
