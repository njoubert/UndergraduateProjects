#ifdef OGRE

#include "meshrenderer.h"
#include "ogrerendersystem.h"
#include "ogreroot.h"
#include "topology.h"
void CMeshRenderer::ComputeVerticesNormal() {
   for (int i = 0; i < numNodes; i++) {
      cpuvbuf1[i].normal = Wm4::Vector3f::ZERO;
   }

   for (int i = 0; i < numTris; i++) {
      Wm4::Vector3f v0 = cpuvbuf0[cpuibuf0[i][1]].pos - cpuvbuf0[cpuibuf0[i][0]].pos;
      Wm4::Vector3f v1 = cpuvbuf0[cpuibuf0[i][2]].pos - cpuvbuf0[cpuibuf0[i][0]].pos;
      Wm4::Vector3f normal = v0.Cross(v1);
      cpuvbuf1[cpuibuf0[i][0]].normal += normal;
      cpuvbuf1[cpuibuf0[i][1]].normal += normal;
      cpuvbuf1[cpuibuf0[i][2]].normal += normal;
   }

   for (int i = 0; i < numNodes; i++) {
      real slen = cpuvbuf1[i].normal.SquaredLength();
      if (slen > 0.0f) {
         cpuvbuf1[i].normal /= sqrtf(slen) * 0.5;
      }
   }
}

void CMeshRenderer::UpdateVertices() {
   if ((numNodes != fem->numNodes) || (numTris != fem->numTris)) {
      numNodes = fem->numNodes;
      numTris = fem->numTris;
     
      cpuvbuf0.resize(numNodes);
      cpuvbuf1.resize(numNodes);
      cpuvbuf2.resize(numNodes);
      cpuibuf0.resize(numTris);

     
      // Copy topology
      for (int i = 0; i < numTris; i++) {
         cpuibuf0[i][0] = fem->tri2node[i][0];
         cpuibuf0[i][1] = fem->tri2node[i][1];
         cpuibuf0[i][2] = fem->tri2node[i][2];
      }

      for (int i = 0; i < numNodes; i++) {
         cpuvbuf2[i].color = ColourValue(1.0,0.0,0.0).getAsRGBA();
      }

      // Create vertex and index buffer

      /// Create the msh via the MeshManager
            
      msh = (MeshPtr)MeshManager::getSingleton().getByName(meshName);

      if (!msh.isNull()) {
         
         thisSceneNode->detachObject(ent);
         mgr->destroyEntity(ent);

         // Remove the resource if already exists
         MeshManager::getSingleton().remove(msh->getHandle());
         
         if (showRadius) {
            for (int i = 0; i < verticesEntity.size(); i++) {

               verticesSceneNode[i]->detachObject(verticesEntity[i]);
               mgr->destroyEntity(verticesEntity[i]);         
               mgr->destroySceneNode(verticesSceneNode[i]->getName());
               
            }
            verticesEntity.clear();
            verticesSceneNode.clear();
         }
         thisSceneNode->detachObject(bbset);
         mgr->destroyBillboardSet(bbset);

       
      }


      msh = MeshManager::getSingleton().createManual(meshName,
      ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
      subMesh = msh->createSubMesh();
      subMesh->useSharedVertices=false;

      // Vertex buffers
      subMesh->vertexData = new VertexData();
      subMesh->vertexData->vertexStart = 0;
      subMesh->vertexData->vertexCount = numNodes;

      VertexDeclaration* vdecl = subMesh->vertexData->vertexDeclaration;
      VertexBufferBinding* vbind = subMesh->vertexData->vertexBufferBinding;


      vdecl->addElement(0, 0, VET_FLOAT3, VES_POSITION);
      vdecl->addElement(1, 0, VET_FLOAT3, VES_NORMAL);
      vdecl->addElement(2, 0, VET_COLOUR, VES_DIFFUSE);

      // Prepare buffer for positions - todo: first attempt, slow
      vbuf0 =
            HardwareBufferManager::getSingleton().createVertexBuffer(
               3*sizeof(float),
		      numNodes,
		      HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE, false);

      vbind->setBinding(0, vbuf0);

      // Prepare buffer for normals - write only
      vbuf1 =
            HardwareBufferManager::getSingleton().createVertexBuffer(
               3*sizeof(float),
		      numNodes,
		      HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE, false);
      vbind->setBinding(1, vbuf1);

      //memcpy(static_cast<float*>(vbuf0->lock(HardwareBuffer::HBL_DISCARD)), &cpuvbuf0[0], vbuf0->getSizeInBytes()); vbuf0->unlock();
      //memcpy(static_cast<float*>(vbuf1->lock(HardwareBuffer::HBL_DISCARD)), &cpuvbuf1[0], vbuf1->getSizeInBytes()); vbuf1->unlock();

      // Prepare texture coords buffer - static one

      vbuf2 =
            HardwareBufferManager::getSingleton().createVertexBuffer(
               sizeof(RGBA),
		      numNodes,
		      HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
      vbuf2->writeData(0,
	      vbuf2->getSizeInBytes(),
	      &cpuvbuf2[0],
	      true); // true?
      vbind->setBinding(2, vbuf2);

      // Prepare buffer for indices
      ibuf =
	      HardwareBufferManager::getSingleton().createIndexBuffer(
		      HardwareIndexBuffer::IT_16BIT,
		      3*numTris,
		      HardwareBuffer::HBU_STATIC, true);
      unsigned short *faceVertexIndices = (unsigned short*)
         ibuf->lock(0, ibuf->getSizeInBytes(), HardwareBuffer::HBL_DISCARD);
      memcpy(faceVertexIndices, &cpuibuf0[0], ibuf->getSizeInBytes());

      ibuf->unlock();

      // Set index buffer for this submesh
      subMesh->indexData->indexBuffer = ibuf;
      subMesh->indexData->indexStart = 0;
      subMesh->indexData->indexCount = 3*numTris;

      /*	prepare vertex positions
       *	note - we use 3 vertex buffers, since algorighm uses two last phases
       *	to calculate the next one
       */

      AxisAlignedBox meshBounds;
      meshBounds.setInfinite();
      msh->_setBounds(meshBounds);


      msh->load();
      msh->touch();

      // WEIRD!! : For some reason, ogre create a new vertex buffer with double size..
      vbuf0 = vbind->getBuffer(0);
      

      // Create Entity
      ent = mgr->createEntity(entName, meshName);
      ent->setMaterialName(matName);
      ent->setCastShadows(false);

      thisSceneNode->attachObject(ent);

      bbset = mgr->createBillboardSet("mySet");
      thisSceneNode->attachObject(bbset);
   
      bbset->setMaterialName("Nut/ParticleBillboard");

      for (int i = 0; i < numNodes; i++) {
         Billboard* myBillboard = bbset->createBillboard(Vector3(0,0,0));         
         myBillboard->setDimensions(0.001, 0.001);
         verticesbb.push_back(myBillboard);
      }
   

      // Create entities of the particles
      if (showRadius) {
         vector<real> radius;
         fem->topology->ComputeVertexSafeRadius(fem->matPos, radius);
         char veName[100];
         for (int i = 0; i < numNodes; i++) {
            sprintf(veName, "%s_par_%d", entName.c_str(), i);
            SceneNode* sn = thisSceneNode->createChildSceneNode();

            sn->setPosition(0, 0, 0);
            real sc = radius[i];
            sn->setScale(sc,sc,sc);  

            verticesSceneNode.push_back(sn);

            Entity* ve = mgr->createEntity(veName, "sphere");
            sn->attachObject(ve);
            ve->setMaterialName("MatGreen");
            ve->setCastShadows(false);
            verticesEntity.push_back(ve);
         }
      }
     
   }

   // Update vertices position

   Wm4::Vector3f mins(1e10, 1e10, 1e10), maxs(-mins);
   real* vFEM;
   if (worldMesh) vFEM = &fem->worldPos[0][0]; else vFEM = &fem->matPos[0][0];
   float* vRender = &cpuvbuf0[0].pos[0];
   for (int i = 0; i < numNodes; i++) {
      

      vRender[0] = vFEM[0];
      vRender[1] = vFEM[1];
      vRender[2] = vFEM[2];

      if (vRender[0] < mins[0]) mins[0] = vRender[0];
      if (vRender[1] < mins[1]) mins[1] = vRender[1];
      if (vRender[2] < mins[2]) mins[2] = vRender[2];

      if (vRender[0] > maxs[0]) maxs[0] = vRender[0];
      if (vRender[1] > maxs[1]) maxs[1] = vRender[1];
      if (vRender[2] > maxs[2]) maxs[2] = vRender[2];
      vRender+=3;
      vFEM+=3;
   }


   ComputeVerticesNormal();


   /// Upload the vertex data to the card
//   vbuf0->writeData(0, vbuf0->getSizeInBytes(), &cpuvbuf0[0], true);
  // thisSceneNode->detachObject(ent);  


   /*
   VertexBufferBinding* vbind = subMesh->vertexData->vertexBufferBinding;
   int nb = vbind->getBufferCount();
   cout<<vbind->getBuffer(0)->getSizeInBytes()<<endl;
   cout<<vbind->getBuffer(1)->getSizeInBytes()<<endl;
   cout<<vbind->getBuffer(2)->getSizeInBytes()<<endl;
   HardwareVertexBufferSharedPtr ggg;
   ggg = vbind->getBuffer(0);
   Wm4::Vector3f* vp = static_cast<Wm4::Vector3f*>(ggg->lock(HardwareBuffer::HBL_DISCARD));
   for (int i = 0; i < numNodes; i++) {
      vp[0] = cpuvbuf0[i].pos;
      //vp[1] = cpuvbuf1[i].normal;
      //vp+=2;
      vp++;
   }
   ggg->unlock();*/


   memcpy(static_cast<float*>(vbuf0->lock(HardwareBuffer::HBL_DISCARD)), &cpuvbuf0[0], sizeof(CVertexB0)*numNodes); vbuf0->unlock();
   memcpy(static_cast<float*>(vbuf1->lock(HardwareBuffer::HBL_DISCARD)), &cpuvbuf1[0], vbuf1->getSizeInBytes()); vbuf1->unlock();
   //msh->load();
   //msh->touch();

   if (showRadius) {
      
      // Update position of vertices
      for (int i = 0; i < numNodes; i++) {
         verticesSceneNode[i]->setPosition(cpuvbuf0[i].pos[0], cpuvbuf0[i].pos[1], cpuvbuf0[i].pos[2]);
      }
   }

   for (int i = 0; i < numNodes; i++) {
      verticesbb[i]->setPosition(cpuvbuf0[i].pos[0], cpuvbuf0[i].pos[1], cpuvbuf0[i].pos[2]);
   }


   /// Set bounding information (for culling)
   msh->_setBounds(AxisAlignedBox(mins[0], mins[1], mins[2], maxs[0], maxs[1], maxs[2]));
   msh->_setBoundingSphereRadius(Math::Sqrt((maxs[0]-mins[0])*(maxs[0]-mins[0])+(maxs[1]-mins[1])*(maxs[1]-mins[1])+(maxs[2]-mins[2])*(maxs[2]-mins[2]) ));

   // Does not support shadow volume
   // Re add the entity to the scene
//   thisSceneNode->attachObject(ent);


}

CMeshRenderer::CMeshRenderer( SceneManager *mgri, CFEMMesh* femi, const string meshNamei, const string entNamei, const string matNamei, bool worldMeshi, bool showRadius) {
   this->showRadius = showRadius;
   msh.setNull();
   fem = femi;
   worldMesh = worldMeshi;
   meshName = meshNamei;
   mgr = mgri;
   entName = entNamei;
   matName = matNamei;
   ent = NULL;


   numNodes = 0;
   numTris = 0;
   cpuvbuf0.clear();
   cpuvbuf1.clear();
   cpuibuf0.clear();
   if (fem->numNodes > 65535) {
      Error("16 bit index buffer only works when number of vertices is < 65535");
   }

 
   
   thisSceneNode = mgr->getRootSceneNode()->createChildSceneNode();

   // Copy the data
   UpdateVertices();

}


void CMeshRenderer::SetPosition(const VEC3& pos) {
   position = pos;
   thisSceneNode->setPosition(pos[0], pos[1], pos[2]);
}

void CMeshRenderer::SetScale(const real sc) {
   scale = sc;
   thisSceneNode->setScale(sc,sc,sc);
}

#endif