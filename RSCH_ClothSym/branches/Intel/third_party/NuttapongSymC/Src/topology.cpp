
#include "topology.h"
#include "fem.h"
#include <fstream>
#include "debughelper.h"
using namespace std;

//extern LOGGILE_TYPE logfile;
// Outward facing
int CTetTopology::dualPyramidFaces[6][3] = {
   {0,1,3},
   {0,3,2},
   {0,2,1},
   {1,4,3},
   {1,2,4},
   {3,4,2}
};

// The opposite face of each vertex
int CTetTopology::oppositeFaces[4][3] = 
{
   {1,3,2},
   {0,2,3},
   {0,3,1},
   {0,1,2}
};

int CTetTopology::edges[6][3] = // Edges and a tip, with outward facing orientation
{
   {0,1,2},
   {0,2,3},
   {0,3,1},
   {1,2,0},
   {1,3,2},
   {2,3,0}
};

int CTetTopology::adjFaces[4][2] = // Adjacent faces to a node, (i, adjFaces[i][0], adjFaces[i][1]) is an outward facing face
{
   {1,2},
   {3,2},
   {0,1},
   {0,2}
};

real CTetTopology::ComputeVolume() {
   int numTets = tet2node.size();   
   real vol = 0.0;
   for (int i = 0; i < numTets; i++) {
      CTet& t2n = tet2node[i];
      vol += TetVol(nodePos[t2n[0]], nodePos[t2n[1]], nodePos[t2n[2]], nodePos[t2n[3]]);      
   }
   return vol;

}
CTetTopology::CTetTopology(CFEMMesh& femi) : fem(femi), tet2node(femi.tet2node), nodePos(femi.matPos), groups(femi.groups), config(fem.config) {
   lvIndex = 1;
/*
   char st[1] = "";   
   char* sst[1] = {st};
   parsecommandline(1, sst, &behave);*/

   // Initialize behave
  /* See the comments above the definition of `struct behavior' for the */
  /*   meaning of most of these variables.                              */
  behave.inspherecount = 0;
  behave.orientcount = 0;
  behave.orient4dcount = 0;
  behave.tetcircumcentercount = 0;
  behave.tricircumcentercount = 0;

  behave.poly = behave.refine = behave.quality = 0;
  behave.varvolume = behave.fixedvolume = behave.usertest = 0;
  behave.regionattrib = behave.convex = behave.weighted = behave.jettison = 0;
  behave.firstnumber = 1;
  behave.edgesout = behave.facesout = behave.voronoi = behave.neighbors = behave.geomview = 0;
  behave.nobound = behave.nopolywritten = behave.nonodewritten = behave.noelewritten = 0;
  behave.noiterationnum = 0;
  behave.noholes = behave.noexact = 0;
  behave.docheck = 0;
  behave.nobisect = 0;
  behave.conformdel = 0;
  /* A negative `steiner' value indicates no constraint on number of */
  /*   Steiner points.                                               */
  behave.steiner = -1;
  behave.order = 1;
  behave.qmeasure = 0.0;
  behave.minangle = 0.0;
  /* A negative volume indicates no constraint on tetrahedron volumes. */
  behave.maxvolume = -1.0;
  behave.quiet = behave.verbose = 0;


   primitivesinit();
   quadHash.clear();
//   pairHash.clear();
   int numTets = tet2node.size();

   int ne = 0;
   // Create the tetrahedral mesh data structure
   tetcomplexinit(&mesh, (struct proxipool *) NULL, 0);

   // Create hash table for edges
   for (int i = 0; i < numTets; i++) {
      CTet& t = tet2node[i];
      tetcomplexinserttet(&mesh, t[0], t[1], t[2], t[3]);
      quadHash[CSortedQuad(t[0], t[1], t[2], t[3])] = i;

  /*    for (int j = 0; j < 3; j++) {
         for (int k = j + 1; k < 4; k++) {
            CSortedPair sp(t[j], t[k]);
            pItr = pairHash.find(sp);
            if (pItr == pairHash.end()) {
               pairHash[sp] = ne++;
            }
         }
      }*/

   }
   totalVolume = ComputeVolume();
}

CTetTopology::~CTetTopology() {
   /* Free the mesh. */
   tetcomplexdeinit(&mesh);
}

void CTetTopology::Combine4TetsStar(CTet& t2n, int oldTetIndex) {
   // This works only when the 4 tets was the most recent topological operation
   int vtx1 = t2n[0], 
       vtx2 = t2n[1], 
       vtx3 = t2n[2], 
       vtx4 = t2n[3];
   tag newV = nodePos.size() - 1;

   #ifdef _DEBUG
      // Check to make sure this is valid to do so
      int numTets = tet2node.size() - 3;

      if ((quadHash[CSortedQuad(newV, vtx2, vtx3, vtx4)] != oldTetIndex) ||
          (quadHash[CSortedQuad(vtx1, newV, vtx3, vtx4)] != numTets) ||
          (quadHash[CSortedQuad(vtx1, vtx2, newV, vtx4)] != numTets + 1) ||
          (quadHash[CSortedQuad(vtx1, vtx2, vtx3, newV)] != numTets + 2)) {
          Error("Invalid!!!!!");
      }     
   #endif
   // Now do the 4-1 flip
   tetcomplex41flip(&mesh, vtx1, vtx2, vtx3, vtx4, newV);

   // Fix up the hash table
   quadHash.erase(CSortedQuad(newV, vtx2, vtx3, vtx4));
   quadHash.erase(CSortedQuad(vtx1, newV, vtx3, vtx4));
   quadHash.erase(CSortedQuad(vtx1, vtx2, newV, vtx4));
   quadHash.erase(CSortedQuad(vtx1, vtx2, vtx3, newV));

   quadHash[CSortedQuad(vtx1, vtx2, vtx3, vtx4)] = oldTetIndex;

   // Remove a node
   nodePos.pop_back();

   // Fix up tet2node
   tet2node.pop_back();
   tet2node.pop_back();
   tet2node.pop_back();
   tet2node[oldTetIndex] = t2n;

      
}

void CTetTopology::SplitEdgeTetsStar(int v0, int v1, vector<int>& ring, vector<int>& tets, VEC3& newPos, vector<int>& newTets) {
   // Try to reuse index as much as possible
   // Try using Jonathan split

   // Fix our tet2node first
   int ntetsInRing = tets.size();
   newTets = tets;
   int newv = nodePos.size();
   nodePos.push_back(newPos);

   // "bottom" tets
   for (int i = 0; i < ntetsInRing; i++) {
      quadHash.erase(CSortedQuad(tet2node[tets[i]]));
    /*  //logfile<<tet2node[tets[i]][0]<<" "
          <<tet2node[tets[i]][1]<<" "
          <<tet2node[tets[i]][2]<<" "
          <<tet2node[tets[i]][3]<<" -- "
          <<v0<<" "<<v1<<" "<<ring[i+1]<<" "<<ring[i]<<endl;*/
      tet2node[tets[i]] = CTet(v0, newv, ring[i+1], ring[i]);
      ////logfile<<orient3d(&behave, nodePos[v0], nodePos[newv], nodePos[ring[i+1]], nodePos[ring[i]])<<endl;
      quadHash[CSortedQuad(tet2node[tets[i]])] = tets[i];
   }

   // "top" tets
   for (int i = 0; i < ntetsInRing; i++) {
      tet2node.push_back( CTet(newv, v1, ring[i+1], ring[i]));
      quadHash[CSortedQuad(tet2node.back())] = tet2node.size()-1;
      newTets.push_back(tet2node.size()-1);
      groups.push_back(groups[tets[i]]); // Top and bottom tets of the same old tet use same material property
      ////logfile<<orient3d(&behave, nodePos[newv], nodePos[v1], nodePos[ring[i+1]], nodePos[ring[i]])<<endl;

   }

   
   // Now handle the stars
/*   
   for (int i = 0; i < ntetsInRing; i++) {
      
      tetcomplex12fliponedge(&mesh,
                            v0,
                            v1,
                            ring[i+1],
                            ring[i],
                            newv);      
   }         
*/   
   // Remove old Tets
   for (int i = 0; i < ntetsInRing; i++) {
      tetcomplexdeletetet(&mesh, v0, v1, ring[i+1], ring[i]);
   }
   for (int i = 0; i < ntetsInRing; i++) {
      tetcomplexinserttet(&mesh, v0, newv, ring[i+1], ring[i]);
      tetcomplexinserttet(&mesh, newv, v1, ring[i+1], ring[i]);
   }
}

void CTetTopology::FindRingAroundEdgeStar(int v0, int v1, int atip, vector<int>& ring) {
   int activeDir = 0;
   int ctip = atip;
   tag adj[2];
   ring.clear();
   static vector<int> backup;
   bool halfRing = false;

   for (activeDir = 0; activeDir < 2; activeDir++) {
      ctip = atip;
      adj[activeDir] = atip;
      
      do {
         
         ring.push_back(adj[activeDir]);
         tetcomplexadjacencies(&mesh, v0, v1, ctip, adj);     
         ////logfile<<adj[activeDir]<<endl;
         ctip = adj[activeDir];
      } while ((adj[activeDir] != GHOSTVERTEX) && (adj[activeDir] != atip));

      if (adj[activeDir] == atip) {
         // Full ring!
         ring.push_back(atip);
         break;
      } else {
         if (activeDir == 0) {
            backup = ring;
            ring.clear();
         }
         halfRing = true;

         // Reverse the order of ring        
      }

      /* // Need not worry about order of vertices
      if (activeDir == 0) {
         for (int i = 1; i < ring.size(); i++) {
            tets.push_back(quadHash[CSortedQuad(v0,v1,ring[i-1], ring[i])]);
         }
      } else {
         for (int i = 1; i < ring.size(); i++) {
            tets.push_back(quadHash[CSortedQuad(v0,v1,ring[i], ring[i-1])]);
         }
      }
      */
   }  

   if (halfRing) {      
      std::reverse(ring.begin(), ring.end());
      for (int i = 1; i < backup.size(); i++) {
         ring.push_back(backup[i]);
      }
   }

}
void CTetTopology::FindTetsAroundEdgeStar(int v0, int v1, int atip, vector<int>& tets,  vector<int>& ring) {
   tets.clear();

   FindRingAroundEdgeStar(v0, v1, atip, ring);
   for (int i = 1; i < ring.size(); i++) {
      tets.push_back(quadHash[CSortedQuad(v0,v1,ring[i-1], ring[i])]);
   }      
}

int CTetTopology::Tet2Tet(int t, int f) {
   // Return the index of the adjacent tet
   CTet& t2n = tet2node[t];
   tag adj[2];
   tetcomplexadjacencies(&mesh, t2n[oppositeFaces[f][0]], t2n[oppositeFaces[f][1]], t2n[oppositeFaces[f][2]], adj);
   ////logfile<<"My vertex is "<<t2n[f]<<" -- "<<adj[0]<<" "<<adj[1]<<endl;
   if (adj[0] == GHOSTVERTEX) return -1; else
   return quadHash[CSortedQuad(t2n[oppositeFaces[f][1]], t2n[oppositeFaces[f][0]], t2n[oppositeFaces[f][2]], adj[0])];
}


int CTetTopology::Tet2Tet(int t, int f, tag& tip) {
   // Return the index of the adjacent tet
   CTet& t2n = tet2node[t];
   tag adj[2];
   tetcomplexadjacencies(&mesh, t2n[oppositeFaces[f][0]], t2n[oppositeFaces[f][1]], t2n[oppositeFaces[f][2]], adj);
   ////logfile<<"My vertex is "<<t2n[f]<<" -- "<<adj[0]<<" "<<adj[1]<<endl;
   tip = adj[0];
   if (adj[0] == GHOSTVERTEX) return -1; else
   return quadHash[CSortedQuad(t2n[oppositeFaces[f][1]], t2n[oppositeFaces[f][0]], t2n[oppositeFaces[f][2]], adj[0])];
}


void CTetTopology::Split2TetsStar(int whichTet, int whichFace, int* activeV, VEC3& newPos, vector<int>& newTets) {
   newTets.resize(6);

   // Figure out v0, v1, v2, v3, v4
   // Create v5
   // Figure out tf0, tf1, tf2, tf3, tf4, tf5
   int oldTet0 = whichTet;
   CTet& t2n0 = tet2node[oldTet0];
   tag tip;
   int oldTet1 = Tet2Tet(whichTet, whichFace, tip);  

   if (oldTet1 == -1) {
      Error("Can't split 2 tet with non-existing neighbor");
   }

   
   CTet& t2n1 = tet2node[oldTet1];
//   CTet& t2t0 = tet2tet[oldTet0];
//   CTet& t2t1 = tet2tet[oldTet1];

   activeV[0] = t2n0[whichFace];
   activeV[1] = t2n0[oppositeFaces[whichFace][0]];
   activeV[2] = t2n0[oppositeFaces[whichFace][2]];
   activeV[3] = t2n0[oppositeFaces[whichFace][1]];
   activeV[4] = tip;


   // Get rid of the old 2 tets in the tetcomplex
   tetcomplexdeletetet(&mesh, t2n0[0], t2n0[1], t2n0[2], t2n0[3]); quadHash.erase(CSortedQuad(t2n0[0], t2n0[1], t2n0[2], t2n0[3]));
   tetcomplexdeletetet(&mesh, t2n1[0], t2n1[1], t2n1[2], t2n1[3]); quadHash.erase(CSortedQuad(t2n1[0], t2n1[1], t2n1[2], t2n1[3]));

   int oldGroup0 = groups[oldTet0], oldGroup1 = groups[oldTet1];
   // Figure out the index of the new tets
   newTets[0] = oldTet0;
   newTets[1] = oldTet1;
   int oldNumTets = tet2node.size();
   tet2node.resize(oldNumTets + 4);
   groups.resize(oldNumTets + 4);
   newTets[2] = oldNumTets;
   newTets[3] = oldNumTets + 1;
   newTets[4] = oldNumTets + 2;
   newTets[5] = oldNumTets + 3;

   // Figure out the new node position
   activeV[5] = nodePos.size();
   nodePos.push_back(newPos);
   
   // Now add the new tets with correct topology
   tet2node[newTets[0]] = CTet(activeV[0], activeV[3], activeV[1], activeV[5]); tetcomplexinserttet(&mesh, activeV[0], activeV[3], activeV[1], activeV[5]); quadHash[CSortedQuad(tet2node[newTets[0]])] = newTets[0];
   tet2node[newTets[1]] = CTet(activeV[0], activeV[2], activeV[3], activeV[5]); tetcomplexinserttet(&mesh, activeV[0], activeV[2], activeV[3], activeV[5]); quadHash[CSortedQuad(tet2node[newTets[1]])] = newTets[1];
   tet2node[newTets[2]] = CTet(activeV[0], activeV[1], activeV[2], activeV[5]); tetcomplexinserttet(&mesh, activeV[0], activeV[1], activeV[2], activeV[5]); quadHash[CSortedQuad(tet2node[newTets[2]])] = newTets[2];

   tet2node[newTets[3]] = CTet(activeV[1], activeV[3], activeV[4], activeV[5]); tetcomplexinserttet(&mesh, activeV[1], activeV[3], activeV[4], activeV[5]); quadHash[CSortedQuad(tet2node[newTets[3]])] = newTets[3];
   tet2node[newTets[4]] = CTet(activeV[1], activeV[4], activeV[2], activeV[5]); tetcomplexinserttet(&mesh, activeV[1], activeV[4], activeV[2], activeV[5]); quadHash[CSortedQuad(tet2node[newTets[4]])] = newTets[4];
   tet2node[newTets[5]] = CTet(activeV[3], activeV[2], activeV[4], activeV[5]); tetcomplexinserttet(&mesh, activeV[3], activeV[2], activeV[4], activeV[5]); quadHash[CSortedQuad(tet2node[newTets[5]])] = newTets[5];

   // Copy the group number
   groups[newTets[0]] = oldGroup0;
   groups[newTets[1]] = oldGroup0;
   groups[newTets[2]] = oldGroup0;

   groups[newTets[3]] = oldGroup1;
   groups[newTets[4]] = oldGroup1;
   groups[newTets[5]] = oldGroup1;

   


}

void CTetTopology::SplitBoundaryFaceStar(int whichTet, int whichFace, int* activeV, VEC3& newPos, vector<int>& newTets) {
   newTets.resize(3);

   // Figure out v0, v1, v2, v3, v4
   // Create v5
   // Figure out tf0, tf1, tf2, tf3, tf4, tf5
   int oldTet0 = whichTet;
   CTet& t2n0 = tet2node[oldTet0];
   
   tag adj[2];
   tetcomplexadjacencies(&mesh, t2n0[oppositeFaces[whichFace][0]], t2n0[oppositeFaces[whichFace][1]], t2n0[oppositeFaces[whichFace][2]], adj);

   if (adj[0] != GHOSTVERTEX) {
      Error("Can't spli boundary face when there is neighbor");
   }
   activeV[0] = t2n0[whichFace];
   activeV[1] = t2n0[oppositeFaces[whichFace][0]];
   activeV[2] = t2n0[oppositeFaces[whichFace][2]];
   activeV[3] = t2n0[oppositeFaces[whichFace][1]];

   // Get rid of the old 2 tets in the tetcomplex
   tetcomplexdeletetet(&mesh, t2n0[0], t2n0[1], t2n0[2], t2n0[3]); quadHash.erase(CSortedQuad(t2n0[0], t2n0[1], t2n0[2], t2n0[3]));

   int oldGroup0 = groups[oldTet0];

   // Figure out the index of the new tets
   newTets[0] = oldTet0;
   int oldNumTets = tet2node.size();
   tet2node.resize(oldNumTets + 2);
   groups.resize(oldNumTets + 2);
   newTets[1] = oldNumTets;
   newTets[2] = oldNumTets + 1;

   // Figure out the new node position
   activeV[4] = nodePos.size();
   nodePos.push_back(newPos);
   
   // Now add the new tets with correct topology
   tet2node[newTets[0]] = CTet(activeV[0], activeV[3], activeV[1], activeV[4]); tetcomplexinserttet(&mesh, activeV[0], activeV[3], activeV[1], activeV[5]); quadHash[CSortedQuad(tet2node[newTets[0]])] = newTets[0];
   tet2node[newTets[1]] = CTet(activeV[0], activeV[2], activeV[3], activeV[4]); tetcomplexinserttet(&mesh, activeV[0], activeV[2], activeV[3], activeV[5]); quadHash[CSortedQuad(tet2node[newTets[1]])] = newTets[1];
   tet2node[newTets[2]] = CTet(activeV[0], activeV[1], activeV[2], activeV[4]); tetcomplexinserttet(&mesh, activeV[0], activeV[1], activeV[2], activeV[5]); quadHash[CSortedQuad(tet2node[newTets[2]])] = newTets[2];


   // Copy the group number
   groups[newTets[0]] = oldGroup0;
   groups[newTets[1]] = oldGroup0;
   groups[newTets[2]] = oldGroup0;


}

void CTetTopology::WriteMeshStar(const string fname) {
   ofstream of(fname.c_str());
   int numNodes = nodePos.size();
   int numTets = tet2node.size();
   of<<numNodes<<endl;
   for (int i = 0; i < numNodes; i++) {
      of<<nodePos[i][0]<<" "<<nodePos[i][1]<<" "<<nodePos[i][2]<<endl;
   }
   of<<numTets<<endl;
   for (int i = 0; i < numTets; i++) {
      of<<groups[i]<<" "<<(tet2node[i][0]+1)<<" "<<(tet2node[i][1]+1)<<" "<<(tet2node[i][2]+1)<<" "<<(tet2node[i][3]+1)<<endl;
   }
   vector<CTri> tris;
   tag adj[2];

   for (int i = 0; i < numTets; i++) {
      CTet& t2n = tet2node[i];
      for (int j = 0; j < 4; j++) {
         tetcomplexadjacencies(&mesh, t2n[oppositeFaces[j][0]], t2n[oppositeFaces[j][1]], t2n[oppositeFaces[j][2]], adj);

         if (adj[0] == GHOSTVERTEX) {
            tris.push_back(CTri(t2n[oppositeFaces[j][0]],
                                t2n[oppositeFaces[j][1]],
                                t2n[oppositeFaces[j][2]]));

         }
      }
   }

   of<<tris.size()<<endl;
   for (int i = 0; i < tris.size(); i++) {
      of<<"1 "<<(tris[i][0]+1)<<" "<<(tris[i][1]+1)<<" "<<(tris[i][2]+1)<<endl;
   }
   of.close();
}

inline real CTetTopology::DistanceToPlane(VEC3& v0, VEC3& v1, VEC3& v2, VEC3& v) const {
   VEC3 a = v1-v0;
   VEC3 b = v2-v0;
   VEC3 n = a.Cross(b);
   n.Normalize();
   return -n.Dot(v-v0);
   
}


void CTetTopology::ComputeVertexSafeRadius(vector<VEC3>& nodePos, vector<real>& radius) {
   int numTets = tet2node.size();
   int numNodes = nodePos.size();
   radius.resize(numNodes);
   for (int i = 0; i < numNodes; i++) {
      radius[i] = 10;
   }
   for (int i = 0; i < numTets; i++) {
      CTet& t2n = tet2node[i];
      for (int j = 0; j < 4; j++) {
         real r = DistanceToPlane(nodePos[t2n[oppositeFaces[j][0]]], nodePos[t2n[oppositeFaces[j][1]]], nodePos[t2n[oppositeFaces[j][2]]], nodePos[t2n[j]]);
         if (r < radius[t2n[j]]) {
            radius[t2n[j]] = r;
         }
      }
   }
   ////logfile<<"------Radius------"<<endl;
   //for (int i = 0; i < numNodes; i++) {
      //if (radius[i] == 10) {
        // //logfile<<i<<endl;
      //}
   //}
   ////logfile<<"-----------------"<<endl;
}
/*void CTetTopology::ComputeEdgeSafeRadius(vector<VEC3>& nodePos, vector<real>& nradius, vector<real>& eradius) {
   // Un-implemented
   Error("Not implemented!");
}

*/
// Split one tet into 4 tets
void CTetTopology::SplitTetStar(int whichTet, VEC3& newPos, vector<int>& newTets) {
   newTets.resize(4);
   int numNodes = nodePos.size();
   int numTets = tet2node.size();
   CTet& t2n = tet2node[whichTet];
   tag vtx1 = t2n[0], 
       vtx2 = t2n[1],
       vtx3 = t2n[2],
       vtx4 = t2n[3];

   tetcomplex14flip(&mesh, vtx1, vtx2, vtx3, vtx4, numNodes);

   // Handle nodePos
   nodePos.push_back(newPos);

   // Handle the tet2node
   tet2node[whichTet]=CTet(numNodes, vtx2, vtx3, vtx4);
   tet2node.push_back(CTet(vtx1, numNodes, vtx3, vtx4));
   tet2node.push_back(CTet(vtx1, vtx2, numNodes, vtx4));
   tet2node.push_back(CTet(vtx1, vtx2, vtx3, numNodes));

   // Handle the hash table
   quadHash.erase(CSortedQuad(vtx1, vtx2, vtx3, vtx4));
   quadHash[CSortedQuad(numNodes, vtx2, vtx3, vtx4)] = whichTet;
   quadHash[CSortedQuad(vtx1, numNodes, vtx3, vtx4)] = numTets;
   quadHash[CSortedQuad(vtx1, vtx2, numNodes, vtx4)] = numTets + 1;
   quadHash[CSortedQuad(vtx1, vtx2, vtx3, numNodes)] = numTets + 2;

   // Handle the group
   groups.resize(tet2node.size());
   int oldGroup = groups[whichTet];
   groups[numTets] = oldGroup;
   groups[numTets+1] = oldGroup;
   groups[numTets+2] = oldGroup;
   
   newTets[0] = whichTet;
   newTets[1] = numTets;
   newTets[2] = numTets + 1;
   newTets[3] = numTets + 2;

   

}


/*
void CTetTopology::WriteMesh(const string fname) {
   ofstream of(fname.c_str());
   int numNodes = nodePos.size();
   int numTets = tet2node.size();
   of<<numNodes<<endl;
   for (int i = 0; i < numNodes; i++) {
      of<<nodePos[i][0]<<" "<<nodePos[i][1]<<" "<<nodePos[i][2]<<endl;
   }
   of<<numTets<<endl;
   for (int i = 0; i < numTets; i++) {
      of<<groups[i]<<" "<<(tet2node[i][0]+1)<<" "<<(tet2node[i][1]+1)<<" "<<(tet2node[i][2]+1)<<" "<<(tet2node[i][3]+1)<<endl;
   }
   vector<CTri> tris;
   for (int i = 0; i < numTets; i++) {
      for (int j = 0; j < 4; j++) {
         if (tet2tet[i][j] == -1) {
            tris.push_back(CTri(tet2node[i][oppositeFaces[j][0]],
                                tet2node[i][oppositeFaces[j][1]],
                                tet2node[i][oppositeFaces[j][2]]));

         }
      }
   }

   of<<tris.size()<<endl;
   for (int i = 0; i < tris.size(); i++) {
      of<<"1 "<<(tris[i][0]+1)<<" "<<(tris[i][1]+1)<<" "<<(tris[i][2]+1)<<endl;
   }
   of.close();
}*/
class CTetCombo{
public:
   CTetCombo() {
      v0=v1=v2=v3=0;
      faceIndex=-1;
   }

   CTetCombo(const CTetCombo& c) {
      v0 = c.v0;
      v1 = c.v1;
      v2 = c.v2;
      v3 = c.v3;
      fromTet = c.fromTet;
      faceIndex = c.faceIndex;
   }
   CTetCombo& operator = (const CTetCombo& c) {
      v0 = c.v0;
      v1 = c.v1;
      v2 = c.v2;
      v3 = c.v3;

      fromTet = c.fromTet;
      faceIndex = c.faceIndex;
      return *this;		
   }
   bool same(const CTetCombo& c) {
      return ((v0 == c.v0) && (v1 == c.v1) && (v2 == c.v2)) || ((v0 == c.v2) && (v1 == c.v1) && (v2 == c.v0));
   }

   CTetCombo(const int a0,const int a1,const int a2,const int a3, const int fromT, const int faceI) {
      faceIndex = faceI;
      fromTet = fromT;
      if (a0 < a1) {
         if (a1 < a2) {
            v0=a0;
            v1=a1;
            v2=a2;
         } else {
            if (a2 < a0) {
               v0=a2;
               v1=a0;
               v2=a1;
            } else {
               v0=a0;
               v1=a2;
               v2=a1;
            }
         }
      } else {
         if (a0 < a2) {
            v0 = a1;
            v1 = a0;
            v2 = a2;
         } else {
            if (a1 < a2) {
               v0 = a1;
               v1 = a2;
               v2 = a0;
            } else {
               v0 = a2;
               v1 = a1;
               v2 = a0;
            }
         }
      }

      v3=a3;

   }


   int v0,v1,v2,v3;
   int fromTet;
   int faceIndex;

};
class CTetComboCompare{
public:
   bool operator()(const CTetCombo& a, const CTetCombo& b) {
      if (a.v0 < b.v0) return true;
      if (a.v0 > b.v0) return false;
      if (a.v1 < b.v1) return true;
      if (a.v1 > b.v1) return false;
      if (a.v2 < b.v2) return true;
      return false;
   }
};

bool CTetTopology::VerifyValidTopology(CCutTip* tip) {
   // Check if edges along shafts exist
   if (tip) {
      CCutTip tmp(config);
      InitTip(tip->index, tmp);
      sort(tmp.affectedTets.begin(), tmp.affectedTets.end());
      sort(tip->affectedTets.begin(), tip->affectedTets.end());
      if (tmp.affectedTets != tip->affectedTets) {
         ////logfile<<"Affected tet is not correct!!!!"<<endl;
         return false;
      }

      for (int i = 1; i < tip->shaftNodes.size(); i++) {
         bool ee = fem.EdgeExist(tip->shaftNodes[i-1],  tip->shaftNodes[i]);
         if (!ee) {
            ////logfile<<"A shaft edge "<<tip->shaftNodes[i-1]<<" "<<tip->shaftNodes[i]<<" does not exist!"<<endl;
            return false;
         }
      }
      ////logfile<<"Shaft edges exist"<<endl;
   }
   

   // Check volume
   ////logfile<<"NumTets = "<<tet2node.size()<<endl;
   real currentVol = ComputeVolume();
   ////logfile<<"Original volume is "<<totalVolume<<" current is "<<currentVol<<endl;
   int numTets = tet2node.size();

   if (fabs((currentVol-totalVolume)/totalVolume) > 0.01) {
     // //logfile<<"Change in volume detected!"<<endl;
   }
   // First, check if the tet2tet relationship is the same as what I would compute brute force
   vector<CTet> tet2tet;
   ComputeTet2Tet(tet2tet);
   for (int i = 0; i < numTets; i++) {
      CTet& t2t = tet2tet[i];
      for (int j = 0; j < 4; j++) {
         if (Tet2Tet(i, j) != t2t[j]) {
          //  //logfile<<"Inconsistent t2t at tet "<<i<<" face "<<j<<" should be "<<t2t[j]<<" but is "<<Tet2Tet(i,j)<<endl;
            return false;
         }
      }
   }

   // Check if it's correct
   VEC3 normal;
   for (int i = 0; i < numTets; i++) {
      CTet& tet = tet2node[i];
//      CTet& t2t = tet2tet[i];
      
      // Verify opposite face
      for (int j = 0; j < 4; j++) {
         normal = (nodePos[tet[oppositeFaces[j][1]]]-nodePos[tet[oppositeFaces[j][0]]]).Cross(
                        nodePos[tet[oppositeFaces[j][2]]]-nodePos[tet[oppositeFaces[j][0]]]
               );
         if (normal.Dot(nodePos[tet[j]]-nodePos[tet[oppositeFaces[j][0]]]) > 0 ){
            return false;
         }

         int ot = Tet2Tet(i,j);
         if (ot != -1) {
            //CTet& ot2t = tet2tet[ot];
            CTetCombo c0(tet[oppositeFaces[j][0]],
               tet[oppositeFaces[j][1]],
               tet[oppositeFaces[j][2]],
               tet[j], i, j);

            bool found = false;
            CTet& otet = tet2node[ot];
            for (int k = 0; k < 4; k++) {
               if (Tet2Tet(ot,k) == i) {
                  found = true;

                  // Neighbor exist
                  // Now check if they really share the same face

                  CTetCombo c1(otet[oppositeFaces[k][0]],
                               otet[oppositeFaces[k][1]],
                               otet[oppositeFaces[k][2]],
                               otet[k], ot, k);
                  if (!c0.same(c1)) {
                     return false;
                  }                            
                  break;
               }
            }
            if (!found) {
               return false;
            }
         } 
      }

      return true;
   }

}



void CTetTopology::ComputeTet2Tet(vector<CTet>& tet2tet) {

   int numTets = tet2node.size();    

   tet2tet.resize(numTets);
   CTet* t = &tet2tet[0];
   // If no adjacent tet, it will be 0


   vector<CTetCombo> allFaces;
   int* tets = &tet2node[0][0];
   for (int i = 0; i < numTets; i++) {
      allFaces.push_back(CTetCombo(tets[2],tets[1],tets[3],tets[0],i,0));
      allFaces.push_back(CTetCombo(tets[0],tets[3],tets[2],tets[1],i,1));
      allFaces.push_back(CTetCombo(tets[0],tets[1],tets[3],tets[2],i,2));
      allFaces.push_back(CTetCombo(tets[0],tets[1],tets[2],tets[3],i,3));
      tets += 4;
   }
   sort(allFaces.begin(), allFaces.end(), CTetComboCompare());
   int nf = allFaces.size();
   //vector<int> sfaces;
   //vector<int> faces;
   //vector<int> sface2face;
   int pos = 0;
   //vector<int> numNeighbors(numTets, 0);
   //vector<pair<int, int> > face2tetv;

   // Sorting trick, assume manifoldness, this is correct
   while (pos < nf-1) {	
      if (allFaces[pos].same(allFaces[pos+1])) {
         //faces.push_back(pos);
         //face2tetv.push_back(pair<int, int>(t0, t1));
         //tet2face[4*(t0-1) + numNeighbors[t0-1]] = faces.size();
         //tet2face[4*(t1-1) + numNeighbors[t1-1]] = faces.size();
         int t0 = allFaces[pos].fromTet;
         int f0 = allFaces[pos].faceIndex;
         int t1 = allFaces[pos+1].fromTet;
         int f1 = allFaces[pos+1].faceIndex;
         tet2tet[t0][f0] = t1;
         tet2tet[t1][f1] = t0;
         pos+=2;
      } else {

         //            if (IsTetFlip(&nodePos[(allFaces[pos].v0-1)*3],&nodePos[(allFaces[pos].v1-1)*3],
         //               &nodePos[(allFaces[pos].v2-1)*3],&nodePos[(allFaces[pos].v3-1)*3])) {
         //                  swap(allFaces[pos].v0, allFaces[pos].v2);
         //            }
         //            faces.push_back(pos);
         //            sfaces.push_back(pos);
         //            sface2face.push_back(faces.size());

         int t0 = allFaces[pos].fromTet;
         int f0 = allFaces[pos].faceIndex;
         tet2tet[t0][f0] = -1; // No tet on the other side

         //          face2tetv.push_back(pair<int, int>(t0, 0));
         //        tet2face[4*(t0-1) + numNeighbors[t0-1]] = faces.size();
         //      numNeighbors[t0-1]++;

         pos+=1;
      }
   }
   if (pos == nf-1) {

      int t0 = allFaces[pos].fromTet;
      int f0 = allFaces[pos].faceIndex;
      tet2tet[t0][f0] = -1; // No tet on the other side


   }
   
}
/*

void CTetTopology::Split2Tet( vector<CTet>& tet2tet, vector<int>& groups, int whichTet, int whichFace, int* activeV, int* neighborTets, int* newTets) {
   if (tet2tet[whichTet][whichFace] == -1) {
      Error("Can't split 2 tet with non-existing neighbor");
   }

   // Figure out v0, v1, v2, v3, v4
   // Create v5
   // Figure out tf0, tf1, tf2, tf3, tf4, tf5
   int oldTet0 = whichTet;
   int oldTet1 = tet2tet[whichTet][whichFace];
   CTet& t2n0 = tet2node[oldTet0];
   CTet& t2n1 = tet2node[oldTet1];
   CTet& t2t0 = tet2tet[oldTet0];
   CTet& t2t1 = tet2tet[oldTet1];

   activeV[0] = t2n0[whichFace];
   activeV[1] = t2n0[oppositeFaces[whichFace][0]];
   activeV[2] = t2n0[oppositeFaces[whichFace][2]];
   activeV[3] = t2n0[oppositeFaces[whichFace][1]];
   neighborTets[0] = t2t0[oppositeFaces[whichFace][2]];
   neighborTets[1] = t2t0[oppositeFaces[whichFace][0]];
   neighborTets[2] = t2t0[oppositeFaces[whichFace][1]];

   for (int i = 0; i < 4; i++) {
      if (t2n1[i] == activeV[1]) {
         neighborTets[4] = t2t1[i];
      } else
      if (t2n1[i] == activeV[2]) {
         neighborTets[3] = t2t1[i];
      } else
      if (t2n1[i] == activeV[3]) {
         neighborTets[5] = t2t1[i];
      } else {
         activeV[4] = t2n1[i];
      }
   }

   int oldGroup0 = groups[oldTet0], oldGroup1 = groups[oldTet1];
   // Figure out the index of the new tets
   newTets[0] = oldTet0;
   newTets[1] = oldTet1;
   int oldNumTets = tet2node.size();
   tet2node.resize(oldNumTets + 4);
   tet2tet.resize(oldNumTets + 4);
   groups.resize(oldNumTets + 4);
   newTets[2] = oldNumTets;
   newTets[3] = oldNumTets + 1;
   newTets[4] = oldNumTets + 2;
   newTets[5] = oldNumTets + 3;

   // Figure out the new node position
   activeV[5] = nodePos.size();
   nodePos.push_back((nodePos[activeV[1]] + nodePos[activeV[2]] + nodePos[activeV[3]]) / 3.0f);
   
   // Fixing all the existing tets's topology
   if (neighborTets[0] != -1) for (int j = 0; j < 4; j++) if (tet2tet[neighborTets[0]][j] == oldTet0) {tet2tet[neighborTets[0]][j] = newTets[0];} 
   if (neighborTets[1] != -1) for (int j = 0; j < 4; j++) if (tet2tet[neighborTets[1]][j] == oldTet0) {tet2tet[neighborTets[1]][j] = newTets[1];} 
   if (neighborTets[2] != -1) for (int j = 0; j < 4; j++) if (tet2tet[neighborTets[2]][j] == oldTet0) {tet2tet[neighborTets[2]][j] = newTets[2];} 

   if (neighborTets[3] != -1) for (int j = 0; j < 4; j++) if (tet2tet[neighborTets[3]][j] == oldTet1) {tet2tet[neighborTets[3]][j] = newTets[3];} 
   if (neighborTets[4] != -1) for (int j = 0; j < 4; j++) if (tet2tet[neighborTets[4]][j] == oldTet1) {tet2tet[neighborTets[4]][j] = newTets[5];} 
   if (neighborTets[5] != -1) for (int j = 0; j < 4; j++) if (tet2tet[neighborTets[5]][j] == oldTet1) {tet2tet[neighborTets[5]][j] = newTets[4];} 

   // Now add the new tets with correct topology
   tet2node[newTets[0]] = CTet(activeV[0], activeV[3], activeV[1], activeV[5]); tet2tet[newTets[0]] = CTet(newTets[3], newTets[2], newTets[1], neighborTets[0]);
   tet2node[newTets[1]] = CTet(activeV[0], activeV[2], activeV[3], activeV[5]); tet2tet[newTets[1]] = CTet(newTets[5], newTets[0], newTets[2], neighborTets[1]);
   tet2node[newTets[2]] = CTet(activeV[0], activeV[1], activeV[2], activeV[5]); tet2tet[newTets[2]] = CTet(newTets[4], newTets[1], newTets[0], neighborTets[2]);

   tet2node[newTets[3]] = CTet(activeV[1], activeV[3], activeV[4], activeV[5]); tet2tet[newTets[3]] = CTet(newTets[5], newTets[4], newTets[0], neighborTets[3]);
   tet2node[newTets[4]] = CTet(activeV[1], activeV[4], activeV[2], activeV[5]); tet2tet[newTets[4]] = CTet(newTets[5], newTets[2], newTets[3], neighborTets[5]);
   tet2node[newTets[5]] = CTet(activeV[3], activeV[2], activeV[4], activeV[5]); tet2tet[newTets[5]] = CTet(newTets[4], newTets[3], newTets[1], neighborTets[4]);

   // Copy the group number
   groups[newTets[0]] = oldGroup0;
   groups[newTets[1]] = oldGroup0;
   groups[newTets[2]] = oldGroup0;

   groups[newTets[3]] = oldGroup1;
   groups[newTets[4]] = oldGroup1;
   groups[newTets[5]] = oldGroup1;

}

*/

// Compute the quality of a tet
real CTetTopology::TetQuality(VEC3& v0, VEC3& v1, VEC3& v2, VEC3& v3) {
   real lrms = 0;
   lrms += (v1-v0).SquaredLength();
   lrms += (v2-v0).SquaredLength();
   lrms += (v3-v0).SquaredLength();
   lrms += (v2-v1).SquaredLength();
   lrms += (v3-v1).SquaredLength();
   lrms += (v3-v2).SquaredLength();
   return orient3d(&behave, v0, v1, v2, v3) / sqrt(lrms*lrms*lrms);
}

// Check whether pts is inside or on tet or not
bool CTetTopology::InTet(VEC3& pts, int tet) {
   CTet& t2n = tet2node[tet];
   for (int j = 0; j < 4; j++) {
      if (orient3d(&behave, nodePos[t2n[oppositeFaces[j][0]]], nodePos[t2n[oppositeFaces[j][1]]], nodePos[t2n[oppositeFaces[j][2]]], pts) < 0) return false;
   }
   return true;

}

void CTetTopology::BaryCoordsAnyCoord(vector<VEC3>& nodePos, VEC3& pts, int tet, real& b0, real& b1, real& b2, real& b3) {
   CTet& t2n = tet2node[tet];
   b0 = orient3d(&behave, nodePos[t2n[oppositeFaces[0][0]]], nodePos[t2n[oppositeFaces[0][1]]], nodePos[t2n[oppositeFaces[0][2]]], pts);
   b1 = orient3d(&behave, nodePos[t2n[oppositeFaces[1][0]]], nodePos[t2n[oppositeFaces[1][1]]], nodePos[t2n[oppositeFaces[1][2]]], pts);
   b2 = orient3d(&behave, nodePos[t2n[oppositeFaces[2][0]]], nodePos[t2n[oppositeFaces[2][1]]], nodePos[t2n[oppositeFaces[2][2]]], pts);
   b3 = orient3d(&behave, nodePos[t2n[oppositeFaces[3][0]]], nodePos[t2n[oppositeFaces[3][1]]], nodePos[t2n[oppositeFaces[3][2]]], pts);
   real invSum = 1.0/(b0+b1+b2+b3);
   b0 *= invSum;
   b1 *= invSum;
   b2 *= invSum;
   b3 *= invSum;  
}

void CTetTopology::BaryCoords(VEC3& pts, int tet, real& b0, real& b1, real& b2, real& b3) {
   BaryCoordsAnyCoord(nodePos, pts, tet, b0, b1, b2, b3);
}
int CTetTopology::PointLocation(VEC3& pts, int guess) {
   return PointLocationAnyCoord(nodePos, pts, guess);
}

// Do the point location, using tet walking using the initial guess as the starting tet 
int CTetTopology::PointLocationAnyCoord(vector<VEC3>& nodePos, VEC3& pts, int guess) {
   /*
   int numTets = tet2node.size();
   for (int i = 0; i < numTets; i++) {
      CTet& t2n = tet2node[i];
      bool in = true;
      for (int j = 0; j < 4; j++) {
          real vol = orient3d(&behave, nodePos[t2n[oppositeFaces[j][0]]], nodePos[t2n[oppositeFaces[j][1]]], nodePos[t2n[oppositeFaces[j][2]]], pts);
          if (vol < 0) {
             in = false;
             break;
          }          
      }
      if (in) return i;
   }
   return -1;
   */

   CTet t2n = tet2node[guess];
   tag adj[2];
   int oldMinI = -1;
   int count = 0;
   while (count < config.pointLocationGiveUpCount) {
      real minVol = 1e10;
      int minI=-1;
      for (int j = 0; j < 4; j++) {
          if (j == oldMinI) continue;
          real vol = orient3d(&behave, nodePos[t2n[oppositeFaces[j][0]]], nodePos[t2n[oppositeFaces[j][1]]], nodePos[t2n[oppositeFaces[j][2]]], pts);
          if (vol < minVol) {
             minVol = vol;
             minI = j;
          }
      }
      

      if (minVol < 0) {
         // Not in this tet
         tetcomplexadjacencies(&mesh, t2n[oppositeFaces[minI][0]], t2n[oppositeFaces[minI][1]], t2n[oppositeFaces[minI][2]], adj);
         if (adj[0] == GHOSTVERTEX) {
            // Let's do an exhaustive check!
            int numTets = tet2node.size();
            for (int i = 0; i < numTets; i++) {
               CTet& t2n = tet2node[i];
               bool in = true;
               for (int j = 0; j < 4; j++) {
                   real vol = orient3d(&behave, nodePos[t2n[oppositeFaces[j][0]]], nodePos[t2n[oppositeFaces[j][1]]], nodePos[t2n[oppositeFaces[j][2]]], pts);
                   if (vol < 0) {
                      in = false;
                      break;
                   }          
               }
               if (in) {
                 
                  //logfile<<"My point location thinks is outside, but indeed inside!!!"<<endl;
                  
                  return i;
               }
            }
            return -1;
         }
         swap(t2n[oppositeFaces[minI][1]],t2n[oppositeFaces[minI][2]]);
         t2n[minI] = adj[0];
         oldMinI = minI;
      } else {
         if (oldMinI == -1) {
            return guess;
         } else {
            return quadHash[CSortedQuad(t2n[0], t2n[1], t2n[2], t2n[3])];
         }
      }
      count++;
   }
   return -1;


}

// Try these split and see what is the worst quality of the newly generated tet
real CTetTopology::TrySplitEdgeTetsStar(int v0, int v1, vector<int>& ring, VEC3& newPos) {
   // Now, compute the minimum quality after splitting

   int ntetsInRing = ring.size() - 1;
   real minQ = 1e10;
   
   for (int i = 0; i < ntetsInRing; i++) {
      // bottom tet
      real q = TetQuality(nodePos[v0], newPos, nodePos[ring[i+1]], nodePos[ring[i]]);
      if (q < minQ) minQ = q;

      // top tet
      q = TetQuality(newPos, nodePos[v1], nodePos[ring[i+1]], nodePos[ring[i]]);
      if (q < minQ) minQ = q;      
   }

   return minQ;
}

real CTetTopology::TrySplitBoundaryFaceStar(int whichTet, int whichFace, VEC3& newPos) { // For boundary that 
   CTet& t2n = tet2node[whichTet];
   tag adj[2];
   tetcomplexadjacencies(&mesh, t2n[oppositeFaces[whichFace][0]], t2n[oppositeFaces[whichFace][1]], t2n[oppositeFaces[whichFace][2]], adj);

   if (adj[0] != GHOSTVERTEX) return -1e10; // Not a boundary face
   int activeV[5];
   activeV[0] = t2n[whichFace];
   activeV[1] = t2n[oppositeFaces[whichFace][0]];
   activeV[2] = t2n[oppositeFaces[whichFace][2]];
   activeV[3] = t2n[oppositeFaces[whichFace][1]];

   real q, minQ = TetQuality(nodePos[activeV[0]], nodePos[activeV[3]], nodePos[activeV[1]], newPos); 
   q = TetQuality(nodePos[activeV[0]], nodePos[activeV[2]], nodePos[activeV[3]], newPos); if (q < minQ) minQ = q;
   q = TetQuality(nodePos[activeV[0]], nodePos[activeV[1]], nodePos[activeV[2]], newPos); if (q < minQ) minQ = q;
      
   return minQ;   
}


real CTetTopology::TrySplit2TetsStar(int whichTet, int whichFace, VEC3& newPos) {
   CTet& t2n = tet2node[whichTet];
   tag adj[2];
   tetcomplexadjacencies(&mesh, t2n[oppositeFaces[whichFace][0]], t2n[oppositeFaces[whichFace][1]], t2n[oppositeFaces[whichFace][2]], adj);

   if (adj[0] == GHOSTVERTEX) return -1e10;
   int activeV[5];
   activeV[0] = t2n[whichFace];
   activeV[1] = t2n[oppositeFaces[whichFace][0]];
   activeV[2] = t2n[oppositeFaces[whichFace][2]];
   activeV[3] = t2n[oppositeFaces[whichFace][1]];
   activeV[4] = adj[0];

   real q, minQ = TetQuality(nodePos[activeV[0]], nodePos[activeV[3]], nodePos[activeV[1]], newPos); 
   q = TetQuality(nodePos[activeV[0]], nodePos[activeV[2]], nodePos[activeV[3]], newPos); if (q < minQ) minQ = q;
   q = TetQuality(nodePos[activeV[0]], nodePos[activeV[1]], nodePos[activeV[2]], newPos); if (q < minQ) minQ = q;

   q = TetQuality(nodePos[activeV[1]], nodePos[activeV[3]], nodePos[activeV[4]], newPos); if (q < minQ) minQ = q;
   q = TetQuality(nodePos[activeV[1]], nodePos[activeV[4]], nodePos[activeV[2]], newPos); if (q < minQ) minQ = q;
   q = TetQuality(nodePos[activeV[3]], nodePos[activeV[2]], nodePos[activeV[4]], newPos); if (q < minQ) minQ = q;
      
   return minQ;
}

real CTetTopology::TrySplitTetStar(int whichTet, VEC3& newPos) {
   CTet& t2n = tet2node[whichTet];
   real q, minQ = TetQuality(nodePos[t2n[0]], nodePos[t2n[1]], nodePos[t2n[2]], newPos);
   q = TetQuality(nodePos[t2n[0]], nodePos[t2n[2]], nodePos[t2n[3]], newPos); if (q < minQ) minQ = q;
   q = TetQuality(nodePos[t2n[0]], nodePos[t2n[3]], nodePos[t2n[1]], newPos); if (q < minQ) minQ = q;
   q = TetQuality(nodePos[t2n[1]], nodePos[t2n[3]], nodePos[t2n[2]], newPos); if (q < minQ) minQ = q;


   return minQ;
}

real CTetTopology::TryMoveNode(int v, vector<int>& affectedTets, VEC3& newPos) {
   VEC3 backupPos = nodePos[v];
   nodePos[v] = newPos;
   int na = affectedTets.size();

   real minQ = 1e10;
   for (int i = 0; i < na; i++) {
      CTet& t2n = tet2node[affectedTets[i]];
      real q = TetQuality(nodePos[t2n[0]], nodePos[t2n[1]], nodePos[t2n[2]], nodePos[t2n[3]]);
      if (q < minQ) {
         minQ = q;
      }       
   }
   nodePos[v] = backupPos;
   return minQ;
}


void CTetTopology::ComputeWorldPosVelAccKnow(VEC3& mpts, int tet, VEC3& wpts, VEC3& wvel, VEC3& wacc) {
   CTet& t2n = tet2node[tet];
   real b0, b1, b2, b3;
   BaryCoords(mpts, tet, b0, b1, b2, b3);
   wpts = b0*fem.worldPos[t2n[0]] +
          b1*fem.worldPos[t2n[1]] +
          b2*fem.worldPos[t2n[2]] +
          b3*fem.worldPos[t2n[3]];  
   wvel = b0*fem.velocity[t2n[0]] +
          b1*fem.velocity[t2n[1]] +
          b2*fem.velocity[t2n[2]] +
          b3*fem.velocity[t2n[3]];
   wacc = b0*fem.acceleration[t2n[0]] +
          b1*fem.acceleration[t2n[1]] +
          b2*fem.acceleration[t2n[2]] +
          b3*fem.acceleration[t2n[3]];
}
int CTetTopology::ComputeWorldPosVelAccGuess(VEC3& mpts, int guessTet, VEC3& wpts, VEC3& wvel, VEC3& wacc) {
   int tet = PointLocation(mpts, guessTet);
   if (tet != -1)
   ComputeWorldPosVelAccKnow(mpts, tet, wpts, wvel, wacc);
   return tet;

}

void CTetTopology::ComputeWorldPosKnow(VEC3& mpts, int tet, VEC3& wpts) {
   CTet& t2n = tet2node[tet];
   real b0, b1, b2, b3;
   BaryCoords(mpts, tet, b0, b1, b2, b3);
   wpts = b0*fem.worldPos[t2n[0]] +
          b1*fem.worldPos[t2n[1]] +
          b2*fem.worldPos[t2n[2]] +
          b3*fem.worldPos[t2n[3]];  
}
int CTetTopology::ComputeWorldPosGuess(VEC3& mpts, int guessTet, VEC3& wpts) {
   int tet = PointLocation(mpts, guessTet);
   if (tet != -1)
   ComputeWorldPosKnow(mpts, tet, wpts);
   return tet;

}


void CTetTopology::ComputeMatPosKnow(VEC3& wpts, int tet, VEC3& mpts) {
   CTet& t2n = tet2node[tet];
   real b0, b1, b2, b3;
   BaryCoordsAnyCoord(fem.worldPos, wpts, tet, b0, b1, b2, b3);
   mpts = b0*nodePos[t2n[0]] +
          b1*nodePos[t2n[1]] +
          b2*nodePos[t2n[2]] +
          b3*nodePos[t2n[3]];  
}
int CTetTopology::ComputeMatPosGuess(VEC3& wpts, int guessTet, VEC3& mpts) {
   int tet = PointLocationAnyCoord(fem.worldPos, wpts, guessTet);
   if (tet != -1)
   ComputeMatPosKnow(wpts, tet, mpts);
   return tet;

}

// Compute the contribution of tets in this list
void CTetTopology::ComputeContribution(vector<int>& tetsList) {
   for (int i = 0; i < tetsList.size(); i++) {
      fem.ComputeTetProperties(tetsList[i]);
   }
}

// Remove the contribution of tets in this list
void CTetTopology::RemoveContribution(vector<int>& tetsList) {
   for (int i = 0; i < tetsList.size(); i++) {
      fem.SubtractTetProperties(tetsList[i]);

   }
}

void CTetTopology::DoBoundaryFaceSplit(CCutTip& cutTip, int inTet, int minFI, VEC3& newPos, real q) {
   int activeV[6];
   cutTip.SetNotTetSplit(nodePos.size(), newPos, q);

   // Need to remove contribution from 2 tets
   fem.SubtractTetProperties(inTet);

   SplitBoundaryFaceStar(inTet, minFI, activeV, newPos, cutTip.affectedTets);
   cutTip.shaftNodes.push_back(cutTip.index);    
   cutTip.shaftTypes.push_back(SHAFT_DYNAMIC);

   //logfile<<"************ Boundary Face split"<<endl;
}

void CTetTopology::DoFaceSplit(CCutTip& cutTip, int inTet, int minFI, VEC3& newPos, real q) {
   int activeV[6];
   cutTip.SetNotTetSplit(nodePos.size(), newPos, q);

   // Need to remove contribution from 2 tets
   fem.SubtractTetProperties(inTet);
   fem.SubtractTetProperties(Tet2Tet(inTet, minFI));

   Split2TetsStar(inTet, minFI, activeV, newPos, cutTip.affectedTets);
   cutTip.shaftNodes.push_back(cutTip.index);    
   cutTip.shaftTypes.push_back(SHAFT_DYNAMIC);

   /*fem.FindEdges();
   if (!VerifyValidTopology(&cutTip)) {
      //logfile<<"BAD!!"<<endl;
   }*/
   //logfile<<"************ Face split"<<endl;
}

void CTetTopology::DoEdgeSplit(CCutTip& cutTip, int v0, int v1, vector<int>& ring, VEC3& newPos, real q) {
   static vector<int> tetsR; // Tets around an edge, or a node

   // Figure out the tets around this edge from the ring
   tetsR.clear();
   for (int i = 1; i < ring.size(); i++) {
      tetsR.push_back(quadHash[CSortedQuad(v0,v1,ring[i-1], ring[i])]);
   }      
   cutTip.SetNotTetSplit(nodePos.size(), newPos, q);

   // Need to remove contribution from tets around this ring
   RemoveContribution(tetsR);

   SplitEdgeTetsStar(v0, v1, ring, tetsR, newPos, cutTip.affectedTets);
   cutTip.shaftNodes.push_back(cutTip.index);
   cutTip.shaftTypes.push_back(SHAFT_DYNAMIC);

   // Also, need to check if the shaft edge was split or not, if so, need to insert
   int ssn = cutTip.shaftNodes.size() - 1;
   for (int j = 0; j < ssn; j++) {
      if (((cutTip.shaftNodes[j] == v0) && (cutTip.shaftNodes[j+1] == v1)) ||
          ((cutTip.shaftNodes[j] == v1) && (cutTip.shaftNodes[j+1] == v0))) {

         //logfile<<"XXXXXX This case is SHAKY, you have a loop in needle trajectory XXXXXXX"<<endl;
         cutTip.shaftNodes.resize(ssn + 2);
         cutTip.shaftTypes.resize(ssn + 2);
         for (int k = ssn+1; k > j+1; k--) {
            cutTip.shaftNodes[k] = cutTip.shaftNodes[k - 1];
            cutTip.shaftTypes[k] = cutTip.shaftTypes[k-1];
         }
         cutTip.shaftNodes[j+1] = cutTip.index;
         cutTip.shaftTypes[j+1] = SHAFT_DYNAMIC; 
         ssn++;
      }
   }
   
   // modify the shaft nodes,
   /*fem.FindEdges();
   if (!VerifyValidTopology(&cutTip)) {
      //logfile<<"BAD!!"<<endl;
   }*/
   //logfile<<"************ Edge split"<<endl;
}

void CTetTopology::DoNodeSnap(CCutTip& cutTip, int nodeNum, vector<int>& surroundedTets, VEC3& newPos, real q) {
   cutTip.affectedTets = surroundedTets; // Update affected tets as well
   
   // Need to remove the contribution of tets around this node
   RemoveContribution(surroundedTets);

   nodePos[nodeNum] = newPos;

   cutTip.SetNotTetSplit(nodeNum, newPos, q);
   cutTip.shaftNodes.push_back(cutTip.index);
   cutTip.shaftTypes.push_back(SHAFT_DYNAMIC);
   /*fem.FindEdges();
   if (!VerifyValidTopology(&cutTip)) {
      //logfile<<"BAD!!"<<endl;
   }*/
   //logfile<<"************ Node snap"<<endl;

}

void CTetTopology::DoTetSplit(CCutTip& cutTip, int inTet, VEC3& newPos, real q) {
   // Good enough, do it!
   cutTip.SetTetSplit(nodePos.size(), newPos, q, tet2node[inTet], inTet); // MUST set this before calling SplitTetStar, because we use tet2node[inTet] which will be over written

   // Need to remove contribution from this tet
   fem.SubtractTetProperties(inTet);

   SplitTetStar(inTet, newPos, cutTip.affectedTets);
   cutTip.shaftNodes.push_back(cutTip.index);
   cutTip.shaftTypes.push_back(SHAFT_DYNAMIC);
   
   //logfile<<"************ Tet split"<<endl;
}

void CTetTopology::DoNodeMove(CCutTip& cutTip, VEC3& newPos, VEC3& wnewPos, VEC3& wVel, real q) {
   // Remove the contribution from the affected tets
   RemoveContribution(cutTip.affectedTets);

   // Done!
   /*
   if (ComputeWorldPosVelGuess(newPos, cutTip.affectedTets[0], fem.worldPos[cutTip.index], fem.velocity[cutTip.index]) == -1) {
      // Move outside domain, cancel
      ComputeContribution(cutTip.affectedTets);
      return false;
   }*/
   //logfile<<"************ Move node"<<endl;

   fem.worldPos[cutTip.index] = wnewPos;
   fem.velocity[cutTip.index] = wVel;
   nodePos[cutTip.index] = newPos;
   if (q > cutTip.bestMinQual) {
      cutTip.bestMinQual = q;
      cutTip.bestPos = newPos;
   }
   
   // Recompute tet properties
   ComputeContribution(cutTip.affectedTets);
}


// Currently ignore the worldpos and not update any matrices
// Move the tip node and do remeshing if necessary, return the amount of time it actually can move
real CTetTopology::MoveTip(CCutTip& cutTip, VEC3 vel, real dt) {
   VEC3 wnewPos; // World position of the tip node
   VEC3 wnewVel; // Velocity of the tip
   VEC3 wnewAcc; // Acceleration

   //tetcomplexconsistency(&mesh);

   // Try to move as far as possible, unless futile
   // Simple case first, just move the node     
   CBestRemeshOp bestRemeshOp;
   bestRemeshOp.q = -1; // Best remesh operation

   VEC3 newPos = nodePos[cutTip.index] + vel*dt;

   ComputeWorldPosVelAccGuess(newPos, cutTip.affectedTets[0], wnewPos, wnewVel, wnewAcc); 
   real q;
   if (!cutTip.firstTime) {
      if ((q = TryMoveNode(cutTip.index, cutTip.affectedTets, newPos)) > config.lbQual) {
         //cout<<"a1"<<endl;
         DoNodeMove(cutTip, newPos, wnewPos, wnewVel, q);
         return dt;
      } else {
         //cout<<"a2"<<endl;

         if (q > bestRemeshOp.q) {
            bestRemeshOp.SetNodeMove(newPos, q, dt);
         }      
      }
   }
   //logfile<<"Not OK!"<<endl;

   VEC3 backupPos = nodePos[cutTip.index];
   VEC3 backupWorldPos = fem.worldPos[cutTip.index];
   VEC3 backupVel = fem.velocity[cutTip.index];

   // Remove contribution due affected tets, we will either combine 4 tets to 1, or move node back
   RemoveContribution(cutTip.affectedTets);

   // Just move the node fail!
   if (cutTip.isTetSplit) {
      //cout<<"b1"<<endl;


      // Did a tet split, undo it
      Combine4TetsStar(cutTip.splittedTet, cutTip.splittedTetIndex);
      fem.RemoveNode(cutTip.splittedTet); // Pass in the 4 nodes that are affected by this node

      cutTip.affectedTets.resize(1);
      cutTip.affectedTets[0] = cutTip.splittedTetIndex;
      cutTip.shaftNodes.pop_back();    
      cutTip.shaftTypes.pop_back();

   } else {
      //cout<<"b2"<<endl;

      // Not a tet split

      // Figure out the world position of the old position at the present time by barycentric interpolation
      if (ComputeWorldPosVelAccGuess(cutTip.bestPos, cutTip.affectedTets[0], fem.worldPos[cutTip.index], fem.velocity[cutTip.index], fem.acceleration[cutTip.index]) == -1) {

         //cout<<"XXXXXXXXXXX FAIL COMPUTING WORLD POSITION "<<endl;
      }

      // Move node back to the best quality position
      nodePos[cutTip.index] = cutTip.bestPos;
   }

   // Recompute contribution
   ComputeContribution(cutTip.affectedTets);

   bool fail = true;
   vector<int> ring; // Ring of vertices around an edge
   vector<int> tetsR; // Tets around an edge, or a node
   //cout<<"Node move "<<bestRemeshOp.q<<endl;
   while (fail) {
      // Locate where the current node is in the mesh
      int inTet = PointLocation(newPos, cutTip.affectedTets[0]); 
      if (inTet == -1) {
         //cout<<"c1"<<endl;

         // Move outside domain!, let's forget about this case for now
         //logfile<<"Move outside domain!"<<endl;
         nodePos[cutTip.index] = backupPos;
         fem.worldPos[cutTip.index] = backupWorldPos;
         fem.velocity[cutTip.index] = backupVel;

         return 0.0f;
      }
      
//      ComputeWorldPosVelKnow(newPos, inTet, wnewPos, wnewVel); 

      // Try each of the candidate in order
      CTet& it2n = tet2node[inTet];

      // If not just split the tet, try tet splitting
      if (!cutTip.isTetSplit) {
         //cout<<"d1"<<endl;

         if (IsTetSplitLegal(cutTip, inTet)) {
            q = TrySplitTetStar(inTet, newPos);
            //cout<<"Tet split "<<q<<endl;
            if (q > config.lbQual) {

               DoTetSplit(cutTip, inTet, newPos, q);
               fail = false;
            } else 
            if (q > bestRemeshOp.q) {
               bestRemeshOp.SetTetSplit(inTet, newPos, q, dt);
            }
         }
      }
      if (!fail) break;

      // Try face splitting
      // Try all legal face
      for (int minFI = 0; minFI < 4; minFI++) {
         if (IsFaceSplitLegal(cutTip, it2n[oppositeFaces[minFI][0]], it2n[oppositeFaces[minFI][1]], it2n[oppositeFaces[minFI][2]])) {
            q = TrySplit2TetsStar(inTet, minFI, newPos);
            //cout<<"Face split "<<q<<endl;
            if (q > config.lbQual) {
               // Good enough, do it!
               DoFaceSplit(cutTip, inTet, minFI, newPos, q);

               fail = false;
               break;
            } else
            if (q > bestRemeshOp.q) {
               bestRemeshOp.SetFaceSplit(inTet, minFI, newPos, q, dt);
            }
         }
      }
      if (!fail) break;

      // Try edge splitting

      for (int minEI = 0; minEI < 6; minEI++) {
         int v0 = it2n[edges[minEI][0]],
             v1 = it2n[edges[minEI][1]],
             atip = it2n[edges[minEI][2]];
        
         FindRingAroundEdgeStar(v0, v1, atip, ring);
         if (IsEdgeSplitLegal(cutTip, v0,v1,ring)) {
            q = TrySplitEdgeTetsStar(v0, v1, ring, newPos);
            //cout<<"Edge split "<<q<<endl;
            if (q > config.lbQual) {

               DoEdgeSplit(cutTip, v0, v1, ring, newPos, q);
               
               fail = false;
               break;
            } else 
            if (q > bestRemeshOp.q) {
               bestRemeshOp.SetEdgeSplit(v0, v1, ring, newPos, q, dt);
            }

         }
      }
      if (!fail) break;
      // Try node snapping
      // Find nearest node
      for (int minNI = 0; minNI < 4; minNI++) {
         if (IsNodeSnapLegal(cutTip, it2n[minNI])) {
            FindTetsAroundNodeStar(it2n[minNI], it2n[adjFaces[minNI][0]], it2n[adjFaces[minNI][1]], tetsR);
            q = TryMoveNode(it2n[minNI], tetsR, newPos);
            //cout<<"Move node "<<q<<endl;
            if (q > config.lbQual) {
               //vector<int> oldAffectedTets = cutTip.affectedTets;
               
               DoNodeSnap(cutTip, it2n[minNI], tetsR, newPos, q);
               fail = false;
               break;
            } else
            if (q > bestRemeshOp.q) {
               bestRemeshOp.SetNodeSnap(it2n[minNI], tetsR, newPos, q, dt);
            }
         }
      }
      if (!fail) break;

      // None work :(, reduce time step
      break;
      //logfile<<"XXXXXXXXXXXXXXXXXXXX DOES NOT SUPPORT REDUCING TIME STEP NOW XXXXXXXXXXXXXXXXX"<<endl;
      Error("XXXXXXXXXXXXXXXXXXXX DOES NOT SUPPORT REDUCING TIME STEP NOW XXXXXXXXXXXXXXXXX");
      dt *= 0.5;
      newPos = backupPos + vel*dt;
      if (dt < config.giveupdt) {
         // Need to do something
         ////logfile<<"Needle to handle this case when fail at dt = 0!"<<endl;
         // Too small already, give up
       //  return 0.0f;
         break;
      }
   }   
   //cout<<"Fail!"<<endl;
   if (fail) {
      //cout<<bestRemeshOp.q<<endl;
      //return 0.0f;
      if (bestRemeshOp.q < 1e-10) {
        // fail
         return 0.0f;
      }
      dt = bestRemeshOp.dt;
      // Can't meet the quality lowerbound
      //logfile<<"The best i can do is "<<bestRemeshOp.q<<endl;
      if (bestRemeshOp.operation == NODE_MOVE) {
         if (cutTip.isTetSplit) {
            // Need to split it again
            DoTetSplit(cutTip, cutTip.splittedTetIndex, newPos, q);
         } else {
            DoNodeMove(cutTip, bestRemeshOp.newPos, wnewPos, wnewVel, bestRemeshOp.q);
            //fem.worldPos[cutTip.index] = wnewPos;

            return dt;
         }        
      } else
      if (bestRemeshOp.operation == TET_SPLIT) {
         DoTetSplit(cutTip, bestRemeshOp.inTet, bestRemeshOp.newPos, bestRemeshOp.q);
      } else
      if (bestRemeshOp.operation == FACE_SPLIT) {
         DoFaceSplit(cutTip, bestRemeshOp.inTet, bestRemeshOp.minFI, bestRemeshOp.newPos, bestRemeshOp.q);
      } else
      if (bestRemeshOp.operation == EDGE_SPLIT) {
         DoEdgeSplit(cutTip, bestRemeshOp.v0, bestRemeshOp.v1, bestRemeshOp.tets, bestRemeshOp.newPos, bestRemeshOp.q);
      } else
      if (bestRemeshOp.operation == NODE_SNAP) {
         DoNodeSnap(cutTip, bestRemeshOp.nodeNum, bestRemeshOp.tets, bestRemeshOp.newPos, bestRemeshOp.q);
      } 

   }
   // Update the world pos of the cut tip
   if (cutTip.index >= fem.worldPos.size()) {     
      fem.AddNode(cutTip.affectedTets);
   }
   fem.worldPos[cutTip.index] = wnewPos;
   fem.velocity[cutTip.index] = wnewVel;
   fem.acceleration[cutTip.index] = wnewAcc;

   // Add contribution of the affected tets
   ComputeContribution(cutTip.affectedTets);

   // No longer the first time
   cutTip.firstTime = false;
   return dt;
}

void CTetTopology::FindTetsAroundNodeStar(tag v0, tag v1, tag v2, vector<int>& surroundedTets) {

   surroundedTets.clear();
   // Expand the last visit array, if needed, initialize with 0
   lastVisit.resize(tet2node.size(), 0);
   lvIndex++;
   
   if (lvIndex == 0) {
      lvIndex = 1;
   }
   lastVisit[lvIndex % lastVisit.size()] = 0; // Reset one element

   tag adj[2];
   tetcomplexadjacencies(&mesh, v0, v1, v2, adj);

   if (adj[1] != GHOSTVERTEX) {
      int tetNum = quadHash[CSortedQuad(v0,v1,v2,adj[1])];
      lastVisit[tetNum] = lvIndex;      
      surroundedTets.push_back(tetNum);
   }

   if (adj[0] != GHOSTVERTEX) {
      int tetNum = quadHash[CSortedQuad(v0,v2,v1,adj[0])];      
      lastVisit[tetNum] = lvIndex;      
      surroundedTets.push_back(tetNum);
   }

   if (adj[0] != GHOSTVERTEX) {
      ExploreOneSided(v0, adj[0], v2, surroundedTets);
      ExploreOneSided(v0, v1, adj[0], surroundedTets);
   }
   if (adj[1] != GHOSTVERTEX) {
      ExploreOneSided(v0, v2, adj[1], surroundedTets);
      ExploreOneSided(v0, adj[1], v1, surroundedTets);
   }
}

void CTetTopology::ExploreOneSided(tag v0, tag v1, tag v2, vector<int>& surroundedTets) {
   tag adj[2];
   tetcomplexadjacencies(&mesh, v0, v1, v2, adj);
   
   if (adj[0] != GHOSTVERTEX) {
      int tetNum = quadHash[CSortedQuad(v0,v2,v1,adj[0])];
      if (lastVisit[tetNum] != lvIndex) {
         lastVisit[tetNum] = lvIndex;
         surroundedTets.push_back(tetNum);
         ExploreOneSided(v0, adj[0], v2, surroundedTets);
         ExploreOneSided(v0, v1, adj[0], surroundedTets);
      }
      // Otherwise, already visit it, nothing to do
   }
}

inline real CTetTopology::SqDistanceToLine(VEC3& v0, VEC3& v1, VEC3& v) const {
   VEC3 e = v1-v0;
   e.Normalize();
   VEC3 f = v - v0;
   real norm = e.Dot(f);
   return f.SquaredLength() - norm*norm;
}

void CTetTopology::InitTip(int index, CCutTip& cutTip) {

   
   cutTip.firstTime = true;
   cutTip.shaftNodes.clear();
   cutTip.shaftTypes.clear();
   cutTip.shaftNodes.push_back(index);
   cutTip.shaftTypes.push_back(SHAFT_DYNAMIC);
   // Find a tet that contain this index
   int numTets = tet2node.size();
   bool found = false;  
   for (int i = 0; i < numTets; i++) {
      CTet& t2n = tet2node[i];
      for (int j = 0; j < 4; j++) {
         if (t2n[j] == index) {
            // Found
            found = true;

            // tet i vertex j
            FindTetsAroundNodeStar(index, t2n[adjFaces[j][0]], t2n[adjFaces[j][1]], cutTip.affectedTets);
            real q = TryMoveNode(index, cutTip.affectedTets, nodePos[index]);
            cutTip.SetNotTetSplit(index, nodePos[index], q);
      
            break;
         }
      }
      if (found) break;
   }
   real frac = config.dt * config.needleVel * config.needleLFED;
   cutTip.xDisMat = nodePos[index] + frac*config.needleInitForwardDir;
   cutTip.yDisMat = nodePos[index] + frac*config.needleInitUpDir;

}


// Do naively for now, can do more efficient later on, if needed
bool CTetTopology::IsTetSplitLegal(CCutTip& cutTip, int tetNum) {
   // Legal when t has tip as a node
   CTet& t2n = tet2node[tetNum];
   return (t2n[0] == cutTip.index) ||
          (t2n[1] == cutTip.index) ||
          (t2n[2] == cutTip.index) ||
          (t2n[3] == cutTip.index);

}

bool CTetTopology::IsFaceSplitLegal(CCutTip& cutTip, int v0, int v1, int v2) {
   // Legal when face has tip as a node or one of its apex is tip
   if ((v0 == cutTip.index) ||
       (v1 == cutTip.index) ||
       (v2 == cutTip.index))
   return true;
   
   tag adj[2];
   tetcomplexadjacencies(&mesh, v0, v1, v2, adj);
   return ((adj[0] == cutTip.index) || (adj[1] == cutTip.index));
}

bool CTetTopology::IsEdgeSplitLegal(CCutTip& cutTip, int v0, int v1, vector<int>& ring) {
   // Legal when one end point is tip or ring is tip

   if ((v0 == cutTip.index) || (v1 == cutTip.index)) return true;
   for (int i = 0; i < ring.size(); i++) {
      if (ring[i] == cutTip.index) return true;
   }
   return false;
}

bool CTetTopology::IsNodeSnapLegal(CCutTip& cutTip, int v) {
   // When it belong to one of the affected tet
   int na = cutTip.affectedTets.size();

   if (v == cutTip.index) return false;
   for (int i = 0; i < na; i++) {
      CTet& t2n = tet2node[cutTip.affectedTets[i]];
      if ((t2n[0] == v) ||
          (t2n[1] == v) ||
          (t2n[2] == v) ||
          (t2n[3] == v)) {
          //logfile<<"Legal because cut tip is in tet "<<cutTip.affectedTets[i]<<endl;
          return true;
      }
   }
   return false;
}

bool CTetTopology::RayMeshIntersection(VEC3& pos, VEC3& dir, real& intS, real& intT, real& intD, VEC3& intPos, int& tet, int& whichFace) {
   int c;
   //vector<CTri>& tri2node = fem.tri2node;
   intD = 1e10;
   tet = -1;
   for (int i = 0; i < tet2node.size(); i++) {
      CTet& t2n = tet2node[i];
      for (int op = 0; op < 4; op++) {
         int tri[3] = {t2n[oppositeFaces[op][0]], t2n[oppositeFaces[op][1]], t2n[oppositeFaces[op][2]]};
         real intSt, intTt, intDt;
         VEC3 intPost;
         if (FindRayTriangleIntersection(nodePos[tri[0]], nodePos[tri[1]], nodePos[tri[2]], pos, dir, intSt, intTt, intDt, intPost)) {
            if (intDt < intD) {
               intD = intDt;
               intS = intSt;
               intT = intTt;
               intPos = intPost;
               tet = i;
               whichFace = op;
            }
         }
      }
   }
   return tet != -1;
}


int CTetTopology::AddBoundaryVertexFromRayMeshIntersection(VEC3& origin, VEC3& dir) {
   // Figure out interesection point
   CCutTip cutTip(config);
   real intS, intT, intD;
   VEC3 intPos;
   int tet, whichFace;

   RayMeshIntersection(origin, dir, intS, intT, intD, intPos, tet, whichFace);

   int nodes[3] = {tet2node[tet][oppositeFaces[whichFace][0]],
                   tet2node[tet][oppositeFaces[whichFace][1]],
                   tet2node[tet][oppositeFaces[whichFace][2]]};
   
   vector<int> tets;
   vector<int> ring;
   // Now figure out which course of action is best 

   real q;
   CBestRemeshOp bestOp;
   bestOp.q = -1e10;
   // Try node snapping
   for (int i = 0; i < 3; i++) {
      FindTetsAroundNodeStar(nodes[i], nodes[(i+1) % 3], nodes[(i+2) % 3], tets);
      q = TryMoveNode(nodes[i], tets, intPos);
      if (q > bestOp.q) { 
         bestOp.SetNodeSnap(nodes[i], tets, intPos, q, 0);         
      }
   }

   // Try face split
   q = TrySplitBoundaryFaceStar(tet, whichFace, intPos);
   if (q > bestOp.q) {
      bestOp.SetFaceSplit(tet, whichFace, intPos, q, 0);
   }

   // Try edge split
   for (int i = 0; i < 3; i++) {
      int v0 = nodes[i],
          v1 = nodes[(i+1) % 3],
          atip = nodes[(i+2) % 3];
        
      FindRingAroundEdgeStar(v0, v1, atip, ring);
      q = TrySplitEdgeTetsStar(v0, v1, ring, intPos);

      if (q > bestOp.q) {
         bestOp.SetEdgeSplit(v0,v1, ring, intPos, q, 0);
      }
   }

   if (bestOp.q < 0) {
      // Can't find a valid way to create the new node
      return -1;
   }

   // Let's actually do it
   if (bestOp.operation == FACE_SPLIT) {
      cout<<"Face split"<<endl;
      DoBoundaryFaceSplit(cutTip, bestOp.inTet, bestOp.minFI, bestOp.newPos, bestOp.q);
   } else
   if (bestOp.operation == EDGE_SPLIT) {
      cout<<"Edge split"<<endl;
      DoEdgeSplit(cutTip, bestOp.v0, bestOp.v1, bestOp.tets, bestOp.newPos, bestOp.q);
   } else
   if (bestOp.operation == NODE_SNAP) {
      cout<<"Node snap"<<endl;
      DoNodeSnap(cutTip, bestOp.nodeNum, bestOp.tets, bestOp.newPos, bestOp.q);
   } 

   if (bestOp.operation != NODE_SNAP) {
      // Update the world pos of the cut tip
      fem.AddNode(cutTip.affectedTets);

      fem.worldPos[cutTip.index] = nodePos[cutTip.index];
      fem.velocity[cutTip.index] = VEC3(0,0,0);
      fem.acceleration[cutTip.index] = VEC3(0,0,0);
   }
   // Add contribution of the affected tets
   ComputeContribution(cutTip.affectedTets);

   return cutTip.index;
}


void CCutTip::SaveStorage(CLinearStorage& f) {
   WriteOneBin(firstTime, f);
   WriteOneBin(index, f);
   WriteOneBin(bestPos, f);
   WriteOneBin(bestMinQual, f);
   WriteOneBin(isTetSplit, f);
   WriteOneBin(splittedTet, f);
   WriteOneBin(splittedTetIndex, f);
   WriteVecBin(affectedTets, f);
   WriteVecBin(shaftNodes, f);
   // Properties of needle

   WriteVecBin(shaftTypes, f);
   WriteOneBin(xDisMat, f); 
   WriteOneBin(yDisMat, f);
   WriteOneBin(xDisWorld, f);
   WriteOneBin(yDisWorld, f);


}

void CCutTip::LoadStorage(CLinearStorage& f) {
   ReadOneBin(firstTime, f);
   ReadOneBin(index, f);
   ReadOneBin(bestPos, f);
   ReadOneBin(bestMinQual, f);
   ReadOneBin(isTetSplit, f);
   ReadOneBin(splittedTet, f);
   ReadOneBin(splittedTetIndex, f);
   ReadVecBin(affectedTets, f);
   ReadVecBin(shaftNodes, f);


   // Properties of needle

   ReadVecBin(shaftTypes, f);
   ReadOneBin(xDisMat, f); 
   ReadOneBin(yDisMat, f);
   ReadOneBin(xDisWorld, f);
   ReadOneBin(yDisWorld, f);


}