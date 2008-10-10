#include "cvector.h"

using namespace std;

CVector3::CVector3(const real* data1, const CVector3& vec2) {
	data[0] = data1[0] - vec2.data[0];
	data[1] = data1[1] - vec2.data[1];
	data[2] = data1[2] - vec2.data[2];
}

CVector3::CVector3(const real* data1, const real* data2) {
	data[0] = data1[0] - data2[0];
	data[1] = data1[1] - data2[1];
	data[2] = data1[2] - data2[2];
}


CVector3::CVector3(const real* dat) {
	data[0] = dat[0];
	data[1] = dat[1];
	data[2] = dat[2];
}


CVector3::CVector3(const real x, const real y, const real z) {
	data[0] = x;
	data[1] = y;
	data[2] = z;
}
CVector3::CVector3(const CVector3& a, const real& c) {
	for (int i = 0; i < nc; i++) {
		data[i] = a.data[i]*c;
	}
}
CVector3::CVector3(char o, const CVector3& a, const CVector3& b) {
	if (o == '+') {
		for (int i = 0; i < nc; i++) {
			data[i] = a.data[i] + b.data[i];
		}
	} else {
		for (int i = 0; i < nc; i++) {
			data[i] = a.data[i] - b.data[i];
		}
	}
}
CVector3::CVector3() {
	for (int i = 0; i < nc; i++) {
		data[i] = 0;
	}
}
CVector3::CVector3(const CVector3& v) {
	for (int i = 0; i < nc; i++) {
		data[i] = v.data[i];
	}
}
real& CVector3::operator () (int i) {
	return data[i];
}
real& CVector3::operator [] (int i) {
	return data[i];
}
const real& CVector3::operator () (int i) const {
	return data[i];
}
const real& CVector3::operator [] (int i) const {
	return data[i];
}
CVector3& CVector3::operator += (const CVector3& v) {
	for (int i = 0; i < nc; i++) {
		data[i] += v.data[i];
	}
	return *this;
}
CVector3& CVector3::operator -= (const CVector3& v) {
	for (int i = 0; i < nc; i++) {
		data[i] -= v.data[i];
	}
	return *this;
}
CVector3& CVector3::operator *= (const real& d) {
	for (int i = 0; i < nc; i++) {
		data[i] *= d;
	}
	return *this;
}
CVector3& CVector3::operator /= (const real& d) {
	real inv = 1.0 / d;
	for (int i = 0; i < nc; i++) {
		data[i] *= inv;
	}
	return *this;
}
void CVector3::normalize() {
	real inv = norm2();
	if (fabs(inv) < 1e-50) return;
	inv = 1.0/inv;
	for (int i = 0; i < nc; i++) {
		data[i] *= inv;
	}
}

CVector3& CVector3::operator = (const CVector3& v) {
	for (int i = 0; i < nc; i++) {
		data[i] = v.data[i];
	}
	return *this;
}
real CVector3::norm2() const {
	real sum = 0;
	for (int i = 0; i < nc; i++) {
		sum += data[i]*data[i];
	}
	return sqrt(sum);
}
real CVector3::norm2sq() const {
	real sum = 0;
	for (int i = 0; i < nc; i++) {
		sum += data[i]*data[i];
	}
	return sum;
}
void CVector3::set_zero() {
	for (int i = 0; i < nc; i++) {
		data[i] = 0;
	}
}
bool CVector3::is_zero() {
	for (int i = 0; i < nc; i++) {
		if (fabs(data[i]) > 1e-20) return false;
	}
	return true;
}
CVector3 operator + (const CVector3& a, const CVector3& b) {
	return CVector3('+', a, b);
}
CVector3 operator - (const CVector3& a, const CVector3& b) {
	return CVector3('-', a, b);
}

CVector3 operator * (const CVector3& a, const real c) {
	return CVector3(a, c);
}
CVector3 operator * (const real c, const CVector3& a) {
	return CVector3(a, c);
}
CVector3 operator / (const CVector3& a, const real c) {
	return CVector3(a, 1.0/c);
}

CVector3 Cross(const CVector3& v1, const CVector3& v2) {
	return CVector3(  v1.data[1]*v2.data[2]-v2.data[1]*v1.data[2],
		            -(v1.data[0]*v2.data[2]-v2.data[0]*v1.data[2]),
					  v1.data[0]*v2.data[1]-v2.data[0]*v1.data[1]);
}
real Dot(const CVector3& v1, const CVector3& v2) {
	return v1.data[0]*v2.data[0] + 
		   v1.data[1]*v2.data[1] + 
		   v1.data[2]*v2.data[2];
}

real Dot(const CVector3& v1, const double* data) {
	return v1.data[0]*data[0] +
		   v1.data[1]*data[1] +
		   v1.data[2]*data[2];
}

