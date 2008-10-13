#ifndef __INTEGRATOR_H__
#define __INTEGRATOR_H__
#include "Wm4Matrix3.h"
#include <vector>

#include "globals.h"
#include "sparselib.h"
#include "myTimer.h"
#include "config.h"
#include "topology.h"

#ifdef __GNUC__
#include <ext/hash_map>
using namespace __gnu_cxx;
#else
#include <hash_map>
using namespace stdext;
#endif

using namespace sparselib;

class CFEMMesh;
class CIntegrator{
public:
   
   // Also set flag what matrices to build, K, D
   CIntegrator(CFEMMesh& femi);

   // Do a time stepping,
   // by compute force, 
   //    assemble K, D, matrices (elastic jacobian, damping jacobian)
   //    perform an integration    
   virtual real TimeStepping(real dti) {return dti; }; 
   virtual void MultiplyCGMat(vector<VEC3>& dv, vector<VEC3>& out) {}

   inline void PreCondition(const vector<VEC3>& in, vector<VEC3>& out);

   // Filter the solution of the linear system so as not to violate constraints
   virtual void FilterSolution(vector<VEC3>& dvel);

   real MyCg(const vector<VEC3>& b,
       vector<VEC3>       & x,
       const real tol,
      int  const   max_iter,
      int        & iter);

//   int MyBiCGSTAB(vector<VEC3> &x, const vector<VEC3> &b, int &max_iter, real &tol);
   real MyBiCGSTAB(vector<VEC3> &b, vector<VEC3> &x, const real tol, const int max_iter, int& iter, int& mode);

   // Project velocity so that it does not violate constraints
   void EnforceConstraints(vector<VEC3>& pos, vector<VEC3>& vel, vector<VEC3>& acc);

// Variables
   CConfig& config;
   CFEMMesh& fem; 
   // For linear implicit
   vector<real> idiag;

   vector<VEC3> dv, c, q; // CG
   vector<VEC3> p, phat, shat, t, v, rtilde; // BiCGStab
   vector<VEC3> r, s; // Both CG and BiCGStab

   real dt;
   CMyTimer timer; // timer

   vector<VEC3> &worldPos, &matPos, &velocity, &acceleration, &force;
   vector<real> &mass;
};


class CImplicitEuler : public CIntegrator {
public:
   CImplicitEuler(CFEMMesh& femi);
   virtual real TimeStepping(real dti); 
   virtual void MultiplyCGMat(vector<VEC3>& dv, vector<VEC3>& out);
};


class CExplicitEuler : public CIntegrator {
public:
   CExplicitEuler(CFEMMesh& femi);
   virtual real TimeStepping(real dti); 



};

class CExplicitMidpoint : public CIntegrator {
public:
   CExplicitMidpoint(CFEMMesh& femi);
   virtual real TimeStepping(real dti); 

};

class CImplicitNewmark : public CIntegrator {
public:
   CImplicitNewmark(CFEMMesh& femi, real betau, real gammai);
   virtual real TimeStepping(real dti);
   virtual void MultiplyCGMat(vector<VEC3>& dv, vector<VEC3>& out); 

   vector<VEC3> tmpRHS, tmpR;
   const real beta, gamma;

};
#endif