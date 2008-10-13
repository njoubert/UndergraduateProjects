#include "config.h"
#include "FEM.h"
#include "needleSim.h"

class CFixTopBotCond : public CConstraintDecision {
    virtual bool operator () (CConfig& configi, int index, VEC3& pos) const {
       return ((pos[1] > 0.475*configi.sceneScale) || (pos[1] < -0.475*configi.sceneScale));
    }
};

// Main without graphics
int main(int argc,char** argv) {
   // Load configuration from file config.txt
   CConfig config;
   config.LoadConfig("config.txt");

   // Set the initial direction of the needle to x direction
   config.needleInitForwardDir = VEC3(1,0,0);
   config.needleInitUpDir = VEC3(0,1,0);
   CFixTopBotCond cond;

   vector<CNeedleChange> changes;   
   // Has just one change at time 0
   changes.push_back(CNeedleChange(0, 0.0f, 0.0f, 0.0f)); 

   vector<CNeedlePathRecord> path;  
   for (int t = 0; t < 10; t++) {
      // At time 0, set velocity to be something and keep it for the rest of the simulation
      changes[0].needleVel = (t+1)*0.02; 
      CNeedleSimulation sim(config, &cond, VEC3(-1, 0.01, 0.008), changes); // The needle tip is node 25 (left middle node of the cube for the bar_med.mesh)

      // Run the simulation for 0.2 second
      sim.Run(0.2f, path);

      // Write out the trajectory of the needle tip
      cout<<"For speed "<<changes[0].needleVel<<" m/s, we get"<<endl;
      for (int i = 0; i < path.size(); i++) {
         cout<<"   "<<path[i].time<<" : "<<path[i].pos<<endl;
      }

   }
   

   return 0;
}