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
#include "Wm4IntrPlane3Sphere3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrPlane3Sphere3<Real>::IntrPlane3Sphere3 (const Plane3<Real>& rkPlane,
    const Sphere3<Real>& rkSphere)
    :
    m_pkPlane(&rkPlane),
    m_pkSphere(&rkSphere)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Plane3<Real>& IntrPlane3Sphere3<Real>::GetPlane () const
{
    return *m_pkPlane;
}
//----------------------------------------------------------------------------
template <class Real>
const Sphere3<Real>& IntrPlane3Sphere3<Real>::GetSphere () const
{
    return *m_pkSphere;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrPlane3Sphere3<Real>::Test ()
{
    Real fSignedDistance = m_pkPlane->DistanceTo(m_pkSphere->Center);
    return Math<Real>::FAbs(fSignedDistance) <= m_pkSphere->Radius;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrPlane3Sphere3<Real>::SphereIsCulled () const
{
    Real fSignedDistance = m_pkPlane->DistanceTo(m_pkSphere->Center);
    return fSignedDistance <= -m_pkSphere->Radius;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrPlane3Sphere3<float>;

template WM4_FOUNDATION_ITEM
class IntrPlane3Sphere3<double>;
//----------------------------------------------------------------------------
}
