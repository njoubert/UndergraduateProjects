// Physics_Force.h: interface for the Physics_Force class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PHYSICS_FORCE_H__E5FD4540_55BC_4499_B064_19CCF0C0E567__INCLUDED_)
#define AFX_PHYSICS_FORCE_H__E5FD4540_55BC_4499_B064_19CCF0C0E567__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Physics_Force  
{
public:
	virtual void PrepareMatrices( Physics_SymmetricMatrix &A, Physics_SparseSymmetricMatrix &B );
	Physics_Force();
	virtual ~Physics_Force();

	virtual void Fixup( Physics_LargeVector &invmasses, Physics_LargeVector &p );

	virtual void Apply( Physics_t fTime, Physics_LargeVector &masses, bool bDerivs, 
						Physics_LargeVector &p, Physics_LargeVector &v, 
						Physics_LargeVector &f_int, Physics_LargeVector &f_ext,
						Physics_SparseSymmetricMatrix &f_dp, Physics_SparseSymmetricMatrix &f_dv );
};

#endif // !defined(AFX_PHYSICS_FORCE_H__E5FD4540_55BC_4499_B064_19CCF0C0E567__INCLUDED_)
