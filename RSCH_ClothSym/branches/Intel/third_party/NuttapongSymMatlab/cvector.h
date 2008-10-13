#ifndef __CVECTOR_H__
#define __CVECTOR_H__

#include <cmath>
#define real double

using namespace std;

class CVector3{
	static const int nc = 3;
	
	public:
	CVector3(const real* dat);
	CVector3(const real* data1, const real* data2);
	CVector3(const real* data1, const CVector3& vec2);
	CVector3(const real x, const real y, const real z);
	CVector3(const CVector3& a, const real& c);
	CVector3(char o, const CVector3& a, const CVector3& b);
	CVector3();
	CVector3(const CVector3& v);
	 real& operator () (int i);
	 real& operator [] (int i);
	 const real& operator () (int i) const;
	 const real& operator [] (int i) const;
	 CVector3& operator += (const CVector3& v);
	 CVector3& operator -= (const CVector3& v);
	 CVector3& operator *= (const real& d);
	 CVector3& operator /= (const real& d);
	 CVector3& operator = (const CVector3& v);
	 void normalize();

	 real norm2() const;
	 real norm2sq() const;
	 void set_zero();
	 bool is_zero();
	real data[nc];
};
extern CVector3 Cross(const CVector3& v1, const CVector3& v2);
extern real Dot(const CVector3& v1, const CVector3& v2);
extern real Dot(const CVector3& v1, const double* data);

extern  CVector3 operator + (const CVector3& a, const CVector3& b);
extern  CVector3 operator - (const CVector3& a, const CVector3& b);

extern  CVector3 operator * (const CVector3& a, const real c);
extern  CVector3 operator * (const real c, const CVector3& a);
extern  CVector3 operator / (const CVector3& a, const real c);
#endif
