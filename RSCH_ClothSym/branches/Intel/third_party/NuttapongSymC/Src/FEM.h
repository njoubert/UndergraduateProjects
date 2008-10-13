/*
 *  FEM.h
 *  NeedleSim
 *
 *  Created by Nuttapong Chentanez on 10/12/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __FEM_H__
#define __FEM_H__
#include "Wm4Matrix3.h"
#include <vector>

#include "globals.h"
#include "sparselib.h"
#include "mysparselib.h"
#include "myTimer.h"
#include "config.h"
#include "topology.h"

#include "debughelper.h"
using namespace sparselib;

class PCCoef{
public:
	inline real& operator [] (const int a) {
		return coef[a];
	}	
   real coef[44];
};

class CExternalForce{
public:

   CExternalForce(const int nodeNum, const VEC3 force, const real dur) : nodeNum(nodeNum), force(force), duration(dur) {
   }
   int nodeNum;
   VEC3 force;
   real duration;
};

class CVConst{
public:   
   CVConst() {
   }

   CVConst(const int nodeNum, const VEC3 vel) : nodeNum(nodeNum), vel(vel) {
   }
   int nodeNum;
   VEC3 vel;
};

class CEdge{
public:
   CEdge(const CEdge& e) {
      indL = e.indL;
      indH = e.indH;
   }

   CEdge(int i1, int i2) {
      if (i1 < i2) {
         indL = i1; 
         indH = i2;
      } else {
         indL = i2;
         indH = i1;
      }
   }
   inline bool operator == (const CEdge& b) const {
      return (indL == b.indL) && (indH == b.indH);
   }

   inline bool operator < (const CEdge& b) const{
      if (indL < b.indL) return true;
      if (indL > b.indL) return false;
      return indH < b.indH;
   }
   int indL, indH;
};

class CTip{
   // Class for storing information about the tip node
public:
   int index; // The index of the node corresponding to the tip
   VEC3 vel; 
   VEC3 dir;  // Basically normalized vel
   real speed;  // Basically len(vel)

   vector<int> affectedTets; // Tetrahedrons whose properties have to be updated  when the tip move
   // Two tets form a dual pyramid of the volume that the tip is allowed to move without damaging topology
   /*int baseTetIndex;
   int nextTetIndex;*/
   bool firstTime; // This is before the first split
   VEC3 oldCenterMatPos; // The position of the center node that needed to be reset, when the tip move outside the current dual pyramid

   // Use convention in the paper
   int activeNodes[6]; // The six vertices that formed the dual pyramid, the last one is the tip node


   void SaveStorage(CLinearStorage& f) {
      WriteOneBin(index,f);
      WriteOneBin(vel,f);
      WriteOneBin(dir,f);
      WriteOneBin(speed,f);

      WriteVecBin(affectedTets,f);
      WriteOneBin(firstTime,f);
      WriteOneBin(oldCenterMatPos,f);

      for (int i = 0; i < 6; i++) {
         WriteOneBin(activeNodes[i],f);
      }
   }
   void LoadStorage(CLinearStorage& f) {
      ReadOneBin(index,f);
      ReadOneBin(vel,f);
      ReadOneBin(dir,f);
      ReadOneBin(speed,f);

      ReadVecBin(affectedTets,f);
      ReadOneBin(firstTime,f);
      ReadOneBin(oldCenterMatPos,f);

      for (int i = 0; i < 6; i++) {
         ReadOneBin(activeNodes[i],f);
      }
   }
};

class CForceTet{
public:
   VEC3 f1,f2,f3;
};


// Store 2 neighbors of a vertex
class CLocalRot{
public:
   int n1, n2, n3; // 2 neighbors
   MATRIX3 matRot;
   MATRIX3 totalRot; // matRot*worldRot^t
};

// Jacobian block
class CJacobianBlock {
public:
   // d F_qs / d w_kl resides at
   //  R[tet2node[q]+s] + col[q][k] + l
   int col[4][4]; 
   MATRIX3 kcoef[4][4]; // Elastic stiffness matrix
   MATRIX3 dcoef[4][4]; // Damping
};


// Class for storing the normal of the 4 vertices of each tet
class CVNormals{
public:
   VEC3 N[4];

};

// Class that overload an operator that would return true if a node should be constrainted
class CConstraintDecision{
public:
   virtual bool operator () (CConfig& configi, int index, VEC3& pos) const {
      return false;
   };
};

class CIntegrator;
// FEM simulator class
class CFEMMesh {
   friend class CImplicitNewmark;
   friend class CExplicitMidpoint;
   friend class CExplicitEuler;
   friend class CImplicitEuler;
   friend class CIntegrator;
   friend class CTetTopology;
   friend class CMeshRenderer; // Allow meshrenderer to access whatever it needs to 

public: 
   

   CFEMMesh(CConfig& configi, vector<VEC3>& matPosi, vector<VEC3>& worldPosi, vector<VEC3>& velocityi, vector<VEC3>& accelerationi, vector<CTet>& tet2nodei, vector<int>& groupsi, vector<int>& fixPosNodes);
	CFEMMesh(CConfig& configi, CConstraintDecision* cond);
   CFEMMesh(CConfig& configi, CLinearStorage& f);
	~CFEMMesh();

   void CopyMatProps();

   void SaveStorage(CLinearStorage& f);
   void LoadStorage(CLinearStorage& f);

   void CommonConstructor();
   int Pick(int x, int y, int rad); // Return the vertex that is projected to within rad pixels from (x,y)
   void SVD3x3(MATRIX3& mat, MATRIX3& U, MATRIX3& D, MATRIX3& Vt);
	void Render();
   void ComputeTetProperties(int i);
   void SubtractTetProperties(int ti); // Subtract the influence of the mass of the tet ti
   void AddNode(vector<int>& affectedTets); // Prepare a storage for a node
   void RemoveNode(CTet& tet); // Remove a node, this node is due to tet splitting

	void PreComputations();

	real Simulate(real dt);
	real UpdateVelocityAndPosition(real dt);
	
   
   // Verify if the current mesh properties are correct
   bool VerifyValidMeshProperties(); 

	// Member variables
	int numNodes, numTets, numTris;
	
   CConfig& config;
   vector<CExternalForce> extForce;
   vector<VEC3> acceleration; // Acceleration
   vector<VEC3> force; // Internal force acting on the mesh
	vector<VEC3> velocity; // Velocity of nodes
	vector<VEC3> matPos; // Material coordinate position
	vector<VEC3> worldPos; // World coordinate position
   
 	vector<CTet> tet2node;
//   vector<CTet> tet2tet;
	vector<CTri> tri2node;
   vector<int> groups; // Which group does the tet belong to

   // Tet properties
	vector<CMat4x4> baryMat;
   vector<PCCoef> pcCoef;
   vector<real> tetVol;
   vector<real> tetRCond;
   vector<MATRIX3 > Bm;
   vector<CVNormals > tetVNormals;
   vector<MATRIX3 > rotation;
   //vector<CMat3x3> rotMat;

	vector<real> mass;	// Diagonal lumped mass
	real lambdas[numMaterials], mus[numMaterials], rhos[numMaterials], twoMus[numMaterials]; // Material properties
   real phis[numMaterials], psis[numMaterials], twoPhis[numMaterials];

   
   // For implicit integrator
   CMyTimer timer; // timer

   // Matrices for implicit solver, no worry for precondition because we have no time to build it
   // K is jacobian of elastic force
   // D is jacobian of damping
   CSparseRow3x3BlockSquareMatrix *K, *D; 
   bool  buildK, buildD;

   // For constraint
   vector<CVConst> pconst; // Position constraint
   vector<CVConst> vconst, permVconst; // velocity constraints, permanet velocity constraints
   vector<CVConst> vDirConst; // Constraints on the direction the velocity can be
   CTip tip; // Tip 
   CCutTip cutTip;
   
   // For display
   vector<CEdge> tetEdges;

   vector<CForceTet> forceTets;

   void PreComputeElementRotLinear();
   

   // For implicit integration
   vector<CJacobianBlock> jblocks; // the 12x12 sub block of jacobian matrix that each tet has to compute
   
   CTetTopology* topology;
   void FindEdges();
   CIntegrator* integrator;
   
 	#if (_MSC_VER >= 1300) 
		hash_map< CUniquePair, long, CUniquePairHasher> pairHash;
		hash_map< CUniquePair, long, CUniquePairHasher>::iterator hItr;
	#else
		hash_map<CUniquePair, long, CUniquePairHash> pairHash;
		hash_map<CUniquePair, long, CUniquePairHash>::iterator hItr;
	#endif

    void ComputeForce(real dt);

private:

   void AdvanceTimeForExternalForce(real dt);


   void ComputeAcceleration(real dt);

   bool EdgeExist(int v0, int v1);
   void InitializeTipInfo(); // Figure out the information necessary to do tip movement
   void FindUniqueEdges(vector<CEdge>& edges, vector<CEdge>& uedges);
   void RenderEdges(vector<CEdge>& edges, VEC3* pos);
   void RenderShaft(vector<VEC3>& nodePos);

   //void RenderTipComplex(VEC3* pos);
   void RenderAxis(VEC3& vo, VEC3& vx, VEC3& vy);

   inline void ComputeForceCauchy(MATRIX3& D, MATRIX3& theRot, int t, VEC3& f1, VEC3& f2, VEC3& f3, MATRIX3* vD);
   inline void ComputeForceIrving(MATRIX3& mat, MATRIX3& theRot, int t, VEC3& f1, VEC3& f2, VEC3& f3, MATRIX3* vMat);
   bool InTipComplex(VEC3* pos, VEC3& v);


   inline void GenRotMat(const VEC3& v0, const VEC3& v1, const VEC3& v2, MATRIX3& mat);

   void AddJacobianContribution(int t, MATRIX3& U, MATRIX3 V, real& lambda, real& twoMu, real& psi, real& twoPhis);
   void ComputeJBlock(int t);

};


#endif

