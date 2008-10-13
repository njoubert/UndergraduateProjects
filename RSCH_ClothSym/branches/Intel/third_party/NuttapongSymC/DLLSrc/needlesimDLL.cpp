#include "needlesimDLL.h"


class CListCond : public CConstraintDecision {
public:
   vector<int> fixedNodes;
   int index;
   CListCond(vector<int>& fixNodesi) {
      fixedNodes = fixNodesi;
      sort(fixedNodes.begin(), fixedNodes.end());
   }

   virtual bool operator () (CConfig& configi, int index, VEC3& pos) const {
      return binary_search(fixedNodes.begin(), fixedNodes.end(), index);
   }
};

NeedleSimInterface::NeedleSimInterface() {
   fem = NULL;
}
bool NeedleSimInterface::LoadModel(const char* model) {
   //config.Load(
   config.LoadConfig(string(model));
   CListCond cons(config.fixedNodes);
   fem = new CFEMMesh(config, &cons);
   int tipIndex = fem->topology->AddBoundaryVertexFromRayMeshIntersection(config.needleInitPos, config.needleInitForwardDir);
   if (tipIndex != -1) {
      fem->tip.index = tipIndex;
      fem->topology->InitTip(fem->tip.index, fem->cutTip);
   } 


   return true;
}
/// Resets the simulation to the uninitialized state
void NeedleSimInterface::Reset() {
   SAFE_DELETE(fem);
}
/// Return true if the simulation has been initialized
bool NeedleSimInterface::IsInitialized() {
   return fem != NULL;
}

	/// Returns a vector representing the current state information
void NeedleSimInterface::GetCurrentState(State& state) {
   CLinearMemStorage f(state);
   config.SaveStorage(f);
   fem->SaveStorage(f);
   f.Close();

}
/// Sets the current state to the given vector (which should have been returned by a prior state)
void NeedleSimInterface::SetCurrentState(const State& state) {
   SAFE_DELETE(fem);

   CLinearMemStorage f(const_cast<State& >(state));
   config.LoadStorage(f);
   fem = new CFEMMesh(config, f);
   f.Close();

}
/// Returns true if the state is valid
bool NeedleSimInterface::IsValidState(const State& state) const {
   // No easy way yet to know that....
   return true;
}
/// Forward-simulates from the given state using the given control input
void NeedleSimInterface::Simulate(const ControlInput& control,double dt) {
   real ct = 0;
   ControlInputWrapper ci(const_cast<ControlInput&> (control));
   ci.GetControl(config.needleVel, config.needleBendAngularVel, config.needleAngularVel);
   while (ct < dt - ZERO_TOLERANCE_NUT) {
      real amt = min(config.dt, dt - ct);
      fem->cutTip.MoveNeedleForwardAndApplyConstraintsAndForce(*fem->topology, amt);
      fem->tip.index = fem->cutTip.index;
      fem->numNodes = fem->matPos.size();
      fem->numTets = fem->tet2node.size();
      ct += fem->Simulate(amt);
   }  
}

/// Accessors (GetXXX returns the quantity for a given state, GetCurrentXXX returns the quantity for the current state)
/// World coordinates of the needle tip
void NeedleSimInterface::GetNeedleTipPosition(const State& state,Vector3& pos) {
   exit(1);

}

void NeedleSimInterface::GetCurrentNeedleTipPosition(Vector3& pos) {
   pos = fem->worldPos[fem->cutTip.index];
}

/// Direction of the needle tip (for a given state, or the current state 
void NeedleSimInterface::GetNeedleHeading(const State& state,Vector3& dir) {
   exit(1);   
}

void NeedleSimInterface::GetCurrentNeedleHeading(Vector3& dir) {
   Vector3 y, z;
   fem->cutTip.GetCoordSystem(*fem->topology, dir, y, z);
}

/// Transform of the needle tip 
void NeedleSimInterface::GetNeedleTransform(const State& state,Matrix4& T) {
   exit(1);
}

void NeedleSimInterface::GetCurrentNeedleTransform(Matrix4& T){
   Vector3 x, y, z;
   Vector3 pos = fem->worldPos[fem->cutTip.index];
   fem->cutTip.GetCoordSystem(*fem->topology, x, y, z);
   T = Matrix4(x[0], x[1], x[2], pos[0],
             y[0], y[1], y[2], pos[1],
             z[0], z[1], z[2], pos[2],
              0.0,  0.0,  0.0, 1.0);          
}

/// Gets the world->tissue coordinates
void NeedleSimInterface::FindTissuePoint(const Vector3& pos, TissuePoint& tp){
   int gt = 0;
   // Start from a tet that affected tip node
   if ((fem->cutTip.affectedTets.size() > 0) && 
      (fem->cutTip.affectedTets[0] < fem->tet2node.size())) gt = fem->cutTip.affectedTets[0];
   Vector3 wpos = pos;
   // Find which tet the point is in
   gt = fem->topology->PointLocationAnyCoord(fem->worldPos, wpos, gt);
   tp.elementindex = gt;

   // Compute barycentric coordinate
   fem->topology->BaryCoordsAnyCoord(fem->worldPos, wpos, gt, tp.bary[0], tp.bary[1], tp.bary[2], tp.bary[3]);
}

void NeedleSimInterface::GetTissuePointPosition(const TissuePoint& tp,Vector3& pos){
   vector<Vector3>& wps = fem->worldPos;
   CTet& t2n = fem->tet2node[tp.elementindex];
   pos = wps[t2n[0]]*tp.bary[0] + 
         wps[t2n[1]]*tp.bary[1] + 
         wps[t2n[2]]*tp.bary[2] + 
         wps[t2n[3]]*tp.bary[3];
}

/// Returns a dictionary of all stats
void NeedleSimInterface::GetStats(const State& state,std::map<std::string,double>& stats) {
}

/// Turns on/off stat reporting for the given stat name
void NeedleSimInterface:: EnableStat(const string& statName,bool enabled) {
}

/// Turns on/off all stat reporting
void NeedleSimInterface::EnableStats(bool enabled) {
}

