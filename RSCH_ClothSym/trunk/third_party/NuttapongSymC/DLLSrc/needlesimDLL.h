//Created by Kris Hauser
//9/17/2008
// Modify by Nuttapong Chentanez
//09/26/08

//TODO: implement DLL interface code

#ifndef NEEDLESIMLIB_INTERFACE_H
#define NEEDLESIMLIB_INTERFACE_H

#ifdef NEEDLESIM_EXPORT
	#define NEEDLESIMAPI __declspec(dllexport)
#else
	#define NEEDLESIMAPI __declspec(dllimport)
#endif

#include <vector>
#include <map>
#include "globals.h"
#include "Wm4Matrix4.h"
#include "FEM.h"

class ControlInputWrapper{
public:

   ControlInputWrapper(vector<unsigned char>& ci) : c(ci) {
      csize = sizeof(real) * 3;
   }
   void SetControl(const real needleVel, const real needleBendAngularVel, const real needleAngularVel) {
      
      c.resize(csize);
      real* ptr = (real*)&c[0];
      ptr[0] = needleVel;
      ptr[1] = needleBendAngularVel;
      ptr[2] = needleAngularVel;
      
   }

   bool GetControl(real& needleVel, real& needleBendAngularVel, real& needleAngularVel) {
      if (c.size() != csize) return false;
      real* ptr = (real*)&c[0];
      needleVel = ptr[0];
      needleBendAngularVel = ptr[1];
      needleAngularVel = ptr[2];
      return true;
   }
   vector<unsigned char> &c;
   size_t csize;
};
class NEEDLESIMAPI NeedleSimInterface
{
public:
	//data structure definitions
	typedef std::vector<unsigned char> State;
	typedef std::vector<unsigned char> ControlInput;
	typedef VEC3 Vector3;   //a 3d vector
	typedef Wm4::Matrix4<double> Matrix4;  //a 4x4 matrix (column major)
	struct TissuePoint
	{
		int elementindex;
		double bary[4];
	};

   NeedleSimInterface();
	/// Loads the given model file, returns true if successful
	bool LoadModel(const char* model);
	/// Resets the simulation to the uninitialized state
	void Reset();
	/// Return true if the simulation has been initialized
	bool IsInitialized();

	/// Returns a vector representing the current state information
	void GetCurrentState(State& state);
	/// Sets the current state to the given vector (which should have been returned by a prior state)
	void SetCurrentState(const State& state);
	/// Returns true if the state is valid
	bool IsValidState(const State& state) const;
	/// Forward-simulates from the given state using the given control input
	void Simulate(const ControlInput& control,double dt);

	/// Accessors (GetXXX returns the quantity for a given state, GetCurrentXXX returns the quantity for the current state)
	/// World coordinates of the needle tip
	void GetNeedleTipPosition(const State& state,Vector3& pos);
	void GetCurrentNeedleTipPosition(Vector3& pos);
	/// Direction of the needle tip (for a given state, or the current state)
	void GetNeedleHeading(const State& state,Vector3& dir);
	void GetCurrentNeedleHeading(Vector3& dir);
	/// Transform of the needle tip 
	void GetNeedleTransform(const State& state,Matrix4& T);
	void GetCurrentNeedleTransform(Matrix4& T);
	/// Gets the world->tissue coordinates
	void FindTissuePoint(const Vector3& pos,TissuePoint& tp);
	void GetTissuePointPosition(const TissuePoint& tp,Vector3& pos);

	/// Returns a dictionary of all stats
	void GetStats(const State& state,std::map<std::string,double>& stats);
	/// Turns on/off stat reporting for the given stat name
	void EnableStat(const string& statName,bool enabled=true);
	/// Turns on/off all stat reporting
	void EnableStats(bool enabled=true);

private:
   CFEMMesh* fem;
   CConfig config;
   //CDER* der;


};

#endif