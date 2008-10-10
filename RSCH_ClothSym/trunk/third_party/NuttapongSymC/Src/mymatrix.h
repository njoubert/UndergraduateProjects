#ifndef _MYMATRIX_H
#define _MYMATRIX_H
#include "Wm4Matrix4.h"
#include "globals.h"
/*
using namespace Wm4;

template <class Real>
class Matrix43;

template <class Real>
class Matrix32;

// Matrix23
template <class Real>
class Matrix23
{
public:
    // If bZero is true, create the zero matrix.  Otherwise, create the
    // identity matrix.
   Matrix23 () {
      memset(m_afEntry, 0, sizeof(Real)*6);
   }

   // copy constructor
   Matrix23 (const Matrix23& rkM) {
      m_afEntry[0] = rkM.m_afEntry[0];
      m_afEntry[1] = rkM.m_afEntry[1];
      m_afEntry[2] = rkM.m_afEntry[2];
      m_afEntry[3] = rkM.m_afEntry[3];
      m_afEntry[4] = rkM.m_afEntry[4];
      m_afEntry[5] = rkM.m_afEntry[5];      
   }

   // input Mrc is in row r, column c.
   Matrix23 (Real fM00, Real fM01, Real fM02, 
             Real fM10, Real fM11, Real fM12) {
      m_afEntry[0] = fM00;
      m_afEntry[1] = fM01;
      m_afEntry[2] = fM02;
      m_afEntry[3] = fM10;
      m_afEntry[4] = fM11;
      m_afEntry[5] = fM12;
   }

   Matrix23 (const Real* val) {
      memcpy(m_afEntry, val, sizeof(Real)*6);
   }
   // create various matrices
   inline Matrix23& MakeZero () {
      memset(m_afEntry, 0, sizeof(Real)*6);
   }

   // member access
   inline operator const Real* () const {
      return m_afEntry;
   }
   inline operator Real* () {
      return m_afEntry;
   }
   inline const Real* operator[] (int iRow) const {
      return &m_afEntry[3*iRow];
   }
   inline Real* operator[] (int iRow) {
      return &m_afEntry[3*iRow];
   }
   inline Real operator() (int iRow, int iCol) const {
      return m_afEntry[iRow*3 + iCol];
   }
   inline Real& operator() (int iRow, int iCol) {
      return m_afEntry[iRow*3 + iCol];
   }

   // assignment
   inline Matrix23& operator= (const Matrix23& rkM) {
      m_afEntry[0] = rkM.m_afEntry[0];
      m_afEntry[1] = rkM.m_afEntry[1];
      m_afEntry[2] = rkM.m_afEntry[2];
      m_afEntry[3] = rkM.m_afEntry[3];
      m_afEntry[4] = rkM.m_afEntry[4];
      m_afEntry[5] = rkM.m_afEntry[5];
      return *this;
   }

   // arithmetic operations
   inline Matrix23 operator+ (const Matrix23& rkM) const {
      return Matrix23<Real>(m_afEntry[0] + rkM.m_afEntry[0],
                            m_afEntry[1] + rkM.m_afEntry[1],
                            m_afEntry[2] + rkM.m_afEntry[2],
                            m_afEntry[3] + rkM.m_afEntry[3],
                            m_afEntry[4] + rkM.m_afEntry[4],
                            m_afEntry[5] + rkM.m_afEntry[5]);
   }
   inline Matrix23 operator- (const Matrix23& rkM) const {
      return Matrix23<Real>(m_afEntry[0] - rkM.m_afEntry[0],
                            m_afEntry[1] - rkM.m_afEntry[1],
                            m_afEntry[2] - rkM.m_afEntry[2],
                            m_afEntry[3] - rkM.m_afEntry[3],
                            m_afEntry[4] - rkM.m_afEntry[4],
                            m_afEntry[5] - rkM.m_afEntry[5]);
   }

   inline Matrix23 operator* (const Matrix3<Real>& rkM) const {
      const Real* m = rkM;
      return Matrix23<Real>(  
        m_afEntry[0]*m[0] +
        m_afEntry[1]*m[3] +
        m_afEntry[2]*m[6],

        m_afEntry[0]*m[1] +
        m_afEntry[1]*m[4] +
        m_afEntry[2]*m[7],

        m_afEntry[0]*m[2] +
        m_afEntry[1]*m[5] +
        m_afEntry[2]*m[8],

        m_afEntry[3]*m[0] +
        m_afEntry[4]*m[3] +
        m_afEntry[5]*m[6],

        m_afEntry[3]*m[1] +
        m_afEntry[4]*m[4] +
        m_afEntry[5]*m[7],

        m_afEntry[3]*m[2] +
        m_afEntry[4]*m[5] +
        m_afEntry[5]*m[8]);

   }

   inline Matrix2<Real> operator* (const Matrix32<Real>& rkM) const;

   inline Matrix23 operator* (Real fScalar) const {
      return Matrix23<Real>(m_afEntry[0] * fScalar, 
                            m_afEntry[1] * fScalar, 
                            m_afEntry[2] * fScalar, 
                            m_afEntry[3] * fScalar, 
                            m_afEntry[4] * fScalar, 
                            m_afEntry[5] * fScalar);
   }
   inline Matrix23 operator/ (Real fScalar) const {
      Real ifScalar = 1.0 / fScalar;
      return Matrix23<Real>(m_afEntry[0] * ifScalar, 
                            m_afEntry[1] * ifScalar, 
                            m_afEntry[2] * ifScalar, 
                            m_afEntry[3] * ifScalar, 
                            m_afEntry[4] * ifScalar, 
                            m_afEntry[5] * ifScalar);
   }
   inline Matrix23 operator- () const {
      return Matrix23<Real>(-m_afEntry[0],
                            -m_afEntry[1],
                            -m_afEntry[2],
                            -m_afEntry[3],
                            -m_afEntry[4],
                            -m_afEntry[5]);
   }

   // arithmetic updates
   inline Matrix23& operator+= (const Matrix23& rkM) {
      m_afEntry[0] += rkM.m_afEntry[0];
      m_afEntry[1] += rkM.m_afEntry[1];
      m_afEntry[2] += rkM.m_afEntry[2];
      m_afEntry[3] += rkM.m_afEntry[3];
      m_afEntry[4] += rkM.m_afEntry[4];
      m_afEntry[5] += rkM.m_afEntry[5];
      return *this;
   }
   inline Matrix23& operator-= (const Matrix23& rkM) {
      m_afEntry[0] -= rkM.m_afEntry[0];
      m_afEntry[1] -= rkM.m_afEntry[1];
      m_afEntry[2] -= rkM.m_afEntry[2];
      m_afEntry[3] -= rkM.m_afEntry[3];
      m_afEntry[4] -= rkM.m_afEntry[4];
      m_afEntry[5] -= rkM.m_afEntry[5];
      return *this;
   }
   inline Matrix23& operator*= (Real fScalar) {
      m_afEntry[0] *= fScalar;
      m_afEntry[1] *= fScalar;
      m_afEntry[2] *= fScalar;
      m_afEntry[3] *= fScalar;
      m_afEntry[4] *= fScalar;
      m_afEntry[5] *= fScalar;
      return *this;
   }
   inline Matrix23& operator/= (Real fScalar) {
      Real ifScalar = 1.0 / fScalar;
      m_afEntry[0] *= ifScalar;
      m_afEntry[1] *= ifScalar;
      m_afEntry[2] *= ifScalar;
      m_afEntry[3] *= ifScalar;
      m_afEntry[4] *= ifScalar;
      m_afEntry[5] *= ifScalar;
      return *this;
   }

    inline bool IsZero() {
       return (m_afEntry[0] == 0.0) &&
              (m_afEntry[1] == 0.0) &&
              (m_afEntry[2] == 0.0) &&
              (m_afEntry[3] == 0.0) &&
              (m_afEntry[4] == 0.0) &&
              (m_afEntry[5] == 0.0);
    }

   // matrix times vector
   inline Vector2<Real> operator* (const Vector3<Real>& rkV) const {  // M * v
      return Vector2<Real> (m_afEntry[0] * rkV[0] + m_afEntry[1] * rkV[1] + m_afEntry[2] * rkV[2], 
                            m_afEntry[3] * rkV[0] + m_afEntry[4] * rkV[1] + m_afEntry[5] * rkV[2]);
   }

   // other operations
   inline Vector3<Real> TransposeTimes (const Vector2<Real>& rkV) const {  // this^T * v
      return Vector3<Real> (m_afEntry[0] * rkV[0] + m_afEntry[3] * rkV[1],
                            m_afEntry[1] * rkV[0] + m_afEntry[4] * rkV[1],
                            m_afEntry[2] * rkV[0] + m_afEntry[5] * rkV[1]);
   }
   // other operations
   inline Matrix32<Real> TransposeTimes (const Matrix2<Real>& rkM) const {  // this^T * v
      const Real* m = rkM;
      return Matrix32<Real> (m_afEntry[0] * m[0] + m_afEntry[3] * m[2], m_afEntry[0] * m[1] + m_afEntry[3] * m[3],
                             m_afEntry[1] * m[0] + m_afEntry[4] * m[2], m_afEntry[1] * m[1] + m_afEntry[4] * m[3], 
                             m_afEntry[2] * m[0] + m_afEntry[5] * m[2], m_afEntry[2] * m[1] + m_afEntry[5] * m[3]);
   }

   inline Matrix3<Real> TransposeTimes (const Matrix23<Real>& rkM) const {  // this^T * v
      const Real* m = rkM;
      return Matrix3<Real> (m_afEntry[0] * m[0] + m_afEntry[3] * m[3], m_afEntry[0] * m[1] + m_afEntry[3] * m[4], m_afEntry[0] * m[2] + m_afEntry[3] * m[5],
                            m_afEntry[1] * m[0] + m_afEntry[4] * m[3], m_afEntry[1] * m[1] + m_afEntry[4] * m[4], m_afEntry[1] * m[2] + m_afEntry[4] * m[5],
                            m_afEntry[2] * m[0] + m_afEntry[5] * m[3], m_afEntry[2] * m[1] + m_afEntry[5] * m[4], m_afEntry[2] * m[2] + m_afEntry[5] * m[5]);
   }

   inline Matrix32<Real> Transpose() const;

private:

   Real m_afEntry[6];
};

// c * M
template <class Real>
inline Matrix23<Real> operator* (Real fScalar, const Matrix23<Real>& rkM) {
   return rkM * fScalar;
}

// Matrix32
template <class Real>
class Matrix32
{
public:
    // If bZero is true, create the zero matrix.  Otherwise, create the
    // identity matrix.
   Matrix32 () {
      memset(m_afEntry, 0, sizeof(Real)*6);
   }

   // copy constructor
   Matrix32 (const Matrix32& rkM) {
      m_afEntry[0] = rkM.m_afEntry[0];
      m_afEntry[1] = rkM.m_afEntry[1];
      m_afEntry[2] = rkM.m_afEntry[2];
      m_afEntry[3] = rkM.m_afEntry[3];
      m_afEntry[4] = rkM.m_afEntry[4];
      m_afEntry[5] = rkM.m_afEntry[5];      
   }

   // input Mrc is in row r, column c.
   Matrix32 (Real fM00, Real fM01, 
             Real fM10, Real fM11,
             Real fM20, Real fM21) {
      m_afEntry[0] = fM00;
      m_afEntry[1] = fM01;
      m_afEntry[2] = fM10;
      m_afEntry[3] = fM11;
      m_afEntry[4] = fM20;
      m_afEntry[5] = fM21;
   }

   Matrix32 (const Real* val) {
      memcpy(m_afEntry, val, sizeof(Real)*6);
   }
   // create various matrices
   inline Matrix32& MakeZero () {
      memset(m_afEntry, 0, sizeof(Real)*6);
   }

   // member access
   inline operator const Real* () const {
      return m_afEntry;
   }
   inline operator Real* () {
      return m_afEntry;
   }
   inline const Real* operator[] (int iRow) const {
      return &m_afEntry[2*iRow];
   }
   inline Real* operator[] (int iRow) {
      return &m_afEntry[2*iRow];
   }
   inline Real operator() (int iRow, int iCol) const {
      return m_afEntry[iRow*2 + iCol];
   }
   inline Real& operator() (int iRow, int iCol) {
      return m_afEntry[iRow*2 + iCol];
   }

   // assignment
   inline Matrix32& operator= (const Matrix32& rkM) {
      m_afEntry[0] = rkM.m_afEntry[0];
      m_afEntry[1] = rkM.m_afEntry[1];
      m_afEntry[2] = rkM.m_afEntry[2];
      m_afEntry[3] = rkM.m_afEntry[3];
      m_afEntry[4] = rkM.m_afEntry[4];
      m_afEntry[5] = rkM.m_afEntry[5];
      return *this;
   }

   // arithmetic operations
   inline Matrix32 operator+ (const Matrix32& rkM) const {
      return Matrix32<Real>(m_afEntry[0] + rkM.m_afEntry[0],
                            m_afEntry[1] + rkM.m_afEntry[1],
                            m_afEntry[2] + rkM.m_afEntry[2],
                            m_afEntry[3] + rkM.m_afEntry[3],
                            m_afEntry[4] + rkM.m_afEntry[4],
                            m_afEntry[5] + rkM.m_afEntry[5]);
   }
   inline Matrix32 operator- (const Matrix32& rkM) const {
      return Matrix32<Real>(m_afEntry[0] - rkM.m_afEntry[0],
                            m_afEntry[1] - rkM.m_afEntry[1],
                            m_afEntry[2] - rkM.m_afEntry[2],
                            m_afEntry[3] - rkM.m_afEntry[3],
                            m_afEntry[4] - rkM.m_afEntry[4],
                            m_afEntry[5] - rkM.m_afEntry[5]);
   }

   inline Matrix32 operator* (const Matrix2<Real>& rkM) const {
      const Real* m = rkM;
      return Matrix32<Real>(  
        m_afEntry[0]*m[0] + m_afEntry[1]*m[2], m_afEntry[0]*m[1] + m_afEntry[1]*m[3],
        m_afEntry[2]*m[0] + m_afEntry[3]*m[2], m_afEntry[2]*m[1] + m_afEntry[3]*m[3],
        m_afEntry[4]*m[0] + m_afEntry[5]*m[2], m_afEntry[4]*m[1] + m_afEntry[5]*m[3]);

   }

   inline Matrix3<Real> operator* (const Matrix23<Real>& rkM) const;

   inline Matrix32 operator* (Real fScalar) const {
      return Matrix32<Real>(m_afEntry[0] * fScalar, 
                            m_afEntry[1] * fScalar, 
                            m_afEntry[2] * fScalar, 
                            m_afEntry[3] * fScalar, 
                            m_afEntry[4] * fScalar, 
                            m_afEntry[5] * fScalar);
   }
   inline Matrix32 operator/ (Real fScalar) const {
      Real ifScalar = 1.0 / fScalar;
      return Matrix32<Real>(m_afEntry[0] * ifScalar, 
                            m_afEntry[1] * ifScalar, 
                            m_afEntry[2] * ifScalar, 
                            m_afEntry[3] * ifScalar, 
                            m_afEntry[4] * ifScalar, 
                            m_afEntry[5] * ifScalar);
   }
   inline Matrix32 operator- () const {
      return Matrix32<Real>(-m_afEntry[0],
                            -m_afEntry[1],
                            -m_afEntry[2],
                            -m_afEntry[3],
                            -m_afEntry[4],
                            -m_afEntry[5]);
   }

   // arithmetic updates
   inline Matrix32& operator+= (const Matrix32& rkM) {
      m_afEntry[0] += rkM.m_afEntry[0];
      m_afEntry[1] += rkM.m_afEntry[1];
      m_afEntry[2] += rkM.m_afEntry[2];
      m_afEntry[3] += rkM.m_afEntry[3];
      m_afEntry[4] += rkM.m_afEntry[4];
      m_afEntry[5] += rkM.m_afEntry[5];
      return *this;
   }
   inline Matrix32& operator-= (const Matrix32& rkM) {
      m_afEntry[0] -= rkM.m_afEntry[0];
      m_afEntry[1] -= rkM.m_afEntry[1];
      m_afEntry[2] -= rkM.m_afEntry[2];
      m_afEntry[3] -= rkM.m_afEntry[3];
      m_afEntry[4] -= rkM.m_afEntry[4];
      m_afEntry[5] -= rkM.m_afEntry[5];
      return *this;
   }
   inline Matrix32& operator*= (Real fScalar) {
      m_afEntry[0] *= fScalar;
      m_afEntry[1] *= fScalar;
      m_afEntry[2] *= fScalar;
      m_afEntry[3] *= fScalar;
      m_afEntry[4] *= fScalar;
      m_afEntry[5] *= fScalar;
      return *this;
   }
   inline Matrix32& operator/= (Real fScalar) {
      Real ifScalar = 1.0 / fScalar;
      m_afEntry[0] *= ifScalar;
      m_afEntry[1] *= ifScalar;
      m_afEntry[2] *= ifScalar;
      m_afEntry[3] *= ifScalar;
      m_afEntry[4] *= ifScalar;
      m_afEntry[5] *= ifScalar;
      return *this;
   }

    inline bool IsZero() {
       return (m_afEntry[0] == 0.0) &&
              (m_afEntry[1] == 0.0) &&
              (m_afEntry[2] == 0.0) &&
              (m_afEntry[3] == 0.0) &&
              (m_afEntry[4] == 0.0) &&
              (m_afEntry[5] == 0.0);
    }

   // matrix times vector
   inline Vector3<Real> operator* (const Vector2<Real>& rkV) const {  // M * v
      return Vector3<Real> (m_afEntry[0] * rkV[0] + m_afEntry[1] * rkV[1],
                            m_afEntry[2] * rkV[0] + m_afEntry[3] * rkV[1],
                            m_afEntry[4] * rkV[0] + m_afEntry[5] * rkV[1]);
   }

   // other operations
   inline Vector2<Real> TransposeTimes (const Vector3<Real>& rkV) const {  // this^T * v
      return Vector2<Real> (m_afEntry[0] * rkV[0] + m_afEntry[2] * rkV[1] + m_afEntry[4] * rkV[2],
                            m_afEntry[1] * rkV[0] + m_afEntry[3] * rkV[1] + m_afEntry[5] * rkV[2]);
   }

   inline Matrix23<Real> Transpose() const;

private:

   Real m_afEntry[6];
};

// c * M
template <class Real>
inline Matrix32<Real> operator* (Real fScalar, const Matrix32<Real>& rkM) {
   return rkM * fScalar;
}

template <class Real>
inline Matrix2<Real> Matrix23<Real>::operator* (const Matrix32<Real>& rkM) const {
   const Real* m = rkM;
   return Matrix2<Real> (m_afEntry[0] * m[0] + m_afEntry[1] * m[2] + m_afEntry[2] * m[4], m_afEntry[0] * m[1] + m_afEntry[1] * m[3] + m_afEntry[2] * m[5],
                         m_afEntry[3] * m[0] + m_afEntry[4] * m[2] + m_afEntry[5] * m[4], m_afEntry[3] * m[1] + m_afEntry[4] * m[3] + m_afEntry[5] * m[5]);

}

template <class Real>
inline Matrix3<Real> Matrix32<Real>::operator* (const Matrix23<Real>& rkM) const {
   const Real* m = rkM;
   return Matrix3<Real> (m_afEntry[0] * m[0] + m_afEntry[1] * m[3], m_afEntry[0] * m[1] + m_afEntry[1] * m[4], m_afEntry[0] * m[2] + m_afEntry[1] * m[5], 
                         m_afEntry[2] * m[0] + m_afEntry[3] * m[3], m_afEntry[2] * m[1] + m_afEntry[3] * m[4], m_afEntry[2] * m[2] + m_afEntry[3] * m[5], 
                         m_afEntry[4] * m[0] + m_afEntry[5] * m[3], m_afEntry[4] * m[1] + m_afEntry[5] * m[4], m_afEntry[4] * m[2] + m_afEntry[5] * m[5]);
}

template <class Real>
inline Matrix23<Real> Matrix32<Real>::Transpose() const {
   return Matrix23<Real> (m_afEntry[0], m_afEntry[2], m_afEntry[4], 
                          m_afEntry[1], m_afEntry[3], m_afEntry[5]);
}

template <class Real>
inline Matrix32<Real> Matrix23<Real>::Transpose() const {
   return Matrix23<Real> (m_afEntry[0], m_afEntry[3], 
                          m_afEntry[1], m_afEntry[4], 
                          m_afEntry[2], m_afEntry[5]);
}

// Matrix34
template <class Real>
class Matrix34
{
public:
    // If bZero is true, create the zero matrix.  Otherwise, create the
    // identity matrix.
   Matrix34 () {
      memset(m_afEntry, 0, sizeof(Real)*12);
   }

   // copy constructor
   Matrix34 (const Matrix34& rkM) {
      m_afEntry[0] = rkM.m_afEntry[0];
      m_afEntry[1] = rkM.m_afEntry[1];
      m_afEntry[2] = rkM.m_afEntry[2];
      m_afEntry[3] = rkM.m_afEntry[3];
      m_afEntry[4] = rkM.m_afEntry[4];
      m_afEntry[5] = rkM.m_afEntry[5];
      m_afEntry[6] = rkM.m_afEntry[6];
      m_afEntry[7] = rkM.m_afEntry[7];
      m_afEntry[8] = rkM.m_afEntry[8];
      m_afEntry[9] = rkM.m_afEntry[9];
      m_afEntry[10] = rkM.m_afEntry[10];
      m_afEntry[11] = rkM.m_afEntry[11];
   }

   // input Mrc is in row r, column c.
   Matrix34 (Real fM00, Real fM01, Real fM02, Real fM03,
          Real fM10, Real fM11, Real fM12, Real fM13,
          Real fM20, Real fM21, Real fM22, Real fM23) {
      m_afEntry[0] = fM00;
      m_afEntry[1] = fM01;
      m_afEntry[2] = fM02;
      m_afEntry[3] = fM03;
      m_afEntry[4] = fM10;
      m_afEntry[5] = fM11;
      m_afEntry[6] = fM12;
      m_afEntry[7] = fM13;
      m_afEntry[8] = fM20;
      m_afEntry[9] = fM21;
      m_afEntry[10] = fM22;
      m_afEntry[11] = fM23;
   }

   Matrix34 (const Real* val) {
      memcpy(m_afEntry, val, sizeof(Real)*12);
   }
   // create various matrices
   inline Matrix34& MakeZero () {
      memset(m_afEntry, 0, sizeof(Real)*12);
   }

   // member access
   inline operator const Real* () const {
      return m_afEntry;
   }
   inline operator Real* () {
      return m_afEntry;
   }
   inline const Real* operator[] (int iRow) const {
      return &m_afEntry[4*iRow];
   }
   inline Real* operator[] (int iRow) {
      return &m_afEntry[4*iRow];
   }
   inline Real operator() (int iRow, int iCol) const {
      return m_afEntry[iRow*4 + iCol];
   }
   inline Real& operator() (int iRow, int iCol) {
      return m_afEntry[iRow*4 + iCol];
   }

   // assignment
   inline Matrix34& operator= (const Matrix34& rkM) {
      m_afEntry[0] = rkM.m_afEntry[0];
      m_afEntry[1] = rkM.m_afEntry[1];
      m_afEntry[2] = rkM.m_afEntry[2];
      m_afEntry[3] = rkM.m_afEntry[3];
      m_afEntry[4] = rkM.m_afEntry[4];
      m_afEntry[5] = rkM.m_afEntry[5];
      m_afEntry[6] = rkM.m_afEntry[6];
      m_afEntry[7] = rkM.m_afEntry[7];
      m_afEntry[8] = rkM.m_afEntry[8];
      m_afEntry[9] = rkM.m_afEntry[9];
      m_afEntry[10] = rkM.m_afEntry[10];
      m_afEntry[11] = rkM.m_afEntry[11];
      return *this;
   }

   // arithmetic operations
   inline Matrix34 operator+ (const Matrix34& rkM) const {
      return Matrix34<Real>(m_afEntry[0] + rkM.m_afEntry[0],
                            m_afEntry[1] + rkM.m_afEntry[1],
                            m_afEntry[2] + rkM.m_afEntry[2],
                            m_afEntry[3] + rkM.m_afEntry[3],
                            m_afEntry[4] + rkM.m_afEntry[4],
                            m_afEntry[5] + rkM.m_afEntry[5],
                            m_afEntry[6] + rkM.m_afEntry[6],
                            m_afEntry[7] + rkM.m_afEntry[7],
                            m_afEntry[8] + rkM.m_afEntry[8],
                            m_afEntry[9] + rkM.m_afEntry[9],
                            m_afEntry[10] + rkM.m_afEntry[10],
                            m_afEntry[11] + rkM.m_afEntry[11]);
   }
   inline Matrix34 operator- (const Matrix34& rkM) const {
      return Matrix34<Real>(m_afEntry[0] - rkM.m_afEntry[0],
                            m_afEntry[1] - rkM.m_afEntry[1],
                            m_afEntry[2] - rkM.m_afEntry[2],
                            m_afEntry[3] - rkM.m_afEntry[3],
                            m_afEntry[4] - rkM.m_afEntry[4],
                            m_afEntry[5] - rkM.m_afEntry[5],
                            m_afEntry[6] - rkM.m_afEntry[6],
                            m_afEntry[7] - rkM.m_afEntry[7],
                            m_afEntry[8] - rkM.m_afEntry[8],
                            m_afEntry[9] - rkM.m_afEntry[9],
                            m_afEntry[10] - rkM.m_afEntry[10],
                            m_afEntry[11] - rkM.m_afEntry[11]);
   }

   inline Matrix34 operator* (const Matrix4<Real>& rkM) const {
      const Real* m = rkM;
      return Matrix34<Real>(m_afEntry[0] * m[0] + m_afEntry[1] * m[4] + m_afEntry[2] * m[8] + m_afEntry[3] * m[12],
                            m_afEntry[0] * m[1] + m_afEntry[1] * m[5] + m_afEntry[2] * m[9] + m_afEntry[3] * m[13],
                            m_afEntry[0] * m[2] + m_afEntry[1] * m[6] + m_afEntry[2] * m[10] + m_afEntry[3] * m[14],
                            m_afEntry[0] * m[3] + m_afEntry[1] * m[7] + m_afEntry[2] * m[11] + m_afEntry[3] * m[15],

                            m_afEntry[4] * m[0] + m_afEntry[5] * m[4] + m_afEntry[6] * m[8] + m_afEntry[7] * m[12],
                            m_afEntry[4] * m[1] + m_afEntry[5] * m[5] + m_afEntry[6] * m[9] + m_afEntry[7] * m[13],
                            m_afEntry[4] * m[2] + m_afEntry[5] * m[6] + m_afEntry[6] * m[10] + m_afEntry[7] * m[14],
                            m_afEntry[4] * m[3] + m_afEntry[5] * m[7] + m_afEntry[6] * m[11] + m_afEntry[7] * m[15],

                            m_afEntry[8] * m[0] + m_afEntry[9] * m[4] + m_afEntry[10] * m[8] + m_afEntry[11] * m[12],
                            m_afEntry[8] * m[1] + m_afEntry[9] * m[5] + m_afEntry[10] * m[9] + m_afEntry[11] * m[13],
                            m_afEntry[8] * m[2] + m_afEntry[9] * m[6] + m_afEntry[10] * m[10] + m_afEntry[11] * m[14],
                            m_afEntry[8] * m[3] + m_afEntry[9] * m[7] + m_afEntry[10] * m[11] + m_afEntry[11] * m[15]);
   }

   inline Matrix3<Real> operator* (const Matrix43<Real>& rkM) const;

   inline Matrix34 operator* (Real fScalar) const {
      return Matrix34<Real>(m_afEntry[0] * fScalar, 
                            m_afEntry[1] * fScalar, 
                            m_afEntry[2] * fScalar, 
                            m_afEntry[3] * fScalar, 
                            m_afEntry[4] * fScalar, 
                            m_afEntry[5] * fScalar, 
                            m_afEntry[6] * fScalar, 
                            m_afEntry[7] * fScalar, 
                            m_afEntry[8] * fScalar, 
                            m_afEntry[9] * fScalar, 
                            m_afEntry[10] * fScalar, 
                            m_afEntry[11] * fScalar);
   }
   inline Matrix34 operator/ (Real fScalar) const {
      Real ifScalar = 1.0 / fScalar;
      return Matrix34<Real>(m_afEntry[0] * ifScalar, 
                            m_afEntry[1] * ifScalar, 
                            m_afEntry[2] * ifScalar, 
                            m_afEntry[3] * ifScalar, 
                            m_afEntry[4] * ifScalar, 
                            m_afEntry[5] * ifScalar, 
                            m_afEntry[6] * ifScalar, 
                            m_afEntry[7] * ifScalar, 
                            m_afEntry[8] * ifScalar, 
                            m_afEntry[9] * ifScalar, 
                            m_afEntry[10] * ifScalar, 
                            m_afEntry[11] * ifScalar);
   }
   inline Matrix34 operator- () const {
      return Matrix34<Real>(-m_afEntry[0],
                            -m_afEntry[1],
                            -m_afEntry[2],
                            -m_afEntry[3],
                            -m_afEntry[4],
                            -m_afEntry[5],
                            -m_afEntry[6],
                            -m_afEntry[7],
                            -m_afEntry[8],
                            -m_afEntry[9],
                            -m_afEntry[10],
                            -m_afEntry[11]);
   }

   // arithmetic updates
   inline Matrix34& operator+= (const Matrix34& rkM) {
      m_afEntry[0] += rkM.m_afEntry[0];
      m_afEntry[1] += rkM.m_afEntry[1];
      m_afEntry[2] += rkM.m_afEntry[2];
      m_afEntry[3] += rkM.m_afEntry[3];
      m_afEntry[4] += rkM.m_afEntry[4];
      m_afEntry[5] += rkM.m_afEntry[5];
      m_afEntry[6] += rkM.m_afEntry[6];
      m_afEntry[7] += rkM.m_afEntry[7];
      m_afEntry[8] += rkM.m_afEntry[8];
      m_afEntry[9] += rkM.m_afEntry[9];
      m_afEntry[10] += rkM.m_afEntry[10];
      m_afEntry[11] += rkM.m_afEntry[11];
      return *this;
   }
   inline Matrix34& operator-= (const Matrix34& rkM) {
      m_afEntry[0] -= rkM.m_afEntry[0];
      m_afEntry[1] -= rkM.m_afEntry[1];
      m_afEntry[2] -= rkM.m_afEntry[2];
      m_afEntry[3] -= rkM.m_afEntry[3];
      m_afEntry[4] -= rkM.m_afEntry[4];
      m_afEntry[5] -= rkM.m_afEntry[5];
      m_afEntry[6] -= rkM.m_afEntry[6];
      m_afEntry[7] -= rkM.m_afEntry[7];
      m_afEntry[8] -= rkM.m_afEntry[8];
      m_afEntry[9] -= rkM.m_afEntry[9];
      m_afEntry[10] -= rkM.m_afEntry[10];
      m_afEntry[11] -= rkM.m_afEntry[11];
      return *this;
   }
   inline Matrix34& operator*= (Real fScalar) {
      m_afEntry[0] *= fScalar;
      m_afEntry[1] *= fScalar;
      m_afEntry[2] *= fScalar;
      m_afEntry[3] *= fScalar;
      m_afEntry[4] *= fScalar;
      m_afEntry[5] *= fScalar;
      m_afEntry[6] *= fScalar;
      m_afEntry[7] *= fScalar;
      m_afEntry[8] *= fScalar;
      m_afEntry[9] *= fScalar;
      m_afEntry[10] *= fScalar;
      m_afEntry[11] *= fScalar;
      return *this;
   }
   inline Matrix34& operator/= (Real fScalar) {
      Real ifScalar = 1.0 / fScalar;
      m_afEntry[0] *= ifScalar;
      m_afEntry[1] *= ifScalar;
      m_afEntry[2] *= ifScalar;
      m_afEntry[3] *= ifScalar;
      m_afEntry[4] *= ifScalar;
      m_afEntry[5] *= ifScalar;
      m_afEntry[6] *= ifScalar;
      m_afEntry[7] *= ifScalar;
      m_afEntry[8] *= ifScalar;
      m_afEntry[9] *= ifScalar;
      m_afEntry[10] *= ifScalar;
      m_afEntry[11] *= ifScalar;
      return *this;
   }

    inline bool IsZero() {
       return (m_afEntry[0] == 0.0) &&
              (m_afEntry[1] == 0.0) &&
              (m_afEntry[2] == 0.0) &&
              (m_afEntry[3] == 0.0) &&
              (m_afEntry[4] == 0.0) &&
              (m_afEntry[5] == 0.0) &&
              (m_afEntry[6] == 0.0) &&
              (m_afEntry[7] == 0.0) &&
              (m_afEntry[8] == 0.0) &&
              (m_afEntry[9] == 0.0) &&
              (m_afEntry[10] == 0.0) &&
              (m_afEntry[11] == 0.0);
    }

   // matrix times vector
   inline Vector3<Real> operator* (const Vector4<Real>& rkV) const {  // M * v
      return Vector3<Real> (m_afEntry[0] * rkV[0] + m_afEntry[1] * rkV[1] + m_afEntry[2] * rkV[2] + m_afEntry[3] * rkV[3], 
                            m_afEntry[4] * rkV[0] + m_afEntry[5] * rkV[1] + m_afEntry[6] * rkV[2] + m_afEntry[7] * rkV[3], 
                            m_afEntry[8] * rkV[0] + m_afEntry[9] * rkV[1] + m_afEntry[10] * rkV[2] + m_afEntry[11] * rkV[3]);
   }

   // other operations
   inline Vector4<Real> TransposeTimes (const Vector3<Real>& rkV) const {  // this^T * v
      return Vector4<Real> (m_afEntry[0] * rkV[0] + m_afEntry[4] * rkV[1] + m_afEntry[8] * rkV[2],
                            m_afEntry[1] * rkV[0] + m_afEntry[5] * rkV[1] + m_afEntry[9] * rkV[2],
                            m_afEntry[2] * rkV[0] + m_afEntry[6] * rkV[1] + m_afEntry[10] * rkV[2],
                            m_afEntry[3] * rkV[0] + m_afEntry[7] * rkV[1] + m_afEntry[11] * rkV[2]);
   }

private:

   Real m_afEntry[12];
};

// c * M
template <class Real>
inline Matrix34<Real> operator* (Real fScalar, const Matrix34<Real>& rkM) {
   return rkM * fScalar;
}

// Matrix43
template <class Real>
class Matrix43
{
public:
    // If bZero is true, create the zero matrix.  Otherwise, create the
    // identity matrix.
   Matrix43 () {
      memset(m_afEntry, 0, sizeof(Real)*12);
   }

   // copy constructor
   Matrix43 (const Matrix43& rkM) {
      m_afEntry[0] = rkM.m_afEntry[0];
      m_afEntry[1] = rkM.m_afEntry[1];
      m_afEntry[2] = rkM.m_afEntry[2];
      m_afEntry[3] = rkM.m_afEntry[3];
      m_afEntry[4] = rkM.m_afEntry[4];
      m_afEntry[5] = rkM.m_afEntry[5];
      m_afEntry[6] = rkM.m_afEntry[6];
      m_afEntry[7] = rkM.m_afEntry[7];
      m_afEntry[8] = rkM.m_afEntry[8];
      m_afEntry[9] = rkM.m_afEntry[9];
      m_afEntry[10] = rkM.m_afEntry[10];
      m_afEntry[11] = rkM.m_afEntry[11];
   }

   // input Mrc is in row r, column c.
   Matrix43 (Real fM00, Real fM01, Real fM02,
          Real fM10, Real fM11, Real fM12, 
          Real fM20, Real fM21, Real fM22, 
          Real fM30, Real fM31, Real fM32) {
      m_afEntry[0] = fM00;
      m_afEntry[1] = fM01;
      m_afEntry[2] = fM02;
      m_afEntry[3] = fM10;
      m_afEntry[4] = fM11;
      m_afEntry[5] = fM12;
      m_afEntry[6] = fM20;
      m_afEntry[7] = fM21;
      m_afEntry[8] = fM22;
      m_afEntry[9] = fM30;
      m_afEntry[10] = fM31;
      m_afEntry[11] = fM32;
   }

   Matrix43 (const Real* val) {
      memcpy(m_afEntry, val, sizeof(Real)*12);
   }
   // create various matrices
   inline Matrix43& MakeZero () {
      memset(m_afEntry, 0, sizeof(Real)*12);
   }

   // member access
   inline operator const Real* () const {
      return m_afEntry;
   }
   inline operator Real* () {
      return m_afEntry;
   }
   inline const Real* operator[] (int iRow) const {
      return &m_afEntry[3*iRow];
   }
   inline Real* operator[] (int iRow) {
      return &m_afEntry[3*iRow];
   }
   inline Real operator() (int iRow, int iCol) const {
      return m_afEntry[iRow*3 + iCol];
   }
   inline Real& operator() (int iRow, int iCol) {
      return m_afEntry[iRow*3 + iCol];
   }

   // assignment
   inline Matrix43& operator= (const Matrix43& rkM) {
      m_afEntry[0] = rkM.m_afEntry[0];
      m_afEntry[1] = rkM.m_afEntry[1];
      m_afEntry[2] = rkM.m_afEntry[2];
      m_afEntry[3] = rkM.m_afEntry[3];
      m_afEntry[4] = rkM.m_afEntry[4];
      m_afEntry[5] = rkM.m_afEntry[5];
      m_afEntry[6] = rkM.m_afEntry[6];
      m_afEntry[7] = rkM.m_afEntry[7];
      m_afEntry[8] = rkM.m_afEntry[8];
      m_afEntry[9] = rkM.m_afEntry[9];
      m_afEntry[10] = rkM.m_afEntry[10];
      m_afEntry[11] = rkM.m_afEntry[11];
      return *this;
   }

   // arithmetic operations
   inline Matrix43 operator+ (const Matrix43& rkM) const {
      return Matrix43<Real>(m_afEntry[0] + rkM.m_afEntry[0],
                            m_afEntry[1] + rkM.m_afEntry[1],
                            m_afEntry[2] + rkM.m_afEntry[2],
                            m_afEntry[3] + rkM.m_afEntry[3],
                            m_afEntry[4] + rkM.m_afEntry[4],
                            m_afEntry[5] + rkM.m_afEntry[5],
                            m_afEntry[6] + rkM.m_afEntry[6],
                            m_afEntry[7] + rkM.m_afEntry[7],
                            m_afEntry[8] + rkM.m_afEntry[8],
                            m_afEntry[9] + rkM.m_afEntry[9],
                            m_afEntry[10] + rkM.m_afEntry[10],
                            m_afEntry[11] + rkM.m_afEntry[11]);
   }
   inline Matrix43 operator- (const Matrix43& rkM) const {
      return Matrix43<Real>(m_afEntry[0] - rkM.m_afEntry[0],
                            m_afEntry[1] - rkM.m_afEntry[1],
                            m_afEntry[2] - rkM.m_afEntry[2],
                            m_afEntry[3] - rkM.m_afEntry[3],
                            m_afEntry[4] - rkM.m_afEntry[4],
                            m_afEntry[5] - rkM.m_afEntry[5],
                            m_afEntry[6] - rkM.m_afEntry[6],
                            m_afEntry[7] - rkM.m_afEntry[7],
                            m_afEntry[8] - rkM.m_afEntry[8],
                            m_afEntry[9] - rkM.m_afEntry[9],
                            m_afEntry[10] - rkM.m_afEntry[10],
                            m_afEntry[11] - rkM.m_afEntry[11]);
   }

   inline Matrix43 operator* (const Matrix3<Real>& rkM) const {
      const Real* m = rkM;
      return Matrix43<Real>(m_afEntry[0] * m[0] + m_afEntry[1] * m[3] + m_afEntry[2] * m[6],
                            m_afEntry[0] * m[1] + m_afEntry[1] * m[4] + m_afEntry[2] * m[7],
                            m_afEntry[0] * m[2] + m_afEntry[1] * m[5] + m_afEntry[2] * m[8], 

                            m_afEntry[3] * m[0] + m_afEntry[4] * m[3] + m_afEntry[5] * m[6],
                            m_afEntry[3] * m[1] + m_afEntry[4] * m[4] + m_afEntry[5] * m[7],
                            m_afEntry[3] * m[2] + m_afEntry[4] * m[5] + m_afEntry[5] * m[8], 

                            m_afEntry[6] * m[0] + m_afEntry[7] * m[3] + m_afEntry[8] * m[6],
                            m_afEntry[6] * m[1] + m_afEntry[7] * m[4] + m_afEntry[8] * m[7],
                            m_afEntry[6] * m[2] + m_afEntry[7] * m[5] + m_afEntry[8] * m[8],
   
                            m_afEntry[9] * m[0] + m_afEntry[10] * m[3] + m_afEntry[11] * m[6],
                            m_afEntry[9] * m[1] + m_afEntry[10] * m[4] + m_afEntry[11] * m[7],
                            m_afEntry[9] * m[2] + m_afEntry[10] * m[5] + m_afEntry[11] * m[8]);

   }

   inline Matrix4<Real> operator* (const Matrix34<Real>& rkM) const {
      const Real* m = rkM;
      return Matrix4<Real>(m_afEntry[0] * m[0] + m_afEntry[1] * m[4] + m_afEntry[2] * m[8],
                           m_afEntry[0] * m[1] + m_afEntry[1] * m[5] + m_afEntry[2] * m[9],
                           m_afEntry[0] * m[2] + m_afEntry[1] * m[6] + m_afEntry[2] * m[10],
                           m_afEntry[0] * m[3] + m_afEntry[1] * m[7] + m_afEntry[2] * m[11],

                           m_afEntry[3] * m[0] + m_afEntry[4] * m[4] + m_afEntry[5] * m[8],
                           m_afEntry[3] * m[1] + m_afEntry[4] * m[5] + m_afEntry[5] * m[9],
                           m_afEntry[3] * m[2] + m_afEntry[4] * m[6] + m_afEntry[5] * m[10],
                           m_afEntry[3] * m[3] + m_afEntry[4] * m[7] + m_afEntry[5] * m[11],

                           m_afEntry[6] * m[0] + m_afEntry[7] * m[4] + m_afEntry[8] * m[8],
                           m_afEntry[6] * m[1] + m_afEntry[7] * m[5] + m_afEntry[8] * m[9],
                           m_afEntry[6] * m[2] + m_afEntry[7] * m[6] + m_afEntry[8] * m[10],
                           m_afEntry[6] * m[3] + m_afEntry[7] * m[7] + m_afEntry[8] * m[11],

                           m_afEntry[9] * m[0] + m_afEntry[10] * m[4] + m_afEntry[11] * m[8],
                           m_afEntry[9] * m[1] + m_afEntry[10] * m[5] + m_afEntry[11] * m[9],
                           m_afEntry[9] * m[2] + m_afEntry[10] * m[6] + m_afEntry[11] * m[10],
                           m_afEntry[9] * m[3] + m_afEntry[10] * m[7] + m_afEntry[11] * m[11]);


                         
   }

   inline Matrix43 operator* (Real fScalar) const {
      return Matrix43<Real>(m_afEntry[0] * fScalar, 
                            m_afEntry[1] * fScalar, 
                            m_afEntry[2] * fScalar, 
                            m_afEntry[3] * fScalar, 
                            m_afEntry[4] * fScalar, 
                            m_afEntry[5] * fScalar, 
                            m_afEntry[6] * fScalar, 
                            m_afEntry[7] * fScalar, 
                            m_afEntry[8] * fScalar, 
                            m_afEntry[9] * fScalar, 
                            m_afEntry[10] * fScalar, 
                            m_afEntry[11] * fScalar);
   }
   inline Matrix43 operator/ (Real fScalar) const {
      Real ifScalar = 1.0 / fScalar;
      return Matrix43<Real>(m_afEntry[0] * ifScalar, 
                            m_afEntry[1] * ifScalar, 
                            m_afEntry[2] * ifScalar, 
                            m_afEntry[3] * ifScalar, 
                            m_afEntry[4] * ifScalar, 
                            m_afEntry[5] * ifScalar, 
                            m_afEntry[6] * ifScalar, 
                            m_afEntry[7] * ifScalar, 
                            m_afEntry[8] * ifScalar, 
                            m_afEntry[9] * ifScalar, 
                            m_afEntry[10] * ifScalar, 
                            m_afEntry[11] * ifScalar);
   }
   inline Matrix43 operator- () const {
      return Matrix43<Real>(-m_afEntry[0],
                            -m_afEntry[1],
                            -m_afEntry[2],
                            -m_afEntry[3],
                            -m_afEntry[4],
                            -m_afEntry[5],
                            -m_afEntry[6],
                            -m_afEntry[7],
                            -m_afEntry[8],
                            -m_afEntry[9],
                            -m_afEntry[10],
                            -m_afEntry[11]);
   }

   // arithmetic updates
   inline Matrix43& operator+= (const Matrix43& rkM) {
      m_afEntry[0] += rkM.m_afEntry[0];
      m_afEntry[1] += rkM.m_afEntry[1];
      m_afEntry[2] += rkM.m_afEntry[2];
      m_afEntry[3] += rkM.m_afEntry[3];
      m_afEntry[4] += rkM.m_afEntry[4];
      m_afEntry[5] += rkM.m_afEntry[5];
      m_afEntry[6] += rkM.m_afEntry[6];
      m_afEntry[7] += rkM.m_afEntry[7];
      m_afEntry[8] += rkM.m_afEntry[8];
      m_afEntry[9] += rkM.m_afEntry[9];
      m_afEntry[10] += rkM.m_afEntry[10];
      m_afEntry[11] += rkM.m_afEntry[11];
      return *this;
   }
   inline Matrix43& operator-= (const Matrix43& rkM) {
      m_afEntry[0] -= rkM.m_afEntry[0];
      m_afEntry[1] -= rkM.m_afEntry[1];
      m_afEntry[2] -= rkM.m_afEntry[2];
      m_afEntry[3] -= rkM.m_afEntry[3];
      m_afEntry[4] -= rkM.m_afEntry[4];
      m_afEntry[5] -= rkM.m_afEntry[5];
      m_afEntry[6] -= rkM.m_afEntry[6];
      m_afEntry[7] -= rkM.m_afEntry[7];
      m_afEntry[8] -= rkM.m_afEntry[8];
      m_afEntry[9] -= rkM.m_afEntry[9];
      m_afEntry[10] -= rkM.m_afEntry[10];
      m_afEntry[11] -= rkM.m_afEntry[11];
      return *this;
   }
   inline Matrix43& operator*= (Real fScalar) {
      m_afEntry[0] *= fScalar;
      m_afEntry[1] *= fScalar;
      m_afEntry[2] *= fScalar;
      m_afEntry[3] *= fScalar;
      m_afEntry[4] *= fScalar;
      m_afEntry[5] *= fScalar;
      m_afEntry[6] *= fScalar;
      m_afEntry[7] *= fScalar;
      m_afEntry[8] *= fScalar;
      m_afEntry[9] *= fScalar;
      m_afEntry[10] *= fScalar;
      m_afEntry[11] *= fScalar;
      return *this;
   }
   inline Matrix43& operator/= (Real fScalar) {
      Real ifScalar = 1.0 / fScalar;
      m_afEntry[0] *= ifScalar;
      m_afEntry[1] *= ifScalar;
      m_afEntry[2] *= ifScalar;
      m_afEntry[3] *= ifScalar;
      m_afEntry[4] *= ifScalar;
      m_afEntry[5] *= ifScalar;
      m_afEntry[6] *= ifScalar;
      m_afEntry[7] *= ifScalar;
      m_afEntry[8] *= ifScalar;
      m_afEntry[9] *= ifScalar;
      m_afEntry[10] *= ifScalar;
      m_afEntry[11] *= ifScalar;
      return *this;
   }

   // matrix times vector
   inline Vector4<Real> operator* (const Vector3<Real>& rkV) const {  // M * v
      return Vector4<Real> (m_afEntry[0] * rkV[0] + m_afEntry[1] * rkV[1] + m_afEntry[2] * rkV[2], 
                            m_afEntry[3] * rkV[0] + m_afEntry[4] * rkV[1] + m_afEntry[5] * rkV[2], 
                            m_afEntry[6] * rkV[0] + m_afEntry[7] * rkV[1] + m_afEntry[8] * rkV[2]);
   }

   // other operations
   inline Vector3<Real> TransposeTimes (const Vector4<Real>& rkV) const {  // this^T * v
      return Vector3<Real> (m_afEntry[0] * rkV[0] + m_afEntry[3] * rkV[1] + m_afEntry[6] * rkV[2] + m_afEntry[9] * rkV[2],
                            m_afEntry[1] * rkV[0] + m_afEntry[4] * rkV[1] + m_afEntry[7] * rkV[2] + m_afEntry[10] * rkV[2],
                            m_afEntry[2] * rkV[0] + m_afEntry[5] * rkV[1] + m_afEntry[8] * rkV[2] + m_afEntry[11] * rkV[2]);
   }

    inline bool IsZero() {
       return (m_afEntry[0] == 0.0) &&
              (m_afEntry[1] == 0.0) &&
              (m_afEntry[2] == 0.0) &&
              (m_afEntry[3] == 0.0) &&
              (m_afEntry[4] == 0.0) &&
              (m_afEntry[5] == 0.0) &&
              (m_afEntry[6] == 0.0) &&
              (m_afEntry[7] == 0.0) &&
              (m_afEntry[8] == 0.0) &&
              (m_afEntry[9] == 0.0) &&
              (m_afEntry[10] == 0.0) &&
              (m_afEntry[11] == 0.0);
    }
private:

   Real m_afEntry[12];
};
template <class Real>
inline Matrix3<Real> Matrix34<Real>::operator* (const Matrix43<Real>& rkM) const {
   const Real* m = rkM;
   return Matrix3<Real>(m_afEntry[0] * m[0] + m_afEntry[1] * m[3] + m_afEntry[2] * m[6] + m_afEntry[3] * m[9],
                        m_afEntry[0] * m[1] + m_afEntry[1] * m[4] + m_afEntry[2] * m[7] + m_afEntry[3] * m[10],
                        m_afEntry[0] * m[2] + m_afEntry[1] * m[5] + m_afEntry[2] * m[8] + m_afEntry[3] * m[11],

                        m_afEntry[4] * m[0] + m_afEntry[5] * m[3] + m_afEntry[6] * m[6] + m_afEntry[7] * m[9],
                        m_afEntry[4] * m[1] + m_afEntry[5] * m[4] + m_afEntry[6] * m[7] + m_afEntry[7] * m[10],
                        m_afEntry[4] * m[2] + m_afEntry[5] * m[5] + m_afEntry[6] * m[8] + m_afEntry[7] * m[11],

                        m_afEntry[8] * m[0] + m_afEntry[9] * m[3] + m_afEntry[10] * m[6] + m_afEntry[11] * m[9],
                        m_afEntry[8] * m[1] + m_afEntry[9] * m[4] + m_afEntry[10] * m[7] + m_afEntry[11] * m[10],
                        m_afEntry[8] * m[2] + m_afEntry[9] * m[5] + m_afEntry[10] * m[8] + m_afEntry[11] * m[11]);


}

// c * M
template <class Real>
inline Matrix43<Real> operator* (Real fScalar, const Matrix43<Real>& rkM) {
   return rkM * fScalar;
}
template <class Real>
ostream& operator << (ostream& o, const Matrix2<Real>& rkM) {
   const Real* m = rkM;
   o<<m[0]<<" "<<m[1]<<endl
    <<m[2]<<" "<<m[3]<<endl;
   return o;
}
template <class Real>
ostream& operator << (ostream& o, const Matrix34<Real>& rkM) {
   const Real* m = rkM;
   o<<m[0]<<" "<<m[1]<<" "<<m[2]<<" "<<m[3]<<endl
    <<m[4]<<" "<<m[5]<<" "<<m[6]<<" "<<m[7]<<endl
    <<m[8]<<" "<<m[9]<<" "<<m[10]<<" "<<m[11]<<endl;
   return o;
}
template <class Real>
ostream& operator << (ostream& o, const Matrix4<Real>& rkM) {
   const Real* m = rkM;
   o<<m[0]<<" "<<m[1]<<" "<<m[2]<<" "<<m[3]<<endl
    <<m[4]<<" "<<m[5]<<" "<<m[6]<<" "<<m[7]<<endl
    <<m[8]<<" "<<m[9]<<" "<<m[10]<<" "<<m[11]<<endl
    <<m[12]<<" "<<m[13]<<" "<<m[14]<<" "<<m[15]<<endl;
   return o;
}

template <class Real>
ostream& operator << (ostream& o, const Matrix43<Real>& rkM) {
   const Real* m = rkM;
   o<<m[0]<<" "<<m[1]<<" "<<m[2]<<endl
    <<m[3]<<" "<<m[4]<<" "<<m[5]<<endl
    <<m[6]<<" "<<m[7]<<" "<<m[8]<<endl
    <<m[9]<<" "<<m[10]<<" "<<m[11]<<endl;
   return o;
}



template <class Real>
ostream& operator << (ostream& o, const Matrix23<Real>& rkM) {
   const Real* m = rkM;
   o<<m[0]<<" "<<m[1]<<" "<<m[2]<<endl
    <<m[3]<<" "<<m[4]<<" "<<m[5]<<endl;
   return o;
}

template <class Real>
ostream& operator << (ostream& o, const Matrix32<Real>& rkM) {
   const Real* m = rkM;
   o<<m[0]<<" "<<m[1]<<endl
    <<m[2]<<" "<<m[3]<<endl
    <<m[4]<<" "<<m[5]<<endl;
   return o;
}
*/
/*template <class Real>
ostream& operator << (ostream& o, const Matrix34<Real>& rkM) {
   const Real* m = rkM;
   o<<m[0];
   return o;
}
template <class Real>
ostream& operator << (ostream& o, const Matrix4<Real>& rkM) {
   const Real* m = rkM;
   o<<m[0];
   return o;
}

template <class Real>
ostream& operator << (ostream& o, const Matrix43<Real>& rkM) {
   const Real* m = rkM;
   o<<m[0];
   return o;
}


template <class Real>
ostream& operator << (ostream& o, const Matrix3<Real>& rkM) {
   const Real* m = rkM;
   o<<m[0];
   return o;
}
*/

#endif