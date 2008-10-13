// Configuration.h: interface for the CConfiguration class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGURATION_H__B4786515_75E6_442F_A9D6_E16C224C1108__INCLUDED_)
#define AFX_CONFIGURATION_H__B4786515_75E6_442F_A9D6_E16C224C1108__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CConfiguration  
{
public:
	CConfiguration( char *szConfig );
	virtual ~CConfiguration();

	Physics_t m_fStepSize;
	Physics_t m_kStretch, m_kShear, m_kBend;
	Physics_t m_StretchDampPct, m_ShearDampPct, m_BendDampPct;
	Physics_t m_ParticleMass, m_StretchLimitPct;
	bool m_bUseStretch, m_bUseShear, m_bUseBend;
	int m_iConstraints[4];
	int m_iColPoints, m_iRowPoints;
	int m_iInverseIterations;
	int m_iIntegrationMethod;

	Physics_t GetFloat( char *szConfigFile, char *szSection, char *szItem, char *szDefault );
	int GetInt( char *szConfigFile, char *szSection, char *szItem, int iDefault );
};

#endif // !defined(AFX_CONFIGURATION_H__B4786515_75E6_442F_A9D6_E16C224C1108__INCLUDED_)
