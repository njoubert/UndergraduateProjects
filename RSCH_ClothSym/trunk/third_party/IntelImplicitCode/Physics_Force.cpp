// Physics_Force.cpp: implementation of the Physics_Force class.
//
//////////////////////////////////////////////////////////////////////

#include "DRGShell.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Physics_Force::Physics_Force()
{
}

Physics_Force::~Physics_Force()
{
}

void Physics_Force::Apply( Physics_t fTime, Physics_LargeVector &masses, bool bDerivs,
						   Physics_LargeVector &p, 
						   Physics_LargeVector &v, 
						   Physics_LargeVector &f_int, Physics_LargeVector &f_ext,
						   Physics_SparseSymmetricMatrix &f_dp, Physics_SparseSymmetricMatrix &f_dv )
{
}


void Physics_Force::PrepareMatrices( Physics_SymmetricMatrix &A, Physics_SparseSymmetricMatrix &B)
{

}

void Physics_Force::Fixup( Physics_LargeVector &invmasses, Physics_LargeVector &p )
{
}
