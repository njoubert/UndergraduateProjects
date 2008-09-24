/*
	File:		VLInstantiate.cpp
	
	Purpose:	(Attempt to) instantiate templates used by functions in the
				VL library.
	
	Author:		Andrew Willmott
*/

#include "cl\Array.cpp"

// Instantiate float and double arrays, if possible.
// (Unfortunately, many C++ compilers don't yet support the
// ANSI syntax for doing this. It's not a disaster if they
// don't, as, unless the compiler is really broken, it
// should automatically instantiate the templates at
// compile time.

#ifdef CL_SGI_INST
#pragma instantiate Array<Float>
#pragma instantiate Array<Double>

#pragma instantiate Array<Array<Float> >
#pragma instantiate Array<Array<Double> >

#include "vl\VLf.h"
#include "vl\VLd.h"

#pragma instantiate Array<SparsePaird>
#pragma instantiate Array<SparsePairf>
#pragma instantiate Array<SparseVecd>
#pragma instantiate Array<SparseVecf>

#if 1
#pragma instantiate istream &operator>>(istream&, Array<Double>&)
#pragma instantiate istream &operator>>(istream&, Array<Array<Double> >&)
#pragma instantiate istream &operator>>(istream&, Array<SparseVecd>&)
#pragma instantiate istream &operator>>(istream&, Array<Float>&)
#pragma instantiate istream &operator>>(istream&, Array<Array<Float> >&)
#pragma instantiate istream &operator>>(istream&, Array<SparseVecf>&)
#if 0
#pragma instantiate istream &operator>>(istream&, Array<SparsePaird>&)
#pragma instantiate istream &operator>>(istream&, Array<SparsePairf>&)
#endif
#endif

#elif defined(CL_TMPL_INST)

#if 0
template class Array<VL_V_REAL>;
template class Array<Array<VL_V_REAL> >;
template class Array<VL_V_SUFF(SparsePair)>;
template class Array<Array<VL_V_SUFF(SparsePair) > >;
#endif

#endif
