
#include "dsyevh3.h"
#include "Wm4Matrix3.h"
#include "FEM.h"
#include <cstdio>
#include <cstdlib>
#include "debughelper.h"
#ifndef NO_GRAPHICS


#include "GL/glut.h"
#endif
#include <fstream>

void WriteMFile(vector<VEC3>& vec, string name) {
	FILE* f = fopen(name.c_str(), "wt");
	int dim = vec.size();
	for (int i = 0; i < dim; i++) {
		fprintf(f, "%lg\n", vec[i][0]);
      fprintf(f, "%lg\n", vec[i][1]);
      fprintf(f, "%lg\n", vec[i][2]);
	}
	fclose(f);
}
void WriteMFile(CSparseRow3x3BlockSquareMatrix& mat, string name) {
	int nnz = 0;
   int numNodes = mat.numNodes;
   for (int i = 0; i < numNodes; i++) {
      nnz += mat.elements[i].size();
   }
   nnz *= 9;
	FILE* f = fopen(name.c_str(), "wt");

	
	fprintf(f,"%d %d %d\n", numNodes*3, numNodes*3, nnz);
	for (int i = 0; i < numNodes; i++) {
      vector<CSparseRow3x3BlockSquareMatrixElement>& vec = mat.elements[i];
      int nn = vec.size();
      for (int j = 0; j < nn; j++) {
         for (int s = 0; s < 3; s++) {
            for (int l = 0; l < 3; l++) {
               #ifdef USE_DOUBLE
                  fprintf(f, "%d %d %lg\n", i*3+s+1, vec[j].c*3+l+1, vec[j].v[s][l]);
               #else
                  fprintf(f, "%d %d %f\n", i*3+s+1, vec[j].c*3+l+1, vec[j].v[s][l]);
               #endif

            }
         }
      }

	}
	fclose(f);
}

void WriteMFile(vector<real>& vec, string name) {
	FILE* f = fopen(name.c_str(), "wt");
	int dim = vec.size();
	for (int i = 0; i < dim; i++) {
		fprintf(f, "%lg\n", vec[i]);
	}
	fclose(f);
}

void WriteMFile(Vector<real>& vec, string name) {
	FILE* f = fopen(name.c_str(), "wt");
	int dim = vec.size();
	for (int i = 0; i < dim; i++) {
		fprintf(f, "%lg\n", vec[i]);
	}
	fclose(f);
}

void WriteMFile(CCoorMatrix<real>& mat, string name) {
	int nnz = mat.nnz();
	FILE* f = fopen(name.c_str(), "wt");
	
	fprintf(f,"%d %d %d\n", mat.nrows(), mat.ncols(), mat.nnz());
	const unsigned *ip = mat.getI();
	const unsigned *jp = mat.getJ();
	real* vp = mat.getA();
	for (int i = 0; i < nnz; i++) {
		//fprintf(f, "%d %d %lf\n", mat.row_ind(i)+1, mat.col_ind(i)+1, mat.val(i));	
   #ifdef USE_DOUBLE
	   fprintf(f, "%d %d %lg\n", ip[i]+1, jp[i]+1, vp[i]);
   #else
	   fprintf(f, "%d %d %g\n", ip[i]+1, jp[i]+1, vp[i]);
   #endif
	}
	fclose(f);
}

void CFEMMesh::PreComputeElementRotLinear() {
   if (!buildK && !buildD) return;
   vector<vector<int> > nnzs;
   
   for (int i = 0; i < numNodes; i++) {
      nnzs.push_back(vector<int> ());
   }
   // Let's build jacobian
   int* tets = &tet2node[0][0];
   for (int q = 0; q < numTets; q++) {
      for (int i = 0; i < 4; i++) {
         for (int j = 0; j < 4; j++) {
            nnzs[tets[i]].push_back(tets[j]);
         }
      }
      tets+=4;
   }
   if (buildD) D = new CSparseRow3x3BlockSquareMatrix(numNodes, nnzs);
   if (buildK) K = new CSparseRow3x3BlockSquareMatrix(numNodes, nnzs);

   // Figure out the index
   if (buildD) D->ComputePairHash(pairHash); else K->ComputePairHash(pairHash); 
/*
   // Figure out jblocks
   for (int q = 0; q < numTets; q++) {
      int* tets = &tet2node[q][0];
      for (int a = 0; a < 4; a++) {
         for (int b = 0; b < 4; b++) {
            jblocks[q].col[a][b] = pairHash[CUniquePair(tets[a], tets[b])];
            int jb = jblocks[q].col[a][b];
         }
      }
   }
*/

}

void CFEMMesh::AddNode(vector<int>& affectedTets) {
   // Except matpos
   force.push_back(VEC3(0,0,0));
   velocity.push_back(VEC3(0,0,0));
   worldPos.push_back(VEC3(0,0,0));
   acceleration.push_back(VEC3(0,0,0));
   mass.push_back(0);
   numNodes++;

   // Need to update the sparse matrices and the pairHash to reflect this change as well,
   static vector<int> adjoinNodes, uniqueAdjoinNodes;
   adjoinNodes.clear();
   uniqueAdjoinNodes.clear();
   for (int i = 0; i < affectedTets.size(); i++) {
      CTet& tet = tet2node[affectedTets[i]];
      adjoinNodes.push_back(tet[0]);
      adjoinNodes.push_back(tet[1]);
      adjoinNodes.push_back(tet[2]);
      adjoinNodes.push_back(tet[3]);
   }
   CSparseRow3x3BlockSquareMatrix::FindSortedUniqueElements(adjoinNodes, uniqueAdjoinNodes);
   if (buildD) D->AddRowsAndColumns(uniqueAdjoinNodes);
   if (buildK) K->AddRowsAndColumns(uniqueAdjoinNodes);
   
   if (buildD) D->UpdatePairHash(pairHash, uniqueAdjoinNodes); else
   if (buildK) K->UpdatePairHash(pairHash, uniqueAdjoinNodes);
   

  
}

void CFEMMesh::RemoveNode(CTet& tet) { // Remove a node
   // Except matpos
   force.pop_back();
   velocity.pop_back();
   worldPos.pop_back();
   acceleration.pop_back();
   mass.pop_back();

   // POTENTIAL BUG: If we ever access non-existing node pair, we might get incorrect result because we never delete entry in pairHash
   //BUGGY NEED TO REMOVE the column and row correspond to this node in the sparse matrix too otherwise, will break (the add node won't reuse it)
   // Need to temporary decrement node count
   if (buildD) {D->RemoveLastRowColumn(tet.id, 4);}
   if (buildK) {K->RemoveLastRowColumn(tet.id, 4);}
   numNodes--;

}

void CFEMMesh::ComputeJBlock(int t) {
  
    // VERSION 2
   VEC3* N = tetVNormals[t].N;
   CMat4x4& Beta = baryMat[t];
   int* tets = &tet2node[t][0];
   CJacobianBlock& jb = jblocks[t];
   real psi = psis[groups[t]];
   real twoPhi = twoPhis[groups[t]];
   real lambda = lambdas[groups[t]];
   real twoMu = twoMus[groups[t]];

   // Figure out jblock
   for (int a = 0; a < 4; a++) {
      for (int b = 0; b < 4; b++) {
         jb.col[a][b] = pairHash[CUniquePair(tets[a], tets[b])];
      }
   }

   for (int k = 0; k < 4; k++) {
      for (int b = 0; b < 4; b++) {            
         for (int l = 0; l < 3; l++){
            for (int s = 0; s < 3; s++) {            
               real sum1 = Beta[4*k+l]*N[b][s];
               real sum2 = 0;
               //sum1*=lambda;
               //sum1*=psi;
               if (l==s) {                  
                  for (int r = 0; r < 3; r++) {
                     sum2 += Beta[4*k+r]*N[b][r];
                  }
                  //sum2 *= twoMu;
                  //sum2 *= twoPhi;
                  //cout<<sum1<<endl;
               } 
         
               //sum1 = 1;
               jb.kcoef[b][k][s][l] = sum1 * lambda + sum2 * twoMu;
               jb.dcoef[b][k][s][l] = sum1 * psi + sum2 * twoPhi;

               //K->A[K->R[3*tets[b]+s] + jblocks[t].col[b][k] + l] += sum1;
            }
         }
      }
   }
   
}


void CFEMMesh::AddJacobianContribution(int t, MATRIX3& U, MATRIX3 V, real& lambda, real& twoMu, real& psi, real& twoPhi) {
   //return ;
   
   
   /*
   VEC3* N = tetVNormals[t].N;
   CMat4x4& Beta = baryMat[t];
   int* tets = &tet2node[t][0];
   MATRIX3 tmpK, tmpD;
   
   for (int k = 0; k < 4; k++) {
      for (int b = 0; b < 4; b++) {      
         if (buildK) tmpK = (V*jblocks[t].kcoef[b][k]).TimesTranspose(V);
         if (buildD) tmpD = (V*jblocks[t].dcoef[b][k]).TimesTranspose(V);
         //tmp = (U.TransposeTimes(jblocks[t].coef[k][b])) * (V);
         //tmp = jblocks[t].coef[b][k];
         for (int l = 0; l < 3; l++){
            for (int s = 0; s < 3; s++) {
               //sum1 = 1;
               if (buildK) K->elements[tets[b]][jblocks[t].col[b][k]].v += tmpK[s][l];
               if (buildD) D->elements[tets[b]][jblocks[t].col[b][k]].v += tmpD[s][l];
            }
         }
      }
   }
   
   
   return;*/
   // VERSION 1, full blown computation
   
   VEC3* N = tetVNormals[t].N;
   CMat4x4& Beta = baryMat[t];
   int* tets = &tet2node[t][0];

   MATRIX3* Km = 0;
   MATRIX3* Dm = 0;
   for (int b = 0; b < 4; b++) {      
      for (int k = 0; k < 4; k++) {
         if (buildK) Km = &K->elements[tets[b]][jblocks[t].col[b][k]].v;
         if (buildD) Dm = &D->elements[tets[b]][jblocks[t].col[b][k]].v;
         //if (D->elements[tets[b]][jblocks[t].col[b][k]].c != tets[k]) cout<<"WRONG"<<endl;
         for (int s = 0; s < 3; s++) {
            for (int l = 0; l < 3; l++){
               real sum1 = 0;

               
               for (int i = 0; i < 3; i++) {
                  for (int h = 0; h < 3; h++) {
                     for (int q = 0; q < 3; q++) {
                        for (int r = 0; r < 3; r++) {
                           sum1 += U[r][i]*U[h][q]*Beta[4*k+h]*V[s][i]*V[l][q]*N[b][r];
                        }
                     }
                  }
               }

               //sum1*=lambda;
               real sum2 = 0;
               if (l==s) {

                  for (int r = 0; r < 3; r++) {
                     sum2 += Beta[4*k+r]*N[b][r];
                  }
                  //sum2 *= twoMu;
                  //sum1 += sum2;
                  //cout<<sum1<<endl;
               } 
               /*
               real sum3 = 0;
               if (buildD) {
                  for (int i = 0; i < 3; i++) {
                     for (int m = 0; m < 3; m++) {
                        for (int j = 0; j < 3; j++) {
                           for (int r = 0; r < 3; r++) {
                              sum3 += U[r][i]*U[m][j]*Beta[4*k+m]*V[l][i]*V[s][j]*N[b][r];
                           }
                        }
                     }
                  }
               }

         */
               //sum1 = 1;
               if (buildK) (*Km)[s][l] -= sum1*lambda + sum2 * twoMu;
               //if (buildD) (*Dm)[s][l] -= sum1*psi + 0.5*(sum3 + sum2) * twoPhi;
               if (buildD) (*Dm)[s][l] -= sum1*psi + (sum2) * twoPhi;
            }
         }
      }
   }
   
}

void CFEMMesh::RenderShaft(vector<VEC3>& nodePos) {
   #ifndef NO_GRAPHICS

   // Render the shaft
   glBegin(GL_LINE_STRIP);
   glColor3f(1,0,0);
   for (int i = 0; i < cutTip.shaftNodes.size(); i++) {
      glVertex3dv(nodePos[cutTip.shaftNodes[i]]);
   }
   glEnd();
   #endif
}

bool CFEMMesh::EdgeExist(int v0, int v1) {
   // Check if this edge exists or not 
   CEdge e(v0, v1);
   int left = 0,
       right = tetEdges.size(), mid;
   while (left + 1 < right) {
      mid = (left + right) / 2;
      if (tetEdges[mid] == e) {
         return true;
      } else 
      if (tetEdges[mid] < e) {
         left  = mid;
      } else {
         right = mid;
      }
   }
   mid = (left + right) / 2;
   return tetEdges[mid] == e;
}

bool CFEMMesh::VerifyValidMeshProperties() {
   return true;
}

extern bool saveViewingParameters;
extern double modelViewMat[16], projectionMat[16];
extern int viewport[4];
void CFEMMesh::RenderAxis(VEC3& vo, VEC3& vx, VEC3& vy) {
   #ifndef NO_GRAPHICS

   real scale = 0.3 * config.sceneScale;
   VEC3 dx = vx - vo;
   VEC3 dy = vy - vo;
   dx.Normalize();
   dy.Normalize();
   VEC3 px = vo + dx * scale;
   VEC3 py = vo + dy * scale;
   
   glBegin(GL_LINES);
   glColor3f(1,0,1);
   glVertex3dv(vo);
   glVertex3dv(px);
   glColor3f(1,1,1);
   glVertex3dv(vo);
   glVertex3dv(py);
   glEnd();
   #endif
}




void CFEMMesh::Render() {
   #ifndef NO_GRAPHICS
   glLineWidth(2.0f);

   real objScaling = config.objRndrSize / config.sceneScale;
   glDisable(GL_DEPTH_TEST);
   glPointSize(10.0f);
   glMatrixMode(GL_MODELVIEW);
	int* tri;

   glPushMatrix();
   // Render world
   glTranslatef(config.objRndrDispWorld[0], config.objRndrDispWorld[1], config.objRndrDispWorld[2]);
   glScalef(objScaling, objScaling, objScaling);
	glColor3f(0.0f, 0.0f, 1.0f);
   RenderEdges(tetEdges, &worldPos[0]);

   if (saveViewingParameters) {
      glGetDoublev(GL_MODELVIEW_MATRIX, modelViewMat);
      glGetDoublev(GL_PROJECTION_MATRIX, projectionMat);   
      //saveViewingParameters = false;
   }
   RenderShaft(worldPos);

   // Render othonormal frame
   RenderAxis(worldPos[cutTip.index], cutTip.xDisWorld, cutTip.yDisWorld);

   glColor3f(1,1,0);
   glBegin(GL_LINES);
   for (int i = 0; i < cutTip.affectedTets.size(); i++) {
      for (int j = 0; j < 6; j++) {
         glVertex3dv(worldPos[tet2node[cutTip.affectedTets[i]][CTetTopology::edges[j][0]]]);
         glVertex3dv(worldPos[tet2node[cutTip.affectedTets[i]][CTetTopology::edges[j][1]]]);
      }
   }
   glEnd();

   // Now let's look at Jacobian
   //Look at jacobian
   real scale = objScaling;   
   /*
   Vector<real> dirL(numNodes*3);
   VEC3* dir = (VEC3*)&dirL[0];
   for (int i = 0; i < numNodes; i++) {
      dir[i] = VEC3(1,0,0);
   }
   dir[0] = VEC3(1,0,0);
   Vector<real> dforceL(numNodes*3);
   VEC3* dforce = (VEC3*)&dforceL[0];
   dforceL = (*K)*dirL;
   glBegin(GL_LINES);
   
   unsigned* rr = K->R.begin();
   unsigned* jj = K->J.begin();
   real* vv = K->A.begin();
   for (int i = 0; i < numNodes*3; i++) {
      int nw = rr[i+1]-rr[i];;
      real tmp = 0;
      for (int j = 0; j < nw; j++) {
         tmp += dirL[jj[j]]*vv[j];
      }
      jj+=nw;
      vv+=nw;
      dforceL[i] = tmp;

   }

   for (int i = 0; i < numNodes; i++) {
      glColor3f(1.0f, 1.0f, 1.0f);
      glVertex3f(worldPos[i][0], worldPos[i][1], worldPos[i][2]);		
      glColor3f(1.0f, 0.0f, 0.0f);
      VEC3 target = worldPos[i] + 0.1*dforce[i];
      glVertex3f(target[0], target[1], target[2]);
   }
   glEnd();
  
   WriteMFile(CCoorMatrix<real>(*K), "km.mat");
   WriteMFile(dforceL, "dv2.vec");
   WriteMFile(dirL, "dl.vec");

*/


   if (!tip.firstTime) {
      glColor3f(0,0,0.5);
      //RenderTipComplex(&worldPos[0]);
   }

   // Hightlight tip node
   glBegin(GL_POINTS);
   glColor3f(1.0f, 0.0f, 0.0f);
   if (tip.index != -1) {
      glVertex3f(worldPos[tip.index][0], worldPos[tip.index][1], worldPos[tip.index][2]);
   }
   glEnd();


	glPopMatrix();

   // Render material
   glPushMatrix();
   glTranslatef(config.objRndrDispMat[0], config.objRndrDispMat[1], config.objRndrDispMat[2]);
   glScalef(objScaling, objScaling, objScaling);
	glColor3f(0.0f, 1.0f, 0.0f);
   RenderEdges(tetEdges, &matPos[0]);

   if (!tip.firstTime) {
      glColor3f(0,0.5,0.0);
      //RenderTipComplex(&matPos[0]);
   }
   RenderShaft(matPos);
   // Render othonormal frame
   RenderAxis(matPos[cutTip.index], cutTip.xDisMat, cutTip.yDisMat);

   glColor3f(1,1,0);
   glBegin(GL_LINES);
   for (int i = 0; i < cutTip.affectedTets.size(); i++) {
      for (int j = 0; j < 6; j++) {
         glVertex3dv(matPos[tet2node[cutTip.affectedTets[i]][CTetTopology::edges[j][0]]]);
         glVertex3dv(matPos[tet2node[cutTip.affectedTets[i]][CTetTopology::edges[j][1]]]);
      }
   }
   glEnd();

   // Hightlight tip node
   glBegin(GL_POINTS);
   glColor3f(1.0f, 0.0f, 0.0f);
   if (tip.index != -1) {
      glVertex3f(matPos[tip.index][0], matPos[tip.index][1], matPos[tip.index][2]);
   }
   glEnd();

   glPopMatrix();
   
	// Render force field
   glPushMatrix();
   glTranslatef(config.objRndrDispWorld[0], config.objRndrDispWorld[1], config.objRndrDispWorld[2]);
   glScalef(objScaling, objScaling, objScaling);

   
   glBegin(GL_LINES);
//   real scale = 0.0001f * objScaling;
	for (int i = 0; i < numNodes; i++) {
		glColor3f(1.0f, 1.0f, 1.0f);
		///glVertex3f(worldPos[i][0], worldPos[i][1], worldPos[i][2]);		
		glColor3f(1.0f, 0.0f, 0.0f);
		VEC3 target = worldPos[i] + scale*force[i];
		//glVertex3f(target[0], target[1], target[2]);
	}
	glEnd();

   

   glPopMatrix();
   #endif
}

CFEMMesh::CFEMMesh(CConfig& configi, CLinearStorage& f) : config(configi), cutTip(configi) {
   LoadStorage(f);
}

void CFEMMesh::SaveStorage(CLinearStorage& f) {
   WriteVecBin(matPos, f);
   WriteVecBin(worldPos,f);
   WriteVecBin(velocity,f);
   WriteVecBin(acceleration,f);
   WriteVecBin(tet2node, f);
   WriteVecBin(groups, f);
   WriteVecBin(permVconst, f);
   WriteVecBin(mass, f);
   WriteVecBin(force, f);
   cutTip.SaveStorage(f);
   tip.SaveStorage(f);
}
void CFEMMesh::LoadStorage(CLinearStorage& f) {
   ReadVecBin(matPos, f);
   ReadVecBin(worldPos,f);
   ReadVecBin(velocity,f);
   ReadVecBin(acceleration,f);
   ReadVecBin(tet2node, f);
   ReadVecBin(groups, f);
   ReadVecBin(permVconst, f);

   CopyMatProps();
   numNodes = matPos.size();
   numTets = tet2node.size();

   mass.resize(numNodes,0);  
   force.resize(numNodes);
   CommonConstructor();


 	for (int i = 0; i < numTets; i++)  {
      ComputeTetProperties(i);
	}
   
   ReadVecBin(mass, f);
   ReadVecBin(force, f); // Need force because use to make decision about cut

   cutTip.LoadStorage(f);
   tip.LoadStorage(f);

}