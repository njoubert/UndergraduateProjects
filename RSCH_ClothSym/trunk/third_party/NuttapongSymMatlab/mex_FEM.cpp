 /*
* =============================================================
  mex_FEM 

  Do linear element finite element simulation
  by Nuttapong Chentanez
* =============================================================
*/
#define FUNCTIONNAME mex_FEM.cpp

#define MYINT unsigned int
#define MATMYINTNAME  "uint32"


#include "mex.h"
#include <algorithm>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <fstream>
using namespace std;
#include "common.h"
#include "cvector.h"

#ifdef __GNUC__
	#include <ext/hash_map>
	using namespace __gnu_cxx;
#else
	#include <hash_map>
#endif
#include <algorithm>
using namespace std;
#define MODE					prhs[0]

////// MODE 0, Compute stiffness, mass matrix, for Linear Cauchy Strain, Linear-Isotropic stress-strain relationship 
// INPUT
// Assume isotropic
#define M0_NODEPOS				prhs[1]
#define M0_TET2NODE				prhs[2]
#define M0_LAMBDA				prhs[3]
#define M0_MU					prhs[4]
#define M0_RHO					prhs[5]
// OUTPUT
#define M0_KMAT					plhs[0]
#define M0_MMAT					plhs[1]
#define M0_BARYMAT				plhs[2]

////// MODE 1, Compute Matrix H, convert from solid boundary velocity to fluid boundary face flux
// INPUT
#define M1_SOLIDNODEPOS				prhs[1]
#define M1_SOLIDSURFFACE2NODE		prhs[2]
#define M1_FLUIDNODE2SOLIDFACE		prhs[3]
#define M1_FLUIDALLFACE2NODE        prhs[4]
#define M1_FLUIDNODEPOS				prhs[5]

// OUTPUT
#define M1_H						plhs[0]

////// MODE 2, Compute Matrix J (BROKEN!!!!!!!!!), convert from fluid pressure to solid boundary velocity
#define M2_SOLIDNODEPOS				prhs[1]
#define M2_SOLIDSURFFACE2NODE		prhs[2]
#define M2_FLUIDNODE2SOLIDFACE		prhs[3]
#define M2_FLUIDALLFACE2NODE		prhs[4]
#define M2_FLUIDNODEPOS				prhs[5]
#define M2_FLUIDTET2FACE			prhs[6]
#define M2_FLUIDTET2TET				prhs[7]

// OUTPUT
#define M2_J						plhs[0]

////// MODE 3, Compute Matrix D, compute divergence, by summing face flux based on tet2node
#define M3_NUMFACES					prhs[1]
#define M3_FLUIDTET2FACE			prhs[2]

// OUTPUT
#define M3_D						plhs[0]

////// MODE4, Compute stiffness and mass matrix with non-linear term
// INPUT
// Assume isotropic
#define M4_RESTPOS				prhs[1]
#define M4_TET2NODE				prhs[2]
#define M4_LAMBDA				prhs[3]
#define M4_MU					prhs[4]
#define M4_RHO					prhs[5]
#define M4_DISPLACE				prhs[6]
#define M4_BARYMAT				prhs[7]
#define M4_CURPOS				prhs[8]

// OUTPUT
#define M4_KMAT					plhs[0]
#define M4_MMAT					plhs[1]
#define M4_FORCE				plhs[2]

////// MODE 5, Set fluid boundary node velocity to solid boundary node velocity, for visualization
// INPUT
#define M5_SOLIDNODEPOS				prhs[1]
#define M5_SOLIDSURFFACE2NODE		prhs[2]
#define M5_FLUIDNODE2SOLIDFACE		prhs[3]
#define M5_FLUIDALLFACE2NODE        prhs[4]
#define M5_FLUIDNODEPOS				prhs[5]
#define M5_FLUIDNODEVEL				prhs[6]
#define M5_SOLIDNODEVEL				prhs[7]

////// MODE 6, Compute Matrix J, convert from fluid pressure to solid boundary velocity
#define M6_SOLIDNODEPOS				prhs[1]
#define M6_SOLIDSURFFACE2NODE		prhs[2]
#define M6_FLUIDNODE2SOLIDFACE		prhs[3]
#define M6_FLUIDALLFACE2NODE		prhs[4]
#define M6_FLUIDNODEPOS				prhs[5]
#define M6_FLUIDTET2FACE			prhs[6]
#define M6_FLUIDTET2TET				prhs[7]

// OUTPUT
#define M6_J						plhs[0]


////// MODE 1, Compute correct Matrix H, convert from solid boundary velocity to fluid boundary face flux
// INPUT
#define M7_SOLIDNODEPOS				prhs[1]
#define M7_SOLIDSURFFACE2NODE		prhs[2]
#define M7_FLUIDNODE2SOLIDFACE		prhs[3]
#define M7_FLUIDALLFACE2NODE        prhs[4]
#define M7_FLUIDNODEPOS				prhs[5]

// OUTPUT
#define M7_H						plhs[0]

////// MODE 8, Compute 'correct' Matrix D, compute divergence, by summing face flux based on tet2node, 
////// ***** ASSUME THAT THE FACE WE USES ARE BOUNDARY FACES **** ////
////// ****** NOT QUITE GOOD BUT GOOD ENOUGH FOR NOWW!!!!! FIXXXXXX **************
#define M8_NUMFACES					prhs[1]
#define M8_FLUIDTET2FACE			prhs[2]
#define M8_FACEAREA					prhs[3]

// OUTPUT
#define M8_D						plhs[0]
// Geometric Tools, Inc.
// http://www.geometrictools.com
// Copyright (c) 1998-2006.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement
//     http://www.geometrictools.com/License/WildMagic3License.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.

//----------------------------------------------------------------------------
const double ZERO_TOLERANCE = 1e-6;

inline void Inverse3x3(double* inMat, double* outMat) {
    // Invert a 3x3 using cofactors.  This is faster than using a generic
    // Gaussian elimination because of the loop overhead of such a method.

    outMat[0] = inMat[4]*inMat[8] - inMat[5]*inMat[7];
    outMat[1] = inMat[2]*inMat[7] - inMat[1]*inMat[8];
    outMat[2] = inMat[1]*inMat[5] - inMat[2]*inMat[4];
    outMat[3] = inMat[5]*inMat[6] - inMat[3]*inMat[8];
    outMat[4] = inMat[0]*inMat[8] - inMat[2]*inMat[6];
    outMat[5] = inMat[2]*inMat[3] - inMat[0]*inMat[5];
    outMat[6] = inMat[3]*inMat[7] - inMat[4]*inMat[6];
    outMat[7] = inMat[1]*inMat[6] - inMat[0]*inMat[7];
    outMat[8] = inMat[0]*inMat[4] - inMat[1]*inMat[3];

    double fDet = inMat[0]*outMat[0] + inMat[1]*outMat[3]+
        inMat[2]*outMat[6];

    if (fabs(fDet) <= ZERO_TOLERANCE)
    {
		outMat[0] = 
		outMat[1] = 
		outMat[2] = 
		outMat[3] = 
		outMat[4] = 
		outMat[5] = 
		outMat[6] = 
		outMat[7] = 0.0;
        return ;
    }

	double invDet = 1.0/fDet;
	for (int i = 0; i < 9; i++)
    outMat[i]*=invDet;
}

inline void Inverse4x4(double* inMat, double* outMat) {
    double fA0 = inMat[ 0]*inMat[ 5] - inMat[ 1]*inMat[ 4];
    double fA1 = inMat[ 0]*inMat[ 6] - inMat[ 2]*inMat[ 4];
    double fA2 = inMat[ 0]*inMat[ 7] - inMat[ 3]*inMat[ 4];
    double fA3 = inMat[ 1]*inMat[ 6] - inMat[ 2]*inMat[ 5];
    double fA4 = inMat[ 1]*inMat[ 7] - inMat[ 3]*inMat[ 5];
    double fA5 = inMat[ 2]*inMat[ 7] - inMat[ 3]*inMat[ 6];
    double fB0 = inMat[ 8]*inMat[13] - inMat[ 9]*inMat[12];
    double fB1 = inMat[ 8]*inMat[14] - inMat[10]*inMat[12];
    double fB2 = inMat[ 8]*inMat[15] - inMat[11]*inMat[12];
    double fB3 = inMat[ 9]*inMat[14] - inMat[10]*inMat[13];
    double fB4 = inMat[ 9]*inMat[15] - inMat[11]*inMat[13];
    double fB5 = inMat[10]*inMat[15] - inMat[11]*inMat[14];

    double fDet = fA0*fB5-fA1*fB4+fA2*fB3+fA3*fB2-fA4*fB1+fA5*fB0;
    if (fabs(fDet)< 1e-8) {
		memset(outMat, 0, sizeof(double)*16);
	}
    double fInvDet = ((double)1.0)/fDet;

    outMat[0] = (+ inMat[ 5]*fB5 - inMat[ 6]*fB4 + inMat[ 7]*fB3)*fInvDet;
    outMat[4] = (- inMat[ 4]*fB5 + inMat[ 6]*fB2 - inMat[ 7]*fB1)*fInvDet;
    outMat[8] = (+ inMat[ 4]*fB4 - inMat[ 5]*fB2 + inMat[ 7]*fB0)*fInvDet;
    outMat[12] = (- inMat[ 4]*fB3 + inMat[ 5]*fB1 - inMat[ 6]*fB0)*fInvDet;
    outMat[1] = (- inMat[ 1]*fB5 + inMat[ 2]*fB4 - inMat[ 3]*fB3)*fInvDet;
    outMat[5] = (+ inMat[ 0]*fB5 - inMat[ 2]*fB2 + inMat[ 3]*fB1)*fInvDet;
    outMat[9] = (- inMat[ 0]*fB4 + inMat[ 1]*fB2 - inMat[ 3]*fB0)*fInvDet;
    outMat[13] = (+ inMat[ 0]*fB3 - inMat[ 1]*fB1 + inMat[ 2]*fB0)*fInvDet;
    outMat[2] = (+ inMat[13]*fA5 - inMat[14]*fA4 + inMat[15]*fA3)*fInvDet;
    outMat[6] = (- inMat[12]*fA5 + inMat[14]*fA2 - inMat[15]*fA1)*fInvDet;
    outMat[10] = (+ inMat[12]*fA4 - inMat[13]*fA2 + inMat[15]*fA0)*fInvDet;
    outMat[14] = (- inMat[12]*fA3 + inMat[13]*fA1 - inMat[14]*fA0)*fInvDet;
    outMat[3] = (- inMat[ 9]*fA5 + inMat[10]*fA4 - inMat[11]*fA3)*fInvDet;
    outMat[7] = (+ inMat[ 8]*fA5 - inMat[10]*fA2 + inMat[11]*fA1)*fInvDet;
    outMat[11] = (- inMat[ 8]*fA4 + inMat[ 9]*fA2 - inMat[11]*fA0)*fInvDet;
    outMat[15] = (+ inMat[ 8]*fA3 - inMat[ 9]*fA1 + inMat[10]*fA0)*fInvDet;
}
double tmpB_Tet[16];
inline void buildBaryMat(double* v0, double* v1, double* v2, double* v3, double* bary_Tet) {
	tmpB_Tet[0] = v0[0];
	tmpB_Tet[1] = v1[0];
	tmpB_Tet[2] = v2[0];
	tmpB_Tet[3] = v3[0];

	tmpB_Tet[4] = v0[1];
	tmpB_Tet[5] = v1[1];
	tmpB_Tet[6] = v2[1];
	tmpB_Tet[7] = v3[1];

	tmpB_Tet[8] = v0[2];
	tmpB_Tet[9] = v1[2];
	tmpB_Tet[10] = v2[2];
	tmpB_Tet[11] = v3[2];

	tmpB_Tet[12] = 1;
	tmpB_Tet[13] = 1;
	tmpB_Tet[14] = 1;
	tmpB_Tet[15] = 1;

	Inverse4x4(tmpB_Tet, bary_Tet);
}
inline void Tridiagonal3(double* m_kMat, double* m_afDiag, double* m_afSubd, bool& m_bIsRotation)
{
    double fM00 = m_kMat[0];
    double fM01 = m_kMat[1];
    double fM02 = m_kMat[2];
    double fM11 = m_kMat[4];
    double fM12 = m_kMat[5];
    double fM22 = m_kMat[8];

    m_afDiag[0] = fM00;
    m_afSubd[2] = (double)0.0;
    if (fabs(fM02) > ZERO_TOLERANCE)
    {
        double fLength = sqrt(fM01*fM01+fM02*fM02);
        double fInvLength = ((double)1.0)/fLength;
        fM01 *= fInvLength;
        fM02 *= fInvLength;
        double fQ = ((double)2.0)*fM01*fM12+fM02*(fM22-fM11);
        m_afDiag[1] = fM11+fM02*fQ;
        m_afDiag[2] = fM22-fM02*fQ;
        m_afSubd[0] = fLength;
        m_afSubd[1] = fM12-fM01*fQ;
        m_kMat[0] = (double)1.0;
        m_kMat[1] = (double)0.0;
        m_kMat[2] = (double)0.0;
        m_kMat[3] = (double)0.0;
        m_kMat[4] = fM01;
        m_kMat[5] = fM02;
        m_kMat[6] = (double)0.0;
        m_kMat[7] = fM02;
        m_kMat[8] = -fM01;
        m_bIsRotation = false;
    }
    else
    {
        m_afDiag[1] = fM11;
        m_afDiag[2] = fM22;
        m_afSubd[0] = fM01;
        m_afSubd[1] = fM12;
        m_kMat[0] = (double)1.0;
        m_kMat[1] = (double)0.0;
        m_kMat[2] = (double)0.0;
        m_kMat[3] = (double)0.0;
        m_kMat[4] = (double)1.0;
        m_kMat[5] = (double)0.0;
        m_kMat[6] = (double)0.0;
        m_kMat[7] = (double)0.0;
        m_kMat[8] = (double)1.0;
        m_bIsRotation = true;
    }
}

inline bool QLAlgorithm (double* m_kMat, double* m_afDiag, double* m_afSubd, bool& m_bIsRotation)
{
	const int m_iSize = 3;
    const int iMaxIter = 32;

    for (int i0 = 0; i0 < m_iSize; i0++)
    {
        int i1;
        for (i1 = 0; i1 < iMaxIter; i1++)
        {
            int i2;
            for (i2 = i0; i2 <= m_iSize-2; i2++)
            {
                double fTmp = fabs(m_afDiag[i2]) +
                    fabs(m_afDiag[i2+1]);
                if ( fabs(m_afSubd[i2]) + fTmp == fTmp )
                    break;
            }
            if (i2 == i0)
            {
                break;
            }

            double fG = (m_afDiag[i0+1] - m_afDiag[i0])/(((double)2.0) *
                m_afSubd[i0]);
            double fR = sqrt(fG*fG+(double)1.0);
            if (fG < (double)0.0)
            {
                fG = m_afDiag[i2]-m_afDiag[i0]+m_afSubd[i0]/(fG-fR);
            }
            else
            {
                fG = m_afDiag[i2]-m_afDiag[i0]+m_afSubd[i0]/(fG+fR);
            }
            double fSin = (double)1.0, fCos = (double)1.0, fP = (double)0.0;
            for (int i3 = i2-1; i3 >= i0; i3--)
            {
                double fF = fSin*m_afSubd[i3];
                double fB = fCos*m_afSubd[i3];
                if (fabs(fF) >= fabs(fG))
                {
                    fCos = fG/fF;
                    fR = sqrt(fCos*fCos+(double)1.0);
                    m_afSubd[i3+1] = fF*fR;
                    fSin = ((double)1.0)/fR;
                    fCos *= fSin;
                }
                else
                {
                    fSin = fF/fG;
                    fR = sqrt(fSin*fSin+(double)1.0);
                    m_afSubd[i3+1] = fG*fR;
                    fCos = ((double)1.0)/fR;
                    fSin *= fCos;
                }
                fG = m_afDiag[i3+1]-fP;
                fR = (m_afDiag[i3]-fG)*fSin+((double)2.0)*fB*fCos;
                fP = fSin*fR;
                m_afDiag[i3+1] = fG+fP;
                fG = fCos*fR-fB;

				int i4x3 = 0;
                for (int i4 = 0; i4 < m_iSize; i4++)
                {
                    fF = m_kMat[i4x3+(i3+1)];
                    m_kMat[i4x3+i3+1] = fSin*m_kMat[i4x3+i3]+fCos*fF;
                    m_kMat[i4x3+i3] = fCos*m_kMat[i4x3+i3]-fSin*fF;
					i4x3 += 3;
                }
            }
            m_afDiag[i0] -= fP;
            m_afSubd[i0] = fG;
            m_afSubd[i2] = (double)0.0;
        }
        if (i1 == iMaxIter)
        {
            return false;
        }
    }

    return true;
}


inline void GuaranteeRotation (double* m_kMat, double* m_afDiag, double* m_afSubd, bool& m_bIsRotation)
{
	const int m_iSize = 3;
    if (!m_bIsRotation)
    {
		int iRowx3 = 0;
        // change sign on the first column
        for (int iRow = 0; iRow < m_iSize; iRow++)
        {
            m_kMat[iRowx3+0] = -m_kMat[iRowx3+0];
			iRowx3 += 3;
        }
    }
}

void EigenDecomposition(double* m_kMat, double* m_afDiag)
{
	double m_afSubd[3];
	bool m_bIsRotation = false;
    Tridiagonal3(m_kMat, m_afDiag, m_afSubd, m_bIsRotation);
    QLAlgorithm(m_kMat, m_afDiag, m_afSubd, m_bIsRotation);
    GuaranteeRotation(m_kMat, m_afDiag, m_afSubd, m_bIsRotation);
}


void SVD(double* F, double* U, double* V, double* Fhat) {
	// V = Ft*F;
/*	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			*(V++) = F[i]*F[j] + F[3+i]*F[3+j] + F[6+i]*F[6+j];
		}
	}
	V -= 9;
	EigenDecomposition(V, Fhat);*/
}
template<class T> 
T* checkType(const mxArray*& ar, char* arname, char* type) {
	if (strcmp(mxGetClassName(ar), type) != 0) {
		mexPrintf("%s is the wrong data type.", arname);    
		mexErrMsgTxt("wrong data type.");    
	}
	return (T*)mxGetPr(ar);
}

template<class T> 
T* check2DimAndType(const mxArray*& ar, char* arname, MYINT d1, MYINT d2, char* type) {
	if (strcmp(mxGetClassName(ar), type) != 0) {
		mexPrintf("%s is the wrong data type.", arname);    
		mexErrMsgTxt("wrong data type.");    
	}
	if (mxGetM(ar) != d1) {
		mexPrintf("%s is in wrong first dimension, should be %d", arname, d1);    
		mexErrMsgTxt("wrong first dimension");      
	}
	if (mxGetN(ar) != d2) {
		mexPrintf("%s is in wrong second dimension, should be %d", arname, d2);    
		mexErrMsgTxt("wrong second dimension");     
	}
	return (T*)mxGetPr(ar);
}
template<class T>
T* check1DimAndType(const mxArray*& ar, char* arname, MYINT d1, char* type) {
	if (strcmp(mxGetClassName(ar), type) != 0) {
		mexPrintf("%s is the wrong data type.", arname);    
		mexErrMsgTxt("wrong data type.");    
	}
	if (mxGetM(ar) != d1) {
		mexPrintf("%s is in wrong first dimension, should be %d", arname, d1);    
		mexErrMsgTxt("wrong first dimension");    
	}
	// return d2
	return (T*)mxGetPr(ar);
}
template<class T>
T* check1DimAndType2(const mxArray*& ar, char* arname, MYINT d2, char* type) {
	if (strcmp(mxGetClassName(ar), type) != 0) {
		mexPrintf("%s is the wrong data type.", arname);    
		mexErrMsgTxt("wrong data type.");    
	}
	if (mxGetN(ar) != d2) {
		mexPrintf("%s is in wrong 2nd dimension, should be %d", arname, d2);    
		mexErrMsgTxt("wrong 2nd dimension");    
	}
	// return d2
	return (T*)mxGetPr(ar);
}



#define ONESIXTH 0.166666666666666666666666666666666666666666666666666666667
// Find tets volume
double TetVol(double* v0, double* v1, double* v2, double* v3) {
	
	double xot = v0[0] - v1[0];
    double yot = v0[1] - v1[1];
    double zot = v0[2] - v1[2];
    double xdt = v2[0] - v1[0];
    double ydt = v2[1] - v1[1];
    double zdt = v2[2] - v1[2];
    double xft = v3[0] - v1[0];
    double yft = v3[1] - v1[1];
    double zft = v3[2] - v1[2];
    return fabs(ONESIXTH * (xot * (ydt * zft - yft * zdt) +
                              yot * (zdt * xft - zft * xdt) +
                              zot * (xdt * yft - xft * ydt)));    
}
class CSparseElement{
	public:
	CSparseElement(MYINT row, double val) {

		r = row;
		v = val;
	}
	int r;
	double v;
};
class CSparseElementCompare{
	public:
	bool operator() (const CSparseElement& a, const CSparseElement& b) {

		return a.r < b.r;
	}
};
class CSparseMatrixHelper{
public:
	MYINT numRows, numCols;
	vector<CSparseElement>* columns;
	
	CSparseMatrixHelper(MYINT r, MYINT c) {
		numRows = r;
		numCols = c;
		
		columns = new vector<CSparseElement>[numCols];
		
	}

	void CreateMatlabMat(mxArray *& outMat) {
		int* jp = new int[numCols + 1];
		MYINT totalIndex = 0;
		vector<CSparseElement> allElements;
		for (MYINT i = 0; i < numCols; i++) {
			//mexPrintf("%d\n",i);
			vector<CSparseElement>* col = &columns[i];

			jp[i] = totalIndex;
			if (col->size() > 0) {
				sort(col->begin(), col->end(), CSparseElementCompare());
				MYINT rr;
				MYINT t = 0;
				MYINT numEntry = col->size();
				while (t < numEntry) {
					double sum = 0.0;
					rr = col->at(t).r;
					while ((t < numEntry) && (col->at(t).r == rr)) {
						sum += col->at(t).v;
						t++;
					}

					allElements.push_back(CSparseElement(rr, sum));
					totalIndex++;
				}
			}

		}
		jp[numCols] = totalIndex;
		
		// Create actual sparse matrix structure
		outMat = mxCreateSparse(numRows, numCols, allElements.size(),(mxComplexity) 0);

		double* sp  = mxGetPr(outMat);
		int* ip = mxGetIr(outMat);
    	int* ojp = mxGetJc(outMat);		
		memcpy(ojp, jp, sizeof(int)*(numCols+1));
		for (MYINT i = 0; i < allElements.size(); i++) {
			*(sp++) = allElements[i].v;
			*(ip++) = allElements[i].r;
		}
	}

	~CSparseMatrixHelper() {
		delete [] columns;
	}
};

#define delta(a, b) (((a) == (b)) ? 1.0f : 0.0f) 
void getProjectedBaryCentric(real* p0, real* p1, real* p2, real* p, real& fS, real& fT, real& fOther)
{
	/********************
		Modified from a code from www.geometrictools.com
		for computing distance to triangle
	************************/
    CVector3 kDiff(p0, p);
    CVector3 kEdge0(p1, p0);
    CVector3 kEdge1(p2, p0);
    real fA00 = kEdge0.norm2sq();
    real fA01 = Dot(kEdge0, kEdge1);
    real fA11 = kEdge1.norm2sq();
    real fB0 = Dot(kDiff, kEdge0);
    real fB1 = Dot(kDiff, kEdge1);
    //real fC = kDiff.norm2sq();
    real fDet = fabs(fA00*fA11-fA01*fA01);
    fS = fA01*fB1-fA11*fB0;
    fT = fA01*fB0-fA00*fB1;
  
    // minimum at interior point
    real fInvDet = ((real)1.0)/fDet;
    fS *= fInvDet;
    fT *= fInvDet;

	fOther = 1.0-fS-fT;

	if (fS < 0) fS = 0.0; 
	if (fT < 0) fT = 0.0;
	if (fOther < 0) fOther = 0.0;
	double sums = 1/(fS+fT+fOther);
	fS *= sums;
	fT *= sums;
	fOther *= sums;
}

void mexMode1(int nlhs, mxArray *plhs[],
					 int nrhs, const mxArray *prhs[]) {
	MYINT numSolidNodes = mxGetN(M1_SOLIDNODEPOS);
	MYINT numFluidNodes = mxGetN(M1_FLUIDNODEPOS);

	double* solidNodePos = check2DimAndType<double>(M1_SOLIDNODEPOS, "M1_SOLIDNODEPOS", 3, numSolidNodes, "double");	 
	double* fluidNodePos = check2DimAndType<double>(M1_FLUIDNODEPOS, "M1_FLUIDNODEPOS", 3, numFluidNodes, "double");	 

	MYINT numFluidAllFaces = mxGetN(M1_FLUIDALLFACE2NODE);
	MYINT numSolidSurfFaces = mxGetN(M1_SOLIDSURFFACE2NODE);

	MYINT* fluidAllFace2Node = check2DimAndType<MYINT>(M1_FLUIDALLFACE2NODE, "M1_FLUIDALLFACE2NODE", 3, numFluidAllFaces, "uint32");
	MYINT* solidSurfFace2Node = check2DimAndType<MYINT>(M1_SOLIDSURFFACE2NODE, "M1_SOLIDSURFFACE2NODE", 3, numSolidSurfFaces, "uint32");
	int* fluidNode2SolidFace = check2DimAndType<int>(M1_FLUIDNODE2SOLIDFACE, "M1_FLUIDNODE2SOLIDFACE", 1, numFluidNodes, "int32");

	MYINT* fface2node = fluidAllFace2Node;
	CSparseMatrixHelper H(numFluidAllFaces, numSolidNodes*3);
	double alpha[3][3];
	for (MYINT f = 0; f < numFluidAllFaces; f++) {
		if (((fluidNode2SolidFace[fface2node[0]-1] > 0) && (fluidNode2SolidFace[fface2node[0]-1] <= numSolidSurfFaces)) &&
			((fluidNode2SolidFace[fface2node[1]-1] > 0) && (fluidNode2SolidFace[fface2node[1]-1] <= numSolidSurfFaces)) &&
			((fluidNode2SolidFace[fface2node[2]-1] > 0) && (fluidNode2SolidFace[fface2node[2]-1] <= numSolidSurfFaces))) {
		
			// This is a boundary face
			// Find normal for this face
			double* p[3] = {&fluidNodePos[(fface2node[0]-1)*3], &fluidNodePos[(fface2node[1]-1)*3], &fluidNodePos[(fface2node[2]-1)*3]};
			CVector3 t0(p[1],p[0]);
			CVector3 t1(p[2],p[0]);
			CVector3 n = Cross(t0,t1);
			// Flux = 1/3*(v_0 + v_1 + v_2).(n/2) 
			// n is unnormalized normal = normalized normal * area * 2


			//double area = n.norm2();
			//n /= area;
			//area *= 0.5;
	
			// Now, compute barycentric coordinates for the 3 fluid boundary vertices with respect to triangles on the 
			// solid domain
			// Also, add coefficients to various rows
			for (MYINT i = 0; i < 3; i++) {
				MYINT* solidFaceStart = &solidSurfFace2Node[(fluidNode2SolidFace[fface2node[i]-1]-1)*3];
				// Beware!!
				// alpha[i][1] = s
				// alpha[i][2] = t
				// alpha[i][0] = 1-s-t
				getProjectedBaryCentric(&solidNodePos[(solidFaceStart[0]-1)*3], 
									    &solidNodePos[(solidFaceStart[1]-1)*3], 
										&solidNodePos[(solidFaceStart[2]-1)*3], p[i], alpha[i][1], alpha[i][2],alpha[i][0]);

				// Add coefficients to various columns 
				// Loop for x y z
				for (MYINT j = 0; j < 3; j++) {
					H.columns[(solidFaceStart[0] - 1)*3 + j].push_back(CSparseElement(f, ONESIXTH*n[j]*alpha[i][0]));
					H.columns[(solidFaceStart[1] - 1)*3 + j].push_back(CSparseElement(f, ONESIXTH*n[j]*alpha[i][1]));
					H.columns[(solidFaceStart[2] - 1)*3 + j].push_back(CSparseElement(f, ONESIXTH*n[j]*alpha[i][2]));
				}
			}

		}
		fface2node += 3;
	}
	H.CreateMatlabMat(M1_H);
}

void mexMode2(int nlhs, mxArray *plhs[],
					 int nrhs, const mxArray *prhs[]) {
	MYINT numSolidNodes = mxGetN(M2_SOLIDNODEPOS);
	MYINT numFluidNodes = mxGetN(M2_FLUIDNODEPOS);
	MYINT numFluidTets = mxGetN(M2_FLUIDTET2FACE);

	double* solidNodePos = check2DimAndType<double>(M2_SOLIDNODEPOS, "M2_SOLIDNODEPOS", 3, numSolidNodes, "double");	 
	double* fluidNodePos = check2DimAndType<double>(M2_FLUIDNODEPOS, "M2_FLUIDNODEPOS", 3, numFluidNodes, "double");	 

	MYINT numFluidAllFaces = mxGetN(M2_FLUIDALLFACE2NODE);
	MYINT numSolidSurfFaces = mxGetN(M2_SOLIDSURFFACE2NODE);

	MYINT* fluidAllFace2Node = check2DimAndType<MYINT>(M2_FLUIDALLFACE2NODE, "M2_FLUIDALLFACE2NODE", 3, numFluidAllFaces, "uint32");
	MYINT* solidSurfFace2Node = check2DimAndType<MYINT>(M2_SOLIDSURFFACE2NODE, "M2_SOLIDSURFFACE2NODE", 3, numSolidSurfFaces, "uint32");
	int* fluidNode2SolidFace = check2DimAndType<int>(M2_FLUIDNODE2SOLIDFACE, "M2_FLUIDNODE2SOLIDFACE", 1, numFluidNodes, "int32");
	MYINT* fluidTet2Face = check2DimAndType<MYINT>(M2_FLUIDTET2FACE, "M2_FLUIDTET2FACE", 4, numFluidTets, "uint32");
	MYINT* fluidTet2Tet = check2DimAndType<MYINT>(M2_FLUIDTET2TET, "M2_FLUIDTET2TET", 4, numFluidTets, "uint32");

	MYINT* ftet2tet = fluidTet2Tet;
	MYINT* ftet2face = fluidTet2Face;
	CSparseMatrixHelper J(numSolidNodes*3, numFluidTets);

	for (MYINT i = 0; i < numFluidTets; i++) {
		//mexPrintf("i %d\n",i);
		vector<CSparseElement>* JCol = &J.columns[i];
		for (MYINT j = 0; j < 4; j++) {
			//mexPrintf("  j %d\n",j);
			if (ftet2tet[j] == 0) {
				// Boundary!
				MYINT faceNum = ftet2face[j]-1;
				MYINT* f2n = &fluidAllFace2Node[faceNum*3];
				double* p[3] = {&fluidNodePos[(f2n[0]-1)*3], &fluidNodePos[(f2n[1]-1)*3],&fluidNodePos[(f2n[2]-1)*3]};
				CVector3 t0(p[1],p[0]);
				CVector3 t1(p[2],p[0]);
				CVector3 n = Cross(t0,t1);
				n *= ONESIXTH; // 1/6
				// One third of the Area weighted normal 
				// Multiplied by pressure to get force contribution at each node of the face
				double alpha[3];
				for (MYINT k = 0; k < 3; k++) {
					//mexPrintf("    k %d, f2n[k] %d\n",k, f2n[k]);
					int solFace = fluidNode2SolidFace[f2n[k]-1]-1;
					if ((solFace >= 0) && (solFace < numSolidSurfFaces)) {
						// Ignore domain face, and also obstacles that are not deformable
						MYINT* solFaceNodeIdx = &solidSurfFace2Node[solFace*3];
						//mexPrintf("    solFace = %d\n", solFace);

						getProjectedBaryCentric(&solidNodePos[(solFaceNodeIdx[0]-1)*3], 
												&solidNodePos[(solFaceNodeIdx[1]-1)*3],
												&solidNodePos[(solFaceNodeIdx[2]-1)*3],
												&fluidNodePos[(f2n[k]-1)*3], alpha[1], alpha[2],alpha[0]);
						for (MYINT l = 0; l < 3; l++) {
							//mexPrintf("      l %d\n",l);
							JCol->push_back(CSparseElement((solFaceNodeIdx[l]-1)*3+0, alpha[l]*n[0]));
							JCol->push_back(CSparseElement((solFaceNodeIdx[l]-1)*3+1, alpha[l]*n[1]));
							JCol->push_back(CSparseElement((solFaceNodeIdx[l]-1)*3+2, alpha[l]*n[2]));
						}
					}

				}

			}
		}
		ftet2tet += 4;
		ftet2face += 4;
	}
	J.CreateMatlabMat(M2_J);

}
void mexMode0(int nlhs, mxArray *plhs[],
					 int nrhs, const mxArray *prhs[]) {

 
    MYINT numNodes = mxGetN(M0_NODEPOS);
	MYINT numTets = mxGetN(M0_TET2NODE);
	MYINT* tet2node = check2DimAndType<MYINT>(M0_TET2NODE, "M0_TET2NODE", 4, numTets, "uint32");
	double* nodePos = check2DimAndType<double>(M0_NODEPOS, "M0_NODEPOS", 3, numNodes, "double");
	
	double lambda, mu, rho;
	double *lambdap, *mup, *rhop;
	bool varyProps = false;
	if ((mxGetM(M0_LAMBDA) == 1) && (mxGetN(M0_LAMBDA) == 1)) {
		lambda = *check2DimAndType<double>(M0_LAMBDA, "M0_LAMBDA", 1, 1, "double");
		mu = *check2DimAndType<double>(M0_MU, "M0_MU", 1, 1, "double");
		rho = *check2DimAndType<double>(M0_RHO, "M0_RHO", 1, 1, "double");
	} else {
		varyProps = true;
		lambdap = check2DimAndType<double>(M0_LAMBDA, "M0_LAMBDA", 1, numTets, "double");
		mup = check2DimAndType<double>(M0_MU, "M0_MU", 1, numTets, "double");
		rhop = check2DimAndType<double>(M0_RHO, "M0_RHO", 1, numTets, "double");
	}
	// Create bary mat
	M0_BARYMAT = mxCreateDoubleMatrix(16, numTets, mxREAL);
 	
	double* baryMat = mxGetPr(M0_BARYMAT);
	double* bm = baryMat;
	MYINT* tets = tet2node;
	for (MYINT i = 0; i < numTets; i++) {
		buildBaryMat(&nodePos[(tets[0]-1)*3], &nodePos[(tets[1]-1)*3], &nodePos[(tets[2]-1)*3], &nodePos[(tets[3]-1)*3], bm);
		bm += 16;
		tets+=4;
	}

	CSparseMatrixHelper KMat(numNodes*3, numNodes*3); // Stiffness matrix
	CSparseMatrixHelper MMat(numNodes*3, numNodes*3); // Mass matrix

	// Assume Beta is the barymat
	// Compute stiffness matrix
	// Let's build 12x12 element matrix first
	double* beta = baryMat;
	tets = tet2node;
	for (MYINT q = 0; q < numTets; q++) {
		if (varyProps) {
			lambda = lambdap[q];
			mu = mup[q];
			rho = rhop[q];
		}
		double vol = TetVol(&nodePos[(tets[0]-1)*3], &nodePos[(tets[1]-1)*3], &nodePos[(tets[2]-1)*3], &nodePos[(tets[3]-1)*3]);
		double mhvol = -0.5*vol;
		double rhovold20 = rho*vol/20.0f;
		//mexPrintf("%f\n", rhovold20);
		// Assemble stiffness and mass matrix
		for (MYINT j = 0; j < 4; j++) {
			MYINT j4 = j*4;
			int col = (tets[j]-1)*3;						
			for (MYINT b = 0; b < 3; b++) {
				vector<CSparseElement>* forceCol = &KMat.columns[col];
				vector<CSparseElement>* massCol = &MMat.columns[col];
				for (MYINT i = 0; i < 4; i++) {
					MYINT i4 = i*4;
					int row = (tets[i]-1)*3;
					for (MYINT a = 0; a < 3; a++) {

						double sum = 0.0;
						if (a == b) {
							sum = mu*(beta[i4]*beta[j4] + beta[i4+1]*beta[j4+1] + beta[i4+2]*beta[j4+2]);
							massCol->push_back(CSparseElement(row, rhovold20*(1+delta(i,j))));
													
						}
						sum += lambda * beta[i4+a] * beta[j4+b] + mu * beta[i4+b] * beta[j4 + a];
						sum *= mhvol;

						forceCol->push_back(CSparseElement(row, -sum));
						row++;
					}
				}
				col++;
			}
		}
		tets += 4;
		beta += 16;
	}
	mexPrintf("Start\n");
	KMat.CreateMatlabMat(M0_KMAT);
	MMat.CreateMatlabMat(M0_MMAT);
}

 void mexMode3(int nlhs, mxArray *plhs[],
					 int nrhs, const mxArray *prhs[]) {
	MYINT numFluidTets = mxGetN(M3_FLUIDTET2FACE);

	MYINT numFaces = *check2DimAndType<MYINT>(M3_NUMFACES, "M3_NUMFACES", 1, 1, "uint32");
	MYINT* fluidTet2Face = check2DimAndType<MYINT>(M3_FLUIDTET2FACE, "M3_FLUIDTET2FACE", 4, numFluidTets, "uint32");
	CSparseMatrixHelper D(numFluidTets, numFaces);
	for (MYINT i = 0; i < numFluidTets; i++) {
		D.columns[fluidTet2Face[0]-1].push_back(CSparseElement(i, 1));
		D.columns[fluidTet2Face[1]-1].push_back(CSparseElement(i, 1));
		D.columns[fluidTet2Face[2]-1].push_back(CSparseElement(i, 1));
		D.columns[fluidTet2Face[3]-1].push_back(CSparseElement(i, 1));
		fluidTet2Face+=4;
	}
	D.CreateMatlabMat(M3_D);
}
void cleanup() {
}
inline void MAD(double* data, double* in, double& a) {
	data[0] += in[0]*a;
	data[1] += in[1]*a;
	data[2] += in[2]*a;

}
inline void MUL(double* data, double& a) {
	data[0] *= a;
	data[1] *= a;
	data[2] *= a;
}
inline void ADD(double* data, double* in) {
	data[0] += in[0];
	data[1] += in[1];
	data[2] += in[2];

}

void mexMode4(int nlhs, mxArray *plhs[],
					 int nrhs, const mxArray *prhs[]) {

 
    MYINT numNodes = mxGetN(M4_RESTPOS);
	MYINT numTets = mxGetN(M4_TET2NODE);
	MYINT* tet2node = check2DimAndType<MYINT>(M4_TET2NODE, "M4_TET2NODE", 4, numTets, "uint32");
	double* nodePos = check2DimAndType<double>(M4_RESTPOS, "M4_RESTPOS", 3, numNodes, "double");
	
	double lambda, mu, rho;
	double *lambdap, *mup, *rhop;
	bool varyProps = false;
	if ((mxGetM(M4_LAMBDA) == 1) && (mxGetN(M4_LAMBDA) == 1)) {
		lambda = *check2DimAndType<double>(M4_LAMBDA, "M4_LAMBDA", 1, 1, "double");
		mu = *check2DimAndType<double>(M4_MU, "M4_MU", 1, 1, "double");
		rho = *check2DimAndType<double>(M4_RHO, "M4_RHO", 1, 1, "double");
	} else {
		varyProps = true;
		lambdap = check2DimAndType<double>(M4_LAMBDA, "M4_LAMBDA", 1, numTets, "double");
		mup = check2DimAndType<double>(M4_MU, "M4_MU", 1, numTets, "double");
		rhop = check2DimAndType<double>(M4_RHO, "M4_RHO", 1, numTets, "double");
	}

	double* displace = check2DimAndType<double>(M4_DISPLACE, "M4_DISPLACE", numNodes*3, 1, "double");

	// Create bary mat
	//M4_BARYMAT = mxCreateDoubleMatrix(16, numTets, mxREAL);
 	
	double* baryMat = check2DimAndType<double>(M4_BARYMAT, "M4_BARYMAT", 16, numTets, "double");
	double* P = check2DimAndType<double>(M4_CURPOS, "M4_CURPOS", 3, numNodes, "double");
//		mxGetPr(M4_BARYMAT);

	//double* bm = baryMat;
	MYINT* tets = tet2node;
/*	double* P = new double[3*numNodes];

	for (int qq = numNodes*3-1; qq >= 0; qq--) {
		P[qq] = nodePos[qq] + displace[qq];
	}*/
	/*
	for (MYINT i = 0; i < numTets; i++) {
		buildBaryMat(&nodePos[(tets[0]-1)*3], &nodePos[(tets[1]-1)*3], &nodePos[(tets[2]-1)*3], &nodePos[(tets[3]-1)*3], bm);
		bm += 16;
		tets+=4;
	}*/

	CSparseMatrixHelper KMat(numNodes*3, numNodes*3); // Stiffness matrix
	CSparseMatrixHelper MMat(numNodes*3, numNodes*3); // Mass matrix

	// Assume Beta is the barymat
	// Compute stiffness matrix
	// Let's build 12x12 element matrix first
	double* beta = baryMat;
	tets = tet2node;
	double strain[3][3], strain2[3][3];
	double stress[3][3];
	double dxodu[3][3]; // dxodu[i][a] = component a of dx/du_i 
	double Dm[9], DmInv[9];
	double Ds[9];
	double F[9];
	double* force;
    M4_FORCE     = mxCreateDoubleMatrix  (numNodes*3, 1, mxREAL); 
    force = mxGetPr(M4_FORCE);
	memset(force, 0, sizeof(double)*numNodes*3);
	for (MYINT ttt = 0; ttt < numTets; ttt++) {
		if (varyProps) {
			lambda = lambdap[ttt];
			mu = mup[ttt];
			rho = rhop[ttt];
		}
		double* n[4] = {&nodePos[(tets[0]-1)*3], &nodePos[(tets[1]-1)*3], &nodePos[(tets[2]-1)*3], &nodePos[(tets[3]-1)*3]};
		double* p[4] = {&P[(tets[0]-1)*3], &P[(tets[1]-1)*3], &P[(tets[2]-1)*3], &P[(tets[3]-1)*3]};
		double vol = TetVol(&nodePos[(tets[0]-1)*3], &nodePos[(tets[1]-1)*3], &nodePos[(tets[2]-1)*3], &nodePos[(tets[3]-1)*3]);
		double mhvol = -0.5*vol;
		double rhovold20 = rho*vol/20.0f;
		//mexPrintf("%f\n", rhovold20);
		// Compute strain and stress

		// Find Deformation tensor
		/*
		Dm[0] = n[1][0] - n[0][0];
		Dm[3] = n[1][1] - n[0][1];
		Dm[6] = n[1][2] - n[0][2];

		Dm[1] = n[2][0] - n[0][0];
		Dm[4] = n[2][1] - n[0][1];
		Dm[7] = n[2][2] - n[0][2];

		Dm[2] = n[3][0] - n[0][0];
		Dm[5] = n[3][1] - n[0][1];
		Dm[8] = n[3][2] - n[0][2];

		Ds[0] = p[1][0] - p[0][0];
		Ds[3] = p[1][1] - p[0][1];
		Ds[6] = p[1][2] - p[0][2];

		Ds[1] = p[2][0] - p[0][0];
		Ds[4] = p[2][1] - p[0][1];
		Ds[7] = p[2][2] - p[0][2];

		Ds[2] = p[3][0] - p[0][0];
		Ds[5] = p[3][1] - p[0][1];
		Ds[8] = p[3][2] - p[0][2];

		Inverse3x3(Dm, DmInv);
		for (MYINT i = 0; i < 3; i++) {
			for (MYINT j = 0; j < 3; j++) {
				double tmp = 0.0;
				for (MYINT k = 0; k < 3; k++) {
					tmp += Ds[i*3+k]*DmInv[k*3+j];
				}
				F[i*3+j] = tmp;
			}
		}
		for (MYINT i = 0; i < 3; i++) {
			for (MYINT j = 0; j < 3; j++) {
				double tmp = 0.0;
				for (MYINT k = 0; k < 3; k++) {
					tmp += F[k*3+i]*F[k*3+j];
				}
				strain2[i][j] = 0.5*(tmp - delta(i,j));
			}
		}
		*/
		// Find dx/du_i
		for (MYINT i = 0; i < 3; i++) {
			for (MYINT a = 0; a < 3; a++) {
				double tmp = 0.0;
				for (MYINT k = 0; k < 4; k++) {
					tmp += P[(tets[k]-1)*3+a]*beta[k*4+i];
				}
				dxodu[i][a] = tmp;
			}
		}
		//dxodu[3][0] = 0; dxodu[3][1] = 0; dxodu[3][2] = 0; dxodu[3][3] = 0;
		

		// Green strain
		for (MYINT i = 0; i < 3; i++) {
			for (MYINT j = 0; j < 3; j++) {
				strain[i][j] = 0.5*((dxodu[i][0]*dxodu[j][0] + dxodu[i][1]*dxodu[j][1] + dxodu[i][2]*dxodu[j][2])-delta(i,j)); 
			}
		}

		double diffSt = 0.0;
		for (MYINT i = 0; i < 3; i++) {
			for (MYINT j = 0; j < 3; j++) {
				diffSt = fabs(strain[i][j]-strain2[i][j]);
			}
		}
		//mexPrintf("%f\n", diffSt);
		// Stress
		double trace = strain[0][0]+strain[1][1]+strain[2][2];
		for (MYINT i = 0; i < 3; i++) {
			for (MYINT j = 0; j < 3; j++) {
				stress[i][j] = 2*mu*strain[i][j];
			}
			stress[i][i] += lambda*trace;
		}

		// Compute force
		for (MYINT i = 0; i < 4; i++) {
			MYINT i4 = i*4;
			double f[3] = {0,0,0};
			for (MYINT j = 0; j < 4; j++) {
				MYINT j4 = j*4;
				double tmp = 0.0;
				for (MYINT k = 0; k < 3; k++) {
					for (MYINT l = 0; l < 3; l++) {
						tmp += beta[j4+l]*beta[i4+k]*stress[k][l];
					}
				}
				MAD(f, &P[(tets[j]-1)*3], tmp);
			}
		
			MAD(&force[(tets[i]-1)*3],f, mhvol);
		}

		// Assemble stiffness and mass matrix, dfia / ddjb 
		/*
		for (MYINT j = 0; j < 4; j++) {
			MYINT j4 = j*4;
			int col = (tets[j]-1)*3;						
			for (MYINT b = 0; b < 3; b++) {
				vector<CSparseElement>* forceCol = &KMat.columns[col];
				vector<CSparseElement>* massCol = &MMat.columns[col];
				for (MYINT i = 0; i < 4; i++) {
					MYINT i4 = i*4;
					int row = (tets[i]-1)*3;
					for (MYINT a = 0; a < 3; a++) {

						double sum = 0.0;
						if (a == b) {
							sum = mu*(beta[i4]*beta[j4] + beta[i4+1]*beta[j4+1] + beta[i4+2]*beta[j4+2]);
							massCol->push_back(CSparseElement(row, rhovold20*(1+delta(i,j))));
													
						}
						sum += lambda * beta[i4+a] * beta[j4+b] + mu * beta[i4+b] * beta[j4 + a];

						double extra1 = 0.0;
						double extra2 = 0.0;
						double extra3 = 0.0;
						// Add the non-linear term
						for (MYINT k = 0; k < 4; k++) {
							MYINT k4 = k*4;
							double d_kb = displace[(tets[k]-1)*3 + b];

							for (MYINT c = 0; c < 3; c++) {
								extra1 += beta[j4+c]*beta[k4+c]*d_kb;
								extra2 += beta[i4+c]*beta[k4+c]*d_kb;
								extra3 += beta[i4+c]*beta[k4+a]*beta[j4+c]*d_kb;
							}
						}
						extra1 *= lambda*beta[i4+a];
						extra2 *= mu*beta[j4+a];
						extra3 *= mu;

						sum += extra1+extra2+extra3;
						sum *= mhvol;
						forceCol->push_back(CSparseElement(row, sum));
						row++;
					}
				}
				col++;
			}
		}*/
		double* PP[4] = {&P[(tets[0]-1)*3], &P[(tets[1]-1)*3], &P[(tets[2]-1)*3], &P[(tets[3]-1)*3]}; 

		// Build K matrix with my formula
		for (MYINT j = 0; j < 4; j++) {
			MYINT j4 = j << 2;
			double* bj4 = &beta[j4];
			int col = (tets[j]-1)*3;	
			for (MYINT b = 0; b < 3; b++) {
				vector<CSparseElement>* forceCol = &KMat.columns[col];
				vector<CSparseElement>* massCol = &MMat.columns[col];
				for (MYINT i = 0; i < 4; i++) {
					MYINT i4 = i << 2;
					double* bi4 = &beta[i4];
					int row = (tets[i]-1)*3;
	 				for (MYINT a = 0; a < 3; a++) {
						if (row <= col) {
							double sum = 0.0;

							// Mass
							if (a == b) {
								if (row == col) {                           
									massCol->push_back(CSparseElement(row, 0.5*rhovold20*(1+delta(i,j))));												
								} else {
									massCol->push_back(CSparseElement(row, rhovold20*(1+delta(i,j))));												
								}

                        if ((col == 0) || (row == 0)) {
                           mexPrintf("%d %d %lg\n", col, row, massCol->back().v); 
                        }

							}
							
							// First term
							if (a == b) {
								for (MYINT k = 0; k < 3; k++) {
									for (MYINT l = 0; l < 3; l++) {
										sum += bj4[l]*bi4[k]*stress[k][l];
									}
								}
							}
							
							
							// Second term
							for (MYINT q = 0; q < 4; q++) {
								double* bq4 = &beta[q << 2];
								double tmp1 = 0.0;
								for (MYINT l = 0; l < 3; l++) {
									for (MYINT k = 0; k < 3; k++) {
										double tmp2 = 0.0;
										double* bt4 = beta;
										// First part
										if (l == k) {
											double tmp3 = 0.0;
											
											for (MYINT t = 0; t < 4; t++) {
												//for (MYINT m = 0; m < 3; m++) {
												
												tmp3 += (bt4[0]*bj4[0] + bt4[1]*bj4[1] + bt4[2]*bj4[2])*PP[t][b];
												bt4+=4;
												//}
											}
											tmp3 *= 4*lambda;
											tmp2 += tmp3;
											bt4 = beta;
										}
										double tmp4 = 0.0;
										
										for (MYINT t = 0; t < 4; t++) {
											tmp4 += bj4[k]*bt4[l]*PP[t][b];
											bt4+=4;
										}
										tmp4 *= 2*mu;
										tmp2 += tmp4;
										double tmp5 = 0.0;
										bt4 = beta;
										for (MYINT t = 0; t < 4; t++) {
											tmp5+=bt4[k]*bj4[l]*PP[t][b];
											bt4 += 4;
										}
										tmp5 *= 2*mu;
										tmp2 += tmp5;

										tmp2 *= bq4[l]*bi4[k]*PP[q][a];
										tmp1 += tmp2;
									}
								}
								sum += tmp1;
							}
							
							/*
							for (MYINT q = 0; q < 4; q++) {
								double* bq4 = &beta[q << 2];
								double tmp1 = 0.0;
								for (MYINT l = 0; l < 3; l++) {
									for (MYINT k = 0; k < 3; k++) {
										double tmp2 = 0.0;
										double* bt4 = beta;
										// First part
										if (l == k) {
											double tmp3 = 0.0;
											for (MYINT t = 0; t < 4; t++) {
												//for (MYINT m = 0; m < 3; m++) {
												MYINT t4 = t * 4;
												tmp3 += (bt4[0]*bj4[0] + bt4[1]*bj4[1] + bt4[2]*bj4[2])*PP[t][b];
												bt4 += 4;
												//}
											}
											bt4 = beta;
											tmp3 *= 4*lambda;
											tmp2 += tmp3;
										}
										double tmp4 = 0.0;
										for (MYINT t = 0; t < 4; t++) {
											tmp4 += bj4[k]*bt4[l]*PP[t][b];
											bt4 += 4;
										}
										tmp4 *= 2*mu;
										tmp2 += tmp4;
										double tmp5 = 0.0;
										bt4 = beta;
										for (MYINT t = 0; t < 4; t++) {
											tmp5+=bt4[k]*bj4[l]*PP[t][b];
											bt4 += 4;
										}
										tmp5 *= 2*mu;
										tmp2 += tmp5;

										tmp2 *= bq4[l]*bi4[k]*PP[q][a];
										tmp1 += tmp2;
									}
								}
								sum += tmp1;
							}*/

							/*
							for (MYINT q = 0; q < 4; q++) {
								double tmp1 = 0.0;
								for (MYINT l = 0; l < 3; l++) {
									for (MYINT k = 0; k < 3; k++) {
										double tmp2 = 0.0;
										
										// First part
										if (l == k) {
											double tmp3 = 0.0;
											for (MYINT t = 0; t < 4; t++) {
												//for (MYINT m = 0; m < 3; m++) {
												MYINT t4 = t * 4;
												tmp3 += (beta[t4+0]*beta[j4+0] + beta[t4+1]*beta[j4+1] + beta[t4+2]*beta[j4+2])*P[(tets[t]-1)*3+b];
												//}
											}
											tmp3 *= 4*lambda;
											tmp2 += tmp3;
										}
										double tmp4 = 0.0;
										for (MYINT t = 0; t < 4; t++) {
											tmp4 += beta[j4+k]*beta[t*4+l]*P[(tets[t]-1)*3+b];
										}
										tmp4 *= 2*mu;
										tmp2 += tmp4;
										double tmp5 = 0.0;
										for (MYINT t = 0; t < 4; t++) {
											tmp5+=beta[4*t+k]*beta[j4+l]*P[(tets[t]-1)*3+b];
										}
										tmp5 *= 2*mu;
										tmp2 += tmp5;

										tmp2 *= beta[q*4+l]*beta[i4+k]*P[(tets[q]-1)*3+a];
										tmp1 += tmp2;
									}
								}
								sum += tmp1;
							}*/

							
							sum *= mhvol;
							if (row == col) {
								forceCol->push_back(CSparseElement(row, -0.5*sum));
							} else {
								forceCol->push_back(CSparseElement(row, -sum));
							}
						}
						row++;
					}
				}
				col++;
			}
		}
		tets += 4;
		beta += 16;
	}
	mexPrintf("Start\n");
	/*
	for (MYINT i = 0; i < numNodes; i++) {
		nodePos[i*3] -= displace[i*3];
		nodePos[i*3+1] -= displace[i*3+1];
		nodePos[i*3+2] -= displace[i*3+2];
	}*/
	KMat.CreateMatlabMat(M4_KMAT);
	MMat.CreateMatlabMat(M4_MMAT);
}
inline void setDoubles(double* out, double* v0, double* v1, double* v2, double w0, double w1, double w2) {
	out[0] = w0*v0[0]+w1*v1[0]+w2*v2[0];
	out[1] = w0*v0[1]+w1*v1[1]+w2*v2[1];
	out[2] = w0*v0[2]+w1*v1[2]+w2*v2[2];
}
void mexMode5(int nlhs, mxArray *plhs[],
					 int nrhs, const mxArray *prhs[]) {
	MYINT numSolidNodes = mxGetN(M5_SOLIDNODEPOS);
	MYINT numFluidNodes = mxGetN(M5_FLUIDNODEPOS);

	double* solidNodePos = check2DimAndType<double>(M5_SOLIDNODEPOS, "M5_SOLIDNODEPOS", 3, numSolidNodes, "double");	 
	double* solidNodeVel = check2DimAndType<double>(M5_SOLIDNODEVEL, "M5_SOLIDNODEVEL", numSolidNodes*3,1, "double");	 
	double* fluidNodePos = check2DimAndType<double>(M5_FLUIDNODEPOS, "M5_FLUIDNODEPOS", 3, numFluidNodes, "double");	 
	double* fluidNodeVel = check2DimAndType<double>(M5_FLUIDNODEVEL, "M5_FLUIDNODEVEL", 3, numFluidNodes, "double");	 

	MYINT numFluidAllFaces = mxGetN(M5_FLUIDALLFACE2NODE);
	MYINT numSolidSurfFaces = mxGetN(M5_SOLIDSURFFACE2NODE);

	MYINT* fluidAllFace2Node = check2DimAndType<MYINT>(M5_FLUIDALLFACE2NODE, "M5_FLUIDALLFACE2NODE", 3, numFluidAllFaces, "uint32");
	MYINT* solidSurfFace2Node = check2DimAndType<MYINT>(M5_SOLIDSURFFACE2NODE, "M5_SOLIDSURFFACE2NODE", 3, numSolidSurfFaces, "uint32");
	int* fluidNode2SolidFace = check2DimAndType<int>(M5_FLUIDNODE2SOLIDFACE, "M5_FLUIDNODE2SOLIDFACE", 1, numFluidNodes, "int32");

	MYINT* fface2node = fluidAllFace2Node;
	double alpha[3];
	for (MYINT n = 0; n < numFluidNodes; n++) {
		if ((fluidNode2SolidFace[n] > 0) && (fluidNode2SolidFace[n] <= numSolidSurfFaces)) {
			double *p = &fluidNodePos[3*n];
			MYINT* solidFaceStart = &solidSurfFace2Node[(fluidNode2SolidFace[n]-1)*3];
			getProjectedBaryCentric(&solidNodePos[(solidFaceStart[0]-1)*3], 
									    &solidNodePos[(solidFaceStart[1]-1)*3], 
										&solidNodePos[(solidFaceStart[2]-1)*3], p, alpha[1], alpha[2],alpha[0]);
			setDoubles(&fluidNodeVel[3*n], &solidNodeVel[(solidFaceStart[0]-1)*3], &solidNodeVel[(solidFaceStart[1]-1)*3], &solidNodeVel[(solidFaceStart[2]-1)*3], alpha[0], alpha[1], alpha[2]);
		}
	}

}
void mexMode6(int nlhs, mxArray *plhs[],
					 int nrhs, const mxArray *prhs[]) {
	MYINT numSolidNodes = mxGetN(M6_SOLIDNODEPOS);
	MYINT numFluidNodes = mxGetN(M6_FLUIDNODEPOS);
	MYINT numFluidTets = mxGetN(M6_FLUIDTET2FACE);

	double* solidNodePos = check2DimAndType<double>(M6_SOLIDNODEPOS, "M6_SOLIDNODEPOS", 3, numSolidNodes, "double");	 
	double* fluidNodePos = check2DimAndType<double>(M6_FLUIDNODEPOS, "M6_FLUIDNODEPOS", 3, numFluidNodes, "double");	 

	MYINT numFluidAllFaces = mxGetN(M6_FLUIDALLFACE2NODE);
	MYINT numSolidSurfFaces = mxGetN(M6_SOLIDSURFFACE2NODE);

	MYINT* fluidAllFace2Node = check2DimAndType<MYINT>(M6_FLUIDALLFACE2NODE, "M6_FLUIDALLFACE2NODE", 3, numFluidAllFaces, "uint32");
	MYINT* solidSurfFace2Node = check2DimAndType<MYINT>(M6_SOLIDSURFFACE2NODE, "M6_SOLIDSURFFACE2NODE", 3, numSolidSurfFaces, "uint32");
	int* fluidNode2SolidFace = check2DimAndType<int>(M6_FLUIDNODE2SOLIDFACE, "M6_FLUIDNODE2SOLIDFACE", 1, numFluidNodes, "int32");
	MYINT* fluidTet2Face = check2DimAndType<MYINT>(M6_FLUIDTET2FACE, "M6_FLUIDTET2FACE", 4, numFluidTets, "uint32");
	MYINT* fluidTet2Tet = check2DimAndType<MYINT>(M6_FLUIDTET2TET, "M6_FLUIDTET2TET", 4, numFluidTets, "uint32");

	MYINT* ftet2tet = fluidTet2Tet;
	MYINT* ftet2face = fluidTet2Face;
	CSparseMatrixHelper J(numSolidNodes*3, numFluidTets);

	for (MYINT i = 0; i < numFluidTets; i++) {
		//mexPrintf("i %d\n",i);
		vector<CSparseElement>* JCol = &J.columns[i];
		for (MYINT j = 0; j < 4; j++) {
			//mexPrintf("  j %d\n",j);
			if (ftet2tet[j] == 0) {
				// Boundary!
				MYINT faceNum = ftet2face[j]-1;
				MYINT* f2n = &fluidAllFace2Node[faceNum*3];
				double* p[3] = {&fluidNodePos[(f2n[0]-1)*3], &fluidNodePos[(f2n[1]-1)*3],&fluidNodePos[(f2n[2]-1)*3]};
				CVector3 t0(p[1],p[0]);
				CVector3 t1(p[2],p[0]);
				CVector3 n = Cross(t0,t1);
				n *= 0.5; // 1/6
				// One third of the Area weighted normal 
				// Multiplied by pressure to get force contribution at each node of the face
				double alpha[3];
				for (MYINT k = 0; k < 3; k++) {
					//mexPrintf("    k %d, f2n[k] %d\n",k, f2n[k]);
					int solFace = fluidNode2SolidFace[f2n[k]-1]-1;
					if ((solFace >= 0) && (solFace < numSolidSurfFaces)) {
						// Ignore domain face, and also obstacles that are not deformable
						MYINT* solFaceNodeIdx = &solidSurfFace2Node[solFace*3];
						//mexPrintf("    solFace = %d\n", solFace);

						getProjectedBaryCentric(&solidNodePos[(solFaceNodeIdx[0]-1)*3], 
												&solidNodePos[(solFaceNodeIdx[1]-1)*3],
												&solidNodePos[(solFaceNodeIdx[2]-1)*3],
												&fluidNodePos[(f2n[k]-1)*3], alpha[1], alpha[2],alpha[0]);
						double corrective = 1/(alpha[0]*alpha[0]+alpha[1]*alpha[1]+alpha[2]*alpha[2]);
						for (MYINT l = 0; l < 3; l++) {
							//mexPrintf("      l %d\n",l);
							JCol->push_back(CSparseElement((solFaceNodeIdx[l]-1)*3+0, corrective*alpha[l]*n[0]));
							JCol->push_back(CSparseElement((solFaceNodeIdx[l]-1)*3+1, corrective*alpha[l]*n[1]));
							JCol->push_back(CSparseElement((solFaceNodeIdx[l]-1)*3+2, corrective*alpha[l]*n[2]));
						}
					}

				}

			}
		}
		ftet2tet += 4;
		ftet2face += 4;
	}
	J.CreateMatlabMat(M6_J);
}
#define ONE_THIRD 0.33333333333333333333333333333333
		 
void mexMode7(int nlhs, mxArray *plhs[],
					 int nrhs, const mxArray *prhs[]) {
	MYINT numSolidNodes = mxGetN(M7_SOLIDNODEPOS);
	MYINT numFluidNodes = mxGetN(M7_FLUIDNODEPOS);

	double* solidNodePos = check2DimAndType<double>(M7_SOLIDNODEPOS, "M7_SOLIDNODEPOS", 3, numSolidNodes, "double");	 
	double* fluidNodePos = check2DimAndType<double>(M7_FLUIDNODEPOS, "M7_FLUIDNODEPOS", 3, numFluidNodes, "double");	 

	MYINT numFluidAllFaces = mxGetN(M7_FLUIDALLFACE2NODE);
	MYINT numSolidSurfFaces = mxGetN(M7_SOLIDSURFFACE2NODE);

	MYINT* fluidAllFace2Node = check2DimAndType<MYINT>(M7_FLUIDALLFACE2NODE, "M7_FLUIDALLFACE2NODE", 3, numFluidAllFaces, "uint32");
	MYINT* solidSurfFace2Node = check2DimAndType<MYINT>(M7_SOLIDSURFFACE2NODE, "M7_SOLIDSURFFACE2NODE", 3, numSolidSurfFaces, "uint32");
	int* fluidNode2SolidFace = check2DimAndType<int>(M7_FLUIDNODE2SOLIDFACE, "M7_FLUIDNODE2SOLIDFACE", 1, numFluidNodes, "int32");

	MYINT* fface2node = fluidAllFace2Node;
	CSparseMatrixHelper H(numFluidAllFaces, numSolidNodes*3);
	double alpha[3][3];
	for (MYINT f = 0; f < numFluidAllFaces; f++) {
		if (((fluidNode2SolidFace[fface2node[0]-1] > 0) && (fluidNode2SolidFace[fface2node[0]-1] <= numSolidSurfFaces)) &&
			((fluidNode2SolidFace[fface2node[1]-1] > 0) && (fluidNode2SolidFace[fface2node[1]-1] <= numSolidSurfFaces)) &&
			((fluidNode2SolidFace[fface2node[2]-1] > 0) && (fluidNode2SolidFace[fface2node[2]-1] <= numSolidSurfFaces))) {
		
			// This is a boundary face
			// Find normal for this face
			double* p[3] = {&fluidNodePos[(fface2node[0]-1)*3], &fluidNodePos[(fface2node[1]-1)*3], &fluidNodePos[(fface2node[2]-1)*3]};
			CVector3 t0(p[1],p[0]);
			CVector3 t1(p[2],p[0]);
			CVector3 n = Cross(t0,t1);
			n.normalize();
			// Flux = 1/3*(v_0 + v_1 + v_2).(n/2) 
			// n is unnormalized normal = normalized normal * area * 2


			//double area = n.norm2();
			//n /= area;
			//area *= 0.5;
	
			// Now, compute barycentric coordinates for the 3 fluid boundary vertices with respect to triangles on the 
			// solid domain
			// Also, add coefficients to various rows
			for (MYINT i = 0; i < 3; i++) {
				MYINT* solidFaceStart = &solidSurfFace2Node[(fluidNode2SolidFace[fface2node[i]-1]-1)*3];
				// Beware!!
				// alpha[i][1] = s
				// alpha[i][2] = t
				// alpha[i][0] = 1-s-t
				getProjectedBaryCentric(&solidNodePos[(solidFaceStart[0]-1)*3], 
									    &solidNodePos[(solidFaceStart[1]-1)*3], 
										&solidNodePos[(solidFaceStart[2]-1)*3], p[i], alpha[i][1], alpha[i][2],alpha[i][0]);

				// Add coefficients to various columns 
				// Loop for x y z
				for (MYINT j = 0; j < 3; j++) {
					H.columns[(solidFaceStart[0] - 1)*3 + j].push_back(CSparseElement(f, ONE_THIRD*n[j]*alpha[i][0]));
					H.columns[(solidFaceStart[1] - 1)*3 + j].push_back(CSparseElement(f, ONE_THIRD*n[j]*alpha[i][1]));
					H.columns[(solidFaceStart[2] - 1)*3 + j].push_back(CSparseElement(f, ONE_THIRD*n[j]*alpha[i][2]));
				}
			}

		}
		fface2node += 3;
	}
	H.CreateMatlabMat(M7_H);
}
 void mexMode8(int nlhs, mxArray *plhs[],
					 int nrhs, const mxArray *prhs[]) {
	MYINT numFluidTets = mxGetN(M3_FLUIDTET2FACE);

	MYINT numFaces = *check2DimAndType<MYINT>(M8_NUMFACES, "M8_NUMFACES", 1, 1, "uint32");
	MYINT* fluidTet2Face = check2DimAndType<MYINT>(M8_FLUIDTET2FACE, "M8_FLUIDTET2FACE", 4, numFluidTets, "uint32");
	double* faceArea = check2DimAndType<double>(M8_FACEAREA, "M8_FACEAREA", 1, numFaces, "double");

	CSparseMatrixHelper D(numFluidTets, numFaces);
	for (MYINT i = 0; i < numFluidTets; i++) {
		D.columns[fluidTet2Face[0]-1].push_back(CSparseElement(i, faceArea[fluidTet2Face[0]-1]));
		D.columns[fluidTet2Face[1]-1].push_back(CSparseElement(i, faceArea[fluidTet2Face[1]-1]));
		D.columns[fluidTet2Face[2]-1].push_back(CSparseElement(i, faceArea[fluidTet2Face[2]-1]));
		D.columns[fluidTet2Face[3]-1].push_back(CSparseElement(i, faceArea[fluidTet2Face[3]-1]));
		fluidTet2Face+=4;
	}
	D.CreateMatlabMat(M8_D);
}
void mexFunction(int nlhs, mxArray *plhs[],
					 int nrhs, const mxArray *prhs[]) {

	 MYINT mode = *check2DimAndType<MYINT>(MODE, "MODE", 1, 1, MATMYINTNAME);
	 mexAtExit(cleanup);
	 if (mode == 0) {
		mexMode0(nlhs, plhs, nrhs, prhs);

	 } else
	 if (mode == 1) {
		mexMode1(nlhs, plhs, nrhs, prhs);

	 } else
	 if (mode == 2) {
		mexMode2(nlhs, plhs, nrhs, prhs);

	 }else
	 if (mode == 3) {
		mexMode3(nlhs, plhs, nrhs, prhs);

	 }else 
	 if (mode == 4) {
		mexMode4(nlhs, plhs, nrhs, prhs);

	 } else
	 if (mode == 5) {
		mexMode5(nlhs, plhs, nrhs, prhs);

	 } else
	 if (mode == 6) {
		mexMode6(nlhs, plhs, nrhs, prhs);

	 } else
     if (mode == 7) {
		mexMode7(nlhs, plhs, nrhs, prhs);

	 } else 
     if (mode == 8) {
		mexMode8(nlhs, plhs, nrhs, prhs);

	 } else {  
		 mexErrMsgTxt("Invalid mode\n"); 
	 }
}

/*
#define FUNCTIONNAME mex_FEM.cpp

#define MYINT unsigned int
#define MATMYINTNAME  "uint32"


#include "mex.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "../../varTetMeshing/common.h"
#include <fstream>

#ifdef __GNUC__
	#include <ext/hash_map>
	using namespace __gnu_cxx;
#else
	#include <hash_map>
#endif
#include <algorithm>
using namespace std;

// Assume isotropic
#define M0_NODEPOS				prhs[0]
#define M0_NODEVEL				prhs[1]
#define M0_TET2NODE				prhs[2]
#define M0_BARYMAT				prhs[3]
#define M0_LAMBDA				prhs[4]
#define M0_MU					prhs[5]
#define M0_PHI					prhs[6]
#define M0_PSI					prhs[7]

// Just update the input MATLAB beware!

template<class T> 
T* checkType(const mxArray*& ar, char* arname, char* type) {
	if (strcmp(mxGetClassName(ar), type) != 0) {
		mexPrintf("%s is the wrong data type.", arname);    
		mexErrMsgTxt("wrong data type.");    
	}
	return (T*)mxGetPr(ar);
}

template<class T> 
T* check2DimAndType(const mxArray*& ar, char* arname, MYINT d1, MYINT d2, char* type) {
	if (strcmp(mxGetClassName(ar), type) != 0) {
		mexPrintf("%s is the wrong data type.", arname);    
		mexErrMsgTxt("wrong data type.");    
	}
	if (mxGetM(ar) != d1) {
		mexPrintf("%s is in wrong first dimension, should be %d", arname, d1);    
		mexErrMsgTxt("wrong first dimension");      
	}
	if (mxGetN(ar) != d2) {
		mexPrintf("%s is in wrong second dimension, should be %d", arname, d2);    
		mexErrMsgTxt("wrong second dimension");     
	}
	return (T*)mxGetPr(ar);
}
template<class T>
T* check1DimAndType(const mxArray*& ar, char* arname, MYINT d1, char* type) {
	if (strcmp(mxGetClassName(ar), type) != 0) {
		mexPrintf("%s is the wrong data type.", arname);    
		mexErrMsgTxt("wrong data type.");    
	}
	if (mxGetM(ar) != d1) {
		mexPrintf("%s is in wrong first dimension, should be %d", arname, d1);    
		mexErrMsgTxt("wrong first dimension");    
	}
	// return d2
	return (T*)mxGetPr(ar);
}
template<class T>
T* check1DimAndType2(const mxArray*& ar, char* arname, MYINT d2, char* type) {
	if (strcmp(mxGetClassName(ar), type) != 0) {
		mexPrintf("%s is the wrong data type.", arname);    
		mexErrMsgTxt("wrong data type.");    
	}
	if (mxGetN(ar) != d2) {
		mexPrintf("%s is in wrong 2nd dimension, should be %d", arname, d2);    
		mexErrMsgTxt("wrong 2nd dimension");    
	}
	// return d2
	return (T*)mxGetPr(ar);
}

#define M0_NODEPOS				prhs[0]
#define M0_NODEVEL				prhs[1]
#define M0_TET2NODE				prhs[2]
#define M0_BARYMAT				prhs[3]
#define M0_LAMBDA				prhs[4]
#define M0_MU					prhs[5]
#define M0_PHI					prhs[6]
#define M0_PSI					prhs[7]


inline void MulMat3x4w4x4(double* c, double* a, double* b) {
	double s = 0.0;
	double* a1, *b1;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 4; j++) {
			s = 0.0;
			a1 = &a[4*i];
			b1 = &b[j];
			for (int k = 0; k < 4; k++) {
				s += (*a1)*(*b1);
				a1++;
				b1+=4;
			}
			*c = s;
			c++;
		}
	}
}

#define delta(a,b) ((a) == (b) ? 1 : 0)
inline void DotColumn4x4(double* c1, double* c2) {
	return c1[0]*c2[0]+c1[4]*c2[4]+c1[8]*c2[8];
}
inline void GetStrain(double* strain, double* xderiv) {
	for (int i = 0; i < 3; i++) {
		for (int j = i; j < 3; j++) {
			strain[i*3+j] = strain[j*3+i] = DotColumn(&xderiv[i], &xderiv[j]) - delta(i,j);
		}
	}

}
inline void GetStress(double* stress, double* strain, double& lambda, double& mu) {
	double val = lambda*(strain[0]+strain[4]+strain[8]);
	for (int i = 0; i < 3; i++) {	
		for (int j = 0; j < 3; j++) {
			*stress=(*strain)*2*mu;
			stress++;
			strain++;
		}
	}
	stress -= 9;
	stress[0] += val;
	stress[4] += val;
	stress[8] += val;
}
void mexMode0(int nlhs, mxArray *plhs[],
					 int nrhs, const mxArray *prhs[]) {

    MYINT numNodes = mxGetN(M0_NODEPOS);
	MYINT numTets = mxGetN(M0_TET2NODE);
	MYINT* tet2node = check2DimAndType<MYINT>(M0_TET2NODE, "M0_TET2NODE", 4, numTets, "uint32");
	double* nodePos = check2DimAndType<double>(M0_NODEPOS, "M0_NODEPOS", 3, numNodes, "double");
	double* nodeVel = check2DimAndType<double>(M0_NODEVEL, "M0_NODEVEL", 3, numNodes, "double");
	double* baryMat = check2DimAndType<double>(M0_TET2NODE, "M0_TET2NODE", 16, numTets, "double");
	double lambda = check2DimAndType<double>(M0_LAMBDA, "M0_LAMBDA", 1, 1, "double");
	double mu = check2DimAndType<double>(M0_MU, "M0_MU", 1, 1, "double");
	double phi = check2DimAndType<double>(M0_PHI, "M0_PHI", 1, 1, "double");
	double psi = check2DimAndType<double>(M0_PSI, "M0_PSI", 1, 1, "double");

	// Just do stupid forward Euler first

    // Compute stress in each element
    double* sigmaE;
	NEW_ARRAY(sigmaE, double, 9*numTets);
	
    // Compute strain rate in each element
    double* sigmaV;
	NEW_ARRAY(sigmaV, double, 9*numTets);

	double* sE = sigmaE;

	double PB[12];
	double P[12];
	double E[9];

	double VB[12];
	double V[12];
	double EV[9];

	MYINT tet = tet2node;
	
	for (MYINT i = 0; i < numTets; i++) {
		// Strain
		double* P0 = &nodePos[(tet[0]-1)*3];
		double* P1 = &nodePos[(tet[1]-1)*3];
		double* P2 = &nodePos[(tet[2]-1)*3];
		double* P3 = &nodePos[(tet[3]-1)*3];
		P[0] = P0[0]; P[1] = P1[0]; P[2]  = P2[0]; P[3]  = P3[0];
		P[4] = P0[1]; P[5] = P1[1]; P[6]  = P2[1]; P[7]  = P3[1];
		P[8] = P0[2]; P[9] = P1[2]; P[10] = P2[2]; P[11] = P3[2];

		MulMat3x4w4x4(PB, P, &baryMat[16*i]);

		GetStrain(E, PB);

		GetStress(&sigmaE[9*i], E, lambda, mu);

		// Strain rate
		double* V0 = &nodeVel[(tet[0]-1)*3];
		double* V1 = &nodeVel[(tet[1]-1)*3];
		double* V2 = &nodeVel[(tet[2]-1)*3];
		double* V3 = &nodeVel[(tet[3]-1)*3];
		V[0] = V0[0]; V[1] = V1[0]; V[2]  = V2[0]; V[3]  = V3[0];
		V[4] = V0[1]; V[5] = V1[1]; V[6]  = V2[1]; V[7]  = V3[1];
		V[8] = V0[2]; V[9] = V1[2]; V[10] = V2[2]; V[11] = V3[2];

		MulMat3x4w4x4(VB, V, &baryMat[16*i]);



		tet+=4;
	}


	DELETE_ARRAY(sigmaE);
	DELETE_ARRAY(sigmaV);
}
void mexFunction(int nlhs, mxArray *plhs[],
					 int nrhs, const mxArray *prhs[]) {
	// Invalidate the guess to avoid side effect
	latestNearest = 0;

	 MYINT mode = *check2DimAndType<MYINT>(MODE, "MODE", 1, 1, MATMYINTNAME);
	 mexAtExit(cleanup);
	 if (mode == 0) {
		mexMode0(nlhs, plhs, nrhs, prhs);
	 } else 
	 if (mode == 1) {
		mexMode1(nlhs, plhs, nrhs, prhs);
	 } else
	 if (mode == 2) {
		mexMode2(nlhs, plhs, nrhs, prhs);
	 }else
	 if (mode == 3) {
		mexMode3(nlhs, plhs, nrhs, prhs);
	 } else
	 if (mode == 4) {
		if (!octreeRoot) {
			mexErrMsgTxt("Octree has not been built yet!!!");    
		}
		OptimizeNodePos(nlhs, plhs, nrhs, prhs);
	 } else
	 if (mode == 5) {
		 cleanup();
	 }else{ 
		 mexErrMsgTxt("Invalid mode"); 
	 }
}
*/
