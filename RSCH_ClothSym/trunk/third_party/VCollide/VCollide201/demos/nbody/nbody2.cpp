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
#include "VCollide.H"

const int NO_OF_OBJECTS=20;      //number of instances
const int SIMULATION_STEPS=100;  //number of steps in the simulation.

int main(int argc, char *argv[])
{

  if (argc != 3)
    {
      cerr<<argv[0]<<": USAGE: "<<argv[0]<<" <input-file> <transformation-file>\n";
      exit(1);
    }
  
  int num_tri;
  VCollide vc;
  int id[NO_OF_OBJECTS];
  
  int i;
  for (i=0; i<NO_OF_OBJECTS; i++)  //add the objects to the library.
    {
      cout<<"Reading object "<<i<<"\n";
      vc.NewObject(&(id[i]));
      cout<<"Adding triangles\n";
      FILE *fp = fopen(argv[1], "r");
      fscanf(fp, "%d", &num_tri);
      
      for (int j=1; j<=num_tri; j++)
	{
	  double v1[3], v2[3], v3[3];
	  fscanf(fp, "%lf %lf %lf", &(v1[0]), &(v1[1]), &(v1[2]));
	  fscanf(fp, "%lf %lf %lf", &(v2[0]), &(v2[1]), &(v2[2]));
	  fscanf(fp, "%lf %lf %lf", &(v3[0]), &(v3[1]), &(v3[2]));
	  
	  vc.AddTri(v1, v2, v3, j);  // Each triangle has an id
	}
      
      fclose(fp);
      
      cout<<"Calling finish_object\n";
      vc.EndObject();
      
      
      cout<<"Inserted object "<<i<<"\n";
    }
  
  
  FILE *fp = fopen(argv[2], "r");
  
  for (i=1; i<=SIMULATION_STEPS; i++)  //perform the simulation.
    {
      cout<<"Simulation step : "<<i<<"\n";
      int j;
      for (j=0; j<NO_OF_OBJECTS; j++)
	{
	  double trans[4][4];
	  
	  //read in the transformation matrix.
	  fscanf(fp, "%lf", &(trans[0][0]));
	  fscanf(fp, "%lf", &(trans[0][1]));
	  fscanf(fp, "%lf", &(trans[0][2]));
	  fscanf(fp, "%lf", &(trans[0][3]));
	  
	  fscanf(fp, "%lf", &(trans[1][0]));
	  fscanf(fp, "%lf", &(trans[1][1]));
	  fscanf(fp, "%lf", &(trans[1][2]));
	  fscanf(fp, "%lf", &(trans[1][3]));
	  
	  fscanf(fp, "%lf", &(trans[2][0]));
	  fscanf(fp, "%lf", &(trans[2][1]));
	  fscanf(fp, "%lf", &(trans[2][2]));
	  fscanf(fp, "%lf", &(trans[2][3]));
	  
	  fscanf(fp, "%lf", &(trans[3][0]));
	  fscanf(fp, "%lf", &(trans[3][1]));
	  fscanf(fp, "%lf", &(trans[3][2]));
	  fscanf(fp, "%lf", &(trans[3][3]));
	  
	  //update the object's transformation.
	  vc.UpdateTrans(id[j], trans);
	}


      VCReport report;
            
      vc.Collide( &report, VC_ALL_CONTACTS);  //perform collision test.
                                              //default is VC_FIRST_CONTACT
      
      
      for (j = 0; j < report.numObjPairs(); j++)
      {
	cout << "Detected collision between objects "
             << report.obj1ID(j) << " and " << report.obj2ID(j) << "\n";

	cout << "\tNumber of contacts = " << report.numTriPairs(j) << "\n";
        cout << "\tColliding triangle-pairs: ";
	
        int k;
        for ( k = 0; k < report.numTriPairs(j); k++ )
          cout << "[" << report.tri1ID(j, k) << "," << report.tri2ID(j, k) << "] ";

        cout << "\n";
      }

    }

  return 0;
}
