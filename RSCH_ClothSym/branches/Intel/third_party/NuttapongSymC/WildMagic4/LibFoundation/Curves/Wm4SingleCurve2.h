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

#ifndef WM4SINGLECURVE2_H
#define WM4SINGLECURVE2_H

#include "Wm4FoundationLIB.h"
#include "Wm4Curve2.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM SingleCurve2 : public Curve2<Real>
{
public:
    // abstract base class
    SingleCurve2 (Real fTMin, Real fTMax);

    // length-from-time and time-from-length
    virtual Real GetLength (Real fT0, Real fT1) const;
    virtual Real GetTime (Real fLength, int iIterations = 32,
        Real fTolerance = (Real)1e-06) const;

protected:
    using Curve2<Real>::m_fTMin;
    using Curve2<Real>::m_fTMax;
    using Curve2<Real>::GetTotalLength;

    static Real GetSpeedWithData (Real fTime, void* pvData);
};

typedef SingleCurve2<float> SingleCurve2f;
typedef SingleCurve2<double> SingleCurve2d;

}

#endif
