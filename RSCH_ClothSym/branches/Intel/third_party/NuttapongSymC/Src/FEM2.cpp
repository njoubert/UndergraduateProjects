
#include "dsyevh3.h"
#include "Wm4Matrix3.h"
#include "FEM.h"
#include <cstdio>
#include <cstdlib>
#ifndef NO_GRAPHICS

#include "GL/glut.h"
#endif
#include <fstream>
#include "topology.h"


void CFEMMesh::InitializeTipInfo() { // Figure out the information necessary to do tip movement
   #ifdef ONE_TET
      tip.index = 0;
      tip.vel = VEC3(0.0f, 0.0f, 0.0f);
   #else
      tip.index = 0;
      //tip.index = 521;
      //tip.index = 516;
      //tip.index = 26;      //tip.vel = VEC3(0.05f, 0.0015f, 0.0015f);
      //tip.index = 0;
          tip.vel = VEC3(0, 0, 0.0f);
   #endif
   tip.speed = tip.vel.Length();
   tip.dir = tip.vel;
   tip.dir.Normalize();
   //vconst.push_back(CVConst(tip.index, tip.vel));

   // Figure out which tetrahedrons are affected by the node
   for (int i = 0; i < numTets; i++) {
      if ((tet2node[i][0] == tip.index) ||
         (tet2node[i][1] == tip.index) ||
         (tet2node[i][2] == tip.index) ||
         (tet2node[i][3] == tip.index)) {
            tip.affectedTets.push_back(i);          
      }
   }

   tip.firstTime = true;

}
void CFEMMesh::SubtractTetProperties(int ti) { // For now, just subtract the influence of the mass of the tet ti
   int* tet = &tet2node[ti][0];
   real rho = rhos[groups[ti]];

   real vol = TetVol(&matPos[tet[0]][0], &matPos[tet[1]][0], &matPos[tet[2]][0], &matPos[tet[3]][0]);
   tetVol[ti] = vol;
   mass[tet[0]] -= 0.25*rho*vol;
   mass[tet[1]] -= 0.25*rho*vol;
   mass[tet[2]] -= 0.25*rho*vol;
   mass[tet[3]] -= 0.25*rho*vol;
}

bool CFEMMesh::InTipComplex(VEC3* pos, VEC3& v) {

   real sum6 = 0.0;
   for (int i = 0; i < 6; i++) {
      VEC3 &p0 = pos[tip.activeNodes[CTetTopology::dualPyramidFaces[i][0]]], 
         &p1 = pos[tip.activeNodes[CTetTopology::dualPyramidFaces[i][1]]], 
         &p2 = pos[tip.activeNodes[CTetTopology::dualPyramidFaces[i][2]]];
      
      sum6 += TetVol(&p0[0], &p1[0], &p2[0], &v[0]);
   }
   real sum2 = 0.0;
   VEC3 &v0 = pos[tip.activeNodes[0]], 
      &v1 = pos[tip.activeNodes[1]], 
      &v2 = pos[tip.activeNodes[2]],
      &v3 = pos[tip.activeNodes[3]],
      &v4 = pos[tip.activeNodes[4]];
   sum2 = TetVol(&v0[0], &v1[0], &v2[0], &v3[0]) + TetVol(&v4[0], &v1[0], &v2[0], &v3[0]);

   if (abs(sum2 - sum6) > 0.001*sum2) {
      return false;
   } else {
      return true;
   }
}
