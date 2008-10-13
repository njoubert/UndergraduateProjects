// Physics_SpringForce.h: interface for the Physics_SpringForce class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PHYSICS_SPRINGFORCE_H__CDF6271F_1686_4282_B4AC_D14EBAC22265__INCLUDED_)
#define AFX_PHYSICS_SPRINGFORCE_H__CDF6271F_1686_4282_B4AC_D14EBAC22265__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Physics_SpringForce : public Physics_Force  
{
public:
	int m_iParticle[2];
	Physics_t m_kSpring, m_kSpringDamp;
	Physics_t m_RestDistance, m_MaxDistance;
	bool m_bFixup;

public:
	virtual void PrepareMatrices( Physics_SymmetricMatrix &A, Physics_SparseSymmetricMatrix &B );
	Physics_SpringForce();
	virtual ~Physics_SpringForce();

	void Fixup( Physics_LargeVector &invmasses, Physics_LargeVector &p );
	virtual void Apply( Physics_t fTime, Physics_LargeVector &masses, bool bDerivs,
						Physics_LargeVector &p, Physics_LargeVector &v, 
						Physics_LargeVector &f_int, Physics_LargeVector &f_ext,
 					    Physics_SparseSymmetricMatrix &f_dp, Physics_SparseSymmetricMatrix &f_dv );
};

#endif // !defined(AFX_PHYSICS_SPRINGFORCE_H__CDF6271F_1686_4282_B4AC_D14EBAC22265__INCLUDED_)
