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
#include "Wm4IntrPlane3Cylinder3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrPlane3Cylinder3<Real>::IntrPlane3Cylinder3 (const Plane3<Real>& rkPlane,
    const Cylinder3<Real>& rkCylinder)
    :
    m_pkPlane(&rkPlane),
    m_pkCylinder(&rkCylinder)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Plane3<Real>& IntrPlane3Cylinder3<Real>::GetPlane () const
{
    return *m_pkPlane;
}
//----------------------------------------------------------------------------
template <class Real>
const Cylinder3<Real>& IntrPlane3Cylinder3<Real>::GetCylinder () const
{
    return *m_pkCylinder;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrPlane3Cylinder3<Real>::Test ()
{
    // Compute extremes of signed distance Dot(N,X)-d for points on the
    // cylinder.  These are
    //   min = (Dot(N,C)-d) - r*sqrt(1-Dot(N,W)^2) - (h/2)*|Dot(N,W)|
    //   max = (Dot(N,C)-d) + r*sqrt(1-Dot(N,W)^2) + (h/2)*|Dot(N,W)|
    Real fSDist = m_pkPlane->DistanceTo(m_pkCylinder->Segment.Origin);
    Real fAbsNdW = Math<Real>::FAbs(m_pkPlane->Normal.Dot(
        m_pkCylinder->Segment.Direction));
    Real fRoot = Math<Real>::Sqrt(Math<Real>::FAbs((Real)1.0
        - fAbsNdW*fAbsNdW));
    Real fTerm = m_pkCylinder->Radius*fRoot +
        ((Real)0.5)*m_pkCylinder->Height*fAbsNdW;

    // intersection occurs if and only if 0 is in the interval [min,max]
    return Math<Real>::FAbs(fSDist) <= fTerm;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrPlane3Cylinder3<Real>::CylinderIsCulled () const
{
    // Compute extremes of signed distance Dot(N,X)-d for points on the
    // cylinder.  These are
    //   min = (Dot(N,C)-d) - r*sqrt(1-Dot(N,W)^2) - (h/2)*|Dot(N,W)|
    //   max = (Dot(N,C)-d) + r*sqrt(1-Dot(N,W)^2) + (h/2)*|Dot(N,W)|
    Real fSDist = m_pkPlane->DistanceTo(m_pkCylinder->Segment.Origin);
    Real fAbsNdW = Math<Real>::FAbs(m_pkPlane->Normal.Dot(
        m_pkCylinder->Segment.Direction));
    Real fRoot = Math<Real>::Sqrt(Math<Real>::FAbs((Real)1.0
        - fAbsNdW*fAbsNdW));
    Real fTerm = m_pkCylinder->Radius*fRoot +
        ((Real)0.5)*m_pkCylinder->Height*fAbsNdW;

    // culling occurs if and only if max <= 0
    return fSDist + fTerm <= (Real)0.0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrPlane3Cylinder3<float>;

template WM4_FOUNDATION_ITEM
class IntrPlane3Cylinder3<double>;
//----------------------------------------------------------------------------
}
