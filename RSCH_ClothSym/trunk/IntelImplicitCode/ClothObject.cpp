// ClothObject.cpp: implementation of the CClothObject class.
//
//////////////////////////////////////////////////////////////////////

#include "DRGShell.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CClothObject::CClothObject()
{
	m_iCurrentBuffer = 0;
	m_pVertexBufferCopies[0] = NULL;
	m_pVertexBufferCopies[1] = NULL;
	m_bConstraint[0] = m_bConstraint[1] = m_bConstraint[2] = m_bConstraint[3] = false;
	m_bWireframe = false;
}

CClothObject::~CClothObject()
{
	m_pIndexedVertexBuffer = NULL;
	SAFE_RELEASE( m_pVertexBufferCopies[0] );
	SAFE_RELEASE( m_pVertexBufferCopies[1] );
}

HRESULT CClothObject::Init(	CDRGWrapper *pDRGWrapper, CConfiguration &Config, 
							DRGVector pos, DRGVector scale,
							DRGVector rotationaxis, float rotationangle,
							DWORD dwFlags )
{
	int i, j,idx, iRowPoints, iColPoints;

	//
	// Create a rectangular cloth
	//
	m_Config = &Config;
	iRowPoints = Config.m_iRowPoints;
	iColPoints = Config.m_iColPoints;
	m_iRowPoints = iRowPoints;
	m_iColPoints = iColPoints;
	m_fWidth = scale.x;
	m_fHeight = scale.z;
	m_Pos = pos;
	m_iClothParticles = iRowPoints * iColPoints;

	m_pSystem = new Physics_ParticleSystem(m_iClothParticles);
	m_pSystem->m_iInverseIterations = Config.m_iInverseIterations;
	m_pSystem->m_iIntegrationMethod = Config.m_iIntegrationMethod;

	m_iNumVertices = iRowPoints * iColPoints;
	m_iNumIndices = 6*(iRowPoints-1)*(iColPoints-1);
	m_pVertices = NULL;	// We don't allocate any because we're storing the information elsewhere
	m_pIndices = new WORD[ m_iNumIndices ];
	m_d3dPrimType = D3DPT_TRIANGLELIST;

	idx = 0;
	Physics_t Area = (Physics_t)(m_fWidth / (Physics_t)(iColPoints-1)) * (m_fHeight / (Physics_t)(iRowPoints-1)) / (Physics_t)2.0;
	m_ParticleMass = Config.m_ParticleMass;
	for( i=0; i<iRowPoints; i++ )
	{
		for( j=0; j<iColPoints; j++ )
		{
			m_pSystem->m_Positions.m_pData[idx].x = pos.x + ((Physics_t)j/(Physics_t)(iColPoints-1) - (Physics_t)0.5) * m_fWidth;
			m_pSystem->m_Positions.m_pData[idx].y = pos.y;// + ((Physics_t)0.5-(Physics_t)i/(Physics_t)(iRowPoints-1)) * fHeight;
			m_pSystem->m_Positions.m_pData[idx].z = pos.z + ((Physics_t)0.5-(Physics_t)i/(Physics_t)(iRowPoints-1)) * m_fHeight;
			m_pSystem->SetMass( idx, Config.m_ParticleMass );
			idx++;
		}
	}

	//
	// Setup the indices
	//
	idx = 0;
	for( i=0; i<iRowPoints-1; i++ )
	{
		for( j=0; j<iColPoints-1; j++ )
		{
			m_pIndices[idx+0] = i*iColPoints+j;
			m_pIndices[idx+1] = (i+1)*iColPoints+j;
			m_pIndices[idx+2] = i*iColPoints+j+1;

			m_pIndices[idx+3] = i*iColPoints+j+1;
			m_pIndices[idx+4] = (i+1)*iColPoints+j;
			m_pIndices[idx+5] = (i+1)*iColPoints+j+1;

			idx+=6;
		}
	}

	//
	// Add some springs
	//
	bool bStretch = Config.m_bUseStretch;
	bool bShear = Config.m_bUseShear;
	bool bBend = Config.m_bUseBend;

	Physics_SpringForce *pStretch;
	Physics_SpringForce *pShear;
	Physics_SpringForce *pBend;
	Physics_t fColDist = (Physics_t)(m_fWidth / (Physics_t)(iColPoints-1));
	Physics_t fRowDist = (Physics_t)(m_fHeight / (Physics_t)(iRowPoints-1));
	Physics_t fDiagDist = (Physics_t)sqrt( fColDist * fColDist + fRowDist * fRowDist );
	Physics_t kStretch = Config.m_kStretch; 
	Physics_t kStretchDamp = Config.m_StretchDampPct * kStretch;
	Physics_t kShear = Config.m_kShear; 
	Physics_t kShearDamp = Config.m_ShearDampPct * kShear;
	Physics_t kBend = Config.m_kBend; 
	Physics_t kBendDamp = Config.m_BendDampPct * kBend;
	Physics_t bU = 1; 
	Physics_t bV = 1; 
	Physics_Vector3 dir;
	m_fStep = Config.m_fStepSize;

	for( i=0; i<iRowPoints; i++ )
	{
		for( j=0; j<iColPoints; j++ )
		{

			if( bStretch  )
			{
				if( i>0 )
				{
					pStretch = new Physics_SpringForce();
					pStretch->m_iParticle[0] = (i-1)*iColPoints+j;
					pStretch->m_iParticle[1] = i*iColPoints+j;
					pStretch->m_RestDistance = fRowDist;
					pStretch->m_MaxDistance = fRowDist * Config.m_StretchLimitPct;
					pStretch->m_bFixup = true;
					pStretch->m_kSpring = kStretch;
					pStretch->m_kSpringDamp = kStretchDamp;
					m_pSystem->AddForce( *pStretch );
				}
				if( j>0 )
				{
					pStretch = new Physics_SpringForce();
					pStretch->m_iParticle[0] = i*iColPoints+j-1;
					pStretch->m_iParticle[1] = i*iColPoints+j;
					pStretch->m_RestDistance = fColDist;
					pStretch->m_MaxDistance = fColDist * Config.m_StretchLimitPct;
					pStretch->m_bFixup = true;
					pStretch->m_kSpring = kStretch;
					pStretch->m_kSpringDamp = kStretchDamp;
					m_pSystem->AddForce( *pStretch );
				}
			}

			if( bShear  )
			{
				if( i>0 )
				{
					if( j>0 )
					{
						pShear = new Physics_SpringForce();
						pShear->m_iParticle[0] = (i-1)*iColPoints+j-1;
						pShear->m_iParticle[1] = i*iColPoints+j;
						pShear->m_RestDistance = fDiagDist;
						pShear->m_MaxDistance = fDiagDist * Config.m_StretchLimitPct;
						pShear->m_bFixup = true;
						pShear->m_kSpring = kShear;
						pShear->m_kSpringDamp = kShearDamp;
						m_pSystem->AddForce( *pShear );
						if( i<iRowPoints-1)
						{
							pShear = new Physics_SpringForce();
							pShear->m_iParticle[0] = (i+1)*iColPoints+j-1;
							pShear->m_iParticle[1] = i*iColPoints+j;
							pShear->m_RestDistance = fDiagDist;
							pShear->m_MaxDistance = fDiagDist * Config.m_StretchLimitPct;
							pShear->m_bFixup = true;
							pShear->m_kSpring = kShear;
							pShear->m_kSpringDamp = kShearDamp;
							m_pSystem->AddForce( *pShear );
						}
					}
				}
				else if( j>0 )
				{
					pShear = new Physics_SpringForce();
					pShear->m_iParticle[0] = (i+1)*iColPoints+j-1;
					pShear->m_iParticle[1] = i*iColPoints+j;
					pShear->m_RestDistance = fDiagDist;
					pShear->m_MaxDistance = fDiagDist * Config.m_StretchLimitPct;
					pShear->m_bFixup = true;
					pShear->m_kSpring = kShear;
					pShear->m_kSpringDamp = kShearDamp;
					m_pSystem->AddForce( *pShear );
				}
			}
			if( bBend )
			{
				if( i<iRowPoints-2)
				{
					pBend = new Physics_SpringForce();
					pBend->m_iParticle[0] = i*iColPoints+j;
					pBend->m_iParticle[1] = (i+2)*iColPoints+j;
					pBend->m_RestDistance = 2*fRowDist;
					pBend->m_kSpring = kBend;
					pBend->m_kSpringDamp = kBendDamp;
					m_pSystem->AddForce( *pBend );
				}
				if( j<iColPoints-2)
				{
					pBend = new Physics_SpringForce();
					pBend->m_iParticle[0] = i*iColPoints+j;
					pBend->m_iParticle[1] = i*iColPoints+j+2;
					pBend->m_RestDistance = 2*fColDist;
					pBend->m_kSpring = kBend;
					pBend->m_kSpringDamp = kBendDamp;
					m_pSystem->AddForce( *pBend );
				}
			}
		}
	}


	//
	// Add a constraint
	//
	m_iConstrainedParticle[0] = 0;
	m_iConstrainedParticle[1] = iColPoints-1;
	m_iConstrainedParticle[2] = m_iClothParticles-iColPoints;
	m_iConstrainedParticle[3] = m_iClothParticles-1;
	for( i=0; i<4; i++ )
	{
		m_pConstraint[i] = new Physics_Constraint( m_iConstrainedParticle[i], 0, Physics_Vector3( 0, 1, 0 ) );
		if( Config.m_iConstraints[i] )
		{
			m_bConstraint[i] = true;
			m_pSystem->AddConstraint( *m_pConstraint[i] );
			if( Config.m_iIntegrationMethod != 1 )
				m_pSystem->SetMass( m_iConstrainedParticle[i], 0 );
		}
		else 
			m_bConstraint[i] = false;
	}
	m_pSystem->SetupMatrices();
	//
	// Add Gravity
	//
	Physics_GravityForce *pGravity = new Physics_GravityForce( Physics_Vector3( 0, (Physics_t)-9.8, 0 ) );
	m_pSystem->AddForce( *pGravity );

	m_pWrapper = pDRGWrapper;
	SetupIndexedVertexBuffer();

	return S_OK;
}

HRESULT	CClothObject::Update(float	fTimeStep, bool bUpdateChildren )
{
	HRESULT hr;

	//
	// Update the children (rather than calling the parent)
	//
	if (bUpdateChildren)
	{
		CDRGObject	*pTempObject = m_pFirstChild;
		while (pTempObject != NULL)
		{
			pTempObject->Update(fTimeStep, true);
			pTempObject = pTempObject->m_pNext;
		}
	}
//	if( FAILED( hr = CDRGObject::Update( fTime, bUpdateChildren ) ) )
//		return hr;

	//
	// Lock and update our vertex buffers
	//
	D3DVERTEX	*pTemp;
	if ( FAILED( hr = m_pVertexBufferCopies[m_iCurrentBuffer]->Lock( DDLOCK_WAIT, (VOID **)&pTemp, NULL ) ) )
		return hr;

	fTimeStep = (float)m_fStep;
	if( m_pSystem )
		m_pSystem->Update( fTimeStep );

	{
		int idx=0, i, j;
		bool ig, il, jg, jl;
		D3DVECTOR p1, p2, p3, p4, p5, p6, p7;
		D3DVECTOR n1, n2, n3, n4, n5, n6, n;

		//
		// Compute particle normals
		//
		for( i=0; i<m_iRowPoints; i++ )
		{
			for( j=0; j<m_iColPoints; j++ )
			{
				idx = i*m_iColPoints+j;
				ig = (i > 0);
				il = (i < m_iRowPoints-1);
				jg = (j > 0);
				jl = (j < m_iColPoints-1);
				if( ig )
				{
					p1.x = (float)m_pSystem->m_Positions.m_pData[idx-m_iColPoints].x;
					p1.y = (float)m_pSystem->m_Positions.m_pData[idx-m_iColPoints].y;
					p1.z = (float)m_pSystem->m_Positions.m_pData[idx-m_iColPoints].z;
					if( jl )
					{
						p2.x = (float)m_pSystem->m_Positions.m_pData[idx-m_iColPoints+1].x;
						p2.y = (float)m_pSystem->m_Positions.m_pData[idx-m_iColPoints+1].y;
						p2.z = (float)m_pSystem->m_Positions.m_pData[idx-m_iColPoints+1].z;
					}
				}
				if( jg )
				{
					p3.x = (float)m_pSystem->m_Positions.m_pData[idx-1].x;
					p3.y = (float)m_pSystem->m_Positions.m_pData[idx-1].y;
					p3.z = (float)m_pSystem->m_Positions.m_pData[idx-1].z;
					if( il )
					{
						p6.x = (float)m_pSystem->m_Positions.m_pData[idx+m_iColPoints-1].x;
						p6.y = (float)m_pSystem->m_Positions.m_pData[idx+m_iColPoints-1].y;
						p6.z = (float)m_pSystem->m_Positions.m_pData[idx+m_iColPoints-1].z;
					}
				}
				p4.x = (float)m_pSystem->m_Positions.m_pData[idx].x;
				p4.y = (float)m_pSystem->m_Positions.m_pData[idx].y;
				p4.z = (float)m_pSystem->m_Positions.m_pData[idx].z;
				if( jl )
				{
					p5.x = (float)m_pSystem->m_Positions.m_pData[idx+1].x;
					p5.y = (float)m_pSystem->m_Positions.m_pData[idx+1].y;
					p5.z = (float)m_pSystem->m_Positions.m_pData[idx+1].z;
				}
				if( il )
				{
					p7.x = (float)m_pSystem->m_Positions.m_pData[idx+m_iColPoints].x;
					p7.y = (float)m_pSystem->m_Positions.m_pData[idx+m_iColPoints].y;
					p7.z = (float)m_pSystem->m_Positions.m_pData[idx+m_iColPoints].z;
				}

				n1 = n2 = n3 = n4 = n5 = n6 = D3DVECTOR(0,0,0);
				if( ig & jg )
					n1 = Normalize( CrossProduct( p3-p4,p1-p4 ) );
				if( ig & jl )
				{
					n2 = Normalize( CrossProduct( p2-p1,p4-p1 ) );
					n3 = Normalize( CrossProduct( p4-p5,p2-p5 ) );
				}
				if( il & jg )
				{
					n4 = Normalize( CrossProduct( p4-p3,p6-p3 ) );
					n5 = Normalize( CrossProduct( p6-p7,p4-p7 ) );
				}
				if( il & jl )
					n6 = Normalize( CrossProduct( p5-p4,p7-p4 ) );

				n = Normalize( (n1+n2+n3+n4+n5+n6) );
				pTemp[idx].nx = n.x;
				pTemp[idx].ny = n.y;
				pTemp[idx].nz = n.z;
			}
		}
	
		Physics_Vector3 vForce;
		idx = 0;
		for( i=0; i<m_iRowPoints; i++ )
		{
			for( j=0; j<m_iColPoints; j++ )
			{
				pTemp[idx].x = (float)m_pSystem->m_Positions.m_pData[idx].x;
				pTemp[idx].y = (float)m_pSystem->m_Positions.m_pData[idx].y;
				pTemp[idx].z = (float)m_pSystem->m_Positions.m_pData[idx].z;
				pTemp[idx].tu = (float)j / (float)(m_iColPoints-1);
				pTemp[idx].tv = (float)i / (float)(m_iRowPoints-1);


				idx++;
			}
		}

	}

	if ( FAILED( hr = m_pVertexBufferCopies[m_iCurrentBuffer]->Unlock() ) )
		return hr;

	return S_OK;		
}

HRESULT CClothObject::Render(DRGMatrix& MxWorld, bool bRenderChildren )
{
	HRESULT hr;

	//
	// We do double-buffering on the vertex buffer
	//
	m_pIndexedVertexBuffer = m_pVertexBufferCopies[m_iCurrentBuffer];

	if( m_bWireframe )
		m_pWrapper->m_pD3DDevice7->SetRenderState( D3DRENDERSTATE_FILLMODE, D3DFILL_WIREFRAME );
	if( FAILED( hr = CDRGObject::Render( MxWorld, bRenderChildren ) ) )
		return hr;

	m_pWrapper->m_pD3DDevice7->SetTransform( D3DTRANSFORMSTATE_WORLD, &m_MxLocalToWorld.m_Mx );
	m_pWrapper->m_pD3DDevice7->SetTexture(0,NULL);
	m_pWrapper->m_pD3DDevice7->SetTexture(1,NULL);
	m_pWrapper->m_pD3DDevice7->SetTexture(2,NULL);
	m_pWrapper->m_pD3DDevice7->SetRenderState(D3DRENDERSTATE_LIGHTING, FALSE);
	
	if( m_bWireframe )
		m_pWrapper->m_pD3DDevice7->SetRenderState( D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID );


	m_iCurrentBuffer = 1 - m_iCurrentBuffer;

	return S_OK;
}

HRESULT CClothObject::TargetChanged(CDRGWrapper *pDRGWrapper, bool bUpdateChildren )
{
	m_pWrapper = pDRGWrapper;

	// setup VB's
	if(m_iNumVertices)
	{
		if (SetupIndexedVertexBuffer() != S_OK)
		{
			OutputDebugString("unable to set up VB");
			return E_FAIL;
		}
	}

	//recursively refresh children
	if (bUpdateChildren)
	{
		CDRGObject	*pTempObject = m_pFirstChild;
		while (pTempObject != NULL)
		{
			pTempObject->TargetChanged(pDRGWrapper, bUpdateChildren);
			pTempObject = pTempObject->m_pNext;
		}
	}

	return S_OK;
}

//
// We might need multiple vertex buffers so we overloaded this function
//
HRESULT CClothObject::SetupIndexedVertexBuffer()
{
	HRESULT hr;
	SAFE_RELEASE(m_pVertexBufferCopies[0]);
	SAFE_RELEASE(m_pVertexBufferCopies[1]);
	
	// Define the vertex buffer descriptor for creating the
	// vertex buffer for DrawIndexedPrimitiveVB
    D3DVERTEXBUFFERDESC vbIndexedDesc, vbIndexedDesc2;
    ZeroMemory(&vbIndexedDesc, sizeof(D3DVERTEXBUFFERDESC));
    vbIndexedDesc.dwSize        = sizeof(D3DVERTEXBUFFERDESC);
    ZeroMemory(&vbIndexedDesc2, sizeof(D3DVERTEXBUFFERDESC));
    vbIndexedDesc2.dwSize        = sizeof(D3DVERTEXBUFFERDESC);
	
	// need to specify SysMem if on the RGB software renderer or regular HAL
	if ((m_pWrapper->m_bHardware) && 
		(m_pWrapper->m_pCurrentDevice->ddDeviceDesc.dwDevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ))
	{
		//for TnL HAL, need to let the driver decide where to put it
		vbIndexedDesc.dwCaps        = 0l;
		vbIndexedDesc2.dwCaps        = 0l;
	}
	else
	{
		//for regular hal, or rgb sw, need to specify sysmem
		vbIndexedDesc.dwCaps        = D3DVBCAPS_SYSTEMMEMORY ;
		vbIndexedDesc2.dwCaps        = D3DVBCAPS_SYSTEMMEMORY ;
	}
	
    vbIndexedDesc.dwFVF         = D3DFVF_VERTEX;
    vbIndexedDesc2.dwFVF         = D3DFVF_LVERTEX;

    vbIndexedDesc.dwNumVertices = m_iNumVertices;
    vbIndexedDesc2.dwNumVertices = m_iNumVertices*2;
    
    // Create a vertex buffer using D3D7 interface
	if(m_pWrapper)
	{
		if (m_pWrapper->m_pD3D7)
		{
			if ( FAILED( hr = m_pWrapper->m_pD3D7->CreateVertexBuffer(&vbIndexedDesc, 
																&m_pVertexBufferCopies[0], 0)))
				return hr;
			if ( FAILED( hr = m_pWrapper->m_pD3D7->CreateVertexBuffer(&vbIndexedDesc, 
																&m_pVertexBufferCopies[1], 0)))
				return hr;
		}
 
	}

	return S_OK;
}

void CClothObject::ToggleConstraint( int i )
{
	m_bConstraint[i] = !m_bConstraint[i];
	if( m_bConstraint[i] )
	{
		m_pSystem->Velocity(m_iConstrainedParticle[i]).x = 0;
		m_pSystem->Velocity(m_iConstrainedParticle[i]).y = 0;
		m_pSystem->Velocity(m_iConstrainedParticle[i]).z = 0;
		m_pSystem->AddConstraint( *m_pConstraint[i] );
		if( m_Config->m_iIntegrationMethod != 1 )
		m_pSystem->SetMass( m_iConstrainedParticle[i], 0 );
	}
	else
	{
		m_pSystem->DeleteConstraint( *m_pConstraint[i] );
		m_pSystem->SetMass( m_iConstrainedParticle[i], m_ParticleMass );
	}
}

bool CClothObject::ToggleWireframe()
{
	m_bWireframe = !m_bWireframe;
	return m_bWireframe;
}

void CClothObject::Reset()
{
	int i,j, idx=0;

	m_pSystem->Reset();
	for( i=0; i<m_iRowPoints; i++ )
	{
		for( j=0; j<m_iColPoints; j++ )
		{
			m_pSystem->m_Positions.m_pData[idx].x = m_Pos.x + ((Physics_t)j/(Physics_t)(m_iColPoints-1) - (Physics_t)0.5) * m_fWidth;
			m_pSystem->m_Positions.m_pData[idx].y = m_Pos.y;
			m_pSystem->m_Positions.m_pData[idx].z = m_Pos.z + ((Physics_t)0.5-(Physics_t)i/(Physics_t)(m_iRowPoints-1)) * m_fHeight;
			m_pSystem->m_Velocities.Zero();
			idx++;
		}
	}
}