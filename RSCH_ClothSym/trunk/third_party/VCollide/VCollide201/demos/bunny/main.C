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
#include "polyObject.H"

#ifdef OGL_GRAPHICS
#include "graphics.H"
#endif

#define M_PI 3.14159265358979323846

polyObject *list=NULL;
Polytope *polytope;
long num_of_polytopes=0;
VCollide vc;
int WallDist;


//compute a simple collision response.
void ComputeResponse(Vector normal, Vector v, Vector *new_v)
{
  Vector v_normal = normal*(v/normal);
  Vector tan = ((normal*v)*normal);
  tan.normalize();
  Vector v_tan = tan*(v/tan);
  
  *new_v = v_normal*(-1) + v_tan;
  
}

//from the list of polyObjects, return a pointer to the element
//with given id.
polyObject *GetObjectWithId(int id)
{
  polyObject *current = list;
  
  while (current->id != id)
    current = current->next;
  
  return current;
  
}


//removes bunnies which have crossed/collided with the y=YMAX wall.
//for each bunny removed, one is added at the y=YMIN wall.
void SinkAndSource(void)
{
  polyObject *current;
  
  while (list != NULL)
    {
      if (list->position[1] >= WallDist)
	{
	  current = list;
	  list = list->next;
	  vc.DeleteObject(current->id);
	  cout<<"deleting id = "<<current->id<<"\n";
	  delete current;
	  
          polyObject *t = new polyObject(polytope);
          t->position[1] = -WallDist + 0.5;
	  vc.NewObject(&(t->id));

	  Polytope *p = t->p;
	  int j;
	  for (j=0; j<p->num_tris; j++)
	    vc.AddTri(p->vertex[(p->tris[j])[0]].v, p->vertex[(p->tris[j])[1]].v, p->vertex[(p->tris[j])[2]].v );
	  
	  vc.EndObject();

	  cout<<"created polytope with id = "<<t->id<<"\n";

	  t->next = list;
	  list = t;
	  num_of_polytopes++;
	}
      else
	{
	  break;
	}
    }
  
  current = list;
  
  if (current != NULL)
    {
      while (current->next != NULL)
	{
	  if (current->next->position[1] >= WallDist)
	    {
	      polyObject *t = current->next;
	      current->next = current->next->next;
	      vc.DeleteObject(t->id);
	      cout<<"deleting id = "<<t->id<<"\n";
	      delete t;
	      
              t = new polyObject(polytope);
	      t->position[1] = -WallDist + 0.5;
	      
	      vc.NewObject(&(t->id));
	      
	      Polytope *p = t->p;
	      int j;
	      for (j=0; j<p->num_tris; j++)
		vc.AddTri(p->vertex[(p->tris[j])[0]].v, p->vertex[(p->tris[j])[1]].v, p->vertex[(p->tris[j])[2]].v );
	      
	      vc.EndObject();
	      
	      cout<<"created polytope with id = "<<t->id<<"\n";
	      
	      t->next = list;
	      list = t;
	      num_of_polytopes++;
	      
	    }
	  else
	    {
	      current = current->next;
	    }
	}
    }
  
}


//test for collisions and report the results. Also provide a 
//simple collision response to colliding objects.
void CollisionTestReportAndRespond(void)
{
    VCReport report;

    vc.Collide( &report );  //perform collision test.
                            //default is VC_FIRST_CONTACT
    int j;

    for (j = 0; j < report.numObjPairs(); j++)
    {
      cout << "Detected collision between objects "
           << report.obj1ID(j) <<" and "<< report.obj2ID(j) <<"\n";
  
      polyObject *p1 = GetObjectWithId(report.obj1ID(j));
      polyObject *p2 = GetObjectWithId(report.obj2ID(j));

      Vector normal;
	
      normal = p1->position - p2->position;
      normal.normalize();
      ComputeResponse(normal, (p1->v), &(p1->v) );
    
      normal = p2->position - p1->position;
      normal.normalize();
      ComputeResponse(normal, (p2->v), &(p2->v) );
      
    }

    SinkAndSource();
}



//update the transformation of library object with given id.
void UpdatePolytope(int id, double trans[4][4])
{
  vc.UpdateTrans(id, trans);
}

//the simulation loop...
void simulation_loop(int *argc, char *argv[])
{
#ifdef OGL_GRAPHICS
  grInit(argc, argv);
#else
  for (;;)
    {
      static int step=0;
      step++;

      if (step == 3000)
	exit(0);

      cout<<"Step no: "<<step<<"\n";
      for (polyObject *curr=list; curr != NULL; curr = curr->next)
	{
	  double ogl_trans[16];
	  curr->UpdateOneStep(ogl_trans);

	  double trans[4][4];

	  for (int i=0; i<4; i++)
	    for (int j=0; j<4; j++)
	      trans[i][j] = ogl_trans[4*j+i];

	  UpdatePolytope(curr->id, trans);
	}
      CollisionTestReportAndRespond();

    }
#endif
}

int main(int argc, char *argv[])
{
    
  if (argc != 3)
    {
      fprintf(stderr, "USAGE: %s <bunny-file> <number-of-instances>\n", argv[0]);
      exit(1);
    }

  srand(32487723);
  
  polytope = new Polytope(argv[1]);

  int no_of_instances = atoi(argv[2]);
  
  WallDist = (int) pow(no_of_instances*100.0, 0.33) * 100; //maintains constant density.
  for (int i=0; i<no_of_instances; i++)
    {
      polyObject *t = new polyObject(polytope);
      
      vc.NewObject(&(t->id));

      Polytope *p = t->p;
      for (int j=0; j<p->num_tris; j++)
	vc.AddTri(p->vertex[(p->tris[j])[0]].v, p->vertex[(p->tris[j])[1]].v, p->vertex[(p->tris[j])[2]].v );

      vc.EndObject();

      cout<<"created polytope with id = "<<t->id<<"\n";

      t->next = list;
      list = t;
      num_of_polytopes++;
    }
  

  simulation_loop(&argc, argv);

  return 0;
}
