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
// Version: 4.0.0 (2006/06/28)

//----------------------------------------------------------------------------
template <class Real>
Circle3<Real>::Circle3 ()
{
    // uninitialized
}
//----------------------------------------------------------------------------
template <class Real>
Circle3<Real>::Circle3 (const Vector3<Real>& rkCenter,
    const Vector3<Real>& rkU, const Vector3<Real>& rkV,
    const Vector3<Real>& rkN, Real fRadius)
    :
    Center(rkCenter),
    U(rkU),
    V(rkV),
    N(rkN)
{
    Radius = fRadius;
}
//----------------------------------------------------------------------------