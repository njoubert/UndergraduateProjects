#include "globals.h"
#include "Wm4Vector4.h"
#include "Wm4Matrix4.h"
#include "GL/glut.h"
#include <limits>
#include "font.h"
#include "GL/glu.h"
#include "GL/gl.h"
GLUquadric* quadric;



using namespace Wm4;
typedef Matrix4<real> MAT4;
typedef Vector4<real> VEC4;
VEC gravity(0,-10, 0);

class CDER{
public:

   CDER();
   void ComputeForce();

   vector<real> mass, li, invli, ebar;
   vector<VEC> r, e, kb, force, kbden;
   vector<VEC> rp;
   real rho, R, gammat, Ks, Es;


};

CDER::CDER() {
   
   int n = 40;
   real totalLength = 7;
   R = 0.01;
   rho = 1300;
   gammat = 10-6;
   Ks = Es*MYPI*R*R;
   
   Ks = Es*MYPI*R*R;

   mass.resize(n);
   r.resize(n);
   rp.resize(n);
   li.resize(n-1);
   invli.resize(n-1);
   e.resize(n-1);
   ebar.resize(n-1);
   force.resize(n);
   
   for (int i = 0; i < n; i++) {
      r[i] = VEC(i*totalLength/n, 0, 0);
      rp[i] = VEC(0,0,0);
      mass[i] = 0.0f;
   }

   // Compute mass and li
   for (int i = 0; i < n-1; i++) {
      li[i] = (r[i+1] - r[i]).Length();     
      invli[i] = 1.0 / li[i];
      real m = rho*MYPI*R*R*(li[i]);
      mass[i] += 0.5*m;
      mass[i+1] += 0.5*m;
   }
}
MAT3 OuterProd(VEC& u, VEC& v) {
   return MAT3(u[0]*v[0], u[0]*v[1], u[0]*v[2],
               u[1]*v[0], u[1]*v[1], u[1]*v[2],
               u[2]*v[0], u[2]*v[1], u[2]*v[2]);
}
void CDER::ComputeForce() {
   int numNodes = force.size();
   memset(&force[0], 0, sizeof(VEC)*numNodes);

   // Compute e and ebar
   for (int i = 0; i < numNodes-1; i++) {
      e[i] = r[i+1] - r[i];
      ebar[i] = e[i].Length();
   }

   // CORDE Forces
   for (int i = 0; i < numNodes-1; i++) {      
      // Force due to potential energy
      VEC sub = r[i+1]-r[i];     
      real A = (sub).SquaredLength();
      real sqrtA = sqrt(A);
      VEC dVsdri = Ks*(invli[i] - 1.0/sqrtA)*(-sub);
      force[i] += dVsdri;
      force[i+1] -= dVsdri;

      // Force due to dissipation energy
      VEC subv = rp[i+1]-rp[i];
      real il3 = invli[i]*invli[i]*invli[i];
      VEC vrel = il3*(sub*sub.Dot(subv));
      VEC dDtdrpi(0,0,0);
      for (int k = 0; k < 3; k++) {
         dDtdrpi += vrel[k]*il3*sub[k]*(-sub);
      }
      dDtdrpi*=li[i]*gammat;
      force[i] += dDtdrpi;
      force[i+1] -= dDtdrpi;          
   }
   
   // DER Forces
   for (int i = 0; i < numNodes; i++) {
      int im1 = max(0, i-1);
      kbden[i] = 1.0 / (ebar[im1]*ebar[i] + e[im1].Dot(e[i]));
      kb[i] = 2.0*e[im1].Cross(e[i])*kbden[i];

   }
   for (int i = 0; i < numNodes; i++) {
      int im2 = max(0, i-2);
      int ip1 = min(n-1, i+1);


   }

   
}

