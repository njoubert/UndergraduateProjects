// Physics_GravityForce.cpp: implementation of the Physics_GravityForce class.
//
//////////////////////////////////////////////////////////////////////

#include "DRGShell.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Physics_GravityForce::Physics_GravityForce( Physics_Vector3 &dir )
{
	m_Direction = dir;
}

Physics_GravityForce::~Physics_GravityForce()
{

}

void Physics_GravityForce::Apply( Physics_t fTime, Physics_LargeVector &masses, bool bDerivs,
								  Physics_LargeVector &p, 
								  Physics_LargeVector &v, 
								  Physics_LargeVector &f_int, Physics_LargeVector &f_ext,
								  Physics_SparseSymmetricMatrix &f_dp, Physics_SparseSymmetricMatrix &f_dv )
{
	for( int i=0; i<f_ext.m_iElements; i++ )
	{
		f_ext.m_pData[i].x += m_Direction.x * masses.m_pData[i].x;
		f_ext.m_pData[i].y += m_Direction.y * masses.m_pData[i].y;
		f_ext.m_pData[i].z += m_Direction.z * masses.m_pData[i].z;
	}
}
