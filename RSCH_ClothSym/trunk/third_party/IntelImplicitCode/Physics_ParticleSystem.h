// Physics_ParticleSystem.h: interface for the Physics_ParticleSystem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PHYSICS_PARTICLESYSTEM_H__76E37C60_EAA1_49C0_A098_3772AE8AA64C__INCLUDED_)
#define AFX_PHYSICS_PARTICLESYSTEM_H__76E37C60_EAA1_49C0_A098_3772AE8AA64C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define INTEGRATE_EXPLICIT			0
#define INTEGRATE_IMPLICIT			1
#define INTEGRATE_SEMI_IMPLICIT		2

class Physics_ParticleSystem  
{
public:
	int m_iParticles, m_iInverseIterations;
	Physics_LargeVector m_Positions;
	Physics_LargeVector m_Velocities;
	Physics_LargeVector m_TotalForces_int, m_TotalForces_ext;

private:
	Physics_LargeVector m_vTemp1, m_vTemp2, m_PosTemp;
	Physics_LargeVector m_dv;
	Physics_LargeVector m_dp;
	
	Physics_LargeVector m_Masses, m_MassInv;

	//
	// This stuff is for the semi-Implicit Desbrun method
	//
	Physics_SymmetricMatrix m_H, m_W;
	Physics_t m_LastStep, m_ParticleMass;

	//
	// This stuff is for the Implicit implementation
	//
	Physics_SparseSymmetricMatrix m_MxMasses;
	Physics_SparseSymmetricMatrix m_TotalForces_dp;
	Physics_SparseSymmetricMatrix m_TotalForces_dv;

	Physics_SparseSymmetricMatrix m_A;
	Physics_SparseSymmetricMatrix m_MxTemp1, m_MxTemp2;
	Physics_LargeVector m_P, m_PInv;
	Physics_LargeVector m_z, m_b, m_r, m_c, m_q, m_s, m_y;
	Physics_Matrix3x3 *m_S;

#pragma warning(disable:4251)
	LinkedList<Physics_Force> m_Forces;
	LinkedList<Physics_Constraint> m_Constraints;
#pragma warning(default:4251)


public:
	int m_iIntegrationMethod;

	void SetupMatrices();
	Physics_ParticleSystem( int iParticles );
	virtual ~Physics_ParticleSystem();

	inline Physics_Vector3 &Position( int iParticle )
	{
		return m_Positions.m_pData[iParticle];
	}

	inline Physics_Vector3 &Velocity( int iParticle )
	{
		return m_Velocities.m_pData[iParticle];
	}

	inline Physics_Vector3 GetMass( int iParticle )
	{
		return m_Masses.m_pData[iParticle];
	}

	inline void SetMass( int iParticle, Physics_t mass )
	{
		m_Masses.m_pData[iParticle] = Physics_Vector3( mass, mass, mass );
		if( mass > (Physics_t)0.0 )
		{
			m_ParticleMass = mass;
			m_MassInv.m_pData[iParticle].x = (Physics_t)1.0 / mass;
			m_MassInv.m_pData[iParticle].y = (Physics_t)1.0 / mass;
			m_MassInv.m_pData[iParticle].z = (Physics_t)1.0 / mass;
		}
		else
		{
			m_MassInv.m_pData[iParticle].x = (Physics_t)0;
			m_MassInv.m_pData[iParticle].y = (Physics_t)0;
			m_MassInv.m_pData[iParticle].z = (Physics_t)0;
		}
	}

	inline Physics_LargeVector &GetPositions()
	{
		return m_Positions;
	}

	void AddForce( Physics_Force &Force )
	{
		m_Forces.AddItem( &Force );
	}

	bool DeleteForce( Physics_Force &Force )
	{
		return m_Forces.DeleteItem( &Force );
	}

	void AddConstraint( Physics_Constraint &Constraint );
	bool DeleteConstraint( Physics_Constraint &Constraint );

	void Update( float fTime );

	void Update_Explicit( float fTime );
	void Update_Implicit( float fTime );
	void Update_SemiImplicit( float fTime );
	void Reset();
};

#endif // !defined(AFX_PHYSICS_PARTICLESYSTEM_H__76E37C60_EAA1_49C0_A098_3772AE8AA64C__INCLUDED_)
