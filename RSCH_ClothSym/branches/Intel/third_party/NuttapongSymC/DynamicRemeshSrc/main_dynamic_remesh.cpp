#define DYNAMIC_REMESH 

#include <stdlib.h>

#include "dsyevh3.h"
#include "FEM.h"
#include <iostream>
#include <fstream>

// Main with graphics
// Design to work with either win32 or MacOSX XCode
#include "GL/glut.h"

#include "font.h"
#include "myTimer.h"
#include "topology.h"
//#include <process.h>
//#include <windows.h>
using namespace std;
const float fov = 45;

unsigned int g_persp=0,g_ortho=0;

CConfig config;


#include "myTimer.h"

CFEMMesh* mesh;

#ifdef DYNAMIC_REMESH
CFEMMesh* nextMesh;

#endif

double fps = 0.0f;
int width, height;

// Viewing parameters
VEC3 viewFrom(0,1,5), 
    viewLookAt(0,0,0), 
    viewUp(0,1,0);


extern double modelViewMat[16], projectionMat[16];
extern int viewport[4];


void StepSimulation() {
   static int oldTime = 0;
   int time = glutGet(GLUT_ELAPSED_TIME);
   real elapsed = (time - oldTime) * 0.001;
   fps = 1.0 / elapsed;
/*
   if (oldTime == 0) {
      // First time step
      mesh->extForce.push_back(CExternalForce(69, VEC3(-0.4,0,0), 0.1));
   }*/
   oldTime = time;

   mesh->Simulate(config.dt );
}
//------------------------------------------------------------	OnReshape()
//
// This function is called when the window gets resized. It
// allows us to set up the camera projection for the scene.
//
void OnReshape(int w, int h)
{
   width = w;
   height = h;
		// delete the last display lists
	glDeleteLists(g_persp,1);
	glDeleteLists(g_ortho,1);

	// set the drawable region of the window
	glViewport(0,0,w,h);
   glGetIntegerv(GL_VIEWPORT, viewport);

	// generate new display list ID's
	g_persp = glGenLists(1);
	g_ortho = glGenLists(1);

	// generate perspective display list
	glNewList(g_persp,GL_COMPILE);

		// set up the projection matrix 
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		// just use a perspective projection
		gluPerspective(fov,(float)w/h,0.1,100);

		// go back to modelview matrix so we can move the objects about
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

	glEndList();


	// generate orthographic display list
	glNewList(g_ortho,GL_COMPILE);

		// set up the projection matrix 
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		// just use a orthographic projection
		glOrtho(0,w,h,0,-100,100);

		// go back to modelview matrix so we can move the objects about
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

	glEndList();


}



void OnDraw() {


	
	static int frame = 0;
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
	
	// clear the previous transform
	glCallList(g_ortho);	
	glDisable(GL_LIGHTING);
	glColor3f(1.0f,1.0f, 1.0f);
	char str[200];
   sprintf(str, "%0.3g fps", fps); 
   FontDraw(str, 20, 20);

	// set the perspective projection
	glCallList(g_persp);
	
	gluLookAt(viewFrom[0], viewFrom[1], viewFrom[2], viewLookAt[0], viewLookAt[1], viewLookAt[2], viewUp[0], viewUp[1], viewUp[2]);


   // Optionally do remeshing
   #ifdef DYNAMIC_REMESH

      // --------------------- My dummy remeshing that does not really do remeshing ----------------------------------
      // !Look here BryanK!
      // Below should be replaced by your remesher!
      // ******
         vector<VEC3> worldPos = mesh->worldPos; // Copy the world position from old mesh
         vector<CTet> tet2node = mesh->tet2node; // Copy tet2node from old mesh                                 
         vector<int> groups = mesh->groups; // Copy which group a tet is in from old mesh
         // Compute the list of nodes that needed to be constrainted. 
         vector<int> fixPosNodes;
         for (int i = 0; i < mesh->permVconst.size(); i++) {
            fixPosNodes.push_back(mesh->permVconst[i].nodeNum);
         }
/*         for (int i = 0; i < tet2node.size(); i++) {
            swap(tet2node[i], tet2node[rand() % tet2node.size()]);
         }*/

      // ******

      int numNodes = worldPos.size();
      int numTets = tet2node.size();

      // Guess which tet a node is in (For most nodes, you know exactly what tet contain it, for those that you don't know, make a good guess);
      vector<int> guessTet(numNodes); 
      for (int i = 0; i < tet2node.size(); i++) {
         CTet& t2n = tet2node[i];
         guessTet[t2n[0]] = i;
         guessTet[t2n[1]] = i;
         guessTet[t2n[2]] = i;
         guessTet[t2n[3]] = i;
      }
#ifdef USE_MATPOS
      vector<VEC3> matPos(numNodes);
#else
      vector<MATRIX3> baryMat(numTets);


      vector<VEC3> matPos(numNodes);
      vector<VEC3> velocity(numNodes);
      vector<VEC3> acceleration(numNodes);
/*
<<<<<<< .mine
      for (int i = 0; i < numNodes; i++) {
         cout<<worldPos[i][0]<<" -- "<<worldPos[i][1]<<" -- "<<worldPos[i][2]<<" XX "<<mesh->worldPos[i][0]<<" -- "<<mesh->worldPos[i][1]<<" -- "<<mesh->worldPos[i][2]<<endl;
      }
*/
      // Now figure out ve
      for (int i = 0; i < numNodes; i++) {         

         #ifdef USE_MATPOS
            // Now, obtain velocity and acceleration
            guessTet[i] = mesh->topology->ComputeMatPosGuess(worldPos[i], guessTet[i], matPos[i]);
            mesh->topology->ComputeWorldPosVelAccKnow(matPos[i], guessTet[i], worldPos[i], velocity[i], acceleration[i]);
         #else
            guessTet[i] = mesh->topology->PointLocationAnyCoord(mesh->worldPos, worldPos[i], guessTet[i]);

            // Obtain velocity and acceleration by barycentric interpolation
            real b0, b1, b2, b3;
            mesh->topology->BaryCoordsAnyCoord(mesh->worldPos, worldPos[i], guessTet[i], b0, b1, b2, b3);
            CTet& mt = mesh->tet2node[guessTet[i]];
            velocity[i] = b0*mesh->velocity[mt[0]] +
                          b1*mesh->velocity[mt[1]] +
                          b2*mesh->velocity[mt[2]] +
                          b3*mesh->velocity[mt[3]];
            acceleration[i] = b0*mesh->acceleration[mt[0]] +
                              b1*mesh->acceleration[mt[1]] +
                              b2*mesh->acceleration[mt[2]] +
                              b3*mesh->acceleration[mt[3]];
         #endif
      }
/*
      cout<<"AASDDDDDDDDDDDD"<<endl;
      for (int i = 0; i < numNodes; i++) {
         cout<<worldPos[i][0]<<" -- "<<worldPos[i][1]<<" -- "<<worldPos[i][2]<<" XX "<<mesh->worldPos[i][0]<<" -- "<<mesh->worldPos[i][1]<<" -- "<<mesh->worldPos[i][2]<<endl;
      }
      */
      // Interpolate Green Strain and compute deformation gradient 
#ifndef USE_MATPOS
      
      // NEED OPTIMIZATION
      // Need to recompute green strain because the old one is out of date after one time step!!!
      mesh->ComputeGreenStrains(); 
      
      static int ccc = 0;
      if (ccc == 1) {
         cout<<"1"<<endl;
      }
      vector<pair<int, real> > tweight;
      for (int t = 0; t < numTets; t++) {
         //cout<<t<<" "<<numTets<<endl;
         CTet& tet = tet2node[t];
         mesh->topology->GetTetWeights(worldPos[tet[0]], worldPos[tet[1]], worldPos[tet[2]], worldPos[tet[3]], min(t, mesh->numTets - 1), tweight);
         MATRIX3 mat;
         for (int i = 0; i < tweight.size(); i++) {
            mat += tweight[i].second*mesh->greenStrain[tweight[i].first];
         }
        
         //mat = mesh->greenStrain[t];
         mat *= 2;
         mat[0][0] += 1;
         mat[1][1] += 1;
         mat[2][2] += 1;

         MATRIX3 amat = mesh->defgrad[t];
         MATRIX3 R, D, Vt; /*
         real w[3];
         real tmat[3][3];
         real tR[3][3];
         memcpy(&tmat[0][0], mat, 9*sizeof(real));
         dsyevh3(tmat, tR, w);
         memcpy(R, &tR[0][0], 9*sizeof(real));
         D.MakeDiagonal(w[0], w[1], w[2]);*/
         mesh->SVD3x3(mat, R, D, Vt);
         //mat.EigenDecomposition(R, D);

         D[0][0] = sqrt(D[0][0]);
         D[1][1] = sqrt(D[1][1]);
         D[2][2] = sqrt(D[2][2]);
         MATRIX3 F = (R*D).TimesTranspose(R);
         //if (ccc <= 1) 
         //F = amat;
         VEC3 p1 = worldPos[tet[1]] - worldPos[tet[0]];
         VEC3 p2 = worldPos[tet[2]] - worldPos[tet[0]];
         VEC3 p3 = worldPos[tet[3]] - worldPos[tet[0]];
         MATRIX3 X(p1[0], p2[0], p3[0], 
                p1[1], p2[1], p3[1], 
                p1[2], p2[2], p3[2]);
         MATRIX3 Xi = X.Inverse();
         
        if (ccc <= 3) 
         baryMat[t] = mesh->baryMat[t]; 
         else
         baryMat[t] = Xi.TimesTranspose(F); 
      }
      ccc++;
#endif
         MATRIX3 a = baryMat[0];
         MATRIX3 b = mesh->baryMat[0];
         cout<<endl;
      /*
      for (int i = 0; i < numNodes; i++) {
     
         
        // matPos[i] = mesh->matPos[i];
         velocity[i] = mesh->velocity[i];
         acceleration[i] = mesh->acceleration[i];
      }*/
         


      // -------------------------------------------------------------------------------------------------------------

#ifdef USE_MATPOS
      nextMesh = new CFEMMesh(config, matPos, worldPos, velocity, acceleration, tet2node, groups, fixPosNodes);
#else
      nextMesh = new CFEMMesh(config, worldPos, velocity, acceleration, tet2node, baryMat, groups, fixPosNodes);
#endif
//      DumpVec("Bm_n.txt", mesh->Bm);
//      DumpVec("mass_n.txt", mesh->mass);

      SAFE_DELETE(mesh);
      mesh = nextMesh;


   #endif

   const int numSubSteps = 1;
   
   /*if (ccc <= 10) config.gravity = VEC3(0,0,0); else */ 
      config.gravity = VEC3(0,-9.8,0);
   for (int ss = 0; ss < numSubSteps; ss++) {
      StepSimulation();
   }
   //config.gravity = VEC3(0,0,0);
   //memset(&velocity[0], 0, sizeof(VEC3)*numNodes);



	// Evaluate the reflective properties of the material
	float colorBlue[] = { 0.0f, 0.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, colorBlue);
	mesh->Render();
	frame++;

	// currently we've been drawing to the back buffer, we need
	// to swap the back buffer with the front one to make the image visible
	glutSwapBuffers();

}

//------------------------------------------------------------	OnInit()
//
void OnInit() {
	
	FontInit();

	// enable depth testing
	glEnable(GL_DEPTH_TEST);


	glShadeModel(GL_SMOOTH);

}

//------------------------------------------------------------	OnExit()
//
void OnExit() {
	SAFE_DELETE(mesh);
	// delete the last display lists
	glDeleteLists(g_persp,1);
	glDeleteLists(g_ortho,1);
	
	FontCleanup();
}


class CFixTopBotCond : public CConstraintDecision {
    virtual bool operator () (CConfig& configi, VEC3& pos) const {
       return ((pos[1] > 0.45*configi.sceneScale));
    }
};

int main(int argc,char** argv) {

   config.LoadConfig("config.txt");

   // Overide some of the simulation parameter
   config.dt = 0.01;
   config.gravity = VEC3(0,-10,0);
   config.gravity = VEC3(0,-10,0);
   config.phi[0] = 5;
   config.phi[1] = 5;
   config.psi[0] = 5;
   config.psi[1] = 5;
   config.E[0] = 5e2;
   config.E[1] = 4e3;
   config.v[0] = 0.3;
   config.v[1] = 0.3;

   CFixTopBotCond cond;
   #ifdef USE_MATPOS

      mesh = new CFEMMesh(config, &cond, false);
   #else
      mesh = new CFEMMesh(config, &cond, true);
   #endif
//   DumpVec("Bm_m.txt", mesh->Bm);
//   DumpVec("mass_m.txt", mesh->mass);

   // Let's test point location
   /*
   int ir = 10;
   CTet& tet = mesh->tet2node[ir]; 
   vector<VEC3>& worldPos = mesh->worldPos;
   VEC3 pos = (worldPos[tet[0]]+worldPos[tet[1]]+worldPos[tet[2]]+worldPos[tet[3]])*0.25;
   int t = mesh->topology->PointLocationAnyCoord(worldPos, pos, 100);
   cout<<t<<endl;
   return 0;*/
   glutInit(&argc,argv);

	// request a depth buffer, RGBA display mode, and we want double buffering
	glutInitDisplayMode(GLUT_DEPTH|GLUT_RGBA|GLUT_DOUBLE);

	// set the initial window size
   glutInitWindowPosition(40, 0);
	glutInitWindowSize(640,480);

	// create the window
	glutCreateWindow("A simple FEM example");

	// set the function to use to draw our scene
	glutDisplayFunc(OnDraw);

	// set the function to use to draw our scene
	glutIdleFunc(OnDraw);

	// set the function to handle changes in screen size
	glutReshapeFunc(OnReshape);
   
	atexit(OnExit);
	// run our custom initialisation
	OnInit();

	// this function runs a while loop to keep the program running.
	glutMainLoop();
   return 0;
}