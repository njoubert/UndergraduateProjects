// Wild Magic Source Code
// David Eberly
// http://www.geometrictools.com
// Copyright (c) 1998-2007
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or (at
// your option) any later version.  The license is available for reading at
// either of the locations:
//     http://www.gnu.org/copyleft/lgpl.html
//     http://www.geometrictools.com/License/WildMagicLicense.pdf
//
// Version: 4.0.1 (2007/05/06)

#include "Wm4FoundationPCH.h"
#include "Wm4DistVector3Ray3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
DistVector3Ray3<Real>::DistVector3Ray3 (const Vector3<Real>& rkVector,
    const Ray3<Real>& rkRay)
    :
    m_pkVector(&rkVector),
    m_pkRay(&rkRay)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& DistVector3Ray3<Real>::GetVector () const
{
    return *m_pkVector;
}
//----------------------------------------------------------------------------
template <class Real>
const Ray3<Real>& DistVector3Ray3<Real>::GetRay () const
{
    return *m_pkRay;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Ray3<Real>::Get ()
{
    Real fSqrDist = GetSquared();
    return Math<Real>::Sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Ray3<Real>::GetSquared ()
{
    Vector3<Real> kDiff = *m_pkVector - m_pkRay->Origin;
    m_fRayParameter = m_pkRay->Direction.Dot(kDiff);
    if (m_fRayParameter > (Real)0.0)
    {
        m_kClosestPoint1 = m_pkRay->Origin +
            m_fRayParameter*m_pkRay->Direction;
    }
    else
    {
        m_kClosestPoint1 = m_pkRay->Origin;
        m_fRayParameter = (Real)0.0;
    }

    m_kClosestPoint0 = *m_pkVector;
    kDiff = m_kClosestPoint1 - m_kClosestPoint0;
    return kDiff.SquaredLength();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Ray3<Real>::Get (Real fT, const Vector3<Real>& rkVelocity0,
    const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMVector = *m_pkVector + fT*rkVelocity0;
    Vector3<Real> kMOrigin = m_pkRay->Origin + fT*rkVelocity1;
    Ray3<Real> kMRay(kMOrigin,m_pkRay->Direction);
    return DistVector3Ray3<Real>(kMVector,kMRay).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Ray3<Real>::GetSquared (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMVector = *m_pkVector + fT*rkVelocity0;
    Vector3<Real> kMOrigin = m_pkRay->Origin + fT*rkVelocity1;
    Ray3<Real> kMRay(kMOrigin,m_pkRay->Direction);
    return DistVector3Ray3<Real>(kMVector,kMRay).GetSquared();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Ray3<Real>::GetRayParameter () const
{
    return m_fRayParameter;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class DistVector3Ray3<float>;

template WM4_FOUNDATION_ITEM
class DistVector3Ray3<double>;
//----------------------------------------------------------------------------
}
