// Configuration.cpp: implementation of the CConfiguration class.
//
//////////////////////////////////////////////////////////////////////

#include "DRGShell.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfiguration::CConfiguration( char *szConfigFile )
{
	char szSection[30];

	//
	// Flush any cached copy
	//
	WritePrivateProfileString( NULL, NULL, NULL, szConfigFile );

	m_iIntegrationMethod = GetPrivateProfileInt( "Cloth", "IntegrationMethod", 2, szConfigFile );
	switch( m_iIntegrationMethod )
	{
		case 0:
			strcpy( szSection, "Cloth.Explicit" );
			break;
		case 1:
			strcpy( szSection, "Cloth.Implicit" );
			break;
		case 2:
		default:
			strcpy( szSection, "Cloth.Semi-Implicit" );
			break;
	}

	m_fStepSize = GetFloat( szConfigFile, szSection, "StepSize", "0.015" );
	m_iColPoints = GetInt( szConfigFile, szSection, "ColParticles", 10 );
	m_iRowPoints = GetInt( szConfigFile, szSection, "RowParticles", 10 );

	m_kStretch = GetFloat( szConfigFile, szSection, "kStretch", "10000" );
	m_kShear = GetFloat( szConfigFile, szSection, "kShear", "1000" );
	m_kBend = GetFloat( szConfigFile, szSection, "kBend", "100" );

	m_StretchDampPct = GetFloat( szConfigFile, szSection, "StretchDampPct", "0.01" );
	m_ShearDampPct = GetFloat( szConfigFile, szSection, "ShearDampPct", "0.01" );
	m_BendDampPct = GetFloat( szConfigFile, szSection, "BendDampPct", "0.01" );

	m_ParticleMass = GetFloat( szConfigFile, szSection, "ParticleMass", "10.0" );
	m_StretchLimitPct = GetFloat( szConfigFile, szSection, "StretchLimitPct", "1.05" );

#pragma warning(disable:4800)
	m_bUseStretch = (bool)GetInt( szConfigFile, szSection, "UseStretch", 0 );
	m_bUseShear = (bool)GetInt( szConfigFile, szSection, "UseShear", 0 );
	m_bUseBend = (bool)GetInt( szConfigFile, szSection, "UseBend", 0 );
#pragma warning(default:4800)
	m_iConstraints[0] = GetInt( szConfigFile, szSection, "Constraint0", 1 );
	m_iConstraints[1] = GetInt( szConfigFile, szSection, "Constraint1", 1 );
	m_iConstraints[2] = GetInt( szConfigFile, szSection, "Constraint2", 1 );
	m_iConstraints[3] = GetInt( szConfigFile, szSection, "Constraint3", 1 );

	m_iInverseIterations = GetInt( szConfigFile, szSection, "InverseIterations", 5 );
}

CConfiguration::~CConfiguration()
{

}

Physics_t CConfiguration::GetFloat( char *szConfigFile, char *szSection, char *szItem, char *szDefault )
{
	char szTemp[1000];
	double fTemp;

	GetPrivateProfileString( szSection, szItem, "none", szTemp, sizeof( szTemp ), szConfigFile );
	if( !stricmp( szTemp, "none" ) )
	{
		GetPrivateProfileString( "Cloth", szItem, szDefault, szTemp, sizeof( szTemp ), szConfigFile );
	}
	sscanf( szTemp, "%lf", &fTemp );
	return (Physics_t)fTemp;
}

int CConfiguration::GetInt( char *szConfigFile, char *szSection, char *szItem, int iDefault )
{
	int iValue;

	iValue = GetPrivateProfileInt( szSection, szItem, -99999, szConfigFile );
	if( iValue == -99999 )
		iValue = GetPrivateProfileInt( "Cloth", szItem, iDefault, szConfigFile );
	return iValue;
}
