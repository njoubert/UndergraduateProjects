#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include "Wm4Matrix3.h"
#include <algorithm>
using namespace std;

//#define ONE_TET
//#define USE_DOUBLE

#define SAFE_DELETE(p) if (p) delete p; p = 0
#define SAFE_ARRAY_DELETE(p) if (p) delete [] p; p = 0

#define ulong unsigned long

#ifdef USE_DOUBLE
   #define real double
   #define VEC3 Wm4::Vector3<double>
   #define MATRIX3 Wm4::Matrix3<double>
#else 
   #define real float
   #define VEC3 Wm4::Vector3<float>
   #define MATRIX3 Wm4::Matrix3<float>
#endif

#define DIMENSION 3
#define uint unsigned long
#define SAME_REAL_EPSILON 1e-5
#define INFINITY 1e20
#define MYPI 3.1415926535897932384626433832795

#define ZERO_TOLERANCE_NUT 1e-9

extern real sumTime;
class CBBox{
public:
	CBBox();
	CBBox& operator = (const CBBox& b);
	CBBox(const VEC3& minsi, const VEC3& maxsi);	

	void MakeCube(real expandFactor = 1.0) {
		VEC3 center = 0.5 * (mins + maxs);
		#if DIMENSION==2
			real width = max(maxs[0]-mins[0], maxs[1]-mins[1])*0.5*expandFactor;
		#else
			real width = max(max(maxs[0]-mins[0], maxs[1]-mins[1]), maxs[2]-mins[2])*0.5*expandFactor;
		#endif

		#if DIMENSION>=2
			mins[0] = center[0] - width;
			mins[1] = center[1] - width;
			maxs[0] = center[0] + width;
			maxs[1] = center[1] + width;
		#endif
		#if DIMENSION==3
			mins[2] = center[2] - width;
			maxs[2] = center[2] + width;
		#endif
	}

	void UnionWith(const VEC3& p) {
		#if DIMENSION>=2
			mins[0] = min(mins[0],(real)p[0]);
			mins[1] = min(mins[1],(real)p[1]);
			maxs[0] = max(maxs[0],(real)p[0]);
			maxs[1] = max(maxs[1],(real)p[1]);
		#endif

		#if DIMENSION==3
			mins[2] = min(mins[2],(real)p[2]);
			maxs[2] = max(maxs[2],(real)p[2]);
		#endif
	}
	bool StrictlyIntersectOrContain(CBBox& b);
	
	bool Contain(const VEC3& p) const {
		#if DIMENSION==2
			return ((mins[0] <= p[0]) && (p[0] <= maxs[0]) &&
					(mins[1] <= p[1]) && (p[1] <= maxs[1]));
		#endif
		#if DIMENSION==3
			return ((mins[0] <= p[0]) && (p[0] <= maxs[0]) &&
					(mins[1] <= p[1]) && (p[1] <= maxs[1]) &&
					(mins[2] <= p[2]) && (p[2] <= maxs[2]));
		#endif
	}

	bool ContainStrictly(const VEC3& p) const{
		#if DIMENSION==2
			return ((mins[0] < p[0]) && (p[0] < maxs[0]) &&
					(mins[1] < p[1]) && (p[1] < maxs[1]) &&
					(mins[2] < p[2]) && (p[2] < maxs[2]));
	    #endif
		#if DIMENSION==3
			return ((mins[0] < p[0]) && (p[0] < maxs[0]) &&
					(mins[1] < p[1]) && (p[1] < maxs[1]));
		#endif
	}

	void UnionWith(CBBox& a);
	bool Inside(CBBox& a);
	bool insideStrictly(CBBox& a);
	bool Intersect(CBBox& a);
	real Volume();
	int MaximumExtent();
	void Dump();

	VEC3 mins, maxs;
};

template <class T1, class T2>
inline bool SameReal(const T1& a, const T2& b) {
    return fabs((real)(a-b)) < SAME_REAL_EPSILON;
}
// Error Reporting Definitions
#define MERROR_IGNORE 0
#define MERROR_CONTINUE 1
#define MERROR_ABORT 2
extern void processError(const char *format, va_list args, const char *message, int disposition);
extern  void Info(const char *format, ...);
extern  void Warning(const char *format, ...);
extern  void Error(const char *format, ...);
extern  void Severe(const char *format, ...);
inline void DbInfo(const char *format, ...) {
   #ifdef _DEBUG
	va_list args;
	va_start(args, format);
	processError(format, args, "DbInfo", MERROR_CONTINUE);
	va_end(args);
   #endif
}


extern  void Init_genrand(ulong seed);
extern  float RandomFloat();
extern  unsigned long RandomUInt();

#if (defined(__linux__) && defined(__i386__)) || defined(_WIN32)
#define FAST_INT 1
#endif
#define _realmagicroundeps	      (.5-1.4e-11)
	//almost .5f = .5f - 1e^(number of exp bit)

// For WIN32, These 3 functions come from http://ldesoras.free.fr/doc/articles/rounding_en.pdf
// For others, the 3 functions come from pbrt, www.pbrt.org
inline int round_int (real x)
{

	#ifdef FAST_INT
	#define _realmagic			real (6755399441055744.0)
		//2^52 * 1.5,  uses limited precision to floor
		x		= x + _realmagic;
		return ((long*)&x)[0];
	#else
		return int (x+_realmagicroundeps);
   #endif
}
inline int floor_int (real x)
{
	#ifdef FAST_INT
		return round_int(x - _realmagicroundeps);
	#else
		return (int)floorf(x);
	#endif
}

inline int ceil_int (real x)
{
	#ifdef FAST_INT
		return round_int(x + _realmagicroundeps);
	#else
		return (int)ceilf(x);
	#endif
}

inline void UniformSampleDisk(real u1, real u2,
		real *x, real *y) {
	real r = sqrtf(u1);
	real theta = 2.0f * MYPI * u2;
	*x = r * cosf(theta);
	*y = r * sinf(theta);
}

class CMat4x4{
public:

	inline real& operator [] (const int a) {
		return mat[a];
	}	
	real mat[16];
};

class CMat3x3{
public:

	inline real& operator [] (const int a) {
		return mat[a];
	}	
	real mat[9];
};

inline void Inverse3x3(real* inMat, real* outMat) {
    // Invert a 3x3 using cofactors.  This is faster than using a generic
    // Gaussian elimination because of the loop overhead of such a method.

    outMat[0] = inMat[4]*inMat[8] - inMat[5]*inMat[7];
    outMat[1] = inMat[2]*inMat[7] - inMat[1]*inMat[8];
    outMat[2] = inMat[1]*inMat[5] - inMat[2]*inMat[4];
    outMat[3] = inMat[5]*inMat[6] - inMat[3]*inMat[8];
    outMat[4] = inMat[0]*inMat[8] - inMat[2]*inMat[6];
    outMat[5] = inMat[2]*inMat[3] - inMat[0]*inMat[5];
    outMat[6] = inMat[3]*inMat[7] - inMat[4]*inMat[6];
    outMat[7] = inMat[1]*inMat[6] - inMat[0]*inMat[7];
    outMat[8] = inMat[0]*inMat[4] - inMat[1]*inMat[3];

    real fDet = inMat[0]*outMat[0] + inMat[1]*outMat[3]+
        inMat[2]*outMat[6];

    if (fabs(fDet) <= ZERO_TOLERANCE_NUT)
    {
		outMat[0] = 
		outMat[1] = 
		outMat[2] = 
		outMat[3] = 
		outMat[4] = 
		outMat[5] = 
		outMat[6] = 
		outMat[7] = 0.0;
        return ;
    }

	real invDet = 1.0/fDet;
	for (int i = 0; i < 9; i++)
    outMat[i]*=invDet;
}

inline void Inverse4x4(real* inMat, real* outMat) {
    real fA0 = inMat[ 0]*inMat[ 5] - inMat[ 1]*inMat[ 4];
    real fA1 = inMat[ 0]*inMat[ 6] - inMat[ 2]*inMat[ 4];
    real fA2 = inMat[ 0]*inMat[ 7] - inMat[ 3]*inMat[ 4];
    real fA3 = inMat[ 1]*inMat[ 6] - inMat[ 2]*inMat[ 5];
    real fA4 = inMat[ 1]*inMat[ 7] - inMat[ 3]*inMat[ 5];
    real fA5 = inMat[ 2]*inMat[ 7] - inMat[ 3]*inMat[ 6];
    real fB0 = inMat[ 8]*inMat[13] - inMat[ 9]*inMat[12];
    real fB1 = inMat[ 8]*inMat[14] - inMat[10]*inMat[12];
    real fB2 = inMat[ 8]*inMat[15] - inMat[11]*inMat[12];
    real fB3 = inMat[ 9]*inMat[14] - inMat[10]*inMat[13];
    real fB4 = inMat[ 9]*inMat[15] - inMat[11]*inMat[13];
    real fB5 = inMat[10]*inMat[15] - inMat[11]*inMat[14];

    real fDet = fA0*fB5-fA1*fB4+fA2*fB3+fA3*fB2-fA4*fB1+fA5*fB0;
    if (fabs(fDet)< 1e-8) {
		memset(outMat, 0, sizeof(real)*16);
	}
    real fInvDet = ((real)1.0)/fDet;

    outMat[0] = (+ inMat[ 5]*fB5 - inMat[ 6]*fB4 + inMat[ 7]*fB3)*fInvDet;
    outMat[4] = (- inMat[ 4]*fB5 + inMat[ 6]*fB2 - inMat[ 7]*fB1)*fInvDet;
    outMat[8] = (+ inMat[ 4]*fB4 - inMat[ 5]*fB2 + inMat[ 7]*fB0)*fInvDet;
    outMat[12] = (- inMat[ 4]*fB3 + inMat[ 5]*fB1 - inMat[ 6]*fB0)*fInvDet;
    outMat[1] = (- inMat[ 1]*fB5 + inMat[ 2]*fB4 - inMat[ 3]*fB3)*fInvDet;
    outMat[5] = (+ inMat[ 0]*fB5 - inMat[ 2]*fB2 + inMat[ 3]*fB1)*fInvDet;
    outMat[9] = (- inMat[ 0]*fB4 + inMat[ 1]*fB2 - inMat[ 3]*fB0)*fInvDet;
    outMat[13] = (+ inMat[ 0]*fB3 - inMat[ 1]*fB1 + inMat[ 2]*fB0)*fInvDet;
    outMat[2] = (+ inMat[13]*fA5 - inMat[14]*fA4 + inMat[15]*fA3)*fInvDet;
    outMat[6] = (- inMat[12]*fA5 + inMat[14]*fA2 - inMat[15]*fA1)*fInvDet;
    outMat[10] = (+ inMat[12]*fA4 - inMat[13]*fA2 + inMat[15]*fA0)*fInvDet;
    outMat[14] = (- inMat[12]*fA3 + inMat[13]*fA1 - inMat[14]*fA0)*fInvDet;
    outMat[3] = (- inMat[ 9]*fA5 + inMat[10]*fA4 - inMat[11]*fA3)*fInvDet;
    outMat[7] = (+ inMat[ 8]*fA5 - inMat[10]*fA2 + inMat[11]*fA1)*fInvDet;
    outMat[11] = (- inMat[ 8]*fA4 + inMat[ 9]*fA2 - inMat[11]*fA0)*fInvDet;
    outMat[15] = (+ inMat[ 8]*fA3 - inMat[ 9]*fA1 + inMat[10]*fA0)*fInvDet;
}

inline void Inverse3x3(CMat3x3& inMat, CMat3x3& outMat) {
   Inverse3x3(inMat.mat, outMat.mat);
}

inline void Inverse4x4(CMat4x4& inMat, CMat4x4& outMat) {
   Inverse4x4(inMat.mat, outMat.mat);
}

inline void buildBaryMat(real* v0, real* v1, real* v2, real* v3, real* bary_Tet) {
	static real tmpB_Tet[16];
	tmpB_Tet[0] = v0[0];
	tmpB_Tet[1] = v1[0];
	tmpB_Tet[2] = v2[0];
	tmpB_Tet[3] = v3[0];

	tmpB_Tet[4] = v0[1];
	tmpB_Tet[5] = v1[1];
	tmpB_Tet[6] = v2[1];
	tmpB_Tet[7] = v3[1];

	tmpB_Tet[8] = v0[2];
	tmpB_Tet[9] = v1[2];
	tmpB_Tet[10] = v2[2];
	tmpB_Tet[11] = v3[2];

	tmpB_Tet[12] = 1;
	tmpB_Tet[13] = 1;
	tmpB_Tet[14] = 1;
	tmpB_Tet[15] = 1;

	Inverse4x4(tmpB_Tet, bary_Tet);
}

#define ONESIXTH 0.166666666666666666666666666666666666666666666666666666667
// Find tets volume
inline real TetVol(real* v0, real* v1, real* v2, real* v3) {
	
	real xot = v0[0] - v1[0];
    real yot = v0[1] - v1[1];
    real zot = v0[2] - v1[2];
    real xdt = v2[0] - v1[0];
    real ydt = v2[1] - v1[1];
    real zdt = v2[2] - v1[2];
    real xft = v3[0] - v1[0];
    real yft = v3[1] - v1[1];
    real zft = v3[2] - v1[2];
    return fabs(ONESIXTH * (xot * (ydt * zft - yft * zdt) +
                              yot * (zdt * xft - zft * xdt) +
                              zot * (xdt * yft - xft * ydt)));    
}

template <class T>
class CompareUintTPair{
	public:
	bool operator()(const pair<uint, T>& a, const pair<uint, T>& b) {
		return a.first < b.first;
	}
};
inline void House(real* x, int len, real* u) {
   real xlen = 0.0;
   for (int i = 0; i < len; i++) {
      u[i] = x[i];
      xlen += x[i]*x[i];
   }
   if (x[0] > 0) u[0] += sqrt(xlen); else
                 u[0] -= sqrt(xlen);
   xlen += (u[0]*u[0] - x[0]*x[0]);
   real inv = 1.0 / sqrt(xlen);
   for (int i = 0; i < len; i++) {
      u[i] *= inv;
   }
}
inline void QR(real* mat) {
   real tmp[4], u[4], prod[4];
   for (int i = 0; i < 3; i++) {
      for (int j = i; j < 4; j++) {
         tmp[j-i] = mat[4*j + i];
      }
      House(tmp, 4-i, u);
      for (int k = i; k < 4; k++) {
         real sum = 0.0;
         for (int j = i; j < 4; j++) {
            sum += mat[4*j + k] * u[j-i];    
         }
         prod[k-i] = sum;
      }

      for (int j = i; j < 4; j++) {
         for (int k = i; k < 4; k++) {
            mat[4*j + k] -= 2*u[j-i]*prod[k-i];
         }
      }
   }
}

// Will replace mat with R and output Q
inline void QR3(real* mat, real* Qt) {
   #define DIM 3
   real tmp[DIM], u[DIM], prod[DIM], fullU[DIM];
   Qt[1]=Qt[2]=Qt[3]=Qt[5]=Qt[6]=Qt[7]=0.0;
   Qt[0]=Qt[4]=Qt[8]=1.0;

   for (int i = 0; i < DIM-1; i++) {
      for (int j = i; j < DIM; j++) {
         tmp[j-i] = mat[DIM*j + i];
      }
      House(tmp, DIM-i, u);
      fullU[0]=fullU[1]=fullU[2] = 0.0;
      for (int j = i; j < DIM; j++) {
         fullU[j] = u[j-i];
      }
      for (int k = 0; k < DIM; k++) {
         real sum = 0.0;
         for (int j = 0; j < DIM; j++) {
            sum += Qt[DIM*j + k] * fullU[j];    
         }
         prod[k] = sum;
      }
      for (int j = 0; j < DIM; j++) {
         for (int k = 0; k < DIM; k++) {
            Qt[DIM*j + k] -= 2*fullU[j]*prod[k];
         }
      }
   

      for (int k = i; k < DIM; k++) {
         real sum = 0.0;
         for (int j = i; j < DIM; j++) {
            sum += mat[DIM*j + k] * u[j-i];    
         }
         prod[k-i] = sum;
      }

      for (int j = i; j < DIM; j++) {
         for (int k = i; k < DIM; k++) {
            mat[DIM*j + k] -= 2*u[j-i]*prod[k-i];
         }
      }
   }
}

inline void Mul3(CMat3x3& mat, VEC3& a, VEC3& o) {
   o[0] = mat[0]*a[0] + mat[1]*a[1] + mat[2]*a[2];
   o[1] = mat[3]*a[0] + mat[4]*a[1] + mat[5]*a[2];
   o[2] = mat[6]*a[0] + mat[7]*a[1] + mat[8]*a[2];
}

inline void Mul3t(CMat3x3& mat, VEC3& a, VEC3& o) {
   o[0] = mat[0]*a[0] + mat[3]*a[1] + mat[6]*a[2];
   o[1] = mat[1]*a[0] + mat[4]*a[1] + mat[7]*a[2];
   o[2] = mat[2]*a[0] + mat[5]*a[1] + mat[8]*a[2];
}


inline bool FindRayTriangleIntersection(VEC3& v0, VEC3& v1, VEC3& v2, VEC3& origin, VEC3& dir, real& intS, real& intT, real& intDis, VEC3& intPos )
{
   /**
   **
   ** Modified from the ray-triangle intersection code from www.geometrictools.com
   **
   **
   **/
   static const real epsilon = 1e-50;
   // compute the offset origin, edges, and normal
   VEC3 kDiff(origin, v0);
   VEC3 kEdge1(v1, v0);
   VEC3 kEdge2(v2, v0);
   VEC3 kNormal = kEdge1.Cross(kEdge2);
   VEC3 normalizedNormal = kNormal;
   normalizedNormal.Normalize();

   // Solve Q + t*D = b1*E1 + b2*E2 (Q = kDiff, D = ray direction,
   // E1 = kEdge1, E2 = kEdge2, N = Cross(E1,E2)) by
   //   |Dot(D,N)|*b1 = sign(Dot(D,N))*Dot(D,Cross(Q,E2))
   //   |Dot(D,N)|*b2 = sign(Dot(D,N))*Dot(D,Cross(E1,Q))
   //   |Dot(D,N)|*t = -sign(Dot(D,N))*Dot(Q,N)
   real fDdN = dir.Dot(kNormal);
   real fSign;
   if ( fDdN > epsilon )
   {
      fSign = (real)1.0;
   }
   else if ( fDdN < -epsilon )
   {
      fSign = (real)-1.0;
      fDdN = -fDdN;
   }
   else
   {
      // Ray and triangle are parallel, call it a "no intersection"
      // even if the ray does intersect.
      return false;
   }

   real fDdQxE2 = fSign*dir.Dot(kDiff.Cross(kEdge2));
   if ( fDdQxE2 >= (real)0.0 )
   {
      real fDdE1xQ = fSign*dir.Dot(kEdge1.Cross(kDiff));
      if ( fDdE1xQ >= (real)0.0 )
      {
         if ( fDdQxE2 + fDdE1xQ <= fDdN )
         {
            // ray intersects triangle
            real fInv = ((real)1.0)/fDdN;
            // line intersects triangle, check if ray does
            real fQdN = -fSign*kDiff.Dot(kNormal);
            real m_fRayT = fQdN*fInv;

            if ( m_fRayT >= (real)0.0 )
            {
               intDis = m_fRayT;
               intS = fDdQxE2*fInv; //S
               intT = fDdE1xQ*fInv; //T
               intPos = v0 + intS * kEdge1 + intT * kEdge2;

               return true;
            }
            // else: t < 0, no intersection
         }
         // else: b1+b2 > 1, no intersection
      }
      // else: b2 < 0, no intersection
   }
   // else: b1 < 0, no intersection

   return false;
}

inline void GenerateOrthonormalBasis (VEC3& rkU, VEC3& rkV, VEC3& rkW)
{
	double fInvLength;

    if (fabs(rkW[0]) >=   fabs(rkW[1]) )
    {
        // W.x or W.z is the largest magnitude component, swap them
        fInvLength = 1.0 / sqrt(rkW[0]*rkW[0] + rkW[2]*rkW[2]);
        rkU[0] = -rkW[2]*fInvLength;
        rkU[1] = 0.0;
        rkU[2] = +rkW[0]*fInvLength;
        rkV[0] = rkW[1]*rkU[2];
        rkV[1] = rkW[2]*rkU[0] - rkW[0]*rkU[2];
        rkV[2] = -rkW[1]*rkU[0];
    }
    else
    {
        // W.y or W.z is the largest magnitude component, swap them
        fInvLength = 1.0 /sqrt(rkW[1]*rkW[1] + rkW[2]*rkW[2]);
        rkU[0] = 0.0;
        rkU[1] = +rkW[2]*fInvLength;
        rkU[2] = -rkW[1]*fInvLength;
        rkV[0] = rkW[1]*rkU[2] - rkW[2]*rkU[1];
        rkV[1] = -rkW[0]*rkU[2];
        rkV[2] = rkW[0]*rkU[1];
    }
}


class CTet{
public:
   CTet() {}
   CTet(const int v0, const int v1, const int v2, const int v3) {
      id[0] = v0;
      id[1] = v1;
      id[2] = v2;
      id[3] = v3;
   }
   inline int& operator [] (const int a) {
      return id[a];
   }
   inline const int& operator [] (const int a) const{
      return id[a];
   }

   int id[4];
};

class CTri{
public:
   CTri() {}
   CTri(const int v0, const int v1, const int v2) {
      id[0] = v0;
      id[1] = v1;
      id[2] = v2;
   }
   inline int& operator [] (const int a) {
      return id[a];
   }

   int id[3];
};


#if _DEBUG
#define LOGGILE_TYPE ofstream

#else

#define LOGGILE_TYPE ofstream
#endif
#endif