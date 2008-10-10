// Physics_Constraint.cpp: implementation of the Physics_Constraint class.
//
//////////////////////////////////////////////////////////////////////

#include "DRGShell.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Physics_Constraint::Physics_Constraint( int iParticle, int iDegreesOfFreedom, Physics_Vector3 axis1, Physics_Vector3 axis2, Physics_t ConstrainedVelocity)
{
	m_iParticle = iParticle;
	m_iDegreesOfFreedom = iDegreesOfFreedom;
	m_p = axis1;
	m_q = axis2;
	m_ConstrainedVelocity = ConstrainedVelocity;
	m_op1.SetFromOuterProduct( m_p, m_p );
	m_op2.SetFromOuterProduct( m_q, m_q );
}

Physics_Constraint::~Physics_Constraint()
{

}

void Physics_Constraint::Apply( Physics_Matrix3x3 S[], Physics_LargeVector &z )
{
	Physics_Matrix3x3 I, tmp;

	I.SetIdentity();
	z.m_pData[m_iParticle] = m_ConstrainedVelocity;

	switch( m_iDegreesOfFreedom )
	{
		case 0:
			S[m_iParticle].Zero();
			break;
		case 1:
			I.Subtract( m_op1, tmp );
			tmp.Subtract( m_op2, S[m_iParticle] );
			break;
		case 2:
			I.Subtract( m_op1, S[m_iParticle] );
			break;
		case 3:
		default:
			S[m_iParticle].SetIdentity();
			z.m_pData[m_iParticle] = 0;
			break;
	}
}

