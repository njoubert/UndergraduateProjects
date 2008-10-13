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

int pickedVertex = -1;
bool holdRight = false, holdMid = false;

VEC3 gravity(0,-10, 0);
class CSpMat4{
public:
   CSpMat4() {
   }
   CSpMat4(char i0, real c0,
           char i1, real c1,
           char i2, real c2,
           char i3, real c3) {
      index[0] = i0; coef[0] = c0;
      index[1] = i1; coef[1] = c1;
      index[2] = i2; coef[2] = c2;
      index[3] = i3; coef[3] = c3;
   }
   int index[4];
   real coef[4];
};

VEC4 operator * (const CSpMat4& m, const VEC4& v) {
   return VEC4 (m.coef[0]*v[m.index[0]], m.coef[1]*v[m.index[1]], m.coef[2]*v[m.index[2]], m.coef[3]*v[m.index[3]]);
}
class CExternalForce{
public:

   CExternalForce(const int nodeNum, const VEC3 force, const real dur) : nodeNum(nodeNum), force(force), duration(dur) {
   }
   int nodeNum;
   VEC3 force;
   real duration;
};
class CExternalTorque{
public:

   CExternalTorque(const int nodeNum, const VEC3 torque, const real dur) : nodeNum(nodeNum), torque(torque), duration(dur) {
   }
   int nodeNum;
   VEC3 torque;
   real duration;
};


class CCorde{
public:
   CCorde();
   void ComputeForceTorque();
   void UpdateStates(real dt);
   void AdvanceTimeForExternalForceTorque(real dt);


   vector<real> mass;
   vector<VEC3> r;
   vector<VEC3> rp;
   vector<VEC3> ohm;
   vector<VEC4 > q, qp;
   vector<real> li, lj, invli, invlj; // Lengths for position and orientation respectively, and their recipocal
   vector<VEC3> force;
   vector<VEC4> torque; // This is quaternion acceleration 
   vector<VEC3> tauBar; // Eulerian torque computed from torque
   real R; // Radius, in m
   real rho; // Density, in kg / m^3
   real E;   // Young modulus
   real G;   // Shear modulus
   real Es;  // Stetch modulus
   real Ks;
   real kappa; // Spring const
   real gammat; 
   real gammar;
   MATRIX3 I, invI;    // Inertia matrix and its inverse
   MATRIX3 K;    // Stiffness tensor
   VEC3 uk;   // Intrinsic bending

   vector<CSpMat4> B0, B; 
   vector<CExternalForce> extForce;
   vector<CExternalTorque> extTorque;



};
void CCorde::AdvanceTimeForExternalForceTorque(real dt) {
   // Add external force
   int numExt = extForce.size();
   for (int i = 0; i < numExt; i++) {
      //cout<<"Force at "<<extForce[i].nodeNum<<" was "<<force[extForce[i].nodeNum]<<" now "<<extForce[i].force<<endl;
      extForce[i].duration -= dt;
      if (extForce[i].duration <= 1e-10) {
         extForce[i] = extForce[numExt - 1];
         extForce.pop_back();
         numExt--;
         i--;
      }
   }
   numExt = extTorque.size();
   for (int i = 0; i < numExt; i++) {
      //cout<<"Force at "<<extForce[i].nodeNum<<" was "<<force[extForce[i].nodeNum]<<" now "<<extForce[i].force<<endl;
      extTorque[i].duration -= dt;
      if (extTorque[i].duration <= 1e-10) {
         extTorque[i] = extTorque[numExt - 1];
         extTorque.pop_back();
         numExt--;
         i--;
      }
   }
}



CCorde::CCorde() {
   // Initialize B and B0
   B.resize(3);
   B0.resize(3);
   B[0] = CSpMat4(3,1,2,1,1,-1,0,-1);
   B[1] = CSpMat4(2,-1,3,1,0,1,1,-1);
   B[2] = CSpMat4(1,1,0,-1,3,1,2,-1);

   B0[0] = CSpMat4(3,1,2,-1,1,1,0,-1);
   B0[1] = CSpMat4(2,1,3,1,0,-1,1,-1);
   B0[2] = CSpMat4(1,-1,0,1,3,1,2,-1);

   
   int n = 40;
   real totalLength = 7;
   R = 0.01;
   rho = 1300;
   E = 0.5e5;
   G = 0.5e5;
   Es = 20e5;
   kappa = 100000;
   gammat = 10-6;
   gammar = 0.1e-6;

   uk = VEC3(0,0,0);
   Ks = Es*MYPI*R*R;
   
/*
   int n = 100;
   real totalLength = 0.1;
   R = 0.001;
   rho = 7860;
   E = 200e6;
   G = 100e6;
   Es = 100e6;
   kappa = 300000;
   gammat = 0.05e-6;
   gammar = 0.01e-6;
*/
   uk = VEC3(0,0,0);
   Ks = Es*MYPI*R*R;

   mass.resize(n);
   r.resize(n);
   rp.resize(n);
   ohm.resize(n);
   q.resize(n-1);
   qp.resize(n-1);
   li.resize(n-1);
   invli.resize(n-1);
   lj.resize(n-2);
   invlj.resize(n-2);
   force.resize(n);
   torque.resize(n-1);
   
   /*
   R = 0.01;
   rho = 1300;
   E = 0.5e6;
   G = 0.5e6;
   Es = 20e6;
   kappa = 100e3;
   gammat = 10;
   gammar = 1;
   */


   K.MakeDiagonal(E*MYPI*R*R*0.25, E*MYPI*R*R*0.25, G*MYPI*R*R*0.5);
   I.MakeDiagonal(rho*MYPI*R*R*0.25, rho*MYPI*R*R*0.25, rho*MYPI*R*R*0.5);
   invI.MakeDiagonal(1.0 / I[0][0], 1.0 / I[0][0], 1.0 / I[0][0]);
   for (int i = 0; i < n; i++) {
      r[i] = VEC3(i*totalLength/n, 0, 0);
      rp[i] = VEC3(0,0,0);
      ohm[i] = VEC3(0,0,0);
      mass[i] = 0.0f;
      if (i != n-1) {
         q[i] = VEC4(0, 1.0/sqrt(2.0), 0.0, 1.0/sqrt(2.0)); // So d3 align with x
      }
   }

   // Compute mass and li
   for (int i = 0; i < n-1; i++) {
      li[i] = (r[i+1] - r[i]).Length();     
      invli[i] = 1.0 / li[i];
      real m = rho*MYPI*R*R*(li[i]);
      mass[i] += 0.5*m;
      mass[i+1] += 0.5*m;
   }

   // Compute lj
   for (int i = 0; i < n-2; i++) {
      lj[i] = 0.5*(li[i]+li[i+1]);
      invlj[i] = 1.0 / lj[i];
   }

}

inline int delta(int i, int j) {
   return (i==j);
}

inline MAT4 QuadMat(const VEC4& qi) {
   /*
   return MAT4(qi[3], -qi[2], qi[1], qi[0],
               qi[2], qi[3], -qi[0], qi[1],
              -qi[1], qi[0], qi[3], qi[2],
              -qi[0], -qi[1], -qi[2], qi[3]); 
              */
   return MAT4(qi[3], qi[2], -qi[1], qi[0],
               -qi[2], qi[3], qi[0], qi[1],
               qi[1], -qi[0], qi[3], qi[2],
               -qi[0], -qi[1], -qi[2], qi[3]);
}

inline MATRIX3 ToRotation(const VEC4& q) {
   /*
   return MATRIX3(1-2*q[1]*q[1]-2*q[2]*q[2], 2*q[0]*q[1]-2*q[3]*q[2], 2*q[0]*q[2]+2*q[3]*q[1],
               2*q[0]*q[1]+2*q[3]*q[2], 1-2*q[0]*q[0]-2*q[2]*q[2], 2*q[1]*q[2]-2*q[3]*q[0],
               2*q[0]*q[2]-2*q[3]*q[1], 2*q[1]*q[2]-2*q[3]*q[0], 1-2*q[0]*q[0]-2*q[1]*q[1]);*/
   return MATRIX3(q[0]*q[0] - q[1]*q[1]-q[2]*q[2]+q[3]*q[3],2*(q[0]*q[1] - q[2]*q[3]), 2*(q[0]*q[2]+q[1]*q[3]),
               2*(q[0]*q[3]+q[2]*q[3]), -q[0]*q[0]+q[1]*q[1]-q[2]*q[2]+q[3]*q[3], 2*(q[1]*q[2]-q[0]*q[3]),
               2*(q[0]*q[2]-q[1]*q[3]), 2*(q[1]*q[2]+q[0]*q[3]), -q[0]*q[0]-q[1]*q[1]+q[2]*q[2]+q[3]*q[3]);
}

void CCorde::UpdateStates(real dt) {
   int numNodes = force.size();
   int numExt = extForce.size();
   for (int i = 0; i < numExt; i++) {
      force[extForce[i].nodeNum] -= extForce[i].force;
   }

   for (int i = 0; i < numNodes; i++) {
      rp[i] += dt*(gravity - force[i]/mass[i]);
      r[i] += dt*rp[i];
     // cout<<i<<" : "<<force[i]<<endl;
      rp[i]*= 0.9999;

   }
   //cout<<endl;
   r[0] = VEC3(0,0,0);
   rp[0] = VEC3(0,0,0);


   static real cc = 6;
   static int sss = 0;
   sss++;
   if (sss < 10000) {
      cc -= dt*2;
   }

   real yc = sin(sss*dt*4)*0.1;
   real zc = cos(sss*dt*4)*0.1;
   //r[numNodes-1] = VEC3(cc,0,0);
   //rp[numNodes-1] = VEC3(0,0,0);
   //ohm[0] = VEC3(0,0,0);
   //q[0] = VEC4(1.0/sqrt(2.0), 0.0, 1.0/sqrt(2.0), 0.0); 
/*
   VEC4 tq(1,0,0,0.1);
   tq.Normalize();
   MAT4 QQ = QuadMat(tq);
   VEC4 tmp(1,2,3,4);
   tmp.Normalize();
   VEC4 test = QQ.TransposeTimes(tmp);
   VEC4 test2 = QQ * tmp;*/
   numExt = extTorque.size();
   for (int i = 0; i < numExt; i++) {
      MAT4 Q = QuadMat(q[extTorque[i].nodeNum]);     
      VEC4 tt = 0.5*Q*VEC4(extTorque[i].torque[0], extTorque[i].torque[1], extTorque[i].torque[2],0);
    //  cout<<tt<<" --- "<<torque[extTorque[i].nodeNum]<<endl;
      torque[extTorque[i].nodeNum] -= tt;
   }

   for (int i = 0; i < numNodes-1; i++) {
      MAT4 Q = QuadMat(q[i]);     
      VEC4 tt = Q.TransposeTimes(torque[i]);
      VEC3 tauBar(tt[0],tt[1],tt[2]);
      tauBar*=-2;

      /*real nt = tauBar.Length();
      if (nt > 50) {tauBar.Normalize(); tauBar *= 50;}*/
      VEC3 eTau(0,0,0);
      /*if (sss > 30000) {
         if (i == numNodes -1) {
            eTau = VEC3(1,0,0);
         }
      }*/
     // cout<<i<<" : "<<tauBar<<endl;
      MATRIX3 RMat = ToRotation(q[i]);
      MATRIX3 Ic =(RMat*(I)).TimesTranspose(RMat);
      MATRIX3 invIc =(RMat*(invI)).TimesTranspose(RMat);

      ohm[i] = invli[i]*invIc*(eTau + tauBar - ohm[i].Cross(li[i]*Ic*ohm[i]))*dt + ohm[i];
      //ohm[i] = invli[i]*invIc*(eTau + tauBar)*dt + ohm[i];
      //ohm[i] = invIc*(eTau + tauBar)*dt + ohm[i];
      //ohm[i] = invli[i]*invI*(eTau + tauBar + ohm[i].Cross(li[i]*I*ohm[i]))*dt + ohm[i];
      //ohm[i] = invI*(eTau + tauBar - ohm[i].Cross(I*ohm[i]))*dt + ohm[i];
      //ohm[i] = invIc*(eTau + tauBar + ohm[i].Cross(Ic*ohm[i]))*dt + ohm[i];
      ohm[i]*= 0.9999;

      q[i] = 0.5*Q*VEC4(ohm[i][0], ohm[i][1], ohm[i][2],0)*dt + q[i];
      q[i].Normalize();      
      
   }
   //cout<<endl;
}

void CCorde::ComputeForceTorque() {
   int numNodes = force.size();
   memset(&force[0], 0, sizeof(VEC3)*numNodes);
   memset(&torque[0], 0, sizeof(VEC4)*(numNodes-1));

   // Compute qp, time derivative of quarternion
   for (int i = 0; i < numNodes-1; i++) {      
      VEC4& qi=q[i];
      VEC3& ohmi=ohm[i];
        
  
      qp[i] = 0.5*QuadMat(qi)*VEC4(ohmi[0], ohmi[1], ohmi[2],0);
   }

   for (int i = 0; i < numNodes-1; i++) {      
      // Force due to potential energy
      VEC4& qi = q[i];
      VEC3 sub = r[i+1]-r[i];     
      real A = (sub).SquaredLength();
      real sqrtA = sqrt(A);
      VEC3 dVsdri = Ks*(invli[i] - 1.0/sqrtA)*(-sub);
      force[i] += dVsdri;
      force[i+1] -= dVsdri;

      // Force due to dissipation energy
      /*
      VEC3 vrel = invli[i]*invli[i]*invli[i]*(sub*(rp[i+1]-rp[i])
      VEC3 dDtdri = -2*gammat*invli[i]*invli[i]*(sub[0] + sub[1] + sub[2])*(rp[i+1]-rp[i]);
      force[i] -= dDtdri;
      force[i+1] += dDtdri;
      */
      VEC3 subv = rp[i+1]-rp[i];
      real il3 = invli[i]*invli[i]*invli[i];
      VEC3 vrel = il3*(sub*sub.Dot(subv));
      VEC3 dDtdrpi(0,0,0);
      for (int k = 0; k < 3; k++) {
         dDtdrpi += vrel[k]*il3*sub[k]*(-sub);
      }
      dDtdrpi*=li[i]*gammat;
      force[i] += dDtdrpi;
      force[i+1] -= dDtdrpi;

      
      // Force due to quaternion alignment
      VEC3 d3(2*(qi[0]*qi[2] + qi[1]*qi[3]),
             2*(qi[1]*qi[2] - qi[0]*qi[3]),
             -qi[0]*qi[0] - qi[1]*qi[1] + qi[2]*qi[2] + qi[3]*qi[3]);

      VEC3 subN = sub;
      subN.Normalize();
      VEC3 dif = subN - d3;
      VEC3 dEpdri(0.0, 0.0, 0.0);
      for (int c = 0; c < 3; c++) {
         real sum = 0;
         for (int a = 0; a < 3; a++) {
            sum += dif[a]*(-delta(a,c)*sqrtA - (sub[a]*(-sub[c]/sqrtA)))/(A);
         }
         dEpdri[c] = sum*li[i]*kappa;
      }
      force[i] += dEpdri;
      force[i+1] -= dEpdri;      

      if (i < numNodes-2) {
         int j = i;
         
         // Torque due to potential energy      
         VEC3 S(0,0,0);
         for (int k = 0; k < 3; k++) {
            for (int b = 0; b < 4; b++) {
               int a = B[k].index[b];
               real tmp = q[j][a]*q[j+1][b] - 
                          q[j][a]*q[j][b] + 
                          q[j+1][a]*q[j+1][b] - 
                          q[j+1][a]*q[j][b];
               S[k] += invlj[j]*B[k].coef[b]*(tmp);
            }
         }
         S -= uk; // subtract with intrinsic bending
         VEC4 dVbdqj(0,0,0,0);
         VEC4 dVbdqjp1(0,0,0,0);

         for (int c = 0; c < 4; c++) {
            for (int k = 0; k < 3; k++) {
               real dSdqjk = 0;
               real dSdqjp1k = 0;
               for (int b = 0; b < 4; b++) {
                  int a = B[k].index[b];
                  dSdqjk += B[k].coef[b]*(delta(a,c)*q[j+1][b] - delta(a,c)*q[j][b] - delta(b,c)*q[j][a] - delta(b,c)*q[j+1][a]);
                  dSdqjp1k += B[k].coef[b]*(q[j][a]*delta(b,c) + delta(a,c)*q[j+1][b] + q[j+1][a]*delta(b,c) - delta(a,c)*q[j][b]);
               }
               dSdqjk*=invlj[j];
               dSdqjp1k*=invlj[j];

               dVbdqj[c] += K[k][k]*S[k]*dSdqjk;
               dVbdqjp1[c] += K[k][k]*S[k]*dSdqjp1k;
            }
            dVbdqj[c]*=lj[j];
            dVbdqjp1[c]*=lj[j];
         }
         torque[j] += dVbdqj;
         torque[j+1] += dVbdqjp1;

         // Torque due kenetic energy      
         VEC4 sumQ = q[j]+q[j+1];
         VEC4 sumQp = qp[j]+qp[j+1];      
         VEC3 T((B[0]*sumQ).Dot(sumQp),
               (B[1]*sumQ).Dot(sumQp), 
               (B[2]*sumQ).Dot(sumQp));
         VEC4 dTdqj(0,0,0,0);
         for (int k = 0; k < 3; k++) {
            for (int b = 0; b < 4; b++) {
               dTdqj[B[k].index[b]] += I[k][k]*T[k]*B[k].coef[b]*sumQp[b];
            }
         }
         dTdqj *= lj[j]*0.25;
         torque[j] -= dTdqj;
         torque[j+1] -= dTdqj;
         
         // Torque due to damping 
         
         VEC3 D((B0[0]*q[j+1]).Dot(qp[j+1]) - (B0[0]*q[j]).Dot(qp[j]),
               (B0[1]*q[j+1]).Dot(qp[j+1]) - (B0[1]*q[j]).Dot(qp[j]),
               (B0[2]*q[j+1]).Dot(qp[j+1]) - (B0[2]*q[j]).Dot(qp[j]));
         VEC4 dDdqpj(0,0,0,0),
              dDdqpp1j(0,0,0,0);
         for (int c = 0; c < 4; c++) {
            for (int k = 0; k < 3; k++) {
               dDdqpj[c] += D[k]*(-B0[k].coef[c]*q[j][B0[k].index[c]]);
               dDdqpp1j[c] += D[k]*B0[k].coef[c]*q[j+1][B0[k].index[c]];
            }
         }
         dDdqpj *= 4*gammar*invlj[j];
         dDdqpp1j *= 4*gammar*invlj[j];
         torque[j] += dDdqpj;
         torque[j+1] += dDdqpp1j;

      }
      // Torque due to quaternion alignment
      
      real tmpA[3][4] = {{qi[2], qi[3], qi[0], qi[1]},
                         {-qi[3], qi[2], qi[1], -qi[0]},  
                         {-qi[0], -qi[1], qi[2], qi[3]}};
      VEC4 dEpdqj(dif[0]*tmpA[0][0]+dif[1]*tmpA[1][0]+dif[2]*tmpA[2][0],
                  dif[0]*tmpA[0][1]+dif[1]*tmpA[1][1]+dif[2]*tmpA[2][1],
                  dif[0]*tmpA[0][2]+dif[1]*tmpA[1][2]+dif[2]*tmpA[2][2],
                  dif[0]*tmpA[0][3]+dif[1]*tmpA[1][3]+dif[2]*tmpA[2][3]);
      dEpdqj *= (-2)*li[i]*kappa;
      torque[i] += dEpdqj;
      
   }
}


//=============

CCorde rod;
using namespace std;

const float fov = 45;

unsigned int g_persp=0,g_ortho=0;
int width, height;
// Viewing parameters
VEC3 viewFrom(5,5,10), 
    viewLookAt(0,0,0), 
    viewUp(0,1,0);
double modelViewMat[16], projectionMat[16];
int viewport[4];

int holdX, holdY;
int mouseX, mouseY;

bool saveViewingParameters= true;

void OnMouseClick(int button, int state, int x, int y) {
   if (button == GLUT_MIDDLE_BUTTON) {
      if (state == GLUT_DOWN) {
         holdMid = true;
         holdX = x;
         holdY = y;
      } else {
         holdMid = false;
      }
   }

   if (button == GLUT_RIGHT_BUTTON) {
      if (state == GLUT_DOWN) {
         holdRight = true;
         holdX = x;
         holdY = y;
      } else {
         holdRight = false;
      }
   }
   if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
      // Pick vertex
      int ind;
      cout<<"Mouse click at "<<x<<" "<<(viewport[3] - y)<<endl;
      int minI = -1;
      real minZ = 1e10;
      int rad = 10;
      int numNodes = rod.force.size();
      for (int i = 0; i < numNodes; i++) {
         double px,py,pz;
         gluProject(rod.r[i][0], rod.r[i][1],rod.r[i][2],
                    modelViewMat, projectionMat, viewport, &px, &py, &pz);
         cout<<px<<" "<<py<<" "<<pz<<endl;
         if (pz > 0) {
            if ((abs(px-x) < rad) && (abs(py-(viewport[3] -y)) < rad)) {
               if (pz < minZ) {
                  minI = i;
                  minZ = pz;
               }
            }
         }
      }
      pickedVertex = minI;
      if (minI != -1) cout<<"Picked vertex "<<minI<<endl;
 

   }

}

void OnMouseMoveActive(int x, int y) {
   mouseX = x;
   mouseY = y;

}
void OnMouseMovePassive(int x, int y) {
   OnMouseMoveActive(x,y);
}

void OnReshape(int w, int h)
{
   width = w;
   height = h;
		// delete the last display lists
	glDeleteLists(g_persp,1);
	glDeleteLists(g_ortho,1);

	// set the drawable region of the window
	glViewport(0,0,w,h);
   glGetIntegerv(GL_VIEWPORT, viewport);

	// generate new display list ID's
	g_persp = glGenLists(1);
	g_ortho = glGenLists(1);

	// generate perspective display list
	glNewList(g_persp,GL_COMPILE);

		// set up the projection matrix 
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		// just use a perspective projection
		gluPerspective(fov,(float)w/h,0.1,100);

		// go back to modelview matrix so we can move the objects about
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

	glEndList();


	// generate orthographic display list
	glNewList(g_ortho,GL_COMPILE);

		// set up the projection matrix 
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		// just use a orthographic projection
		glOrtho(0,w,h,0,-100,100);

		// go back to modelview matrix so we can move the objects about
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

	glEndList();

}

void OnDraw() {

   glLineWidth(3);
   static int oldTime = 0;
   int time = glutGet(GLUT_ELAPSED_TIME);
   real elapsed = (time - oldTime) * 0.001;
   real fps = 1.0 / elapsed;
   oldTime = time;
	
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

	// clear the previous transform
	// go back to modelview matrix so we can move the objects about
	glCallList(g_ortho);	
	//glDisable(GL_LIGHTING);
	glColor3f(1.0f,1.0f, 1.0f);
	char str[200];
   sprintf(str, "%0.3g fps", fps); 
   FontDraw(str, 20, 20);

	// Evaluate the reflective properties of the material
	float colorBlue[] = { 0.0f, 0.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, colorBlue);
	
   glCallList(g_persp);
	
	gluLookAt(viewFrom[0], viewFrom[1], viewFrom[2], viewLookAt[0], viewLookAt[1], viewLookAt[2], viewUp[0], viewUp[1], viewUp[2]);

   glBegin(GL_LINES);
   glColor3f(1,1,0);
   glVertex3f(0,0,0);
   glVertex3f(1,0,0);
   glColor3f(0,1,1);
   glVertex3f(0,0,0);
   glVertex3f(0,1,0);
   glColor3f(1,0,1);
   glVertex3f(0,0,0);
   glVertex3f(0,0,1);
   glEnd();
   /*
   static real a = 1, b = 0.5, c = 0.5;
   static real rho = 1;
   static real M = (4.0/3.0)*MYPI*a*b*c;
 //  static VEC4 q(1, 1.0, 0.0, 1.0);
   static VEC4 q(0, 1.0/sqrt(2.0), 3.0, 1.0/sqrt(2.0)); // So d3 align with x

   static VEC3 ohm(0,0,0);
   static MATRIX3 I(M*(b*b+c*c)*0.2,M*(a*a+c*c)*0.2,M*(a*a+b*b)*0.2), invI(1.0/I[0][0], 1.0/I[1][1], 1.0/I[2][2]); 
   static real dt = 0.0001;
   static VEC3 L(0,0,0);

   MATRIX3 R;
   VEC3 tau(0,1,0);
   static int ccc = 0;
   ccc++;
   if (ccc > 100) tau = VEC3(0,0,0);
   for (int ss = 0; ss < 100; ss++) {
      q.Normalize();

      MAT4 Q = QuadMat(q);     
      
      
      R = ToRotation(q);
      MATRIX3 Ic =(R*I).TimesTranspose(R);
      MATRIX3 invIc =(R*invI).TimesTranspose(R);
      //MATRIX3 Ic = I;
      //MATRIX3 invIc = invI;
      //MATRIX3 res = I - Ic;
      ohm = invIc*(tau + ohm.Cross(Ic*ohm))*dt + ohm;
      //L = L + dt*tau;
      //ohm = invIc * L;
      q = 0.5*Q*VEC4(ohm[0], ohm[1], ohm[2],0)*dt + q;
      q.Normalize();      

   }


   

   real len = 1.5;
   VEC3 pos(0,0,0);
   
   VEC3 d1(q[0]*q[0] - q[1]*q[1]-q[2]*q[2]+q[3]*q[3],
          2*(q[0]*q[3]+q[2]*q[3]),
          2*(q[0]*q[2]-q[1]*q[3]));
   VEC3 d2(2*(q[0]*q[1] - q[2]*q[3]),
          -q[0]*q[0]+q[1]*q[1]-q[2]*q[2]+q[3]*q[3],
          2*(q[1]*q[2]+q[0]*q[3]));
   VEC3 d3(2*(q[0]*q[2]+q[1]*q[3]),
          2*(q[1]*q[2]-q[0]*q[3]),
          -q[0]*q[0]-q[1]*q[1]+q[2]*q[2]+q[3]*q[3]);
   
   glBegin(GL_LINES);
   glColor3f(1,0,0);
   glVertex3dv(pos);
   glVertex3d(pos[0]+len*d1[0], pos[1]+len*d1[1], pos[2]+len*d1[2]);
   glColor3f(0,1,0);
   glVertex3dv(pos);
   glVertex3d(pos[0]+len*d2[0], pos[1]+len*d2[1], pos[2]+len*d2[2]);
   glColor3f(0,0,1);
   glVertex3dv(pos);
   glVertex3d(pos[0]+len*d3[0], pos[1]+len*d3[1], pos[2]+len*d3[2]);
   glEnd();

//   real MMM[16] = {R[0][0], R[0][1], R[0][2], 0, 
//                   R[1][0], R[1][1], R[1][2], 0, 
//                   R[2][0], R[2][1], R[2][2], 0, 
//                   0,0,0,1};
   real MMM[16] = {R[0][0], R[1][0], R[2][0], 0, 
                   R[0][1], R[1][1], R[2][1], 0, 
                   R[0][2], R[1][2], R[2][2], 0, 
                   0,0,0,1};
   glEnable(GL_LIGHTING);
   glPushMatrix();
   glMultMatrixd(MMM);
   glScaled(a, b, c);
   gluSphere(quadric, 1, 20, 20);
   glPopMatrix();
   glDisable(GL_LIGHTING);

   */
   if (saveViewingParameters) {
      glGetDoublev(GL_MODELVIEW_MATRIX, modelViewMat);
      glGetDoublev(GL_PROJECTION_MATRIX, projectionMat);   
      //saveViewingParameters = false;
   }
   int numSubSteps = 100;
   real dt = 0.0001;
   VEC3 rayDir;
   real mouseForceScaleOldGUI = 10;
   if ((pickedVertex != -1) && (holdMid)) {
      VEC3 vz = viewLookAt - viewFrom;
      vz.Normalize();
      VEC3 vx = vz.Cross(viewUp);
      vx.Normalize();
      VEC3 vy = vx.Cross(vz);

      real T = tan(0.5*(fov / 180) * MYPI);
      real ry = -2*(T*(mouseY))/height + T;
      real G = width * T / height;
      real rx = -G + 2*(G*mouseX)/width;
      rayDir = VEC3(rx*vx + ry*vy + vz);
      rayDir.Normalize();



   }

   for (int i = 0; i < numSubSteps; i++) {
      if ((pickedVertex != -1) && (holdMid || holdRight)) {
         if (holdMid) {
            VEC3 pos = rod.r[pickedVertex];
            VEC3 dis = (pos-viewFrom);
            real d = dis.Dot(rayDir);
            VEC3 forceDir = rayDir*d - dis;
            //cout<<"Exert Force "<<forceDir*mouseForceScaleOldGUI<<" on vertex "<<pickedVertex<<endl;
            rod.extForce.push_back(CExternalForce(pickedVertex, forceDir*mouseForceScaleOldGUI, dt));
         }
         if (holdRight) {
            //cout<<"Torque!"<<endl;
            rod.extTorque.push_back(CExternalTorque(pickedVertex, VEC3(0,30,0), dt));
         }
      }

      rod.ComputeForceTorque();
      rod.UpdateStates(dt);
      rod.AdvanceTimeForExternalForceTorque(dt);
   }
   real maxM = -1;
   for (int i = 0; i < rod.force.size() - 1; i++) {
      if (rod.ohm[i].Length() > maxM) maxM = rod.ohm[i].Length();
   }
//   cout<<"MaxM = "<<maxM<<endl;

   glBegin(GL_LINE_STRIP);
   glColor3f(1,1,1);
   for (int i = 0; i < rod.force.size(); i++) {
      glVertex3dv(rod.r[i]);
   }
   glEnd();
   glPointSize(5.0f);
   

   glBegin(GL_POINTS);
   glColor3f(1,0,0);
   for (int i = 0; i < rod.force.size(); i++) {
      //glVertex3dv(rod.r[i]);
   }
   glEnd();
   glBegin(GL_POINTS);
   glColor3f(1,0,0);
   if (pickedVertex >= 0)
   glVertex3dv(rod.r[pickedVertex]);
   glEnd();


   // Render d1, d2, d3
   /*
   glBegin(GL_LINES);
   glColor3f(1,1,1);
   real len = 0.5; 
   for (int i = 0; i < rod.force.size() - 1; i++) {
      VEC3 pos = (rod.r[i]+rod.r[i])*0.5;
      VEC4& q = rod.q[i];
      VEC3 d1(q[0]*q[0] - q[1]*q[1]-q[2]*q[2]+q[3]*q[3],
             2*(q[0]*q[3]+q[2]*q[3]),
             2*(q[0]*q[2]-q[1]*q[3]));
      VEC3 d2(2*(q[0]*q[1] - q[2]*q[3]),
             -q[0]*q[0]+q[1]*q[1]-q[2]*q[2]+q[3]*q[3],
             2*(q[1]*q[2]+q[0]*q[3]));
      VEC3 d3(2*(q[0]*q[2]+q[1]*q[3]),
             2*(q[1]*q[2]-q[0]*q[3]),
             -q[0]*q[0]-q[1]*q[1]+q[2]*q[2]+q[3]*q[3]);
      glColor3f(1,0,0);
      glVertex3dv(pos);
      glVertex3d(pos[0]+len*d1[0], pos[1]+len*d1[1], pos[2]+len*d1[2]);
      glColor3f(0,1,0);
      glVertex3dv(pos);
      glVertex3d(pos[0]+len*d2[0], pos[1]+len*d2[1], pos[2]+len*d2[2]);
      glColor3f(0,0,1);
      glVertex3dv(pos);
      glVertex3d(pos[0]+len*d3[0], pos[1]+len*d3[1], pos[2]+len*d3[2]);
   }
   glEnd();*/

	
	// currently we've been drawing to the back buffer, we need
	// to swap the back buffer with the front one to make the image visible
	glutSwapBuffers();
   
}

//------------------------------------------------------------	OnInit()
//
void OnInit() {
	
	FontInit();

	// enable depth testing
	glEnable(GL_DEPTH_TEST);

   //glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// Create light components
	GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8, 1.0f };
	GLfloat specularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat position[] = { -1.5f, 1.0f, -4.0f, 1.0f };

	// Assign created components to GL_LIGHT0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glShadeModel(GL_SMOOTH);

}

//------------------------------------------------------------	OnExit()
//
void OnExit() {
	// delete the last display lists
	//glDeleteLists(g_persp,1);
	//glDeleteLists(g_ortho,1);
	
	//FontCleanup();
   gluDeleteQuadric(quadric);
}

int main(int argc,char** argv) {
	// initialise glut
	glutInit(&argc,argv);
	

	// request a depth buffer, RGBA display mode, and we want double buffering
	glutInitDisplayMode(GLUT_DEPTH|GLUT_RGBA|GLUT_DOUBLE);

	// set the initial window size
   glutInitWindowPosition(400, 0);
	glutInitWindowSize(640,480);


	// create the window
	glutCreateWindow("A simple FEM example");

  	OnInit();
   quadric = gluNewQuadric();
	// set the function to use to draw our scene
	glutDisplayFunc(OnDraw);

	// set the function to use to draw our scene
	glutIdleFunc(OnDraw);

	// set the function to handle changes in screen size
	glutReshapeFunc(OnReshape);
   glutMouseFunc(OnMouseClick);
   glutMotionFunc(OnMouseMoveActive);
   glutPassiveMotionFunc(OnMouseMoveActive);
   //glutKeyboardFunc(OnKeyboard);
   // set the function to be called when we exit
	atexit(OnExit);
	// run our custom initialisation


	// this function runs a while loop to keep the program running.
	glutMainLoop();
   return 0;
}