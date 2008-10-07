// Physics_SpringForce.cpp: implementation of the Physics_SpringForce class.
//
//////////////////////////////////////////////////////////////////////

#include "DRGShell.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Physics_SpringForce::Physics_SpringForce()
{
	m_kSpring = 100;
	m_kSpringDamp = 10;
	m_RestDistance = 1;
	m_MaxDistance = (Physics_t)1.05;
	m_bFixup = false;
}

Physics_SpringForce::~Physics_SpringForce()
{

}

void Physics_SpringForce::Apply( Physics_t fTime, Physics_LargeVector &masses, bool bDerivs,
								 Physics_LargeVector &p, Physics_LargeVector &v, 
								 Physics_LargeVector &f_int, Physics_LargeVector &f_ext,
								 Physics_SparseSymmetricMatrix &f_dp, Physics_SparseSymmetricMatrix &f_dv )
{
	Physics_Vector3 p1, p2, *vv[2], v1, dC_dp[2], force;
	Physics_Matrix3x3 dp, d2C_dp2[2][2];
	Physics_t C, C_Dot, len;
	int i, j;

	p1 = p.m_pData[m_iParticle[0]];
	p2 = p.m_pData[m_iParticle[1]];

	vv[0] = &v.m_pData[m_iParticle[0]];
	vv[1] = &v.m_pData[m_iParticle[1]];

	VECTOR3_SUB( p2, p1, v1 );

	len = VECTOR3_LENGTH( v1 );
	C = len - m_RestDistance;

	Physics_t cnst1 = (Physics_t)1.0/len;
	Physics_t cnst2 = (Physics_t)1.0/len/len/len;
	VECTOR3_SCALE( v1, -cnst1, dC_dp[0] );
	VECTOR3_SCALE( v1, cnst1, dC_dp[1] );

	C_Dot = dC_dp[0].x * vv[0]->x + 
			dC_dp[0].y * vv[0]->y + 
			dC_dp[0].z * vv[0]->z + 
			dC_dp[1].x * vv[1]->x + 
			dC_dp[1].y * vv[1]->y + 
			dC_dp[1].z * vv[1]->z;

	force.x = -m_kSpring * dC_dp[0].x * C - m_kSpringDamp * dC_dp[0].x * C_Dot;
	force.y = -m_kSpring * dC_dp[0].y * C - m_kSpringDamp * dC_dp[0].y * C_Dot;
	force.z = -m_kSpring * dC_dp[0].z * C - m_kSpringDamp * dC_dp[0].z * C_Dot;

	VECTOR3_ADD( f_int.m_pData[m_iParticle[0]], force, f_int.m_pData[m_iParticle[0]] );

	force.x = -m_kSpring * dC_dp[1].x * C - m_kSpringDamp * dC_dp[1].x * C_Dot;
	force.y = -m_kSpring * dC_dp[1].y * C - m_kSpringDamp * dC_dp[1].y * C_Dot;
	force.z = -m_kSpring * dC_dp[1].z * C - m_kSpringDamp * dC_dp[1].z * C_Dot;

	VECTOR3_ADD( f_int.m_pData[m_iParticle[1]], force, f_int.m_pData[m_iParticle[1]] );

	if( bDerivs )
	{
		d2C_dp2[0][0].m_Mx[0] = -cnst2 * ( (v1.x * v1.x) ) + cnst1 ;
		d2C_dp2[0][0].m_Mx[1] = 0;
		d2C_dp2[0][0].m_Mx[2] = 0;
		d2C_dp2[0][0].m_Mx[3] = 0;
		d2C_dp2[0][0].m_Mx[4] = -cnst2 * ( (v1.y * v1.y) ) + cnst1 ;
		d2C_dp2[0][0].m_Mx[5] = 0;
		d2C_dp2[0][0].m_Mx[6] = 0;
		d2C_dp2[0][0].m_Mx[7] = 0;
		d2C_dp2[0][0].m_Mx[8] = -cnst2 * ( (v1.z * v1.z) ) + cnst1 ;

		d2C_dp2[0][1].m_Mx[0] = cnst2 * ( (v1.x * v1.x) ) - cnst1 ;
		d2C_dp2[0][1].m_Mx[1] = 0;
		d2C_dp2[0][1].m_Mx[2] = 0;
		d2C_dp2[0][1].m_Mx[3] = 0;
		d2C_dp2[0][1].m_Mx[4] = cnst2 * ( (v1.y * v1.y) ) - cnst1 ;
		d2C_dp2[0][1].m_Mx[5] = 0;
		d2C_dp2[0][1].m_Mx[6] = 0;
		d2C_dp2[0][1].m_Mx[7] = 0;
		d2C_dp2[0][1].m_Mx[8] = cnst2 * ( (v1.z * v1.z) ) - cnst1 ;

		d2C_dp2[1][0].m_Mx[0] = cnst2 * ( (v1.x * v1.x) ) - cnst1 ;
		d2C_dp2[1][0].m_Mx[1] = 0;
		d2C_dp2[1][0].m_Mx[2] = 0;
		d2C_dp2[1][0].m_Mx[3] = 0;
		d2C_dp2[1][0].m_Mx[4] = cnst2 * ( (v1.y * v1.y) ) - cnst1 ;
		d2C_dp2[1][0].m_Mx[5] = 0;
		d2C_dp2[1][0].m_Mx[6] = 0;
		d2C_dp2[1][0].m_Mx[7] = 0;
		d2C_dp2[1][0].m_Mx[8] = cnst2 * ( (v1.z * v1.z) ) - cnst1 ;

		d2C_dp2[1][1].m_Mx[0] = -cnst2 * ( (v1.x * v1.x) ) + cnst1 ;
		d2C_dp2[1][1].m_Mx[1] = 0;
		d2C_dp2[1][1].m_Mx[2] = 0;
		d2C_dp2[1][1].m_Mx[3] = 0;
		d2C_dp2[1][1].m_Mx[4] = -cnst2 * ( (v1.y * v1.y) ) + cnst1 ;
		d2C_dp2[1][1].m_Mx[5] = 0;
		d2C_dp2[1][1].m_Mx[6] = 0;
		d2C_dp2[1][1].m_Mx[7] = 0;
		d2C_dp2[1][1].m_Mx[8] = -cnst2 * ( (v1.z * v1.z) ) + cnst1 ;

		Physics_Matrix3x3 df_dp, df_dv;

		for( i=0; i<2; i++ )
		{
			for( j=i; j<2; j++ )
			{
				dp.SetFromOuterProduct( dC_dp[i], dC_dp[j] );
				df_dp.m_Mx[0] = -m_kSpring * ( dp.m_Mx[0] + d2C_dp2[i][j].m_Mx[0] * C ) -
								 m_kSpringDamp * ( d2C_dp2[i][j].m_Mx[0] * C_Dot );
				df_dp.m_Mx[1] = -m_kSpring * ( dp.m_Mx[1] + d2C_dp2[i][j].m_Mx[1] * C ) -
								 m_kSpringDamp * ( d2C_dp2[i][j].m_Mx[1] * C_Dot );
				df_dp.m_Mx[2] = -m_kSpring * ( dp.m_Mx[2] + d2C_dp2[i][j].m_Mx[2] * C ) -
								 m_kSpringDamp * ( d2C_dp2[i][j].m_Mx[2] * C_Dot );
				df_dp.m_Mx[3] = -m_kSpring * ( dp.m_Mx[3] + d2C_dp2[i][j].m_Mx[3] * C ) -
								 m_kSpringDamp * ( d2C_dp2[i][j].m_Mx[3] * C_Dot );
				df_dp.m_Mx[4] = -m_kSpring * ( dp.m_Mx[4] + d2C_dp2[i][j].m_Mx[4] * C ) -
								 m_kSpringDamp * ( d2C_dp2[i][j].m_Mx[4] * C_Dot );
				df_dp.m_Mx[5] = -m_kSpring * ( dp.m_Mx[5] + d2C_dp2[i][j].m_Mx[5] * C ) -
								 m_kSpringDamp * ( d2C_dp2[i][j].m_Mx[5] * C_Dot );
				df_dp.m_Mx[6] = -m_kSpring * ( dp.m_Mx[6] + d2C_dp2[i][j].m_Mx[6] * C ) -
								 m_kSpringDamp * ( d2C_dp2[i][j].m_Mx[6] * C_Dot );
				df_dp.m_Mx[7] = -m_kSpring * ( dp.m_Mx[7] + d2C_dp2[i][j].m_Mx[7] * C ) -
								 m_kSpringDamp * ( d2C_dp2[i][j].m_Mx[7] * C_Dot );
				df_dp.m_Mx[8] = -m_kSpring * ( dp.m_Mx[8] + d2C_dp2[i][j].m_Mx[8] * C ) -
								 m_kSpringDamp * ( d2C_dp2[i][j].m_Mx[8] * C_Dot );

				f_dp(m_iParticle[i], m_iParticle[j]).Add( df_dp, f_dp(m_iParticle[i],m_iParticle[j]) );

				df_dv.m_Mx[0] = -m_kSpringDamp * dp.m_Mx[0];
				df_dv.m_Mx[1] = -m_kSpringDamp * dp.m_Mx[1];
				df_dv.m_Mx[2] = -m_kSpringDamp * dp.m_Mx[2];

				df_dv.m_Mx[3] = -m_kSpringDamp * dp.m_Mx[3];
				df_dv.m_Mx[4] = -m_kSpringDamp * dp.m_Mx[4];
				df_dv.m_Mx[5] = -m_kSpringDamp * dp.m_Mx[5];

				df_dv.m_Mx[6] = -m_kSpringDamp * dp.m_Mx[6];
				df_dv.m_Mx[7] = -m_kSpringDamp * dp.m_Mx[7];
				df_dv.m_Mx[8] = -m_kSpringDamp * dp.m_Mx[8];

				f_dv(m_iParticle[i], m_iParticle[j]).Add( df_dv, f_dv(m_iParticle[i],m_iParticle[j]) );
			}
		}
	}
 }

void Physics_SpringForce::PrepareMatrices( Physics_SymmetricMatrix &A, Physics_SparseSymmetricMatrix &B)
{
	Physics_t temp;

	A(m_iParticle[0], m_iParticle[1]) += m_kSpring;
	A(m_iParticle[0], m_iParticle[0]) -= m_kSpring;
	A(m_iParticle[1], m_iParticle[1]) -= m_kSpring;
	for( int i=0; i<2; i++ )
		for( int j=i; j<2; j++ )
		{
			temp = B(m_iParticle[i], m_iParticle[j]).m_Mx[0];
			B(m_iParticle[i], m_iParticle[j]).m_Mx[0] = temp * (Physics_t)2.0;
		}

}

void Physics_SpringForce::Fixup( Physics_LargeVector &invmasses, Physics_LargeVector &p )
{
	if( m_bFixup )
	{
		Physics_Vector3 Dist;
		Physics_t len;

		VECTOR3_SUB( p.m_pData[m_iParticle[0]], p.m_pData[m_iParticle[1]], Dist ); 
		len = Dist.Length();

		if( len > m_MaxDistance ) 
		{
			len -= m_MaxDistance;
			len /= 2;
			Dist.Normalize();
			VECTOR3_SCALE( Dist, len, Dist );
			if( invmasses.m_pData[ m_iParticle[0] ].x )
			{
				if( invmasses.m_pData[ m_iParticle[1] ].x )
				{
					p.m_pData[m_iParticle[0]].x -= Dist.x;
					p.m_pData[m_iParticle[0]].y -= Dist.y;
					p.m_pData[m_iParticle[0]].z -= Dist.z;
					p.m_pData[m_iParticle[1]].x += Dist.x;
					p.m_pData[m_iParticle[1]].y += Dist.y;
					p.m_pData[m_iParticle[1]].z += Dist.z;
				}
				else
				{
					p.m_pData[m_iParticle[0]].x -= Dist.x*(Physics_t)2.0;
					p.m_pData[m_iParticle[0]].y -= Dist.y*(Physics_t)2.0;
					p.m_pData[m_iParticle[0]].z -= Dist.z*(Physics_t)2.0;
				}
			}
			else
			{
				if( invmasses.m_pData[ m_iParticle[1] ].x )
				{
					p.m_pData[m_iParticle[1]].x += Dist.x*(Physics_t)2.0;
					p.m_pData[m_iParticle[1]].y += Dist.y*(Physics_t)2.0;
					p.m_pData[m_iParticle[1]].z += Dist.z*(Physics_t)2.0;
				}
			}
		}
	}
}
