
#include "FEM.h"
#include "integrator.h"

// x += ay + bz
void ALinComb(vector<VEC3>& x, const real a, const vector<VEC3>& y, const real b, const vector<VEC3>& z) {
   VEC3* xp = &x[0];
   const VEC3* yp = &y[0];
   const VEC3* zp = &z[0];
   const int n = x.size();
   for (int i = 0; i < n; i++) {
      (*xp) += a*(*yp) + b*(*zp);
      xp++;
      yp++;
      zp++;
   }
}
// x = y + az
void AddMul(vector<VEC3>& x, const vector<VEC3>& y, const real a, const vector<VEC3>& z) {
   VEC3* xp = &x[0];
   const VEC3* yp = &y[0];
   const VEC3* zp = &z[0];
   const int n = x.size();
   for (int i = 0; i < n; i++) {
      (*xp) = (*yp) + a*(*zp);
      xp++;
      yp++;
      zp++;
   }

}

// a += c * b
void Mad(vector<VEC3>& a, const real c, const vector<VEC3>& b) {
   VEC3* ap = &a[0];
   const VEC3* bp = &b[0];
   const int n = a.size();
   for (int i = 0; i < n; i++) {
      (*ap) += c*(*bp);
      ap++;
      bp++;
   }
}

// a = c*a + b
void Macc(vector<VEC3>& a, const real c, const vector<VEC3>& b) {
   VEC3* ap = &a[0];
   const VEC3* bp = &b[0];
   const int n = a.size();
   for (int i = 0; i < n; i++) {
      (*ap) = c*(*ap) + (*bp);
      ap++;
      bp++;
   }
}

real Dot(const vector<VEC3>& a, const vector<VEC3>& b) {
   /*
   const VEC3* ap = &a[0];
   const VEC3* bp = &b[0];
   static vector<real> tmp;
   int num = a.size();
   tmp.resize(num + 1);
   for (int i = 0; i < num; i++) {
      tmp[i] = (*ap).Dot(*bp);
      ap++;
      bp++;
   }
   

   while (num > 1) {
      //cout<<num<<endl;
      int nd2 = num >> 1;
      if (num & 1) {
         tmp[num] = 0;
         nd2++;
      }

     
      real *p = &tmp[0], 
           *q = &tmp[0];

      for (int i = 0; i < nd2; i++) {
         *p = *q + *(q+1);
         q+=2;
         p++;
      }
      num = nd2;
      
   }
   

   
   return tmp[0];
   
*/
   const VEC3* ap = &a[0];
   const VEC3* bp = &b[0];
   real d = 0;
   const int n = a.size();
   for (int i = 0; i < n; i++) {
      d += (*ap).Dot(*bp);
      ap++;
      bp++;
   }
   return d;
   
}

real Norm(const vector<VEC3>& a) {
   return sqrt(Dot(a,a));
}
void CIntegrator::EnforceConstraints(vector<VEC3>& pos, vector<VEC3>& vel, vector<VEC3>& acc) {
   // Constrain the position
   int nc = fem.pconst.size();
   for (int n = 0; n < nc; n++) {
      pos[fem.pconst[n].nodeNum] = fem.pconst[n].vel;
   }

   // Constrain the velocity
   nc = fem.permVconst.size();
   for (int n = 0; n < nc; n++) {
      vel[fem.permVconst[n].nodeNum] = fem.permVconst[n].vel;

      // Make acceleration 0
      acc[fem.permVconst[n].nodeNum] = VEC3(0.0f, 0.0f, 0.0f);
   }
   nc = fem.vconst.size();
   for (int n = 0; n < nc; n++) {
      vel[fem.vconst[n].nodeNum] = fem.vconst[n].vel;

      // Make acceleration 0
      acc[fem.vconst[n].nodeNum] = VEC3(0.0f, 0.0f, 0.0f);
   }

   // Project velocity to constrait direction
   nc = fem.vDirConst.size();
   for (int n = 0; n < nc; n++) {
      real dp = vel[fem.vDirConst[n].nodeNum].Dot(fem.vDirConst[n].vel);
      vel[fem.vDirConst[n].nodeNum] = dp * fem.vDirConst[n].vel;

      // Project acceleration
      dp = acc[fem.vDirConst[n].nodeNum].Dot(fem.vDirConst[n].vel);
      acc[fem.vDirConst[n].nodeNum] = dp * fem.vDirConst[n].vel;

   }
}


CIntegrator::CIntegrator(CFEMMesh& femi) : fem(femi), worldPos(femi.worldPos), matPos(femi.matPos), velocity(femi.velocity), acceleration(femi.acceleration), mass(femi.mass), force(femi.force), config(femi.config) {

}

void CIntegrator::FilterSolution(vector<VEC3>& dvel) {
   VEC3* dv = (VEC3*)&dvel[0];

   // Those with constant velocity must be constrainted to have velocity change = 0
   int nc = fem.vconst.size();
   for (int n = 0; n < nc; n++) {
      dv[fem.vconst[n].nodeNum] = VEC3(0.0f,0.0f,0.0f);
   }
   nc = fem.permVconst.size();
   for (int n = 0; n < nc; n++) {
      dv[fem.permVconst[n].nodeNum] = VEC3(0.0f,0.0f,0.0f);
   }

   // Those with constant direction velocity must be projected to have velocity change only in certain direction
   nc = fem.vDirConst.size();
   for (int n = 0; n < nc; n++) {
      dv[fem.vDirConst[n].nodeNum] = dv[fem.vDirConst[n].nodeNum].Dot(fem.vDirConst[n].vel)*fem.vDirConst[n].vel;
   }
   
}

inline void CIntegrator::PreCondition(const vector<VEC3>& in, vector<VEC3>& out) {
   int nn = in.size();
   const VEC3* vi = &in[0];
   VEC3* vo = &out[0];
   for (int i = 0; i < nn; i++) {
      (*vo) = (*vi);
      (*vo) *= idiag[i];
      vo++;
      vi++;
   }
}

extern void WriteMFile(vector<VEC3>& vec, string name);
extern void WriteMFile(CSparseRow3x3BlockSquareMatrix& mat, string name);
extern void WriteMFile(vector<real>& vec, string name);
extern void WriteMFile(Vector<real>& vec, string name);
extern void WriteMFile(CCoorMatrix<real>& mat, string name);



real CIntegrator::MyCg(vector<VEC3> const & b,
          vector<VEC3>       & x,
          const real tol,
          int  const   max_iter,
          int        & iter) {


   int  neq = b.size() ;
   real  resid, delta_old, delta_new ;
   real delta0 = 0.0f, alpha;
   
   r = b;
   FilterSolution(r);
   // Todo: Diagonal Preconditioner only.
   for (int i = 0; i < neq; i++) {
      delta0 += r[i].SquaredLength() / idiag[i];
   }
   if (delta0 == 0) return delta0;
   MultiplyCGMat(x, r);
   //WriteMFile(r, "rm.m");
   //Mad(r, -1, b);
   Macc(r, -1, b);
//   r*=-1;
  // r+=b;
   FilterSolution(r);
   //WriteMFile(r, "r.m");
   PreCondition(r, c);
   FilterSolution(c);
   //WriteMFile(c, "c.m");
   delta_new = Dot(r, c);
   iter = 0;
   real cutoff = tol * delta0;

   while (iter < max_iter) {
      MultiplyCGMat(c, q);
      FilterSolution(q);
      alpha = delta_new / Dot(c, q);
      Mad(x, alpha, c);
      Mad(r, -alpha, q);
      /*
      for (int i = 0; i < neq; i++) {
         x[i]+=alpha*c[i];
         r[i]-=alpha*q[i];
      }
      */
      PreCondition(r, s);
      delta_old=delta_new;
      delta_new=Dot(r,s);
      real frac = delta_new / delta_old;            
      Macc(c, frac, s);
      /*for (int i = 0; i < neq; i++) {
         c[i] = s[i] + frac*c[i];
      }*/
      FilterSolution(c);

      if (delta_new < cutoff) return delta_new / delta0;
      //cout<<delta_new / delta0<<endl;
      iter++;
   }
   

   return delta_new / delta0;
}

// ------------------------------------- Implicit Euler --------------------------------------------------

CImplicitEuler::CImplicitEuler(CFEMMesh& femi) : CIntegrator(femi) {
}

real CImplicitEuler::TimeStepping(real dti) {
   int numNodes = fem.numNodes;
   dt = dti;
   fem.ComputeAcceleration(dt);

   timer.Start();
 
   vector<VEC3>& b = acceleration;
   dv.resize(numNodes);
   c.resize(numNodes);
   r.resize(numNodes);
   q.resize(numNodes);
   s.resize(numNodes);
   idiag.resize(numNodes);
   memset(&dv[0], 0, sizeof(real)*numNodes*3);
   for (int i = 0; i < numNodes; i++) {
      idiag[i] = 1;
   }

   
   //cout<<"dt = "<<dt<<endl;
   //WriteMFile(b, "rhs.m");
   //WriteMFile(fem.mass, "mass.m");

   //WriteMFile(*fem.D, "D.m");
   
   //WriteMFile(ran, "ran.m");
   //WriteMFile(oran, "oran.m");

	real tolerance = 1e-5;
	int maxIter = 200;
	int numIter = 0;
   real res = MyCg(b,dv,tolerance, maxIter, numIter);
   //cout<<"Cg takes "<<timer.Stop()<<endl;
   //cout<<"Cg converge to "<<res<<" at iteration "<<numIter<<endl;
   
   for (int n = 0; n < numNodes; n++) {
      velocity[n] += dt*dv[n];
   }
   EnforceConstraints(worldPos, velocity, acceleration);


	for (int n = 0; n < numNodes; n++) {
		worldPos[n] += dt * velocity[n];
	}		
   return dt;

}
void CImplicitEuler::MultiplyCGMat(vector<VEC3>& dv, vector<VEC3>& out) {
   fem.D->Multiply(dv, out);
//   WriteMFile(dv, "dv.m");
 //  WriteMFile(out, "out.m");
  VEC3* dvvec = (VEC3 *)(&dv[0]);
  VEC3* outvec= (VEC3 *)(&out[0]);
  for (int i = 0; i < fem.numNodes; i++) {
     outvec[i] = dvvec[i] + dt*outvec[i]/fem.mass[i];
  }
   //out = dv;

}

//-----------------------------------------------------------------------------------------------------------

//------------------------------------- Explicit Euler ------------------------------------------------------

CExplicitEuler::CExplicitEuler(CFEMMesh& femi) : CIntegrator(femi) {
}

real CExplicitEuler::TimeStepping(real dti) {
   int numNodes = fem.numNodes;
   dt = dti;
   fem.ComputeAcceleration(dt);
	// Use lumped mass
	for (int n = 0; n < numNodes; n++) {
		velocity[n] += dt * acceleration[n];
      if (!config.dampPhysical) {
         velocity[n] *= config.velMulFactor;
      }

	}
   EnforceConstraints(worldPos, velocity, acceleration);

	for (int n = 0; n < numNodes; n++) {
		worldPos[n] += dt * velocity[n];
   }
   return dt;
}

//-------------------------------------------------------------------------------------------------------------
//--------------------------------------- Explitcit Midpoint --------------------------------------------------
CExplicitMidpoint::CExplicitMidpoint(CFEMMesh& femi) : CIntegrator(femi) {
}

real CExplicitMidpoint::TimeStepping(real dti) {
   int numNodes = fem.numNodes;
   dt = dti;
   vector<VEC3> tmpVel(numNodes);
   vector<VEC3> oldVel = velocity;

   // Update velocity to half time step
   fem.ComputeAcceleration(dt*0.5);
	for (int n = 0; n < numNodes; n++) {
		velocity[n] += 0.5*dt * acceleration[n];

	}
   EnforceConstraints(worldPos, velocity, acceleration);
   // Update world pos
	for (int n = 0; n < numNodes; n++) {
		worldPos[n] += dt * velocity[n];
   }
   vector<VEC3> oldAcc = acceleration;

   // Update velocity
   fem.ComputeAcceleration(dt*0.5);
	for (int n = 0; n < numNodes; n++) {
		velocity[n] = oldVel[n] + 0.5*dt*(acceleration[n] + oldAcc[n]);
      if (!config.dampPhysical) {
         velocity[n] *= config.velMulFactor;
      }
	}
   EnforceConstraints(worldPos, velocity, acceleration);
   return dt;
}

//------------------------------------------------------------------------------------------------------------
//----------------------------------- Implicit Newmark Integrator --------------------------------------------


CImplicitNewmark::CImplicitNewmark(CFEMMesh& femi, real betai, real gammai) : CIntegrator(femi), gamma(gammai), beta(betai) {

}
real CImplicitNewmark::TimeStepping(real dti) {
   int numNodes = fem.numNodes;

   dt = dti;

   // Compute RHS
   vector<VEC3> &an = acceleration;
   vector<VEC3> &vn= velocity;
   vector<VEC3> &dn = worldPos;
   vector<VEC3> &F = force;
   dv.resize(numNodes);
   c.resize(numNodes);
   r.resize(numNodes);
   q.resize(numNodes);
   s.resize(numNodes);
   idiag.resize(numNodes);
   tmpRHS.resize(numNodes);
   tmpR.resize(numNodes);

   // BiCG Test
   p.resize(numNodes);
   phat.resize(numNodes);
   shat.resize(numNodes);
   t.resize(numNodes);
   v.resize(numNodes);
   rtilde.resize(numNodes); // BiCGStab



   // Enforce velocity constraint
   EnforceConstraints(worldPos, velocity, acceleration);
   fem.ComputeForce(dt); // Compute the non-linear force term

   // Solve M a_n+1 = F + K(h v + h^2((1/2-beta)a_n + beta a_n+1) + D(h((1-gamma)a_n + gamma a_n+1)))
   // Try to solve (M + h^2 beta K + h gamma D) a_n+1 = (- hK) v_n + F + (-h^2(1/2 - beta)K - h(1-gamma)C) a_n
   /*
   for (int i = 0; i < numNodes; i++) {
      if (fabs(force[i][0]) < 1e-6) force[i][0] = 0;
      if (fabs(force[i][1]) < 1e-6) force[i][1] = 0;
      if (fabs(force[i][2]) < 1e-6) force[i][2] = 0;
      if (fabs(velocity[i][0]) < 1e-6) velocity[i][0] = 0;
      if (fabs(velocity[i][1]) < 1e-6) velocity[i][1] = 0;
      if (fabs(velocity[i][2]) < 1e-6) velocity[i][2] = 0;
      if (fabs(acceleration[i][0]) < 1e-6) acceleration[i][0] = 0;
      if (fabs(acceleration[i][1]) < 1e-6) acceleration[i][1] = 0;
      if (fabs(acceleration[i][2]) < 1e-6) acceleration[i][2] = 0;
   }*/
   tmpRHS = F;

/*   fem.D->Multiply(vn, tmpR);
   Mad(tmpRHS, -1, tmpR); */

   fem.K->Multiply(vn, tmpR);
   Mad(tmpRHS, -dt, tmpR); 

   fem.K->Multiply(an, tmpR);
   Mad(tmpRHS, -dt*dt*(0.5-beta), tmpR); 


   fem.D->Multiply(an, tmpR);
   Mad(tmpRHS, -dt*(1-gamma), tmpR); 

   // Solve an+1 into dv
   dv = an; // Initialize with an
   //memset(&dv[0], 0, sizeof(real)*3*numNodes); 
   real* ii = &idiag[0];
   for (int i = 0; i < numNodes; i++) {      
      *(ii++) =  1.0 / mass[i];
      //*(ii++) =  1.0;
   }
   //cout<<"dt = "<<dt<<" beta = "<<beta<<" gamma = "<<gamma<<endl;
   //WriteMFile(tmpRHS, "rhs.m");
   //WriteMFile(CCoorMatrix<real>(*fem.K), "K.m");
   //WriteMFile(CCoorMatrix<real>(*fem.D), "D.m");

//   memset(dv.begin(), 0, sizeof(real)*nn3);
 	real tolerance = 1e-5;
	int maxIter = 2000;
	int numIter = 0;
   timer.Start();
   real res = MyCg(tmpRHS,dv,tolerance, maxIter, numIter);
   
   // BiCG
      //int mode = 0;
      //real res = MyBiCGSTAB(tmpRHS,dv,tolerance, maxIter, numIter, mode);

   //cout<<"BiCG converge to "<<res<<" at iteration "<<numIter<<" with mode "<< mode<<endl;
   cout<<"Cg takes "<<timer.Stop()<<endl;
   cout<<"Cg converge to "<<res<<" at iteration "<<numIter<<endl;
   VEC3* dvel = (VEC3*)&dv[0];
   
   for (int n = 0; n < numNodes; n++) {
      worldPos[n] += dt*velocity[n] + dt*dt*((0.5-beta)*acceleration[n] + beta*dvel[n]);;
      velocity[n] += dt*((1-gamma)*acceleration[n] + gamma*dvel[n]);
      //acceleration[n] = dvel[n];
      acceleration[n] = ((1-gamma)*acceleration[n] + gamma*dvel[n]);
      //acceleration[n] = VEC3(0,0,0);
      //acceleration[n] = 0.5*(acceleration[n] + dvel[n]); // Artificially low pass out acceleration
   }
   EnforceConstraints(worldPos, velocity, acceleration);





   return dt;
}
void CImplicitNewmark::MultiplyCGMat(vector<VEC3>& dv, vector<VEC3>& out) {

   // (M + h^2 beta K + h gamma D)
   int numNodes = fem.numNodes;
   VEC3* dvVec = (VEC3*) &dv[0];
   VEC3* outVec = (VEC3*) &out[0];
   for (int i = 0; i < numNodes; i++) {
      (*outVec++) = mass[i]*(*dvVec++);

   }
   fem.K->Multiply(dv, tmpR);
   Mad(out, dt*dt*beta, tmpR);

   fem.D->Multiply(dv, tmpR);
   Mad(out, dt*gamma, tmpR);
   /*
  out = (*fem.D)*dv;
  VEC3* dvvec = (VEC3 *)(dv.begin());
  VEC3* outvec= (VEC3 *)(out.begin());
  for (int i = 0; i < fem.numNodes; i++) {
     outvec[i] = dvvec[i] - dt*outvec[i]/fem.mass[i];
  }
  */
   //out = dv;

}
//------------------------------------------------------------------------------------------------------------

//*****************************************************************
// Iterative template routine -- BiCGSTAB
//
// BiCGSTAB solves the unsymmetric linear system Ax = b 
// using the Preconditioned BiConjugate Gradient Stabilized method
//
// BiCGSTAB follows the algorithm described on p. 27 of the 
// SIAM Templates book.
//
// The return value indicates convergence within max_iter (input)
// iterations (0), or no convergence within max_iter iterations (1).
//
// Upon successful return, output arguments have the following values:
//  
//        x  --  approximate solution to Ax = b
// max_iter  --  the number of iterations performed before the
//               tolerance was reached
//      tol  --  the residual after the final iteration
//  
//*****************************************************************
real CIntegrator::MyBiCGSTAB(vector<VEC3> &b, vector<VEC3> &x, const real tol, const int max_iter, int& iter, int& mode)
//int CIntegrator::MyBiCGSTAB(vector<VEC3> &x, const vector<VEC3> &b, int &max_iter, real &tol)
{
  real resid;
  real rho_1, rho_2, alpha, beta, omega;

//  FilterSolution(b);
  real normb = Norm(b);
  
  MultiplyCGMat(x, r); // r = Ax
  Macc(r, -1, b); // r = b + (-1)r
  FilterSolution(r);
  //r = b - A * x;

  rtilde = r;

  if (normb == 0.0)
    normb = 1;
  
  if ((resid = Norm(r) / normb) <= tol) {
    iter = 0;
    mode = 0;
    return resid;
  }

  for (int i = 1; i <= max_iter; i++) {
    rho_1 = Dot(rtilde, r);
    if (rho_1 == 0) {
      mode = 2;
      iter = i;
      return Norm(r) / normb;
    }
    if (i == 1)
      p = r;
    else {
      beta = (rho_1/rho_2) * (alpha/omega);
      Mad(p, -omega, v); // p -= omega*v
      Macc(p, beta, r); // p = r + beta*p;
      FilterSolution(p);
      //p = r + beta * (p - omega * v);
    }
    PreCondition(p, phat);
    FilterSolution(phat);
    //phat = M.solve(p);
    MultiplyCGMat(phat, v);
    FilterSolution(v);
    //v = A * phat;
  
    alpha = rho_1 / Dot(rtilde, v);
    AddMul(s, r, -alpha, v);
    FilterSolution(s);
    // s = r - alpha * v;
    if ((resid = Norm(s)/normb) < tol) {
      Mad(x, alpha, phat);
      //x += alpha * phat;
      
      mode = 0;
      iter = i;
      return resid;
    }
    PreCondition(s, shat);
    FilterSolution(shat);
    //shat = M.solve(s);
    MultiplyCGMat(shat, t);
    FilterSolution(t);
    //t = A * shat;
    omega = Dot(t,s) / Dot(t,t);
    ALinComb(x, alpha, phat, omega, shat);
    FilterSolution(x);
    //x += alpha * phat + omega * shat;

    AddMul(r, s, -omega, t);
    FilterSolution(r);
    //r = s - omega * t;

    rho_2 = rho_1;
    if ((resid = Norm(r) / normb) < tol) {
      iter = i;
      mode = 0;
      return resid;
    }
    if (omega == 0) {
      iter = i;
      mode = 3;
      return Norm(r) / normb;
    }
  }

  
  iter = max_iter;
  mode = 1;
  return resid;

}
