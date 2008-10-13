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
#include "Wm4IntrSegment3Box3.h"
#include "Wm4IntrLine3Box3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrSegment3Box3<Real>::IntrSegment3Box3 (const Segment3<Real>& rkSegment,
    const Box3<Real>& rkBox, bool bSolid)
    :
    m_pkSegment(&rkSegment),
    m_pkBox(&rkBox)
{
    m_bSolid = bSolid;
}
//----------------------------------------------------------------------------
template <class Real>
const Segment3<Real>& IntrSegment3Box3<Real>::GetSegment () const
{
    return *m_pkSegment;
}
//----------------------------------------------------------------------------
template <class Real>
const Box3<Real>& IntrSegment3Box3<Real>::GetBox () const
{
    return *m_pkBox;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrSegment3Box3<Real>::Test ()
{
    Real afAWdU[3], afADdU[3], afAWxDdU[3], fRhs;

    Vector3<Real> kDiff = m_pkSegment->Origin - m_pkBox->Center;

    afAWdU[0] = Math<Real>::FAbs(m_pkSegment->Direction.Dot(m_pkBox->Axis[0]));
    afADdU[0] = Math<Real>::FAbs(kDiff.Dot(m_pkBox->Axis[0]));
    fRhs = m_pkBox->Extent[0] + m_pkSegment->Extent*afAWdU[0];
    if (afADdU[0] > fRhs)
    {
        return false;
    }

    afAWdU[1] = Math<Real>::FAbs(m_pkSegment->Direction.Dot(m_pkBox->Axis[1]));
    afADdU[1] = Math<Real>::FAbs(kDiff.Dot(m_pkBox->Axis[1]));
    fRhs = m_pkBox->Extent[1] + m_pkSegment->Extent*afAWdU[1];
    if (afADdU[1] > fRhs)
    {
        return false;
    }

    afAWdU[2] = Math<Real>::FAbs(m_pkSegment->Direction.Dot(m_pkBox->Axis[2]));
    afADdU[2] = Math<Real>::FAbs(kDiff.Dot(m_pkBox->Axis[2]));
    fRhs = m_pkBox->Extent[2] + m_pkSegment->Extent*afAWdU[2];
    if (afADdU[2] > fRhs)
    {
        return false;
    }

    Vector3<Real> kWxD = m_pkSegment->Direction.Cross(kDiff);

    afAWxDdU[0] = Math<Real>::FAbs(kWxD.Dot(m_pkBox->Axis[0]));
    fRhs = m_pkBox->Extent[1]*afAWdU[2] + m_pkBox->Extent[2]*afAWdU[1];
    if (afAWxDdU[0] > fRhs)
    {
        return false;
    }

    afAWxDdU[1] = Math<Real>::FAbs(kWxD.Dot(m_pkBox->Axis[1]));
    fRhs = m_pkBox->Extent[0]*afAWdU[2] + m_pkBox->Extent[2]*afAWdU[0];
    if (afAWxDdU[1] > fRhs)
    {
        return false;
    }

    afAWxDdU[2] = Math<Real>::FAbs(kWxD.Dot(m_pkBox->Axis[2]));
    fRhs = m_pkBox->Extent[0]*afAWdU[1] + m_pkBox->Extent[1]*afAWdU[0];
    if (afAWxDdU[2] > fRhs)
    {
        return false;
    }

    return true;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrSegment3Box3<Real>::Find ()
{
    Real fT0 = -m_pkSegment->Extent, fT1 = m_pkSegment->Extent;
    return IntrLine3Box3<Real>::DoClipping(fT0,fT1,m_pkSegment->Origin,
        m_pkSegment->Direction,*m_pkBox,m_bSolid,m_iQuantity,m_akPoint,
        m_iIntersectionType);
}
//----------------------------------------------------------------------------
template <class Real>
int IntrSegment3Box3<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& IntrSegment3Box3<Real>::GetPoint (int i) const
{
    assert(0 <= i && i < m_iQuantity);
    return m_akPoint[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrSegment3Box3<float>;

template WM4_FOUNDATION_ITEM
class IntrSegment3Box3<double>;
//----------------------------------------------------------------------------
}
