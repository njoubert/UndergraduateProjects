#ifndef NO_GRAPHICS

#ifndef _DEBUG
#ifndef NOOMP

   #include <omp.h>
#endif
#endif

#include <CEGUI/CEGUIImageset.h>
#include <CEGUI/CEGUISystem.h>
#include <CEGUI/CEGUILogger.h>
#include <CEGUI/CEGUISchemeManager.h>
#include <CEGUI/CEGUIWindowManager.h>
#include <CEGUI/CEGUIWindow.h>
#include <CEGUI/elements/CEGUICombobox.h>
#include <CEGUI/elements/CEGUIListbox.h>
#include <CEGUI/elements/CEGUIListboxTextItem.h>
#include <CEGUI/elements/CEGUIPushButton.h>
#include <CEGUI/elements/CEGUIScrollbar.h>
#include "OgreCEGUIRenderer.h"
#include "OgreCEGUIResourceProvider.h"
#include "ExampleApplication.h"
#include <fstream>
//#include "volumemesh.h"
#include "globals.h"
#include "constream.h"
#include <ogremath.h>
#include "FEM.h"
#include "meshrenderer.h"
#include "Mesh_Geosphere.h"

using namespace std;

using namespace OIS;
using namespace Ogre;
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

CEGUI::MouseButton convertOgreButtonToCegui(int buttonID)
{
   switch (buttonID)
   {
   case 0:
      return CEGUI::LeftButton;
   case 1:
      return CEGUI::RightButton;
   case 2:
      return CEGUI::MiddleButton;
   case 3:
      return CEGUI::X1Button;
   default:
      return CEGUI::LeftButton;
   }
}

real beta = 0.1;
real alpha = 0.9;

class OptionLayoutData {
public:

   CEGUI::Scrollbar* mAlpha;
   CEGUI::Scrollbar* mBeta;
   CEGUI::Scrollbar* mRadius;
   //CEGUI::StaticText* mAlphaT;
   //CEGUI::StaticText* mBetaT;
   //CEGUI::StaticText* mRadiusT;
   CEGUI::Scrollbar* mRXS;
   CEGUI::Scrollbar* mRYS;
   CEGUI::Scrollbar* mRZS;

   CEGUI::PushButton* mPause;
   CEGUI::PushButton* mShade;
   CEGUI::PushButton* mShadow;
   CEGUI::PushButton* mHold;
   CEGUI::PushButton* mShowMesh;
   CEGUI::PushButton* mRegion;
   CEGUI::PushButton* mParticle;

};

class MyListener : public ExampleFrameListener, public MouseListener, public KeyListener
{
   CEGUI::Renderer* mGUIRenderer;
   CEGUI::Window* mEditorGuiSheet;
   //CVolumeMesh* volMesh;
   OptionLayoutData* optd;
   RaySceneQuery *mRaySceneQuery;     // The ray scene query pointer
   real rotAmount; 
   bool pauseSimulation;
   int mSceneDetailIndex;
   bool shadow;
   bool hold;
   real regionRadius;
   real springK;
   VEC3 regionCenter;
   vector< int > affectedVertices;
   bool mContinue;
   CConfig& config;
   


public:

   int mouseX, mouseY;
   int leftClickX, leftClickY;
   bool leftClick;
   bool rightClick;
   bool grabObject;
   bool region;
   bool showMesh;
   bool particle;
   CFEMMesh* fem;
   CMeshRenderer* meshRenderer;

   MyListener(RenderWindow* win, Camera* cam, SceneManager *mgr, CEGUI::Renderer* mguir, CEGUI::Window* mEGS//, CVolumeMesh* ivolMesh
      , OptionLayoutData* ioptd, CFEMMesh* femi, CMeshRenderer* meshRendereri)
      : ExampleFrameListener(win, cam, true, true), config(femi->config)
   {
      meshRenderer = meshRendereri;
      fem = femi;
      rotAmount = PI / 2048; 
      grabObject = false;
      leftClick = rightClick = false;
      optd = ioptd;
      //volMesh = ivolMesh;
      mEditorGuiSheet = mEGS;
      mGUIRenderer = mguir;
      mSceneMgr = mgr;

      pauseSimulation = false; // do not stop simulation
      mSceneDetailIndex = 0; // show solid
      shadow = true; // show shadow
      hold = true; // not hold object to the ground
      showMesh = true; // show ninja
      region = false; // move the whole object 
      regionRadius = 0.1;
      particle = false;

      // Populate the camera and scene manager containers
      // The grand parent of camera
      mCamNode = cam->getParentSceneNode( )->getParentSceneNode( );

      // set the rotation and move speed
      mRotate = 72;
      mMove = 2.5;
      mDirection = Vector3::ZERO;
      springK = 500;


      // Add myself to the listener

      setupOptionsEventHandler();

      // Setup ray scene query
      mRaySceneQuery = mgr->createRayQuery(Ray());

      mMouse->setEventCallback(this);
      mKeyboard->setEventCallback(this);
      mContinue=true;

      #ifndef _DEBUG
      #ifndef NOOMP

         omp_set_num_threads(5);
      #endif
      #endif

   }

   void setupOptionsEventHandler() {
      // Add 

/*
      optd->mRXS->setScrollPosition(128);
      optd->mRYS->setScrollPosition(128);
      optd->mRZS->setScrollPosition(128);

      optd->mRXS->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MyListener::handleRXS, this));
      optd->mRYS->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MyListener::handleRYS, this));
      optd->mRZS->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MyListener::handleRZS, this));

      optd->mPause->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MyListener::handlePause, this));*/
      optd->mShade->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MyListener::handleShade, this));
      /*
      optd->mShadow->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MyListener::handleShadow, this));
      optd->mHold->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MyListener::handleHold, this));
*/
      optd->mShowMesh->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MyListener::handleShowMesh, this));
      //optd->mRegion->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MyListener::handleRegion, this));*/
      optd->mParticle->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MyListener::handleParticle, this));
/*
      optd->mAlpha->setScrollPosition(alpha * 256);
      optd->mBeta->setScrollPosition(beta * 256);
      optd->mRadius->setScrollPosition(regionRadius * 256);
      updateTexts();

      optd->mAlpha->subscribeEvent(
         CEGUI::Scrollbar::EventScrollPositionChanged, 
         CEGUI::Event::Subscriber(&MyListener::handleAlphaChanged, this));

      optd->mBeta->subscribeEvent(
         CEGUI::Scrollbar::EventScrollPositionChanged, 
         CEGUI::Event::Subscriber(&MyListener::handleBetaChanged, this));

      optd->mRadius->subscribeEvent(
         CEGUI::Scrollbar::EventScrollPositionChanged, 
         CEGUI::Event::Subscriber(&MyListener::handleRadiusChanged, this));*/
   }
/*
   void RotateObject(CMDeformation* p, int axis, real amount) {
      VEC3 cm;
      real tmass = 0.0;
      for (int i = 0; i < p->n; i++) {
         tmass += p->masses[i];
         cm += p->masses[i] * p->particles->P(i);
      }
      cm/=tmass;
      gsl::matrix mat(3,3);

      if (axis == 0) {		
         mat(0,0) = 1;
         mat(1,0) = 0;
         mat(2,0) = 0;

         mat(0,1) = 0;
         mat(1,1) = cos(amount);
         mat(2,1) = sin(amount);

         mat(0,2) = 0;
         mat(1,2) = -sin(amount);
         mat(2,2) = cos(amount);
      } else
         if (axis == 1) {		
            mat(0,0) = cos(amount);
            mat(1,0) = 0;
            mat(2,0) = -sin(amount);

            mat(0,1) = 0;
            mat(1,1) = 1;
            mat(2,1) = 0;

            mat(0,2) = sin(amount);
            mat(1,2) = 0;
            mat(2,2) = cos(amount);
         } else
            if (axis == 2) {		
               mat(0,0) = cos(amount);
               mat(1,0) = sin(amount);
               mat(2,0) = 0;

               mat(0,1) = -sin(amount);
               mat(1,1) = cos(amount);
               mat(2,1) = 0;

               mat(0,2) = 0;
               mat(1,2) = 0;
               mat(2,2) = 1;
            } 

            for (int i = 0; i < p->n; i++) {
               p->particles->P(i) = mat*(p->particles->P(i) - cm) + cm;
               p->particles->V(i) = VEC3();
            }
   }*/
   void updateTexts() {
      alpha = optd->mAlpha->getScrollPosition() / 256;
      static char temp[50];
      sprintf(temp, "%0.3f", alpha);
      String str = temp;
      //optd->mAlphaT->setText(str);

      beta = optd->mBeta->getScrollPosition() / 256;
      sprintf(temp, "%0.3f", beta);
      str = temp;
      //optd->mBetaT->setText(str);

      regionRadius = optd->mRadius->getScrollPosition() / 256;
      sprintf(temp, "%0.3f", regionRadius);
      str = temp;
      //optd->mRadiusT->setText(str);
   }

   bool handleShowMesh(const CEGUI::EventArgs& e) {

      showMesh = !showMesh;
      mSceneMgr->getEntity("fem_entity")->setVisible(showMesh);
      return true;
   }

   bool handleRegion(const CEGUI::EventArgs& e) {

      region = !region;
      if (region) {
         optd->mRegion->setText("Whole");
      } else {
         optd->mRegion->setText("Region");
      }
      return true;
   }
   bool handleParticle(const CEGUI::EventArgs& e) {

      particle = !particle;
      if (particle) {
         optd->mParticle->setText("NParticle");
         meshRenderer->bbset->setVisible(true);
         // Make all particles visible
         ///for (int i = 0; i < volMesh->sVerticesE.size(); i++) {
         ///   volMesh->sVerticesE[i]->setVisible(true);
         ///}
      } else {
         optd->mParticle->setText("Particle");
         meshRenderer->bbset->setVisible(false);
         // Make all particles invisible
         ///for (int i = 0; i < volMesh->sVerticesE.size(); i++) {
         ///   volMesh->sVerticesE[i]->setVisible(false);
         ///}
      }
      return true;
   }
   bool handleHold(const CEGUI::EventArgs& e) {
      hold = !hold;
      if (hold) {
         optd->mHold->setText("Unhold");
      } else {
         optd->mHold->setText("Hold");
      }
      return true;
   }

   void togglePause() {
      pauseSimulation = !pauseSimulation;
      if (pauseSimulation) {
         optd->mPause->setText("Continue"); 
      } else {
         optd->mPause->setText("Pause"); 
      }	

   }

   bool handlePause(const CEGUI::EventArgs& e) {
      togglePause();
      return true;
   }

   bool handleShadow(const CEGUI::EventArgs& e) {
      if (shadow) {
         optd->mShadow->setText("Shadow");
      } else {
         optd->mShadow->setText("No Shadow");
      }
      shadow = !shadow;
      ///volMesh->ent->setCastShadows(shadow);
      return true;
   }

   bool handleShade(const CEGUI::EventArgs& e) {
      mSceneDetailIndex = (mSceneDetailIndex+1)%3 ;
      switch(mSceneDetailIndex) {
         case 0 : {mCamera->setPolygonMode(PM_SOLID); optd->mShade->setText("Wire"); } break ;
         case 1 : {mCamera->setPolygonMode(PM_WIREFRAME); optd->mShade->setText("Points"); } break ;
         case 2 : {mCamera->setPolygonMode(PM_POINTS);  optd->mShade->setText("Solid"); } break ;
      }

      return true;
   }

   bool handleRXS(const CEGUI::EventArgs& e) {
      real div = optd->mRXS->getScrollPosition() - 128;
      //RotateObject(volMesh->deform, 0, rotAmount*div);
      optd->mRXS->setScrollPosition(128);
      return true;
   }
   bool handleRYS(const CEGUI::EventArgs& e) {
      real div = optd->mRYS->getScrollPosition() - 128;
      //RotateObject(volMesh->deform, 1, rotAmount*div);
      optd->mRYS->setScrollPosition(128);
      return true;
   }
   bool handleRZS(const CEGUI::EventArgs& e) {
      real div = optd->mRZS->getScrollPosition() - 128;
      //RotateObject(volMesh->deform, 2, rotAmount*div);
      optd->mRZS->setScrollPosition(128);
      return true;
   }

   bool handleAlphaChanged(const CEGUI::EventArgs& e) {
      updateTexts();
      return true;
   }

   bool handleBetaChanged(const CEGUI::EventArgs& e) {
      updateTexts();
      return true;

   }
   bool handleRadiusChanged(const CEGUI::EventArgs& e) {
      updateTexts();
      return true;

   }

   void requestShutdown() {
      mContinue = false;
   }

/*   void Step(CMDeformation* cube, int n, real h, real alpha, real beta, real floory) {
      // Add force here
      VEC3 df;
      df(1) = -9.8;
      for (int j = 0; j < n; j++) {
         cube->AddAcceleration(j,df);
      }

      cube->ComputeDV(h, alpha, beta);

      real r = 10.0;
      // Handle collision
      for (int j = 0; j < n; j++) {
         VEC3 op, p, tv;

         tv = cube->particles->DV(j) + cube->particles->V(j);
         p = cube->particles->P(j);
         op = p + h * tv;
         if (op(1) < floory) {
            real thit = (floory - p(1)) / (tv)(1);
            VEC3 hitP = p + thit*(tv);
            // Will penetrate ground 
            cube->particles->V(j) = tv;
            cube->particles->V(j)(1) *= -1;
            cube->particles->V(j) *= 0.9f;
            cube->particles->P(j) = hitP + (-thit * cube->particles->V(j));
            cube->particles->DV(j) = VEC3();								
         }

      }
      cube->particles->AdvanceTime(h);
   }*/
   bool frameStarted(const FrameEvent& evt)
   {
      mKeyboard->capture();
      mMouse->capture();
      mCamNode->translate( mCamNode->getOrientation() * mCamNode->getChild(0)->getOrientation() * mDirection * evt.timeSinceLastFrame );
      
      const real h = 0.01;
      static real currentTime = 0.0;
      static real nowTime = 0.0;

      nowTime += evt.timeSinceLastFrame;
      int numStep = (int) (floor((nowTime - currentTime) / h) + 0.1f);

      // Update the positions of the surface particles, if needed
      //if (particle) {

       //  for (int i = 0; i < volMesh->sVerticesE.size(); i++) {
        //    const VEC3& pos = volMesh->deform->particles->P(volMesh->sVtoV[i]);
        //    ((SceneNode*)volMesh->sVerticesE[i]->getParentNode())->setPosition(pos(0),pos(1),pos(2));
        // }
      //}

      /*
      if (!pauseSimulation) {		
         // Simulate a couple of steps, if needed
         for (int i = 0; i < numStep; i++) {	
            if (grabObject && region) {
               // Apply additional spring force
               for (int i = 0; i <affectedVertices.size(); i++) {
                  VEC3 dir = regionCenter - volMesh->deform->particles->P(affectedVertices[i]);
                  real dist = dir.norm2();
                  if (fabs(dist) >= 0.000001) {					
                     dir /= dist;
                     // Spring
                     dir *= springK * dist;

                     // Add acceleration
                     volMesh->deform->AddAcceleration(affectedVertices[i], dir);

                  }
               }
            }
            Step(volMesh->deform, volMesh->nVertices, h, alpha, beta, 0.0);
         }
      }

      if (hold) {
         // Hold some of the vertices to the ground
         for (int i = 0; i < volMesh->numBaseVertices; i++) {
            volMesh->deform->particles->P(volMesh->baseVerticesIndices[i]) = volMesh->baseVerticesPos[i];
            volMesh->deform->particles->V(volMesh->baseVerticesIndices[i]).set_zero();
         }
      }*/

      /*
      if (grabObject && region) {
      for (int i = 0; i <affectedVertices.size(); i++) {
      volMesh->deform->particles->P(affectedVertices[i]) = affectedPositions[i];
      volMesh->deform->particles->V(affectedVertices[i]) = VEC3();
      }
      }*/

      // Update current time
      currentTime += h * numStep;
      if (leftClick) {
         Matrix4 vm = mCamera->getViewMatrix();
         VEC3 pp = fem->worldPos[fem->tip.index] * meshRenderer->scale + meshRenderer->position;
         Vector3 pos(pp[0],pp[1],pp[2]);

         Matrix4 pm = mCamera->getProjectionMatrix();

         Vector3 projPos = pm*(vm*pos);
         projPos.x = (projPos.x * 0.5 + 0.5);
         projPos.y = (-projPos.y * 0.5 + 0.5);

//         projPos.x = 1.0 - ( projPos.x / projPos.z + 0.5);
//         projPos.y = projPos.y / projPos.z + 0.5;
         cout<<projPos.x*mGUIRenderer->getWidth()<<" "<<projPos.y*mGUIRenderer->getHeight()<<endl;

         Ray zRay = mCamera->getCameraToViewportRay( 0.5f, 0.5f );
         Ray xRay = mCamera->getCameraToViewportRay( 0.5f + 0.1f, 0.5f );


         VEC3 zAxis(zRay.getDirection());
         VEC3 x(xRay.getDirection());
         real coss = zAxis.Dot(x);
         x = (x-coss*zAxis);
         x /= x.Length();
         VEC3 y = x.Cross(zAxis);
         VEC3 displace = x * ((mouseX - projPos.x*mGUIRenderer->getWidth()) / mGUIRenderer->getWidth())
                      - y * ((mouseY - projPos.y*mGUIRenderer->getHeight()) / mGUIRenderer->getHeight());
         displace /= config.sceneScale;

         VEC3 force = displace*config.mouseForceScale;
         cout<<"Exert force on node "<<fem->tip.index<<" with "<<force[0]<<" "<<force[1]<<" "<<force[2]<<endl;
         fem->extForce.push_back(CExternalForce(fem->tip.index, force, config.dt * 10));

      }

      for (int i = 0; i < 10; i++) {
         fem->Simulate(config.dt);
      }
      /*
      for (int i = 0; i < fem->numNodes; i++) {
         fem->worldPos[i] *= 1.001;
      }*/
      meshRenderer->UpdateVertices();
      /// volMesh->UpdateMesh();
      return mContinue && !mKeyboard->isKeyDown(OIS::KC_ESCAPE);
   }
   bool quit(const CEGUI::EventArgs &e)
   {
      mContinue = false;
      return true;
   }

   // MouseDragged
   
   bool mouseMoved( const MouseEvent &e) {
      mouseX = e.state.X.abs;
      mouseY = e.state.Y.abs;
      CEGUI::System::getSingleton().injectMouseMove(
         e.state.X.rel, 
         e.state.Y.rel);

      if (rightClick) {         
         mCamNode->yaw( Degree(-e.state.X.rel /  mGUIRenderer->getWidth() * mRotate) );
         mCamNode->getChild( 0 )->pitch( Degree(-e.state.Y.rel /  mGUIRenderer->getHeight()* mRotate) );        
      }
      if (grabObject) {

         Ray zRay = mCamera->getCameraToViewportRay( 0.5f, 0.5f );
         Ray xRay = mCamera->getCameraToViewportRay( 0.5f + 0.1f, 0.5f );

         VEC3 zAxis(zRay.getDirection());
         VEC3 x(xRay.getDirection());
         real coss = zAxis.Dot(x);
         x = (x-coss*zAxis);
         x /= x.Length();
         VEC3 y = x.Cross(zAxis);
         VEC3 displace = x * 10.0f * e.state.X.rel 
                      - y * 10.0f * e.state.Y.rel;

         if (region) {
            // Move only vertices inside the region
            /*
            for (int j = 0; j < affectedVertices.size(); j++) {
            if (affectedPositions[j] (1) + displace(1) < 0) {
            affectedPositions[j] (0) += displace(0);
            affectedPositions[j] (2) += displace(2);
            } else {				   
            affectedPositions[j] += displace;
            }
            }*/
            regionCenter += displace;

         } else {
            // Move whole object
            /*
            for (int i = 0; i < volMesh->nVertices; i++) {
            if (volMesh->deform->particles->P(i) (1) + displace(1) < 0) {
            volMesh->deform->particles->P(i) (0) += displace(0);
            volMesh->deform->particles->P(i) (2) += displace(2);
            } else {				   
            volMesh->deform->particles->P(i) += displace;
            }
            }*/
         }

      }
      return true;
   }



   bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
   {
     if ( id == OIS::MB_Left ) {
        
         cout<<"Left click"<<endl;
         leftClick = true;
         leftClickX = e.state.X.abs;
         leftClickY = e.state.Y.abs;

      }
      if ( id == OIS::MB_Right ) {
         rightClick = true;
      }

      if (id == OIS::MB_Middle) {
         cout<<"Middle mouse press at "<<e.state.X.abs<<", "<<e.state.Y.abs<<" width = "<<mGUIRenderer->getWidth()<<" height = "<<mGUIRenderer->getHeight()<<" mw = "<<e.state.width<<" mh = "<<e.state.height<<endl;
         
         Ray mouseRay = mCamera->getCameraToViewportRay(e.state.X.abs /  mGUIRenderer->getWidth(), e.state.Y.abs /  mGUIRenderer->getHeight());
         VEC3 origin(mouseRay.getOrigin()[0], mouseRay.getOrigin()[1], mouseRay.getOrigin()[2]);
         VEC3 dir(mouseRay.getDirection()[0], mouseRay.getDirection()[1], mouseRay.getDirection()[2]);

         cout<<"Origin : "<<origin[0]<<" "<<origin[1]<<" "<<origin[2]<<endl;
         cout<<"Dir : "<<dir[0]<<" "<<dir[1]<<" "<<dir[2]<<endl;
         // Obtain the ray in object space
         //origin -= meshRenderer->position;
         //origin /= meshRenderer->scale;
         real okdis = 0.01;
         real okdis2 = okdis*okdis;
         real minD = 1e10;
         int minI = -1;
         real minD2Ray = 1e10;
         VEC3 vvvX = origin + dir * 5.0f;
          mSceneMgr->getEntity("Cube")->getParentNode()->setPosition(vvvX[0], vvvX[1], vvvX[2]);
         

         for (int i = 0; i < fem->numNodes; i++) {
            VEC3 node = (fem->worldPos[i]) * meshRenderer->scale + meshRenderer->position;
            VEC3 disp = node - origin;
            real d = disp.Dot(dir);
            real disToRay = disp.SquaredLength() - d*d;
            if (disToRay < minD2Ray) {
               minD2Ray = disToRay;
            } 
            if (disToRay < okdis2) {
               if (d < minD) {
                  minD = d;
                  minI = i;
               }
            }            
         }
         if (minI != -1) {
            static int oldI = -1;
            if (oldI != -1) {
               meshRenderer->verticesbb[oldI]->setColour(ColourValue(1,1,1));               
            }

            fem->tip.index = minI;
            meshRenderer->verticesbb[minI]->setColour(ColourValue(1,0,0));
            oldI = minI;
         }
         cout<<"Min dis to ray = "<<minD2Ray<<endl;
         cout<<"Pick on node "<<minI<<endl;
 
      }

      /*	   if ( e->getButtonID() & MouseEvent::BUTTON0_MASK ) {
      Light *light = mSceneMgr->getLight( "Light1" );
      light->setVisible( ! light->isVisible() );
      }*/
      CEGUI::System::getSingleton().injectMouseButtonDown(
         convertOgreButtonToCegui(id));

      return true;
   }
   bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id) {
      if (id == OIS::MB_Left ) {
         cout<<"Left released"<<endl;
         leftClick = false;

      }
      if (id == OIS::MB_Right ) {
         rightClick = false;
      }

      CEGUI::System::getSingleton().injectMouseButtonUp(
         convertOgreButtonToCegui(id));

      return true;
   }

   // KeyListener
   bool keyPressed( const KeyEvent &arg )
   {
      switch (arg.key) {
         case KC_T: {
            // do tip like spring
            region = true;
            grabObject = true;
            //affectedVertices.clear();
            //affectedVertices.push_back(volMesh->tipIndex);
            regionCenter = VEC3(2,2,2);
            break;
                    }
         case KC_P: {
            togglePause();
            break;
                    }
         case KC_8: {
            if (region && grabObject) {
               regionCenter[0] += 1.0;
            }
            break;
                    }
         case KC_9: {
            if (region && grabObject) {
               regionCenter[0] -= 1.0;
            }
            break;
                    }

         case KC_I: {
            if (region && grabObject) {
               regionCenter[1] += 1.0;
            }
            break;
                    }
         case KC_O: {
            if (region && grabObject) {
               regionCenter[1] -= 1.0;
            }
            break;
                    }
         case KC_K: {
            if (region && grabObject) {
               regionCenter[2] += 1.0;
            }
            break;
                    }
         case KC_L: {
            if (region && grabObject) {
               regionCenter[2] -= 1.0;
            }
            break;
                    }
         case KC_U: {
            VEC3 up;
            up[1] = 1.0;
            /*for (int i = 0; i < volMesh->nVertices; i++) {
               volMesh->deform->particles->P(i) += up;
               volMesh->deform->particles->V(i) = VEC3();
            }*/
                    }
                    break;
         case KC_H:
            if (mEditorGuiSheet->isVisible()) {
               mEditorGuiSheet->hide();
            } else {
               mEditorGuiSheet->show();
            }
            break;
         case KC_ESCAPE:
            mContinue = false;
            break;			
         case KC_1:
            mCamera->getParentSceneNode()->detachObject( mCamera );
            mCamNode = mSceneMgr->getSceneNode( "CamNode1" );
            mSceneMgr->getSceneNode( "PitchNode1" )->attachObject( mCamera );
            break;

         case KC_2:
            mCamera->getParentSceneNode()->detachObject( mCamera );
            mCamNode = mSceneMgr->getSceneNode( "CamNode2" );
            mSceneMgr->getSceneNode( "PitchNode2" )->attachObject( mCamera );
            break;
         case KC_UP:
         case KC_W:
            mDirection.z -= mMove;
            break;

         case KC_DOWN:
         case KC_S:
            mDirection.z += mMove;
            break;

         case KC_LEFT:
         case KC_A:
            mDirection.x -= mMove;
            break;

         case KC_RIGHT:
         case KC_D:
            mDirection.x += mMove;
            break;

         case KC_PGDOWN:
         case KC_E:
            mDirection.y -= mMove;
            break;

         case KC_PGUP:
         case KC_Q:
            mDirection.y += mMove;
            break;

      }
      CEGUI::System::getSingleton().injectKeyDown(arg.key);
      CEGUI::System::getSingleton().injectChar(arg.text);
      return true;
   }
   bool keyReleased( const KeyEvent &arg ) {
      switch ( arg.key ){
         case KC_UP:
         case KC_W:
            mDirection.z += mMove;
            break;

         case KC_DOWN:
         case KC_S:
            mDirection.z -= mMove;
            break;

         case KC_LEFT:
         case KC_A:
            mDirection.x += mMove;
            break;

         case KC_RIGHT:
         case KC_D:
            mDirection.x -= mMove;
            break;

         case KC_PGDOWN:
         case KC_E:
            mDirection.y += mMove;
            break;

         case KC_PGUP:
         case KC_Q:
            mDirection.y -= mMove;
            break;
      } // switch
      CEGUI::System::getSingleton().injectKeyUp(arg.key);

      return true;
   }

protected:

   Real mRotate;
   Real mMove;
   Vector3 mDirection;
   SceneManager *mSceneMgr;
   SceneNode *mCamNode;

};

class MyApplication : public ExampleApplication
{
public:

   CEGUI::OgreCEGUIRenderer* mGUIRenderer;
   CEGUI::System* mGUISystem;
   CEGUI::Window* mEditorGuiSheet;
   //CVolumeMesh* volMesh;
   OptionLayoutData optd;
   CFEMMesh *fem;
   CMeshRenderer* meshRenderer;
   Mesh_Geosphere mSphere;

   CConfig config;
   MyApplication() : mGUIRenderer(0), mGUISystem(0), mEditorGuiSheet(0), mSphere(1, 2,1) {

      config.LoadConfig("config.txt");
      fem = new CFEMMesh(config);
      

   }
   ~MyApplication() {
      delete fem;
      delete meshRenderer;
//      delete volMesh;
      if(mEditorGuiSheet)
      {
         CEGUI::WindowManager::getSingleton().destroyWindow(mEditorGuiSheet);
      }
      if(mGUISystem)
      {
         delete mGUISystem;
         mGUISystem = 0;
      }
      if(mGUIRenderer)
      {
         delete mGUIRenderer;
         mGUIRenderer = 0;
      }

   }


protected:
   virtual void createCamera(void) {
      mCamera = mSceneMgr->createCamera("PlayerCam");
      //mCamera->setPosition(Vector3(0,10,100));
      //mCamera->lookAt(Vector3(0,0,0));
      mCamera->setNearClipDistance(0.1);
      mCamera->setFarClipDistance(1000);

      SceneNode *camNode;
      camNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("CamNode1", Vector3(0, 1, 0));
      camNode->yaw(Degree(0));

      camNode = camNode->createChildSceneNode("PitchNode1");
      camNode->attachObject(mCamera);

      // create the second camera node/pitch node
      camNode = mSceneMgr->getRootSceneNode()->createChildSceneNode( "CamNode2", Vector3( 0, 2, 4 ) );
      camNode = camNode->createChildSceneNode( "PitchNode2" );



   }
   virtual void createViewports(void) {
      Viewport* vp = mWindow->addViewport(mCamera);
      vp->setBackgroundColour(ColourValue(0,0,0));
      mCamera->setAspectRatio(((Real)vp->getActualWidth()) / ((Real)vp->getActualHeight()));

   }
   void setupLoadedLayout(void)
   {

      CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
      /*
      optd.mAlpha = static_cast<CEGUI::Scrollbar*>(
         wmgr.getWindow((CEGUI::utf8*)"Meshless/Window1/Controls/Alpha"));
      optd.mBeta = static_cast<CEGUI::Scrollbar*>(
         wmgr.getWindow((CEGUI::utf8*)"Meshless/Window1/Controls/Beta"));
      optd.mRadius = static_cast<CEGUI::Scrollbar*>(
         wmgr.getWindow((CEGUI::utf8*)"Meshless/Window1/Controls/Radius"));
*/
      //optd.mAlphaT = static_cast<CEGUI::StaticText*>(
         //wmgr.getWindow((CEGUI::utf8*)"Meshless/Window1/Controls/AlphaT2"));

      //optd.mBetaT = static_cast<CEGUI::StaticText*>(
         //wmgr.getWindow((CEGUI::utf8*)"Meshless/Window1/Controls/BetaT2"));

      //optd.mRadiusT = static_cast<CEGUI::StaticText*>(
         //wmgr.getWindow((CEGUI::utf8*)"Meshless/Window1/Controls/RadiusT2"));

      /*
      optd.mRXS = static_cast<CEGUI::Scrollbar*>(
         wmgr.getWindow((CEGUI::utf8*)"Demo8/Window1/Controls2/RXS"));

      optd.mRYS = static_cast<CEGUI::Scrollbar*>(
         wmgr.getWindow((CEGUI::utf8*)"Demo8/Window1/Controls2/RYS"));

      optd.mRZS = static_cast<CEGUI::Scrollbar*>(
         wmgr.getWindow((CEGUI::utf8*)"Demo8/Window1/Controls2/RZS"));

      optd.mPause = static_cast<CEGUI::PushButton*>(
         wmgr.getWindow((CEGUI::utf8*)"Demo8/Window1/Controls2/Pause"));
*/
      optd.mShade = static_cast<CEGUI::PushButton*>(
         wmgr.getWindow((CEGUI::utf8*)"Demo8/Window1/Controls2/Shade"));
/*
      optd.mShadow = static_cast<CEGUI::PushButton*>(
         wmgr.getWindow((CEGUI::utf8*)"Demo8/Window1/Controls2/Shadow"));

      optd.mHold = static_cast<CEGUI::PushButton*>(
         wmgr.getWindow((CEGUI::utf8*)"Demo8/Window1/Controls2/Hold"));
*/
      optd.mShowMesh = static_cast<CEGUI::PushButton*>(
         wmgr.getWindow((CEGUI::utf8*)"Demo8/Window1/Controls2/ShowMesh"));
/*
      optd.mRegion = static_cast<CEGUI::PushButton*>(
         wmgr.getWindow((CEGUI::utf8*)"Demo8/Window1/Controls2/Region"));
*/
      optd.mParticle = static_cast<CEGUI::PushButton*>(
         wmgr.getWindow((CEGUI::utf8*)"Demo8/Window1/Controls2/Particle"));

      /*
      (static_cast<CEGUI::Window*>(
         wmgr.getWindow((CEGUI::utf8*)"Meshless/Window1")))->setAlpha(0.7f);
*/
      /*		(static_cast<CEGUI::StaticText*>(
      wmgr.getWindow((CEGUI::utf8*)"Meshless/Window1/Controls")))->setAlpha(0.9f);*/

   }
   void createScene(void) {
      mSphere.createMesh("sphere");
      // setup GUI system
      mGUIRenderer = new CEGUI::OgreCEGUIRenderer(mWindow, Ogre::RENDER_QUEUE_OVERLAY, false, 0, mSceneMgr);
      cout<<1<<endl;
      mGUISystem = new CEGUI::System(mGUIRenderer, (CEGUI::ResourceProvider *)0, (CEGUI::XMLParser*)0,
         (CEGUI::ScriptModule*)0, (CEGUI::utf8*)"needlesim.config");
      cout<<2<<endl;

      mEditorGuiSheet = CEGUI::WindowManager::getSingleton().getWindow((CEGUI::utf8*)"demo_root");
      mEditorGuiSheet->setVisible(true);
      cout<<3<<endl;
      Ogre::MovableObject::setDefaultVisibilityFlags(0x00000001);

      CEGUI::Logger::getSingleton().setLoggingLevel(CEGUI::Informative);
      // Setup Render To Texture for preview window
      //RenderTexture* rttTex = mRoot->getRenderSystem()->createRenderTexture( "RttTex", 512, 512, TEX_TYPE_2D, PF_R8G8B8 );
      /*
      try {
         Camera* rttCam = mSceneMgr->createCamera("RttCam");
         SceneNode* camNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("rttCamNode");
         camNode->attachObject(rttCam);
         rttCam->setPosition(0,0,200);
         Viewport *v = rttTex->addViewport( rttCam );
         v->setOverlaysEnabled(false);
         v->setClearEveryFrame( true );
         v->setBackgroundColour( ColourValue::Black );

         // Retrieve CEGUI texture for the RTT
         CEGUI::Texture* rttTexture = mGUIRenderer->createTexture((CEGUI::utf8*)"RttTex");
         CEGUI::Imageset* rttImageSet = CEGUI::ImagesetManager::getSingleton().createImageset((CEGUI::utf8*)"RttImageset", rttTexture);

         rttImageSet->defineImage((CEGUI::utf8*)"RttImage", CEGUI::Point(0.0f, 0.0f), CEGUI::Size(rttTexture->getWidth(), rttTexture->getHeight()), CEGUI::Point(0.0f,0.0f));



      } catch( Ogre::Exception& e ) {
      }*/
      
      mGUISystem->setDefaultMouseCursor((CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"MouseArrow");

      // put your scene creation in here
      mSceneMgr->setAmbientLight( ColourValue(0.1, 0.1, 0.1));
      mSceneMgr->setShadowTechnique( SHADOWTYPE_STENCIL_ADDITIVE);
      //SHADOWTYPE_STENCIL_ADDITIVE );

      Entity *ent = mSceneMgr->createEntity("Ninja", "ninja.mesh");
      ent->setVisible(false);
      ent->setCastShadows( true );
      mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject( ent );
      ((SceneNode *)ent->getParentNode())->scale(0.01,0.01,0.01);

      cout<<"Done Ninja"<<endl;
      ent = mSceneMgr->createEntity("Cube", "cube.mesh");
      ent->setMaterialName("MatGreen");
      ent->setCastShadows( true );
      ent->setVisible(true);
      mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject( ent );
      ((SceneNode *)ent->getParentNode())->scale(0.001,0.001,0.001);

      Plane plane(Vector3::UNIT_Y, 0);
      MeshManager::getSingleton().createPlane("ground",
         ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 15, 15, 20, 20, true, 1, 5, 5, Vector3::UNIT_Z);

      Entity *entg = mSceneMgr->createEntity("GroundEntity", "ground");
      entg->setMaterialName("Nut/Ground");
      entg->setCastShadows(false);
      //entg->setVisible(false);
      mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject( entg );

      Light* light = mSceneMgr->createLight("Light1");
      light->setType(Light::LT_POINT);
      light->setPosition(Vector3(0, 150, 250));
      light->setDiffuseColour(1.0, 1.0, 1.0);
      light->setSpecularColour(1.0, 1.0, 1.0);

      ///volMesh = new CVolumeMesh(mSceneMgr, "volE", "volM", VEC3(0,2,0), 5,15,5,1,3,1, VEC3(1,4,1), 1, 1.0f);
     
      setupLoadedLayout();

      meshRenderer = new CMeshRenderer(mSceneMgr, fem, "fem_mesh", "fem_entity", "Nut/AmbientFromV", true, true);      
      meshRenderer->SetPosition(VEC3(0,2,0));
      meshRenderer->SetScale(1.0f / config.sceneScale);
   }

   // Create new frame listener
   void createFrameListener(void)
   {
      mFrameListener = new MyListener(mWindow, mCamera, mSceneMgr, mGUIRenderer, mEditorGuiSheet, &optd, fem, meshRenderer );
      mFrameListener->showDebugOverlay(true);
      mRoot->addFrameListener(mFrameListener);
   }
};

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
   INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
   int main(int argc, char **argv)
#endif
   {
      // Create application object
      MyApplication app;

      activateConsole();
      try {
         app.go();
      } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
         MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
         std::cerr << "An exception has occured: " << e.getFullDescription();
#endif
      }

      return 0;
   }

#ifdef __cplusplus
}
#endif

#endif