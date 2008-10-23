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

#include <stdio.h>
#include <stdlib.h>
#include "VCollide.H"
#include "polytope.H"

#ifdef OGL_GRAPHICS
#include "graphics.H"
#endif

VCollide vc;
Polytope polytope[2];
FILE *fp[2];
int number_of_steps;


//Reads in the transformation matrix and applies it to the corresponding
//polytope in the library.
void UpdatePolytope(int i, double trans[4][4])
{
  
  fscanf(fp[i], "%lf %lf %lf", &(trans[0][0]), &(trans[0][1]), &(trans[0][2]));
  fscanf(fp[i], "%lf %lf %lf", &(trans[1][0]), &(trans[1][1]), &(trans[1][2]));
  fscanf(fp[i], "%lf %lf %lf", &(trans[2][0]), &(trans[2][1]), &(trans[2][2]));
  fscanf(fp[i], "%lf %lf %lf", &(trans[0][3]), &(trans[1][3]), &(trans[2][3]));
  
  trans[3][0] = 0.0;
  trans[3][1] = 0.0;
  trans[3][2] = 0.0;
  trans[3][3] = 1.0;
  
  vc.UpdateTrans(polytope[i].id, trans); //apply the transformation to the
                                         //corresponding library object.
}


//Perform collision test and report the results.
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

//runs the simulation.
void simulation_loop(int *argc, char *argv[])
{
#ifdef OGL_GRAPHICS
  grInit(argc, argv);
#else
  for (;;)
    {
      static int step=0;
      step++;
      
      if (step < number_of_steps)
	{
	  cout<<"Step no: "<<step<<"\n";
	  int i;
	  for (i=0; i<2; i++)
	    {
	      double trans[4][4];
	      UpdatePolytope(i, trans); //read in the transformation matrix
                                        //and update the polytope's transformation.
	    }
	  CollisionTestAndReport();  //test collisions and report the results.
	  
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
  
  if (argc != 5)
    {
      fprintf(stderr, "USAGE: %s <psoup1> <path1> <psoup2> <path2>\n", argv[0]);
      exit(1);
    }
  
  int i;
  for (i=0; i<2; i++)
    {
      polytope[i].Read(argv[2*i+1]); //read in the geometry from input file.
      
      //add the object to the library.
      vc.NewObject(&(polytope[i].id));
      
      int j;
      for (j=0; j<polytope[i].num_tris; j++)
	vc.AddTri(polytope[i].tris[j].p[0].v, polytope[i].tris[j].p[1].v, polytope[i].tris[j].p[2].v);
      
      vc.EndObject();
      
      if ( (fp[i] = fopen(argv[2*i+2], "r")) == NULL )
	{
	  fprintf(stderr, "%s: Error opening file %s\n", argv[i], argv[2*i+2]);
	  exit(1);
	}
      
    }
  
  for (i=0; i<2; i++)
    fscanf(fp[i], "%d", &number_of_steps);
  
  simulation_loop(&argc, argv);  //perform the simulation.
  
  return 0;
}


