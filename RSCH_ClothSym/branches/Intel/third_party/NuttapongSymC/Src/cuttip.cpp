#include "FEM.h"
#include "topology.h"
CCutTip::CCutTip(CConfig& configi) : config(configi) {
}
real CCutTip::MoveTipForwardManual(CTetTopology& top, real dt) {
   // Let's first estimate the local orientation from xDisMat, yDisMat
   VEC3 xDisWorld, yDisWorld;
   int xDisT = top.ComputeWorldPosGuess(xDisMat, affectedTets[0], xDisWorld);
   int yDisT = top.ComputeWorldPosGuess(yDisMat, affectedTets[0], yDisWorld);
   if ((xDisT == -1) || (yDisT == -1)) return 0.0f;

   VEC3 xw = xDisWorld - top.fem.worldPos[index];
   VEC3 yw = yDisWorld - top.fem.worldPos[index];
   xw.Normalize();
   yw.Normalize();
   VEC3 zw = xw.Cross(yw);
   zw.Normalize();
   yw = zw.Cross(xw);
   
   VEC3 dir = cos(config.needleBendAngularVel*config.dt) * xw + 
             sin(config.needleBendAngularVel*config.dt) * yw; //  Must be orthogonal to z
   dt = top.MoveTip(*this, dir*config.needleVel, dt);

   // Now, update xDisM, yDisM
   // 1. Now x = dir
   // 2. Twist by rotation amount around x
   
   // Build new basis
   xw = dir;
   yw = zw.Cross(xw);

   MATRIX3 rotMat;
   rotMat.FromAxisAngle(xw, config.needleAngularVel*dt);
   yw = rotMat * yw;
   real estDis = config.needleVel*config.needleLFED*dt;
   xDisWorld = top.fem.worldPos[index] + xw*estDis;
   yDisWorld = top.fem.worldPos[index] + yw*estDis;

   if (top.ComputeMatPosGuess(xDisWorld, affectedTets[0], xDisMat) == -1) return 0.0f;
   if (top.ComputeMatPosGuess(yDisWorld, affectedTets[0], yDisMat) == -1) return 0.0f;
   //xDisT = top.PointLocationAnyCoord(top.fem.worldPos, xDisWorld, xDisT);
   //yDisT = top.PointLocationAnyCoord(top.fem.worldPos, yDisWorld, yDisT);
   //if ((xDisT == -1) || (yDisT == -1)) return 0.0f;

   return dt; 
}

void CCutTip::GetCoordSystem(CTetTopology& top, VEC3& xw, VEC3& yw, VEC3& zw) { // x is needle forward dir, y is current up
   int xDisT = top.ComputeWorldPosGuess(xDisMat, affectedTets[0], xDisWorld);
   if (xDisT == -1) {
      // If fail, use x dir
      xDisWorld = top.fem.worldPos[index] + config.needleInitForwardDir*config.needleVel*config.needleLFED*config.dt;
      //cout<<"Fx"<<endl;
   }
   int yDisT = top.ComputeWorldPosGuess(yDisMat, affectedTets[0], yDisWorld);
   if (yDisT == -1) {
      // If fail, use y dir
      yDisWorld = top.fem.worldPos[index] + config.needleInitUpDir*config.needleVel*config.needleLFED*config.dt;
      //cout<<"Fy"<<endl;
   }

   xw = xDisWorld - top.fem.worldPos[index];
   yw = yDisWorld - top.fem.worldPos[index];
   xw.Normalize();
   yw.Normalize();
   zw = xw.Cross(yw);
   zw.Normalize();
   yw = zw.Cross(xw);
}


real CCutTip::MoveNeedleForwardAndApplyConstraintsAndForce(CTetTopology& top, real dt) {
   // Ignore the friction due to twist, basically assume the needle can twist freely without affecting tissue
   
   top.fem.pconst.clear();
   top.fem.vconst.clear();
   top.fem.vDirConst.clear();


   // Let's first estimate the local orientation from xDisMat, yDisMat
   VEC3 xw, yw, zw;
   GetCoordSystem(top, xw, yw, zw);

   
   VEC3 dir = cos(config.needleBendAngularVel*config.dt) * xw + 
             sin(config.needleBendAngularVel*config.dt) * yw; //  Must be orthogonal to z
//   dir = VEC3(1,0,0);

   // Let's check if this is a cut or not
   vector<VEC3>& force = top.fem.force;
   vector<VEC3>& worldPos = top.fem.worldPos;
   vector<VEC3>& velocity = top.fem.velocity;
 //  cout<<index<<" - "<<force.size()<<" - "<<dir<<endl;
 //   cout<<"Force is "<<force[index].Dot(dir)<<endl;
   if (force[index].Dot(dir) < -config.fcut) {
      // Enough force for cutting
      // Perform the cut
      // Figure out material direction
      VEC3 wp = worldPos[index] + dir*dt*config.needleVel;
      VEC3 mp;
      top.ComputeMatPosGuess(wp, affectedTets[0], mp);
      VEC3 mvel = (mp-top.nodePos[index])/dt;
      //top.ComputeWorldPosGuess(top.nodePos[index] + mvel*dt, affectedTets[0], wwp);
      dt = top.MoveTip(*this, mvel, dt);      

      // Move tip by cutting
      top.fem.pconst.push_back(CVConst(index, top.fem.worldPos[index]));
      top.fem.vconst.push_back(CVConst(index, dir*config.needleVel));
      //top.fem.vconst.push_back(CVConst(index, VEC3(0,0,0)));
   } else {

      // Move tip by pushing, modify the node position, works for explicit integration...
      //worldPos[index] += dir*dt*config.needleVel;
      top.fem.pconst.push_back(CVConst(index, top.fem.worldPos[index] + dir*dt*config.needleVel));
      top.fem.vconst.push_back(CVConst(index, dir*config.needleVel));
   }

   // Build new basis
   xw = dir;
   yw = zw.Cross(xw);

   MATRIX3 rotMat;
   rotMat.FromAxisAngle(xw, config.needleAngularVel*dt);
   yw = rotMat * yw;
   real estDis = max(config.needleVel*config.needleLFED*dt, config.minFeatureSize);
   xDisWorld = top.fem.worldPos[index] + xw*estDis;
   yDisWorld = top.fem.worldPos[index] + yw*estDis; 

   if (top.ComputeMatPosGuess(xDisWorld, affectedTets[0], xDisMat) == -1) {
      xDisMat = top.nodePos[index] + config.needleInitForwardDir*config.needleVel*config.needleLFED*dt;
   }
   if (top.ComputeMatPosGuess(yDisWorld, affectedTets[0], yDisMat) == -1) {
      yDisMat = top.nodePos[index] + config.needleInitUpDir*config.needleVel*config.needleLFED*dt;
   }
   
   //logfile<<endl;
   // Now, need to consider each shaft node whether it is slip or not, will move forward regardless whether it is a cut or not
   
   for (int i = 0; i < shaftNodes.size() - 1; i++) {
      VEC3 dir = worldPos[shaftNodes[i+1]] - worldPos[shaftNodes[i]];
      dir.Normalize();
      if (shaftTypes[i] == SHAFT_STATIC) {
         // Static    
         //worldPos[shaftNodes[i]] = dir*dt*config.needleVel;
         top.fem.vconst.push_back(CVConst(shaftNodes[i], dir*config.needleVel));
         if (force[shaftNodes[i]].Dot(dir) < -config.fsmax) {
            shaftTypes[i] = SHAFT_DYNAMIC;
         }
         ////logfile<<"Static"<<endl;
      } else {         
         // Dynamic
         real relVel = velocity[shaftNodes[i]].Dot(dir);
         if (relVel > 0) { 
            top.fem.extForce.push_back(CExternalForce(shaftNodes[i], -dir*config.fsmax, config.dt));
         } else {
            top.fem.extForce.push_back(CExternalForce(shaftNodes[i], dir*config.fsmax, config.dt));
         }
         top.fem.vDirConst.push_back(CVConst(shaftNodes[i], dir));

         if (fabs(relVel) < config.vel0) {
            shaftTypes[i] = SHAFT_STATIC;
         }
        // //logfile<<"Dynamic"<<endl;
      }
   }
   ////logfile<<endl;
   return dt;
}

