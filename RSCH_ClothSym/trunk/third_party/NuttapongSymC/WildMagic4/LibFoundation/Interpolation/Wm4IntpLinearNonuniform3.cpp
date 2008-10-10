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
#include "Wm4IntpLinearNonuniform3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntpLinearNonuniform3<Real>::IntpLinearNonuniform3 (
    const Delaunay3<Real>& rkDT, Real* afF, bool bOwner)
    :
    m_pkDT(&rkDT)
{
    assert(afF);
    m_afF = afF;
    m_bOwner = bOwner;
}
//----------------------------------------------------------------------------
template <class Real>
IntpLinearNonuniform3<Real>::~IntpLinearNonuniform3 ()
{
    if (m_bOwner)
    {
        WM4_DELETE[] m_afF;
    }
}
//----------------------------------------------------------------------------
template <class Real>
bool IntpLinearNonuniform3<Real>::Evaluate (const Vector3<Real>& rkP,
    Real& rfF)
{
    int i = m_pkDT->GetContainingTetrahedron(rkP);
    if (i == -1)
    {
        return false;
    }

    // Get the barycentric coordinates of P with respect to the tetrahedron,
    // P = b0*V0 + b1*V1 + b2*V2 + b3*V3, where b0+b1+b2+b3 = 1.
    Real afBary[4];
    bool bValid = m_pkDT->GetBarycentricSet(i,rkP,afBary);
    assert(bValid);
    if (!bValid)
    {
        return false;
    }

    // get the vertex indices for look up into the function-value array
    int aiIndex[4];
    bValid = m_pkDT->GetIndexSet(i,aiIndex);
    assert(bValid);
    if (!bValid)
    {
        return false;
    }

    // result is a barycentric combination of function values
    rfF = afBary[0]*m_afF[aiIndex[0]] + afBary[1]*m_afF[aiIndex[1]] +
        afBary[2]*m_afF[aiIndex[2]] + afBary[3]*m_afF[aiIndex[3]];

    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntpLinearNonuniform3<float>;

template WM4_FOUNDATION_ITEM
class IntpLinearNonuniform3<double>;
//----------------------------------------------------------------------------
}
