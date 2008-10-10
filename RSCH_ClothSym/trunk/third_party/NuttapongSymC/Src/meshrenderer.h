#ifdef OGRE
#ifndef __MESHRENDERER_H__
#define __MESHRENDERER_H__

#include "FEM.h"

#include "OgreCEGUIRenderer.h"
#include "OgreCEGUIResourceProvider.h"
#include "ogrecolourvalue.h"
#include "ogremeshmanager.h"
#include "ogrehardwarebuffermanager.h"
#include "ogresubmesh.h"
#include "ogreentity.h"
#include "ogrebillboard.h"
#include "ogrebillboardset.h"
#include <vector>

using namespace Ogre;
using namespace std;
// Whatever has to be changed every frame
class CVertexB0 {
public:
   Wm4::Vector3f pos;
};

class CVertexB1 {
public:
   Wm4::Vector3f normal;
};

// Whatever does not have to be changed 
class CVertexB2 {
public:
   RGBA color;  // Just color for now
};

// Index buffer of triangle
class CIndexB {
public:
   unsigned short ind[3];
   unsigned short & operator [] (const int i) {
      return ind[i];
   }
};

class CMeshRenderer {
public:
   CMeshRenderer( SceneManager *mgri, CFEMMesh* femi, const string meshNamei, const string entNamei, const string matNamei, bool worldMeshi, bool showRadius);
   void UpdateVertices();
   void SetPosition(const VEC3& pos);
   void SetScale(const real sc);

public:

   bool showRadius;
   vector<CVertexB0> cpuvbuf0;
   vector<CVertexB1> cpuvbuf1;
   vector<CVertexB2> cpuvbuf2;
   vector<CIndexB> cpuibuf0;

   CFEMMesh* fem;
   bool worldMesh;
   string meshName;
   string entName;
   string matName;
   int numNodes;
   int numTris;
   MeshPtr msh;
   SceneManager *mgr;
   SubMesh* subMesh;
   Entity* ent;
   SceneNode* thisSceneNode;
   HardwareVertexBufferSharedPtr vbuf0, vbuf1, vbuf2;
   HardwareIndexBufferSharedPtr ibuf;

   BillboardSet* bbset;
   vector<Billboard*> verticesbb;

   // Where to place this in the world
   VEC3 position;
   real scale;
   vector<SceneNode*> verticesSceneNode;
   vector<Entity*> verticesEntity;

private:
   void ComputeVerticesNormal();

};
#endif
#endif
