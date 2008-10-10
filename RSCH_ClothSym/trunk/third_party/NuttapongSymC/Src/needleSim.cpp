#include "needleSim.h"
CNeedleChange::CNeedleChange(real timei,
                 real needleVeli,
                 real needleBendAngularVeli,
                 real needleAngularVeli) {
   time = timei;
   needleVel = needleVeli;
   needleBendAngularVel = needleBendAngularVeli;
   needleAngularVel = needleAngularVeli;
}
CNeedlePathRecord::CNeedlePathRecord(real timei, VEC3& posi) {
   time = timei;
   pos = posi;
}


CNeedleSimulation::~CNeedleSimulation() {
   SAFE_DELETE(mesh);
}

CNeedleSimulation::CNeedleSimulation(CConfig& configi, CConstraintDecision* condp, VEC3 pos, vector<CNeedleChange>& changesi) : config(configi), changes(changesi) {
   mesh = new CFEMMesh(config, condp);
   // Need to create a new vertex




}

CNeedleSimulation::CNeedleSimulation(CConfig& configi, CConstraintDecision* condp, int tipIndexi, vector<CNeedleChange>& changesi) : config(configi), changes(changesi) {
   mesh = new CFEMMesh(config, condp);
   tipIndex = tipIndexi;
   mesh->tip.index = tipIndex;
   mesh->topology->InitTip(mesh->tip.index, mesh->cutTip);
}

void CNeedleSimulation::Run(real totalTime, vector<CNeedlePathRecord>& result) {
   if (tipIndex == -1) return; // Return immediately if tipIndex == -1
   real ct = 0;
   result.clear();
   result.push_back(CNeedlePathRecord(0.0, mesh->matPos[mesh->cutTip.index]));
   int t = 0;
   while (ct < totalTime) {
      real targetTime = 0;
      if (t < changes.size()) {
         config.needleAngularVel = changes[t].needleAngularVel;
         config.needleBendAngularVel = changes[t].needleBendAngularVel;
         config.needleVel = changes[t].needleVel;
         targetTime = changes[t].time;
      } else {
         targetTime = totalTime;
      }
      while (ct < targetTime) {
         real ddt = mesh->cutTip.MoveNeedleForwardAndApplyConstraintsAndForce(*mesh->topology, config.dt);
         //cout<<ct<<" : "<<ddt<<endl;
         if (ddt < 1e-10) {
            // Over, move outside domain
            return;
         }
         mesh->tip.index = mesh->cutTip.index;
         mesh->numNodes = mesh->matPos.size();
         mesh->numTets = mesh->tet2node.size();
         ct += mesh->Simulate(config.dt);
         result.push_back(CNeedlePathRecord(ct, mesh->matPos[mesh->cutTip.index]));
      
     }
     t++;
   }
}
