// Physics_Physics_ParticleSystem.cpp: implementation of the Physics_Physics_ParticleSystem class.
//
//////////////////////////////////////////////////////////////////////

#include "DRGShell.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define FLOOR_Y	((Physics_t)-0.99)

Physics_ParticleSystem::Physics_ParticleSystem( int iParticles ) :
	m_Positions( iParticles ), m_Velocities( iParticles ),
	m_dv( iParticles ), m_dp( iParticles ), 
	m_vTemp1( iParticles ), m_vTemp2( iParticles ), m_PosTemp( iParticles ),
	m_TotalForces_int( iParticles ), m_TotalForces_ext( iParticles ),
	m_W( iParticles ), m_H( iParticles ),
	m_Masses( iParticles ), m_MassInv( iParticles ),
	m_MxMasses( iParticles, iParticles ),
	m_TotalForces_dp( iParticles, iParticles ),
	m_TotalForces_dv( iParticles, iParticles ),
	m_A( iParticles, iParticles ),
	m_P( iParticles ),
	m_PInv( iParticles ),
	m_MxTemp1( iParticles, iParticles ), m_MxTemp2( iParticles, iParticles ),
	m_z( iParticles ), m_b( iParticles ), m_r( iParticles ), 
	m_c( iParticles ), m_q( iParticles ), m_s( iParticles ), m_y( iParticles )
{
	m_iIntegrationMethod = INTEGRATE_SEMI_IMPLICIT;

	for( int i=0; i<iParticles; i++ )
	{
		m_Masses.m_pData[i] = Physics_Vector3( 1,1,1 );
		m_MassInv.m_pData[i] = Physics_Vector3( 1,1,1 );
		m_MxMasses(i,i).SetIdentity();
	}

	m_S = new Physics_Matrix3x3[iParticles];

	m_iParticles = iParticles;
	m_TotalForces_int.Zero();
	m_TotalForces_ext.Zero();
	m_LastStep = 1.0;
}

Physics_ParticleSystem::~Physics_ParticleSystem()
{
	SAFE_DELETE_ARRAY( m_S );
}

void Physics_ParticleSystem::Update( float fTime )
{
	switch( m_iIntegrationMethod )
	{
		case INTEGRATE_EXPLICIT:
			Update_Explicit( fTime );
			break;
		case INTEGRATE_IMPLICIT:
			Update_Implicit( fTime );
			break;
		case INTEGRATE_SEMI_IMPLICIT:
			Update_SemiImplicit( fTime );
			break;
	}
}

void Physics_ParticleSystem::SetupMatrices()
{
	void *Pos = NULL;
	Physics_Force *pForce = m_Forces.NextItem( &Pos );
	DWORD dwTimeIn = GetTickCount(), dwTimeOut;

	if( m_iIntegrationMethod != INTEGRATE_EXPLICIT )
	{
		m_H.Zero();
		while( pForce )
		{
			pForce->PrepareMatrices(m_H, m_TotalForces_dp);
			pForce = m_Forces.NextItem( &Pos );
		}	
	}

	//
	// Setup the implicit matrices
	//
	if( m_iIntegrationMethod == INTEGRATE_IMPLICIT )
	{
		m_TotalForces_dv.Copy( m_TotalForces_dp );
		m_A.Copy( m_TotalForces_dp );
		m_MxTemp1.Copy( m_TotalForces_dp );
		m_MxTemp2.Copy( m_TotalForces_dp );
	}

	//
	// Setup the semi-implicit matrices
	//
	if( m_iIntegrationMethod == INTEGRATE_SEMI_IMPLICIT )
	{
		m_W.SetIdentity();
		m_H.Scale( (Physics_t)1.0/m_ParticleMass, m_H );
		m_W.Subtract( m_H, m_W );
		m_W.Invert();
	}


	dwTimeOut = GetTickCount();
	char szTemp[50];
	sprintf( szTemp, "Initialize: %lu msec\r\n", dwTimeOut - dwTimeIn );
	OutputDebugString( szTemp );

}

//
// Use Explicit integration with Deformation Constraints
//
void Physics_ParticleSystem::Update_Explicit( float fTime )
{
	Physics_t fTotal = 0.0f, fStep = (Physics_t)fTime;
	Physics_Vector3 vCOG, dTorque, tmp, tmp2;
	int i;

	while( fTotal < fTime )
	{
		// 
		// Zero out everything
		//
		m_TotalForces_int.Zero();
		m_TotalForces_ext.Zero();


		//
		// Apply the forces
		//
		void *Pos = NULL;
		Physics_Force *pForce = m_Forces.NextItem( &Pos );
		while( pForce )
		{
			pForce->Apply(fStep, m_Masses, false, m_Positions, m_Velocities,
						  m_TotalForces_int, m_TotalForces_ext,
						  m_TotalForces_dp, m_TotalForces_dv );
			pForce = m_Forces.NextItem( &Pos );
		}


		//
		// Compute the new velocities and positions
		//
		m_TotalForces_ext.Add( m_TotalForces_int, m_vTemp1 );
		m_vTemp1.ElementMultiply( m_MassInv, m_dv );
		m_dv.Scale( fStep, m_dv );
		m_Velocities.Add( m_dv, m_Velocities );
		m_Velocities.Scale( fStep, m_vTemp1 );
		m_Positions.Add( m_vTemp1, m_PosTemp );

		//
		// Apply inverse dynamics to prevent excessive stretch
		// We need 10 or so iterations to be stable -- with more particles in the
		// mesh, we need more iterations.
		//
		for( i=0; i<m_iInverseIterations; i++ )
		{
			Pos = NULL;
			pForce = m_Forces.NextItem( &Pos );
			while( pForce )
			{
				pForce->Fixup( m_Masses, m_PosTemp );
				pForce = m_Forces.NextItem( &Pos );
			}
		}
		//
		// Update velocity and position
		//
		m_PosTemp.Subtract( m_Positions, m_vTemp1 );
		m_vTemp1.Scale( (Physics_t)1.0 / fStep, m_Velocities );
		for( i=0; i<m_iParticles; i++ )
			if( m_PosTemp.m_pData[i].y < FLOOR_Y )
			{
				m_PosTemp.m_pData[i].y = FLOOR_Y;
				m_Velocities.m_pData[i].y = 0;
			}
		m_Positions = m_PosTemp;

		fTotal += (Physics_t)fabs( fStep );
	}
}

void Physics_ParticleSystem::Update_Implicit( float fTime )
{
	int i, iIterations = 0, iMaxIterations = (int)sqrt(m_iParticles)*3+3;
	Physics_t fTotal = (Physics_t)0.0f, fStep = (Physics_t)fTime;
	Physics_t alpha, Delta_0, Delta_old, Delta_new;
	Physics_t Eps_Sq = (Physics_t)1e-22;

	while( fTotal < fTime )
	{
		// 
		// Zero out everything
		//
		m_TotalForces_int.Zero();
		m_TotalForces_ext.Zero();
		m_TotalForces_dp.Zero();
		m_TotalForces_dv.Zero();
		m_MxTemp1.Zero();
		m_MxTemp2.Zero();


		//
		// Apply the forces
		//
		void *Pos = NULL;
		Physics_Force *pForce = m_Forces.NextItem( &Pos );
		while( pForce )
		{
			pForce->Apply( fStep, m_Masses, true, m_Positions, m_Velocities, 
						   m_TotalForces_int, m_TotalForces_ext, 
						   m_TotalForces_dp, m_TotalForces_dv );
			pForce = m_Forces.NextItem( &Pos );
		}

		//
		// Form the symmetric matrix A = M - h * df/dv - h^2 * df/dx
		// We regroup this as A = M - h * (df/dv + h * df/dx)
		//
		m_TotalForces_int.Add( m_TotalForces_ext, m_TotalForces_int );
		m_TotalForces_dp.Scale( fStep, m_MxTemp1 );
		m_TotalForces_dv.Add( m_MxTemp1, m_MxTemp2 );
		m_MxTemp2.Scale( fStep, m_MxTemp1 );
		m_MxMasses.Subtract( m_MxTemp1, m_A );

		//
		// Compute b = h * ( f(0) + h * df/dx * v(0) + df/dx * y )
		//
		m_Velocities.Scale( fStep, m_vTemp2 );
		m_vTemp2.Add( m_y, m_vTemp2 );
		m_TotalForces_dp.PostMultiply( m_vTemp2, m_vTemp1 );
//		m_vTemp1.Scale( fStep, m_vTemp2 );
		m_TotalForces_int.Add( m_vTemp1, m_vTemp1 );
		m_vTemp1.Scale( fStep, m_b );


		//
		// Setup the inverse of preconditioner -- We use a vector for memory efficiency.  Technically it's the diagonal of a matrix
		//
		for( i=0; i<m_iParticles; i++ )
		{
			m_PInv.m_pData[i].x = (Physics_t)m_A(i,i).m_Mx[0];
			m_PInv.m_pData[i].y = (Physics_t)m_A(i,i).m_Mx[4];
			m_PInv.m_pData[i].z = (Physics_t)m_A(i,i).m_Mx[8];
		}
		m_PInv.Invert( m_P );

		//
		// Modified Preconditioned Conjugate Gradient method
		//

		m_dv = m_z;

		// delta_0 = DotProduct( filter( b ), P * filter( b ) );
		m_b.ElementMultiply( m_S, m_vTemp1 );
		m_P.ElementMultiply( m_vTemp1, m_vTemp2 );
		Delta_0 = m_vTemp2.DotProduct( m_vTemp1 );
		if( Delta_0 < 0 )
		{
			m_b.Dump( "b:\r\n" );
			m_P.Dump( "P:\r\n" );
			OutputDebugString( "Negative Delta_0 most likely caused by a non-Positive Definite matrix\r\n" );
		}

		// r = filter( b - A * dv )
		m_A.PostMultiply( m_dv, m_vTemp1 );
		m_b.Subtract( m_vTemp1, m_vTemp2 );
		m_vTemp2.ElementMultiply( m_S, m_r );

		// c = filter( Pinv * r )
		m_PInv.ElementMultiply( m_r, m_vTemp1 );
		m_vTemp1.ElementMultiply( m_S, m_c );

		Delta_new = m_r.DotProduct( m_c );

		if( Delta_new < Eps_Sq * Delta_0 )
		{
			m_b.Dump( "b: \r\n" );
			m_P.Dump( "P: \r\n" );
			OutputDebugString( "This isn't good!  Probably a non-Positive Definite matrix\r\n" );
		}

		while( (Delta_new > Eps_Sq * Delta_0) && (iIterations < iMaxIterations) )
		{
			m_A.PostMultiply( m_c, m_vTemp1 );

			m_vTemp1.ElementMultiply( m_S, m_q );

			alpha = Delta_new / (m_c.DotProduct( m_q ) );
			m_c.Scale( alpha, m_vTemp1 );
			m_dv.Add( m_vTemp1, m_dv );

			m_q.Scale( alpha, m_vTemp1 );
			m_r.Subtract( m_vTemp1, m_r );

			m_PInv.ElementMultiply( m_r, m_s );
			Delta_old = Delta_new;
			Delta_new = m_r.DotProduct( m_s );

			m_c.Scale( Delta_new / Delta_old, m_vTemp1 );
			m_s.Add( m_vTemp1, m_vTemp2 );
			m_vTemp2.ElementMultiply( m_S, m_c );
			
			iIterations++;
		}

		m_Velocities.Add( m_dv, m_Velocities );
		m_Velocities.Scale( fStep, m_vTemp1 );
		m_Positions.Add( m_vTemp1, m_Positions );
		for( i=0; i<m_iParticles; i++ )
			if( m_Positions.m_pData[i].y < FLOOR_Y )
			{
				//
				// Constraint the velocity of the particle to NOT move in the Y plane
				// Basically we modify the constraint matrix manually				 
				//
				m_S[i].SetIdentity();
				m_S[i].m_Mx[4] = 0;
				m_y.m_pData[i].y = (FLOOR_Y - m_Positions.m_pData[i].y);
				m_Positions.m_pData[i].y = FLOOR_Y;
				m_Velocities.m_pData[i].y = 0;
			}
			else
				m_y.m_pData[i].y = 0;
		fTotal += (Physics_t)fabs( fStep );
	}
}

void Physics_ParticleSystem::Update_SemiImplicit( float fTime )
{
	Physics_t fTotal = 0.0f, fStep = (Physics_t)fTime;
	Physics_Vector3 vCOG, dTorque, tmp, tmp2;
	int i;

	while( fTotal < fTime )
	{
		//
		// Calculate the center of gravity
		//
		vCOG.x = vCOG.y = vCOG.z = 0;
		for( i=0; i<m_iParticles; i++ )
			VECTOR3_ADD( vCOG, m_Positions.m_pData[i], vCOG );
		vCOG.x /= m_iParticles;
		vCOG.y /= m_iParticles;
		vCOG.z /= m_iParticles;

		dTorque.x = dTorque.y = dTorque.z = 0.0f;

		//
		// Update the W matrix if necessary
		//
		if( fStep != m_LastStep )
		{
			m_W.SetIdentity();
			m_H.Scale( fStep * fStep / m_LastStep / m_LastStep, m_H );
			m_W.Subtract( m_H, m_W );
			m_W.Invert();
			m_LastStep = fStep;
		}

		// 
		// Zero out everything
		//
		m_TotalForces_int.Zero();
		m_TotalForces_ext.Zero();


		//
		// Apply the forces
		//
		void *Pos = NULL;
		Physics_Force *pForce = m_Forces.NextItem( &Pos );
		while( pForce )
		{
			pForce->Apply(fStep, m_Masses, false, m_Positions, m_Velocities,
						  m_TotalForces_int, m_TotalForces_ext,
						  m_TotalForces_dp, m_TotalForces_dv );
			pForce = m_Forces.NextItem( &Pos );
		}

		//
		// Filter the internal forces
		//
		m_W.PreMultiply( m_TotalForces_int, m_vTemp1 );

		//
		// Update the torque
		//
		for( i=0; i<m_iParticles; i++ )
		{
			VECTOR3_CROSSPRODUCT( m_vTemp1.m_pData[i], m_Positions.m_pData[i], tmp );
			VECTOR3_ADD( dTorque, tmp, dTorque );
		}


		//
		// Compute the new velocities and positions
		//
		m_vTemp1.Add( m_TotalForces_ext, m_dv );
		m_dv.Scale( fStep, m_dv );
		m_dv.ElementMultiply( m_MassInv, m_dv );
		m_Velocities.Add( m_dv, m_Velocities );
		m_Velocities.Scale( fStep, m_vTemp1 );
		m_Positions.Add( m_vTemp1, m_PosTemp );

		//
		// Keep us above the floor -- cheesey collision detection
		//
		for( i=0; i<m_iParticles; i++ )
			if( m_PosTemp.m_pData[i].y < FLOOR_Y )
			{
				m_PosTemp.m_pData[i].y = FLOOR_Y;
				m_Velocities.m_pData[i].y = 0;
			}

		//
		// Post correct for angular momentum
		//

		for( i=0; i<m_iParticles; i++ )
		{
			if( m_Masses.m_pData[i].x )
			{
				VECTOR3_SUB( vCOG, m_Positions.m_pData[i], tmp2 );
				VECTOR3_CROSSPRODUCT( tmp2, dTorque, tmp );
				VECTOR3_SCALE( tmp, fStep * fStep * fStep * m_MassInv.m_pData[i].x, tmp );
				VECTOR3_ADD( m_PosTemp.m_pData[i], tmp, m_PosTemp.m_pData[i] );
				if( m_PosTemp.m_pData[i].y < FLOOR_Y )
				{
					m_PosTemp.m_pData[i].y = FLOOR_Y;
					m_Velocities.m_pData[i].y = 0;
				}
			}
		}

		//
		// Apply inverse dynamics to prevent excessive stretch
		// We need 10 or so iterations to be stable -- with more particles in the
		// mesh, we need more iterations.
		//
		for( i=0; i<m_iInverseIterations; i++ )
		{
			Pos = NULL;
			pForce = m_Forces.NextItem( &Pos );
			while( pForce )
			{
				pForce->Fixup( m_Masses, m_PosTemp );
				pForce = m_Forces.NextItem( &Pos );
			}
		}
		//
		// Update velocity and position
		//
		for( i=0; i<m_iParticles; i++ )
			if( m_PosTemp.m_pData[i].y < FLOOR_Y )
			{
				m_PosTemp.m_pData[i].y = FLOOR_Y;
				m_Velocities.m_pData[i].y = 0;
			}
		m_PosTemp.Subtract( m_Positions, m_vTemp1 );
		m_vTemp1.Scale( (Physics_t)1.0 / fStep, m_Velocities );
		m_Positions = m_PosTemp;

		fTotal += (Physics_t)fabs( fStep );
	}
}

void Physics_ParticleSystem::AddConstraint( Physics_Constraint &Constraint )
{
	m_Constraints.AddItem( &Constraint );

	//
	// Apply the constraints
	//
	for( int i=0; i<m_iParticles; i++ )
	{
		m_S[i].SetIdentity();
	}
	m_z.Zero();


	void *Pos = NULL;
	Physics_Constraint *pConstraint = m_Constraints.NextItem( &Pos );
	while( pConstraint )
	{
		pConstraint->Apply(m_S, m_z);
		pConstraint = m_Constraints.NextItem( &Pos );
	}
}

bool Physics_ParticleSystem::DeleteConstraint( Physics_Constraint &Constraint )
{
	bool bResult = m_Constraints.DeleteItem( &Constraint );

	if( bResult )
	{
		//
		// Apply the constraints
		//
		for( int i=0; i<m_iParticles; i++ )
		{
			m_S[i].SetIdentity();
		}
		m_z.Zero();


		void *Pos = NULL;
		Physics_Constraint *pConstraint = m_Constraints.NextItem( &Pos );
		while( pConstraint )
		{
			pConstraint->Apply(m_S, m_z);
			pConstraint = m_Constraints.NextItem( &Pos );
		}
	}
	return bResult;
}

void Physics_ParticleSystem::Reset()
{
	//
	// Update the constraints for the Implicit integration scheme
	//
	if( m_iIntegrationMethod == INTEGRATE_IMPLICIT )
	{
		for( int i=0; i<m_iParticles; i++ )
		{
			m_S[i].SetIdentity();
		}
		m_z.Zero();


		void *Pos = NULL;
		Physics_Constraint *pConstraint = m_Constraints.NextItem( &Pos );
		while( pConstraint )
		{
			pConstraint->Apply(m_S, m_z);
			pConstraint = m_Constraints.NextItem( &Pos );
		}
	}
}