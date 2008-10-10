// Physics_GravityForce.h: interface for the Physics_GravityForce class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PHYSICS_GRAVITYFORCE_H__D6C2DAAB_5C83_4E63_B1F6_23F8C3BC2BCB__INCLUDED_)
#define AFX_PHYSICS_GRAVITYFORCE_H__D6C2DAAB_5C83_4E63_B1F6_23F8C3BC2BCB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Physics_GravityForce : public Physics_Force  
{
private:
	Physics_Vector3 m_Direction;

public:
	Physics_GravityForce( Physics_Vector3 &dir );

	virtual ~Physics_GravityForce();

	virtual void Apply( Physics_t fTime, Physics_LargeVector &masses, bool bDerivs,
						Physics_LargeVector &p, Physics_LargeVector &v, 
						Physics_LargeVector &f_int, Physics_LargeVector &f_ext,
						Physics_SparseSymmetricMatrix &dp, Physics_SparseSymmetricMatrix &dv);
};

#endif // !defined(AFX_PHYSICS_GRAVITYFORCE_H__D6C2DAAB_5C83_4E63_B1F6_23F8C3BC2BCB__INCLUDED_)
