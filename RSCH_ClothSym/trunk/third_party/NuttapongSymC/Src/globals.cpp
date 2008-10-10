// Modified from error handling code of PBRT
// Error Reporting Includes0
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "globals.h"
#include <cmath>
#include <iostream>
#include <assert.h>
using namespace std;



// Error Reporting Functions
extern void processError(const char *format, va_list args,
		const char *message, int disposition) {
#ifndef _MSC_VER
   char *errorBuf;
	vasprintf(&errorBuf, format, args);
#else
	char errorBuf[2048];
	_vsnprintf(errorBuf, sizeof(errorBuf), format, args);
#endif
	// Report error
	switch (disposition) {
	case MERROR_IGNORE:
		return;
	case MERROR_CONTINUE:
		fprintf(stderr, "%s: %s\n", message, errorBuf);
		break;
	case MERROR_ABORT:
		fprintf(stderr, "%s: %s\n", message, errorBuf);
		exit(1);
	}
#ifndef _MSC_VER
	free(errorBuf);
#endif
}


extern void Info(const char *format, ...) {
	va_list args;
	va_start(args, format);
	processError(format, args, "Notice", MERROR_CONTINUE);
	va_end(args);
}
extern void Warning(const char *format, ...) {
	va_list args;
	va_start(args, format);
	processError(format, args, "Warning", MERROR_CONTINUE);
	va_end(args);
}
extern void Error(const char *format, ...) {
	va_list args;
	va_start(args, format);
	processError(format, args, "Error", MERROR_CONTINUE);
	va_end(args);
}
extern void Severe(const char *format, ...) {
	va_list args;
	va_start(args, format);
	processError(format, args, "Fatal Error", MERROR_ABORT);
	va_end(args);
}


void  CBBox::Dump() {
	#if DIMENSION==2
	    cout<<"Bbox is "<<mins[0]<<" "<<mins[1]<<mins[2]<<" --- "<<maxs[0]<<" "<<maxs[1]<<endl;
	#endif
	#if DIMENSION==3
		cout<<"Bbox is "<<mins[0]<<" "<<mins[1]<<" "<<mins[2]<<" --- "<<maxs[0]<<" "<<maxs[1]<<" "<<maxs[2]<<endl;
	#endif
 }
 CBBox::CBBox() {
	#if DIMENSION==2
		mins[0] = mins[1] = INFINITY;
		maxs[0] = maxs[1] = -INFINITY;
	#endif
	#if DIMENSION==3
		mins[0] = mins[1] = mins[2] = INFINITY;
		maxs[0] = maxs[1] = maxs[2] = -INFINITY;
	#endif
}
CBBox& CBBox::operator = (const CBBox& b) {
	#if DIMENSION==2
		mins[0] = b.mins[0];
		mins[1] = b.mins[1];

		maxs[0] = b.maxs[0];
		maxs[1] = b.maxs[1];
	#endif
	#if DIMENSION==3
		mins[0] = b.mins[0];
		mins[1] = b.mins[1];
		mins[2] = b.mins[2];

		maxs[0] = b.maxs[0];
		maxs[1] = b.maxs[1];
		maxs[2] = b.maxs[2];
	#endif
	return *this;
}
CBBox::CBBox(const VEC3& minsi, const VEC3& maxsi) {
	mins = minsi;
	maxs = maxsi;
}

void CBBox::UnionWith(CBBox& a) {
	#if DIMENSION==2
		mins[0] = min(mins[0],a.mins[0]);
		mins[1] = min(mins[1],a.mins[1]);

		maxs[0] = max(maxs[0],a.maxs[0]);
		maxs[1] = max(maxs[1],a.maxs[1]);
	#endif
	#if DIMENSION==3
		mins[0] = min(mins[0],a.mins[0]);
		mins[1] = min(mins[1],a.mins[1]);
		mins[2] = min(mins[2],a.mins[2]);

		maxs[0] = max(maxs[0],a.maxs[0]);
		maxs[1] = max(maxs[1],a.maxs[1]);
		maxs[2] = max(maxs[2],a.maxs[2]);
	#endif
}

 bool CBBox::Inside(CBBox& a) {
	#if DIMENSION==2
		// Check if this bounding box is Inside bbox a
		return (mins[0] >= a.mins[0]) && (maxs[0] <= a.maxs[0]) && 
			   (mins[1] >= a.mins[1]) && (maxs[1] <= a.maxs[1]);								
	#endif
	#if DIMENSION==3
		// Check if this bounding box is Inside bbox a
		return (mins[0] >= a.mins[0]) && (maxs[0] <= a.maxs[0]) && 
			   (mins[1] >= a.mins[1]) && (maxs[1] <= a.maxs[1]) && 
			   (mins[2] >= a.mins[2]) && (maxs[2] <= a.maxs[2]);								
	#endif
}
 bool CBBox::insideStrictly(CBBox& a) {
	#if DIMENSION==2
		// Check if this bounding box is Inside bbox a
		return (mins[0] > a.mins[0]) && (maxs[0] < a.maxs[0]) && 
			   (mins[1] > a.mins[1]) && (maxs[1] < a.maxs[1]);			   
	#endif
	#if DIMENSION==3
		// Check if this bounding box is Inside bbox a
		return (mins[0] > a.mins[0]) && (maxs[0] < a.maxs[0]) && 
			   (mins[1] > a.mins[1]) && (maxs[1] < a.maxs[1]) && 
			   (mins[2] > a.mins[2]) && (maxs[2] < a.maxs[2]);								
	#endif
}

 bool CBBox::Intersect(CBBox& a) {
	real mmax, mmin;
	#if DIMENSION==2
		// Check if any part of this bbox intersects with the bbox a
		mmax = min(maxs[0],a.maxs[0]);
		mmin = max(mins[0],a.mins[0]);
		if (mmax < mmin) return false;
		mmax = min(maxs[1],a.maxs[1]);
		mmin = max(mins[1],a.mins[1]);
		if (mmax < mmin) return false;
		return true;
	#endif
	#if DIMENSION==3
		// Check if any part of this bbox intersects with the bbox a
		mmax = min(maxs[0],a.maxs[0]);
		mmin = max(mins[0],a.mins[0]);
		if (mmax < mmin) return false;
		mmax = min(maxs[1],a.maxs[1]);
		mmin = max(mins[1],a.mins[1]);
		if (mmax < mmin) return false;
		mmax = min(maxs[2],a.maxs[2]);
		mmin = max(mins[2],a.mins[2]);
		if (mmax < mmin) return false;
		return true;
    #endif
}

 bool CBBox::StrictlyIntersectOrContain(CBBox& b) {
	#if DIMENSION==2
		// First, must intersects
		real mmax, mmin;
		mmax = min(maxs[0],b.maxs[0]);
		mmin = max(mins[0],b.mins[0]);
		if (mmax < mmin) return false;
		mmax = min(maxs[1],b.maxs[1]);
		mmin = max(mins[1],b.mins[1]);
		if (mmax < mmin) return false;
		
		if ( SameReal(mins[0], b.maxs[0]) || SameReal(maxs[0], b.mins[0])  ||
			 SameReal(mins[1], b.maxs[1]) || SameReal(maxs[1], b.mins[1])) {
			 return false;
		}	
	#endif
	#if DIMENSION==3
		// First, must intersects
		real mmax, mmin;
		mmax = min(maxs[0],b.maxs[0]);
		mmin = max(mins[0],b.mins[0]);
		if (mmax < mmin) return false;
		mmax = min(maxs[1],b.maxs[1]);
		mmin = max(mins[1],b.mins[1]);
		if (mmax < mmin) return false;
		mmax = min(maxs[2],b.maxs[2]);
		mmin = max(mins[2],b.mins[2]);
		if (mmax < mmin) return false;
		
		if ( SameReal(mins[0], b.maxs[0]) || SameReal(maxs[0], b.mins[0])  ||
			 SameReal(mins[1], b.maxs[1]) || SameReal(maxs[1], b.mins[1])  ||
			 SameReal(mins[2], b.maxs[2]) || SameReal(maxs[2], b.mins[2])) {
			 return false;
		}
	#endif

	return true;
}

 real CBBox::Volume() {
	#if DIMENSION==2
		return (maxs[0]-mins[0])*(maxs[1]-mins[1]);
	#endif
	#if DIMENSION==3
		return (maxs[0]-mins[0])*(maxs[1]-mins[1])*(maxs[2]-mins[2]);
	#endif
}

int CBBox::MaximumExtent() {
	#if DIMENSION==2
		real ex = (maxs[0]-mins[0]);
		real ey = (maxs[1]-mins[1]);
		return (ey > ex);
	#endif
	#if DIMENSION==3
		real ex = (maxs[0]-mins[0]);
		real ey = (maxs[1]-mins[1]);
		real ez = (maxs[2]-mins[2]);
		if (ex > ey) {
			if (ex > ez) return 0; else return 2;
		} 
		if (ey > ez) return 1; else return 2;	
	#endif
}


// Random Number State
/*
   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote
        products derived from this software without specific prior written
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */

static unsigned long mt[N]; /* the array for the state vector  */
static int mti=N+1; /* mti==N+1 means mt[N] is not initialized */
// Random Number Functions
extern void Init_genrand(ulong seed) {
	mt[0]= seed & 0xffffffffUL;
	for (mti=1; mti<N; mti++) {
		mt[mti] =
		(1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti);
		/* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
		/* In the previous versions, MSBs of the seed affect   */
		/* only MSBs of the array mt[].                        */
		/* 2002/01/09 modified by Makoto Matsumoto             */
		mt[mti] &= 0xffffffffUL;
		/* for >32 bit machines */
	}
}
extern unsigned long genrand_int32(void)
{
	unsigned long y;
	static unsigned long mag01[2]={0x0UL, MATRIX_A};
	/* mag01[x] = x * MATRIX_A  for x=0,1 */

	if (mti >= N) { /* generate N words at one time */
		int kk;

		if (mti == N+1)   /* if init_genrand() has not been called, */
			Init_genrand(5489UL); /* default initial seed */

		for (kk=0;kk<N-M;kk++) {
			y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
			mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1UL];
		}
		for (;kk<N-1;kk++) {
			y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
			mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
		}
		y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
		mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

		mti = 0;
	}

	y = mt[mti++];

	/* Tempering */
	y ^= (y >> 11);
	y ^= (y << 7) & 0x9d2c5680UL;
	y ^= (y << 15) & 0xefc60000UL;
	y ^= (y >> 18);

	return y;
}
/* generates a random number on [0,1]-real-interval */
extern float genrand_real1(void)
{
	return genrand_int32()*((float)1.0/(float)4294967295.0);
	/* divided by 2^32-1 */
}
/* generates a random number on [0,1)-real-interval */
extern float genrand_real2(void)
{
	return genrand_int32()*((float)1.0/(float)4294967296.0);
	/* divided by 2^32 */
}
extern  float RandomFloat() {
	return genrand_real2();
}

extern  unsigned long RandomUInt() {
	return genrand_int32();
}

