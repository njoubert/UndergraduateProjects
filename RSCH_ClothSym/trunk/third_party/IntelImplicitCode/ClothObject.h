// ClothObject.h: interface for the CClothObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLOTHOBJECT_H__A5325BC4_F6CE_4079_B6A6_EA2503AD26CE__INCLUDED_)
#define AFX_CLOTHOBJECT_H__A5325BC4_F6CE_4079_B6A6_EA2503AD26CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CClothObject : public CDRGObject  
{
private:

	Physics_ParticleSystem *m_pSystem;

	int m_iInterateMethod;

	int m_iConstrainedParticle[4];
	Physics_Constraint *m_pConstraint[4];
	bool m_bConstraint[4];

	//Physics_WindForce *m_pWind;
	DRGVector m_Pos;
	Physics_t m_fWidth, m_fHeight;
	Physics_t m_ParticleMass;

	int m_iClothParticles;
	int m_iRowPoints, m_iColPoints;
	bool m_bWireframe, m_bWind;

	LPDIRECT3DVERTEXBUFFER7 m_pVertexBufferCopies[2];
	int m_iCurrentBuffer;

	Physics_t m_fStep;

	CConfiguration *m_Config;

	virtual HRESULT SetupIndexedVertexBuffer();

public:
	CClothObject();
	virtual ~CClothObject();

	HRESULT Init(	CDRGWrapper *pDRGWrapper,
					CConfiguration &Config,
					DRGVector	pos				= DRGVector(0.0f),
					DRGVector	scale			= DRGVector(1.0f),
					DRGVector	rotationaxis	= DRGVector(0.0f,1.0f,0.0f),
					float		rotationangle	= 0.0f,
					DWORD		dwFlags			= 0 );
	
	virtual HRESULT	Update(float	fTime, bool bUpdateChildren = true);
	virtual HRESULT Render(DRGMatrix& MxWorld, bool bRenderChildren = true );
	virtual HRESULT TargetChanged(CDRGWrapper *pDRGWrapper, bool bUpdateChildren = true);

	void ToggleConstraint( int i );
	bool ToggleWireframe();
	void Reset( void );
};

#endif // !defined(AFX_CLOTHOBJECT_H__A5325BC4_F6CE_4079_B6A6_EA2503AD26CE__INCLUDED_)
