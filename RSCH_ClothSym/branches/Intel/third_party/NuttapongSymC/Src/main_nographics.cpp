#include "config.h"
#include "FEM.h"
#include "needleSim.h"
LOGGILE_TYPE logfile("log.txt");

// Main without graphics
int main(int argc,char** argv) {
   // Let's vary needle velocity
   CConfig config;
   config.LoadConfig("config.txt");

   config.needleInitForwardDir = VEC(1,0,0);
   config.needleInitUpDir = VEC(0,1,0);

   vector<CNeedleChange> changes;
   vector<CNeedlePathRecord> path;
   changes.push_back(CNeedleChange(0, 0.0f, 0.0f, 0.0f)); // At time 0, set velocity to be something
   for (int t = 0; t < 10; t++) {
      changes[0].needleVel = (t+1)*0.1; // At time 0, set velocity to be something
      CNeedleSimulation sim(config, 25, changes);
      sim.Run(2.0f, path);
      cout<<"For speed "<<changes[0].needleVel<<" m/s, we get"<<endl;
      for (int i = 0; i < path.size(); i++) {
         cout<<"   "<<path[i].time<<" : "<<path[i].pos<<endl;
      }

   }
   

   return 0;
}