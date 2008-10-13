#ifndef _DEBUG
#ifndef NOOMP
#include <omp.h>
#endif
#endif
#include <stdlib.h>


#include "FEM.h"
#include <iostream>
#include <fstream>
#include "integrator.h"
//LOGGILE_TYPE logfile("log.txt");

// Main with graphics
// Design to work with either win32 or MacOSX XCode
#include "GL/glut.h"

#include "font.h"
#include "myTimer.h"
#include "topology.h"
//#include <process.h>
#include <windows.h>
using namespace std;
const float fov = 45;

unsigned int g_persp=0,g_ortho=0;

CConfig config;


#include "myTimer.h"
#include "debughelper.h"
//#define NOGUI 
#define TEST_REMESH

CFEMMesh* mesh;
double fps = 0.0f;
int width, height;
// Viewing parameters
VEC3 viewFrom(0,1,5), 
    viewLookAt(0,0,0), 
    viewUp(0,1,0);

extern double modelViewMat[16], projectionMat[16];
extern int viewport[4];

HANDLE meshMutex;

void StepSimulation() {
   static int oldTime = 0;
   int time = glutGet(GLUT_ELAPSED_TIME);
   real elapsed = (time - oldTime) * 0.001;
   fps = 1.0 / elapsed;
   oldTime = time;


#ifdef TEST_REMESH

   mesh->Simulate(config.dt );
   // Nothing
#else
   #ifdef NOGUI
 	   for (int i = 0; i < 1; i++) {
		   mesh->Simulate(config.dt );
	   }
   #else
      const int MAXCOUNT = 1;
      #ifdef ONE_TET
         real maxTimeStep = config.dt ;
      #else
         real maxTimeStep = config.dt ;
      #endif
      int count = 0;
      #ifdef ONE_TET
      for (int i = 0; i < MAXCOUNT; i++) {
      #else
      while (elapsed > maxTimeStep) {
      #endif
         elapsed -= mesh->Simulate(maxTimeStep);
         count++;
         if (count >= MAXCOUNT) break;
      }
      #ifndef ONE_TET
      mesh->Simulate(min(elapsed, maxTimeStep));
      #endif


      

   #endif

#endif
   //mesh->Simulate(0.005);
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

//------------------------------------------------------------	OnDraw()
//
// glut will call this function whenever the window needs to be
// redrawn.
//

bool holdRight = false, holdMid = false;
int holdX, holdY;
int mouseX, mouseY;
void OnMouseClick(int button, int state, int x, int y) {
   WaitForSingleObject( meshMutex, INFINITE );
   if (button == GLUT_MIDDLE_BUTTON) {
      if (state == GLUT_DOWN) {
         holdMid = true;
         holdX = x;
         holdY = y;
      } else {
         holdMid = false;
      }
   }

   if (button == GLUT_RIGHT_BUTTON) {
      if (state == GLUT_DOWN) {
         holdRight = true;
         holdX = x;
         holdY = y;
      } else {
         holdRight = false;
      }
   }
   if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
      // Pick vertex
      int ind;
      cout<<"Mouse click at "<<x<<" "<<(viewport[3] - y)<<endl;
     /*
      if ((ind = mesh->topology->AddBoundaryVertexFromRayMeshIntersection(VEC3(-1,RandomFloat()*0.04 - 0.025, RandomFloat()*0.04 - 0.025), config.needleInitForwardDir)) != -1) {
         //if ((ind = mesh->topology->AddBoundaryVertexFromRayMeshIntersection(VEC3(-1,-0.01,-0.01), config.needleInitForwardDir)) != -1) {
         cout<<"Pick a vertex "<<ind<<endl;
         mesh->FindEdges();
         cout<<"Done find edge"<<endl;
         mesh->tip.index = ind;
                
         #ifdef TEST_REMESH
            mesh->topology->InitTip(mesh->tip.index, mesh->cutTip);
         #endif
         mesh->numNodes = mesh->matPos.size();
         mesh->numTets = mesh->tet2node.size();
      }

*/
     
      if ((ind = mesh->Pick(x,viewport[3] - y,10)) != -1) {
         cout<<"Pick a vertex "<<ind<<endl;
         mesh->tip.index = ind;
         #ifdef TEST_REMESH
            mesh->topology->InitTip(mesh->tip.index, mesh->cutTip);
         #endif
      }
     

   }

   ReleaseMutex( meshMutex );
}

void OnMouseMoveActive(int x, int y) {
   mouseX = x;
   mouseY = y;

}
void OnMouseMovePassive(int x, int y) {
   OnMouseMoveActive(x,y);
}
int globalItr = 0;
ofstream qlog("qual.txt");

void OnDraw() {

   WaitForSingleObject( meshMutex, INFINITE );

	
	static int frame = 0;
   	// clear the screen & depth buffer
   glClearColor(0,0,0,1);
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
	
	// clear the previous transform
	// go back to modelview matrix so we can move the objects about
	glCallList(g_ortho);	
	//glDisable(GL_LIGHTING);
	glColor3f(1.0f,1.0f, 1.0f);
	char str[200];
   sprintf(str, "%0.3g fps", fps); 
   FontDraw(str, 20, 20);
/*	for (int i = 0; i < mesh->numNodes; i++) {
		FontDraw(str, 20,i*12);
	}*/
	
	//glEnable(GL_LIGHTING);

	// set the perspective projection
	glCallList(g_persp);
	
	gluLookAt(viewFrom[0], viewFrom[1], viewFrom[2], viewLookAt[0], viewLookAt[1], viewLookAt[2], viewUp[0], viewUp[1], viewUp[2]);


  /* vector<unsigned char> v;
   CLinearMemStorage mem(v);
   mesh->SaveStorage(mem);
   mem.Reset();
   SAFE_DELETE(mesh);
   mesh = new CFEMMesh(config, mem);
*/
   mesh->vconst.clear();
   mesh->vDirConst.clear();
   mesh->pconst.clear();

   const int numSubSteps = 1;
   if (holdRight || holdMid) 
   {
      real objScaling = config.objRndrSize / config.sceneScale;

      int difX = mouseX-holdX;
      int difY = -(mouseY-holdY);
      real mag = sqrtf(difX*difX+difY*difY);


      VEC3 vz = viewLookAt - viewFrom;
      vz.Normalize();
      VEC3 vx = vz.Cross(viewUp);
      vx.Normalize();
      VEC3 vy = vx.Cross(vz);

      real T = tan(0.5*(fov / 180) * MYPI);
      real ry = -2*(T*(mouseY))/height + T;
      real G = width * T / height;
      real rx = -G + 2*(G*mouseX)/width;
      VEC3 rayDir(rx*vx + ry*vy + vz);
      rayDir.Normalize();
      cout<<rx<<" "<<ry<<" --- "<<mouseX<<" "<<mouseY<<endl;


      for (int ss = 0; ss < numSubSteps; ss++) {
         #ifdef TEST_REMESH
            if (holdRight) {
               // Cut Tip   
               // Always cut
               //mesh->topology->MoveTip(mesh->cutTip, (vx*difX+vy*difY)*config.mouseCutScaleOldGUI, config.dt);
               //if (mesh->cutTip.MoveTipForwardManual(*mesh->topology, config.dt) < 1e-20) {
               //mesh->ComputeForce(config.dt);
               if (mesh->cutTip.MoveNeedleForwardAndApplyConstraintsAndForce(*mesh->topology, config.dt) < 1e-20) {
               
                  // Move node to 0
                  mesh->topology->InitTip(0, mesh->cutTip);
               } else {
                  // Let's compute

                 
                  mesh->tip.index = mesh->cutTip.index;
                  //mesh->worldPos = mesh->matPos;
                  mesh->numNodes = mesh->matPos.size();
                  mesh->numTets = mesh->tet2node.size();
                  //mesh->force.resize(mesh->numNodes);
                  /*
                  memset(&mesh->mass[0], 0, sizeof(real)*mesh->numNodes);
                  for (int i = 0; i < mesh->numTets; i++) {
                     mesh->ComputeTetProperties(i);
                  }*/
                 
               }
            } else {
               cout<<width<<" "<<height<<endl;
//               mesh->extForce.push_back(CExternalForce(mesh->tip.index, (vx*difX+vy*difY)*config.mouseForceScaleOldGUI, config.dt));
               // force based on distance to ray
               VEC3 pos = mesh->worldPos[mesh->cutTip.index]*objScaling + config.objRndrDispWorld;
               VEC3 dis = (pos-viewFrom);
               real d = dis.Dot(rayDir);
               VEC3 forceDir = rayDir*d - dis;
               forceDir /= objScaling;
               mesh->extForce.push_back(CExternalForce(mesh->cutTip.index, forceDir*config.mouseForceScaleOldGUI, config.dt));


               glColor3f(1,1,1);
               glBegin(GL_LINES);
               VEC3 to = viewFrom + d*rayDir;
               glVertex3dv(pos);
               glVertex3dv(to);
               glEnd();
            }

         #else

            mesh->extForce.push_back(CExternalForce(mesh->tip.index, (vx*difX+vy*difY)*config.mouseForceScaleOldGUI, config.dt));
         #endif
     	   //if (frame == 0)	mesh.Simulate(0.00005);
         StepSimulation();


      }
         real minQ = 1e10;
         for (int i = 0; i < mesh->tet2node.size(); i++) {
            real q = mesh->topology->TetQuality(mesh->matPos[mesh->tet2node[i][0]],
                                       mesh->matPos[mesh->tet2node[i][1]],
                                       mesh->matPos[mesh->tet2node[i][2]],
                                       mesh->matPos[mesh->tet2node[i][3]]);
            if (q < minQ) minQ = q;

         }
         if (globalItr < 50) {
            qlog<<minQ<<endl;
            globalItr++;
            if (globalItr == 50) {
               qlog.close();
               cout<<endl<<endl<<endl<<"CLOSE"<<endl<<endl<<endl;
            }
         }

      // Recompute all properties for now                 
      real maxRCond = -1e10;
      for (int i = 0; i < mesh->numTets; i++) {
         if (mesh->tetRCond[i] > maxRCond) maxRCond = mesh->tetRCond[i];
      }
      cout<<"Maximum RCond = "<<maxRCond<<endl;
      

      mesh->FindEdges();
      cout<<"Topology valid ? "<<mesh->topology->VerifyValidTopology(&mesh->cutTip)<<endl;

   } else {
      for (int ss = 0; ss < numSubSteps; ss++) {
         StepSimulation();
      }
   }



	// Evaluate the reflective properties of the material
	float colorBlue[] = { 0.0f, 0.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, colorBlue);
	mesh->Render();
	frame++;
	////logfile<<"Frame "<<frame<<endl;

	// currently we've been drawing to the back buffer, we need
	// to swap the back buffer with the front one to make the image visible
	glutSwapBuffers();
   ReleaseMutex( meshMutex );

}

//------------------------------------------------------------	OnInit()
//
void OnInit() {
	
	////logfile<<"Before font init"<<endl;	
	FontInit();
	////logfile<<"After font init"<<endl;	

	// enable depth testing
	glEnable(GL_DEPTH_TEST);

	// Somewhere in the initialization part of your program…
/*	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// Create light components
	GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8, 1.0f };
	GLfloat specularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat position[] = { -1.5f, 1.0f, -4.0f, 1.0f };

	// Assign created components to GL_LIGHT0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	*/
	glShadeModel(GL_SMOOTH);

}

//------------------------------------------------------------	OnExit()
//
void OnExit() {
	SAFE_DELETE(mesh);
	//logfile.close();
	// delete the last display lists
	glDeleteLists(g_persp,1);
	glDeleteLists(g_ortho,1);
	
	FontCleanup();
   CloseHandle( meshMutex );
}

//------------------------------------------------------------	main()
//

void OnKeyboard(unsigned char key, int x, int y) {
   WaitForSingleObject( meshMutex, INFINITE );

   if (key == 's') {
      cout<<"Write out mesh"<<endl;
      mesh->topology->WriteMeshStar("current_mesh.mesh");
   }
   ReleaseMutex( meshMutex );
}

class CFixTopBotCond : public CConstraintDecision {
//if ((matPos[i][1] > 0.475*config.sceneScale) || (matPos[i][1] < -0.475*config.sceneScale)) {
    virtual bool operator () (CConfig& configi, int index, VEC3& pos) const {
       return ((pos[1] > 0.475*configi.sceneScale) || (pos[1] < -0.475*configi.sceneScale));
       //return ((pos[1] > 0.45*configi.sceneScale));
    }
};

class CTestBICGStab: public CIntegrator {
public:

   vector<vector<real> > mat;
   vector<VEC3> b, x;
   CTestBICGStab(CFEMMesh& femi) : CIntegrator(femi) {  
      int n = 3;
      FILE* f = fopen("A","rt");
      mat.resize(n);
      for (int i = 0; i < n; i++) {
         mat[i].resize(n);
         for (int j = 0; j < n; j++) {
            fscanf(f, "%lf", &mat[i][j]);
         }
      }
      fclose(f);
      cout<<"A is "<<endl;
      for (int i = 0; i < n; i++) {
         for (int j = 0; j < n; j++) {
            cout<<mat[i][j]<<" ";
         }
         cout<<endl;
      }


      b.resize(n/3);
      x.resize(n/3);
      idiag.resize(n);
      FILE* fb = fopen("b","rt");
      for (int i = 0; i < n/3; i++) {
         real t1, t2, t3;
         fscanf(fb,"%lf", &t1);
         fscanf(fb,"%lf", &t2);
         fscanf(fb,"%lf", &t3);
         cout<<t1<<" "<<t2<<" "<<t3<<endl;
         b[i] = VEC3(t1,t2,t3);
         x[i] = VEC3(0,0,0);
         idiag[i] = 1;
      }
      fclose(fb);
      cout<<"b is "<<endl;
      for (int i = 0; i < n/3; i++) {
         cout<<b[i][0]<<endl;
         cout<<b[i][1]<<endl;
         cout<<b[i][2]<<endl;
      }

      dv.resize(n/3);
      c.resize(n/3);
      q.resize(n/3);
      // CG
      p.resize(n/3);
      phat.resize(n/3);
      shat.resize(n/3);
      t.resize(n/3);
      v.resize(n/3);
      rtilde.resize(n/3); // BiCGStab
      r.resize(n/3);
      s.resize(n/3); // Both CG and BiCGStab
   }
   real TimeStepping(real dti) {
      return 0;
   } 
   void MultiplyCGMat(vector<VEC3>& dv, vector<VEC3>& out) {
      real* dvp = &dv[0][0];
      real* outp = &out[0][0];
      int n = mat[0].size();
      
      for (int i = 0; i < n; i++) {
         real sum = 0;
         for (int j = 0; j < n; j++) {
            sum += mat[i][j]*dvp[j];
         }
         outp[i] = sum;
      }

   }
   

   // Filter the solution of the linear system so as not to violate constraints
   void FilterSolution(vector<VEC3>& dvel) {

   }

};

#include "topology.h"
int main(int argc,char** argv) {

   if (argc > 1) {
      config.LoadConfig(argv[1]);
   } else {
      config.LoadConfig("config.txt");
   }
   meshMutex = CreateMutex( NULL, FALSE, NULL );
   #ifndef _DEBUG
      #ifndef NOOMP

        omp_set_num_threads(1);
      #endif
   #endif

   CMyTimer timer;
   /*   mesh = new CFEMMesh(tmpFullName);

   const int NUM_REPEAT = 10;
   const int NUM_ITR = 650;
   for (int nt = 6; nt <= 6; nt++) {
      omp_set_num_threads(nt);
      real sumT = 0.0f;
      for (int rp = 0; rp < NUM_REPEAT; rp++) {
         timer.Start();
         for (int i = 0; i < NUM_ITR; i++) {
            StepSimulation();
            //mesh->ComputeForce(0.0001);
         }  
         real time = timer.Stop();
         cout<<"Time = "<<time<<endl;
         sumT += time;
      }
      cout<<"Average time for "<<nt<<" threads is "<<(sumT / NUM_REPEAT)<<endl;
      
   }
   exit(0);*/
   /*
   CFEMMesh mmm("Input/2tet.mesh");
   vector<CTet> t2t;
   CTetTopology::ComputeTopology(mmm.tet2node, mmm.matPos, t2t);
   CTetTopology::Split2Tet(mmm.tet2node, mmm.matPos, t2t, 1, 3);
   bool ok = CTetTopology::VerifyValidTopology(mmm.tet2node, mmm.matPos, t2t);
   if (ok) cout<<"Topology is ok"<<endl; else cout<<"Topology is bad!"<<endl;
   CTetTopology::WriteMesh(mmm.tet2node, mmm.matPos, t2t, "sp1.mesh");

   CTetTopology::Split2Tet(mmm.tet2node, mmm.matPos, t2t, 0, 1);
   ok = CTetTopology::VerifyValidTopology(mmm.tet2node, mmm.matPos, t2t);
   if (ok) cout<<"Topology is ok"<<endl; else cout<<"Topology is bad!"<<endl;
   CTetTopology::WriteMesh(mmm.tet2node, mmm.matPos, t2t, "sp2.mesh");
   return 0;*/

   /* 
   real mat[9] = {1,2,3,
                   7,9,1,
                   3,1,2};
   real Qt[9];
   QR3(mat, Qt)j ;
   
   for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
         cout<<mat[i*3 + j]<<" ";
      }
      cout<<endl;
   }
   
   cout<<endl;
   for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
         cout<<Qt[i*3 + j]<<" ";
      }
      cout<<endl;
   }

   return 0;
*/


   CFixTopBotCond cond;
   mesh = new CFEMMesh(config, &cond);
   /*
   CTestBICGStab test(*mesh);
   int max_iter = 10;
   real tol = 1e-5;
   int iter = 0;
   int mode=0;
   //real res = test.MyBiCGSTAB(test.b, test.x, tol, max_iter, iter, mode);
   
   real res = test.MyCg(test.b, test.x, tol, max_iter, iter);
   cout<<"Mode = "<<mode<<" residual = "<<res<<endl;
   for (int i = 0; i < test.x.size(); i++) {
      cout<<test.x[i][0]<<endl;
      cout<<test.x[i][1]<<endl;
      cout<<test.x[i][2]<<endl;
   }

   return 0;*/

   CTetTopology top(*mesh);
/*   // Test tet combine
   int newTets[4];
   top.WriteMeshStar("sctet_before.mesh");
   CTet t2n = mesh->tet2node[0];
   top.SplitTetStar(0, newTets, 0.25*(top.nodePos[t2n[0]] + top.nodePos[t2n[1]] + top.nodePos[t2n[2]] + top.nodePos[t2n[3]]));
   top.WriteMeshStar("sctet_after.mesh");
   top.Combine4TetsStar(t2n, 0);
   top.WriteMeshStar("sctet_aftercombine.mesh");

/*
   // Test tet adjacent to node
   int numTets = mesh->tet2node.size();
   int numFails = 0;
   int numTests = 1000;
   for (int i = 0; i < numTests; i++) {
      int t = i % numTets;
      CTet& t2n = mesh->tet2node[t];
      vector<int> stets;
      // Query
      int v0 = t2n[0];
      top.FindTetsAroundNodeStar(v0, t2n[1], t2n[2], stets);

      vector<int> stetsc;
      stetsc.clear();
      // Verify
      for (int j = 0; j < numTets; j++) {
         CTet& t2nd = mesh->tet2node[j];
         if ((t2nd[0] == v0) ||
             (t2nd[1] == v0) ||
             (t2nd[2] == v0) ||
             (t2nd[3] == v0)) {         
            stetsc.push_back(j);
         }
      }
      bool ok = true;
      if (stets.size() == stetsc.size()) {
         
         sort(stets.begin(), stets.end());
         sort(stetsc.begin(), stetsc.end());

         for (int j = 0; j < stets.size(); j++) {
            if (stets[j] != stetsc[j]) {
               ok = false;
               break;
            }
         }
      } else {
         ok = false;
      }
      if (!ok) {
         cout<<"Incorrect neighbors for node "<<v0<<endl;
         cout<<"Query: "<<endl;
         for (int j = 0; j < stets.size(); j++) {
            cout<<stets[j]<<" ";
         }
         cout<<endl;
         cout<<"Actual: "<<endl;
         for (int j = 0; j < stetsc.size(); j++) {
            cout<<stetsc[j]<<" ";
         }
         cout<<endl;
      }      
   }
   cout<<"Fail the node neighbor test "<<numFails<<" times out of "<<numTests<<" times."<<endl;
   /*
   // Test quality
   real minQ = 1e10, maxQ = -1e10;
   for (int i = 0; i < mesh->numTets; i++) {
      CTet& t2n = mesh->tet2node[i];
      real q = top.TetQuality(mesh->matPos[t2n[0]], mesh->matPos[t2n[1]], mesh->matPos[t2n[2]], mesh->matPos[t2n[3]]);
      if (q < minQ) minQ = q;
      if (q > maxQ) maxQ = q;
   }
   cout<<"Minimum quality is "<<minQ<<" maxQ = "<<maxQ<<endl;*/
   /*
   // Test point location
   int i = 0;
   int numNodes = mesh->matPos.size();
   VEC3 mins(1e10,1e10,1e10), maxs(-mins);
   for (int i = 0; i < numNodes; i++) {
      VEC3 p = mesh->matPos[i];
      if (p[0] < mins[0]) mins[0] = p[0]; 
      if (p[1] < mins[1]) mins[1] = p[1]; 
      if (p[2] < mins[2]) mins[2] = p[2]; 

      if (p[0] > maxs[0]) maxs[0] = p[0]; 
      if (p[1] > maxs[1]) maxs[1] = p[1]; 
      if (p[2] > maxs[2]) maxs[2] = p[2]; 
   }

   for (int i = 0; i < 1000; i++) {
      real f0 = RandomFloat(),
           f1 = RandomFloat(),
           f2 = RandomFloat();
      VEC3 pos(mins[0]*f0 + maxs[0]*(1.0-f0), 
              mins[1]*f1 + maxs[1]*(1.0-f1), 
              mins[2]*f2 + maxs[2]*(1.0-f2));

      int inTet = top.PointLocation(pos, 0);
      int cinTet = -1;
      int numTets = mesh->tet2node.size();
      bool ok = false;
      for (int j = 0; j < numTets; j++) {
         if (top.InTet(pos, j)) {
            cinTet = j;
            if (j == inTet) {
               //cout<<j<<" -- "<<inTet<<endl;
               ok = true;
               break;
            }
         }
      }
      if ((ok == false) && (inTet == -1)){
         ok = true;
      }
      if (!ok) {
         cout<<"Incorrect! pos = "<<pos[0]<<", "<<pos[1]<<", "<<pos[2]<<" is in "<<cinTet<<" not in "<<inTet<<endl;

      } else {
         cout<<"Correct "<<i<<endl;
      }
   

   }
  
   return 0;
   */
   // Tet split test
   //int newTets[4];
/*
   vector<int> newTets;
   top.WriteMeshStar("sptet_before.mesh");
   top.SplitTetStar(0, 0.25*(mesh->matPos[0] + mesh->matPos[1] + mesh->matPos[2] + mesh->matPos[3]), newTets);
   top.WriteMeshStar("sptet_after.mesh");

   return 0;*/
   // Face split test
   //int activeV[6];
   //int newTets[6];
   //top.WriteMeshStar(mesh->worldPos, mesh->groups, "spface_before.mesh");
   //top.Split2TetStar(mesh->worldPos, mesh->groups, 0, 3, activeV, newTets);
   //top.WriteMeshStar(mesh->worldPos, mesh->groups, "spface_after.mesh");

   //return 0;
   
   // Edge split test
/*
   vector<int> atets;
   vector<int> ring;
   vector<int> newts;

   atets.clear();
   ring.clear();
   for (int i = 0; i < 100; i++) {
      int rt = rand() % top.tet2node.size();
      int e = rand() % 6;
      CTet& t2n = top.tet2node[rt];
      int v0 = t2n[CTetTopology::edges[e][0]]
         ,v1 = t2n[CTetTopology::edges[e][1]]
         ,atip = t2n[CTetTopology::edges[e][2]];
      top.FindTetsAroundEdgeStar(v0, v1, atip, atets, ring);

      VEC3 pos = 0.5*(top.nodePos[v0] + top.nodePos[v1]);

      top.SplitEdgeTetsStar( v0, v1, ring, atets, pos, newts);
      top.VerifyValidTopology();
   }*/
   /*
   top.FindTetsAroundEdgeStar(mesh->tet2node[0][0], mesh->tet2node[0][1], mesh->tet2node[0][2], atets, ring);
   for (int i = 0; i < atets.size(); i++) {
      cout<<atets[i]<<" "<<ring[i]<<endl;
   }
   vector<int> newts;
   newts.clear();
   top.WriteMeshStar( "spedge_before.mesh");
   top.SplitEdgeTetsStar( mesh->tet2node[0][0], mesh->tet2node[0][1], ring, atets, VEC3(0,0,0), newts);
   top.WriteMeshStar("spedge_after.mesh");
   */
   //return 0;
	////logfile<<"Start main"<<endl;

#ifdef TEST_REMESH
   mesh->tip.index = 25;
   mesh->topology->InitTip(mesh->tip.index, mesh->cutTip);
#endif
   cout<<"Perm v const is "<<mesh->permVconst.size()<<endl;
   for (int i = 0; i < mesh->permVconst.size(); i++) {
      cout<<mesh->permVconst[i].nodeNum<<" ";
   }
   cout<<endl;

#ifndef NOGUI
	// initialise glut
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
   glutMouseFunc(OnMouseClick);
   glutMotionFunc(OnMouseMoveActive);
   glutPassiveMotionFunc(OnMouseMoveActive);
   glutKeyboardFunc(OnKeyboard);
   // set the function to be called when we exit
	atexit(OnExit);
	// run our custom initialisation
	OnInit();


	// this function runs a while loop to keep the program running.
	glutMainLoop();
#else
   OnInit();
   timer.Start();
   const int numItr = 400;
   const float minD = -1.5f;
   const float increment = 0.01f;
   for (int q = 0; q < 3; q++) {
      forceList.clear();
      if (q == 0) {
         config.forceMethod = FORCE_GREEN;
      } else 
      if (q == 1) {
         config.forceMethod = FORCE_IRVING;
      } else {
         config.forceMethod = FORCE_CAUCHY;
      }
      for (int i = 0; i <= numItr; i++) {
         for (int j = 0; j < mesh->velocity.size(); j++) {
            mesh->velocity[j] = VEC3(0.0f, 0.0f, 0.0f);
         }
         mesh->worldPos = mesh->matPos;
         mesh->worldPos[3][0] = minD + i*increment;
         StepSimulation();
      }
      ofstream off;
      if (q == 0) {
         off.open("forceGreen.txt");
      } else 
      if (q == 0) {
         off.open("forceIrving.txt");
      } else {
         off.open("forceCauchy.txt");
      }
      for (int i = 0; i < forceList.size(); i++) {
         off<<forceList[i][0]<<" "<<forceList[i][1]<<" "<<forceList[i][2]<<endl;
      }
      off.close();
   }
   real elapsed = timer.Stop();  
   cout<<"elapsed = "<<elapsed<<endl;
   cout<<"Average amount of time is "<<sumTime / numItr<<endl;
   OnExit();
#endif

   return 0;
}