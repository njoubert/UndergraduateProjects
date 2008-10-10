#ifndef __NEEDLESIM_H__
#define __NEEDLESIM_H__

#include "FEM.h"

// This class is used for indicating the change in needle movement.
class CNeedleChange {
public:
   CNeedleChange(real timei,
           real needleVeli,
           real needleBendAngularVeli,
           real needleAngularVeli);

   real time; // indicates the time when this change happen, in s
   real needleVel; // indicates the speed of the needle in m/s
   real needleBendAngularVel; // indicates how fast the needle bend upward in rad/s
   real needleAngularVel; // indicates how fast the needle rotate the forward direction in rad/s


};

// This class is used to store the history of the needle path
class CNeedlePathRecord{
public:
   CNeedlePathRecord(real timei, VEC3& posi);

   real time; // time that this record indicates, in s
   VEC3 pos; // position in material space of the needle tip at the time 
};

// Class for needle simulation
class CNeedleSimulation{
public:

   
   //Constructor, configi is the configulation of the needle simulation
   //             condp is the pointer to a class with operator to determine if a node should be constrainted or not
   //             tipIndexi is the index of the vertex that is considered tip
   //             changesi is the list of changes of the needle movement
   //                      it has to be sorted in ascending order of time
   CNeedleSimulation(CConfig& configi, CConstraintDecision* condp, int tipIndexi, vector<CNeedleChange>& changesi);

   // Constructor, will make the needle tip to be in the position that is the
   //              first intersection of the ray starting at pos in the direction of
   //              configi.needleInitForwardDir intersects with the mesh
   CNeedleSimulation(CConfig& configi, CConstraintDecision* condp, VEC3 pos, vector<CNeedleChange>& changesi);
   ~CNeedleSimulation();

   // Run the simulation for totalTime
   // The trajectory of needle in material space is stored in result
   void Run(real totalTime, vector<CNeedlePathRecord>& result);

   CFEMMesh* mesh;
   int tipIndex;
   CConfig config;
   vector<CNeedleChange> changes;

};

#endif