#ifndef __TOPOLOGY_H__
#define __TOPOLOGY_H__
#define NO_TIMER
#ifdef USE_DOUBLE
#else
   #define SINGLE
#endif

#include "star.h"

#include "globals.h"
#include <vector>

#ifdef __GNUC__
#include <ext/hash_map>
using namespace __gnu_cxx;
#else
#include <hash_map>
using namespace stdext;
#endif
#include <algorithm>
#include "config.h"
using namespace std;

/*
class CSortedPair{
public:
   CSortedPair(long a, long b) {
      if (a < b) {
         x = a; 
         y = b;
      } else {
         x = b;
         y = a;
      }
   }

   CSortedPair& operator = (const CSortedPair& a) {
      x = a.x;
      y = a.y;

      return *this;
   }
   bool operator == (const CSortedPair& b) const {
      return (x == b.x) && (y == b.y);
   }
   long x, y;
};

class CSortedPairHash{
public:
   size_t operator()(const CSortedPair& a) const{
      return (a.x*73856093) ^ (a.y*19349663);
   }
};

#if (_MSC_VER >= 1300) 
class CSortedPairHasher : public stdext::hash_compare <CSortedPair> {
public:

   size_t operator() (const CSortedPair& a) const
   {
      return (a.x*73856093) ^ (a.y*19349663);
   }

   bool operator() (const CSortedPair& a, const CSortedPair& b) const
   {
      if (a.x < b.x) return true;
      if (a.x > b.x) return false;

      return (a.y < b.y);
   }
};
#endif
*/
class CSortedQuad{
public:

   CSortedQuad(const CTet& t) {
      quad[0] = t[0];
      quad[1] = t[1];
      quad[2] = t[2];
      quad[3] = t[3];
      sort(quad, quad+4);
   }

   CSortedQuad(int a, int b, int c, int d) {
      quad[0] = a;
      quad[1] = b;
      quad[2] = c;
      quad[3] = d;
      sort(quad, quad+4);
   }

   CSortedQuad& operator = (const CSortedQuad& a) {
      quad[0] = a.quad[0];
      quad[1] = a.quad[1];
      quad[2] = a.quad[2];
      quad[3] = a.quad[3];

      return *this;
   }
   bool operator == (const CSortedQuad& b) const {
      return (quad[0] == b.quad[0]) && 
             (quad[1] == b.quad[1]) && 
             (quad[2] == b.quad[2]) && 
             (quad[3] == b.quad[3]);
   }

   int quad[4];

};

class CSortedQuadHash{
public:
   size_t operator()(const CSortedQuad& a) const{
      return (a.quad[0] * 2051537) ^ (a.quad[1] * 73856093) ^ (a.quad[2] * 19349663) ^ (a.quad[3] * 500003909);
   }
};

#if (_MSC_VER >= 1300) 
class CSortedQuadHasher : public stdext::hash_compare <CSortedQuad> {
public:

   size_t operator() (const CSortedQuad& a) const
   {
      return (a.quad[0] * 2051537) ^ (a.quad[1] * 73856093) ^ (a.quad[2] * 19349663) ^ (a.quad[3] * 500003909);
   }

   bool operator() (const CSortedQuad& a, const CSortedQuad& b) const
   {
      if (a.quad[0] < b.quad[0]) return true;
      if (a.quad[0] > b.quad[0]) return false;
      if (a.quad[1] < b.quad[1]) return true;
      if (a.quad[1] > b.quad[1]) return false;
      if (a.quad[2] < b.quad[2]) return true;
      if (a.quad[2] > b.quad[2]) return false;
      return (a.quad[3] < b.quad[3]);      

   }
};
#endif

class CFEMMesh;
class CTetTopology;

enum ShaftType{SHAFT_STATIC, SHAFT_DYNAMIC};
class CCutTip{
public:
   CCutTip(CConfig& configi);

   CConfig& config;
   bool firstTime;           // Can't do node move!
   int index;
   VEC3 bestPos;              // Best position to put the node at, along the trajectory
   real bestMinQual;         // Best minimum quality
   bool isTetSplit;          // Whether the tip comes from the tet split or not, if so the node, will only exist temporarily
   CTet splittedTet;         // Used for only the case when a tet is splited
   int splittedTetIndex;     // The index of the tet that was splitted
   vector<int> affectedTets; // Tets that are affected by this tip's movement

   // Not a tet split
   void SetNotTetSplit(int indexi, VEC3& bestPosi, real bestMinQuali) {
      index = indexi;
      bestPos = bestPosi;
      bestMinQual = bestMinQuali;
      isTetSplit = false;
   }

   // Tet split
   void SetTetSplit(int indexi, VEC3& bestPosi, real bestMinQuali, CTet& splittedTeti, int splittedTetIndexi) {
      index = indexi;
      bestPos = bestPosi;
      bestMinQual = bestMinQuali;
      isTetSplit = true;
      splittedTet = splittedTeti;
      splittedTetIndex = splittedTetIndexi;
   }
   void GetCoordSystem(CTetTopology& top, VEC3& x, VEC3& y, VEC3& z); // x is needle forward dir, y is current up

   vector<int> shaftNodes; // The nodes that represent needle shaft

   // Properties of needle

   vector<ShaftType> shaftTypes; // The type of each shaft node
//   MATRIX3 orientation; // [x y z], x indicates forward direction, y indicate up direction, z indicate the direction to rotate around in world space
   VEC3 xDisMat, yDisMat; // Two points used to figure out the local frame
   VEC3 xDisWorld, yDisWorld;

   real MoveTipForwardManual(CTetTopology& top, real dt); // Perform the remeshing step, ignore any physical thing

   real MoveNeedleForwardAndApplyConstraintsAndForce(CTetTopology& top, real dt); // Consider if needle tip should move forward, apply constraints and force along shaft and tip
   void SaveStorage(CLinearStorage& f);
   void LoadStorage(CLinearStorage& f);   
};

enum RemeshOperation {NODE_MOVE, TET_SPLIT, FACE_SPLIT, EDGE_SPLIT, NODE_SNAP};

// Record the best operation that could be done for remeshing
class CBestRemeshOp{
public:
   real q; // Quality
   real dt;
   VEC3 newPos;
   int inTet;
   int v0, v1;
   int nodeNum;  
   int minFI;
   RemeshOperation operation; // Which remesh operation are we using
   vector<int> tets;


   void SetNodeMove(VEC3& newPosi, real qi, real dti) {

      newPos = newPosi;
      q = qi;
      dt = dti;
      operation = NODE_MOVE;
   }

   void SetTetSplit(int inTeti, VEC3& newPosi, real qi, real dti) {
      inTet = inTeti;
      newPos = newPosi;
      q = qi;
      dt = dti;
      operation = TET_SPLIT;
   }
   void SetFaceSplit(int inTeti, int minFIi, VEC3& newPosi, real qi, real dti) {
      inTet = inTeti;
      minFI = minFIi;
      newPos = newPosi;
      q = qi;
      dt = dti;
      operation = FACE_SPLIT;
   }
   void SetEdgeSplit(int v0i, int v1i, vector<int>& ringi, VEC3& newPosi, real qi, real dti) {
      v0 = v0i;
      v1 = v1i;
      tets = ringi;
      newPos = newPosi;
      q = qi;
      dt = dti;
      operation = EDGE_SPLIT;

   }
   void SetNodeSnap(int nodeNumi, vector<int>& surroundedTetsi, VEC3& newPosi, real qi, real dti) {
      nodeNum = nodeNumi;
      tets = surroundedTetsi;
      newPos = newPosi;
      q = qi;
      dt = dti;
      operation = NODE_SNAP;
   }
};



class CTetTopology {
   friend class CCutTip;
public:

   struct behavior behave;
   vector<CTet>& tet2node;
   vector<VEC3>& nodePos;
   vector<int>& groups;
   real totalVolume;
   CFEMMesh& fem;
   CConfig& config;

   // Initialize the topology
   ~CTetTopology();
   CTetTopology(CFEMMesh& femi);
   

   // Add a new boundary vertex, whose position is computed by the ray mesh intersection. The ray starts at the origin in the direction dir. The mesh quality is maximized with either face split, edge split, or node snap as good as possible
   int AddBoundaryVertexFromRayMeshIntersection(VEC3& origin, VEC3& dir);

   real ComputeVolume(); // Compute the total volume
   // Initialize the cut tip, given an index, a bit expensive, use sparsely
   void InitTip(int index, CCutTip& cutTip); 

   // Move the tip node and do remeshing if necessary, return the amount of time it actually can move
   real MoveTip(CCutTip& cutTip, VEC3 vel, real dt);

   // Find tets around node v0, that has a face (v0,v1,v2) 
   void FindTetsAroundNodeStar(tag v0, tag v1, tag v2, vector<int>& surroundedTets);

   // Find what nodes are around an edge
   void FindRingAroundEdgeStar(int v0, int v1, int atip, vector<int>& ring);

   // Find what tets are around an edge given nodes around an edge
   void FindTetsAroundEdgeStar(int v0, int v1, int atip, vector<int>& tets, vector<int>& ring);

   // Split the tets around an edge (v0,v1) that consist of the ring
   // A node position is computed and append, the list of new tets is returned
   // tet2node is also modified
   void SplitEdgeTetsStar(int v0, int v1, vector<int>& ring, vector<int>& tets, VEC3& newPos, vector<int>& newTets);

   // Split the 2 tets adjointing tets into 6 tets (the 2 tets are specified by whichTet and whichFace), tet2node is modified
   // activeV will contain 6 vertices 
   void Split2TetsStar(int whichTet, int whichFace, int* activeV, VEC3& newPos, vector<int>& newTets);


   // Split the boundary face
   void SplitBoundaryFaceStar(int whichTet, int whichFace, int* activeV, VEC3& newPos, vector<int>& newTets);

   // Split one tet into 4 tets
   void SplitTetStar(int whichTet, VEC3& newPos, vector<int>& newTets);

   // Combine 4 tets, only work when this is the last topological operation
   void Combine4TetsStar(CTet& t2n, int oldTetIndex); // This works only when the 4 tets was the most recent topological operation
   
   // Compute the quality of a tet
   inline real TetQuality(VEC3& v0, VEC3& v1, VEC3& v2, VEC3& v3);

   // Do the point location, using tet walking using the initial guess as the starting tet 
   inline int PointLocationAnyCoord(vector<VEC3>& nodePos, VEC3& pts, int guess);
   inline int PointLocation(VEC3& pts, int guess); // Material coord
   
   // Ray-mesh, intersection. Find intersection of a ray and mesh 
   inline bool RayMeshIntersection(VEC3& pos, VEC3& dir, real& intS, real& intT, real& intD, VEC3& intPos, int& tet, int& whichFace);

   // Compute the barycentric coordinate
   inline void BaryCoords(VEC3& pts, int tet, real& b0, real& b1, real& b2, real& b3); 
   inline void BaryCoordsAnyCoord(vector<VEC3>& nodePos, VEC3& pts, int tet, real& b0, real& b1, real& b2, real& b3); 

   // Given a material position and a guess tetrahedra the the mpts is in, find the correct tetrahedra and use it to compute barycentrically interpolated world position. Return the actual tet that mpts is in
   int ComputeWorldPosVelAccGuess(VEC3& mpts, int guessTet, VEC3& wpts, VEC3& wvel, VEC3& wacc); // Use this when we guess
   void ComputeWorldPosVelAccKnow(VEC3& mpts, int tet, VEC3& wpts, VEC3& wvel, VEC3& wacc); //  Use this when we know exactly what tet it is 
   int ComputeWorldPosGuess(VEC3& mpts, int guessTet, VEC3& wpts); // Use this when we guess
   void ComputeWorldPosKnow(VEC3& mpts, int tet, VEC3& wpts); //  Use this when we know exactly what tet it is 

   int ComputeMatPosGuess(VEC3& wpts, int guessTet, VEC3& mpts); // Use this when we guess
   void ComputeMatPosKnow(VEC3& wpts, int tet, VEC3& mpts); //  Use this when we know exactly what tet it is 


   // Check whether pts is inside or on tet or not
   bool InTet(VEC3& pts, int tet);
   
   // Try these split and see what is the worst quality of the newly generated tet
   real TryMoveNode(int v, vector<int>& affectedTets, VEC3& newPos);
   real TrySplitEdgeTetsStar(int v0, int v1, vector<int>& ring, VEC3& newPos);
   real TrySplit2TetsStar(int whichTet, int whichFace, VEC3& newPos);
   real TrySplitBoundaryFaceStar(int whichTet, int whichFace, VEC3& newPos); // For face that 
   real TrySplitTetStar(int whichTet, VEC3& newPos);

   // Check legal, in topological sense, may still need to check in geometric sense
   bool IsTetSplitLegal(CCutTip& cutTip, int tetNum);
   bool IsFaceSplitLegal(CCutTip& cutTip, int v0, int v1, int v2);
   bool IsEdgeSplitLegal(CCutTip& cutTip, int v0, int v1, vector<int>& ring);
   bool IsNodeSnapLegal(CCutTip& cutTip, int v);


   void WriteMeshStar(const string fname);
   struct tetcomplex mesh;

	#if (_MSC_VER >= 1300) 
		hash_map< CSortedQuad, long, CSortedQuadHasher> quadHash;
		hash_map< CSortedQuad, long, CSortedQuadHasher>::iterator qItr;
	#else
		hash_map<CSortedQuad, long, CSortedQuadHash> quadHash;
		hash_map<CSortedQuad, long, CSortedQuadHash>::iterator qItr;
	#endif

      /*
	#if (_MSC_VER >= 1300) 
		hash_map< CSortedPair, long, CSortedPairHasher> pairHash;
		hash_map< CSortedPair, long, CSortedPairHasher>::iterator pItr;
	#else
		hash_map<CSortedPair, long, CSortedPairHash> pairHash;
		hash_map<CSortedPair, long, CSortedPairHash>::iterator pItr;
	#endif
*/
   int Tet2Tet(int t, int f); // Return the index of the adjacent tet
   int Tet2Tet(int t, int f, tag& tip); // Return the index of the adjacent tet, also return the vertex of the other tet

   static int oppositeFaces[4][3]; // The opposite face of each vertex
   static int dualPyramidFaces[6][3]; // Outward facing faces of the dual pyramid
   static int edges[6][3]; // Edges and a tip, with outward facing orientation
   static int adjFaces[4][2]; // A face adjoint to each node with outward facing orientation (i, adjFaces[i][0], adjFaces[i][1])

   // Compute a conservative estimate on the radius of the sphere, where we can move the node within freely without creating inverted elements
   void ComputeVertexSafeRadius(vector<VEC3>& nodePos, vector<real>& radius); // Compute safe radius of a vertex
   
   // Compute a conservative estimate on the radius of the cylinder around each edge, where we can move freely without creating inverted element
   //void ComputeEdgeSafeRadius(vector<VEC3>& nodePos, vector<real>& nradius, vector<real>& eradius); // Not implemented

   inline real DistanceToPlane(VEC3& v0, VEC3& v1, VEC3& v2, VEC3& v) const;
   inline real SqDistanceToLine(VEC3& v0, VEC3& v1, VEC3& v) const;
   bool VerifyValidTopology(CCutTip* tip = 0);
   
   /// --------------------- Below is obsolete ---------------------
   // Compute the tet2tet, given tet2node and nodepos
   
   void ComputeTet2Tet(vector<CTet>& tet2tet);

   /*
   // Split 2 tets into 6 tets
   static void Split2Tet(vector<CTet>& tet2tet, vector<int>& groups, int whichTet, int whichFace, int* activeV, int* neighborTets, int* newTets);
   // Verify that the topology of the tetrahedral mesh is valid
   // For debugging only

   // Write mesh to file
   static void WriteMesh(vector<int>& groups, const string fname);
   */

   //--------------------------------------------------------------------

private:

   // Compute the contribution of tets in this list
   void ComputeContribution(vector<int>& tetsList);

   // Remove the contribution of tets in this list
   void RemoveContribution(vector<int>& tetsList);

   void ExploreOneSided(tag v0, tag v1, tag v2, vector<int>& surroundedTets);
   vector<unsigned> lastVisit; // The last time this tet is visited, used in FindTetsAroundNodeStar, will be periodically cleared
   unsigned lvIndex; // The current "time"


   void DoNodeMove(CCutTip& cutTip, VEC3& newPos, VEC3& wnewPos, VEC3& wVel, real q);
   void DoTetSplit(CCutTip& cutTip, int inTet, VEC3& newPos, real q);
   void DoFaceSplit(CCutTip& cutTip, int inTet, int minFI, VEC3& newPos, real q);
   void DoBoundaryFaceSplit(CCutTip& cutTip, int inTet, int minFI, VEC3& newPos, real q);
   void DoEdgeSplit(CCutTip& cutTip, int v0, int v1, vector<int>& ring, VEC3& newPos, real q);
   void DoNodeSnap(CCutTip& cutTip, int nodeNum, vector<int>& surroundedTets, VEC3& newPos, real q);
   

};

#endif


/*
Do plane sphere intersection, see Wiki,
then find the maxima of projection onto edge
then find distance to edge of that point.

*/