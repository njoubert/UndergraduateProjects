/*
 * LargeVector.cpp
 *
 *  Created on: Oct 7, 2008
 *      Author: silversatin
 */

// Physics_LargeVector.cpp: implementation of the Physics_LargeVector class.
//
//////////////////////////////////////////////////////////////////////

//#include "DRGShell.h"
#include "intel_LargeVector.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


Physics_LargeVector::Physics_LargeVector( int iElements )
{
	if( iElements > 0 )
	{
		m_iElements = iElements;
		m_pData = new Physics_Vector3[m_iElements];
		memset( m_pData, 0, m_iElements * sizeof( Physics_Vector3 ) );
	}
	else
	{
		m_iElements = 0;
		m_pData = NULL;
	}
}

Physics_LargeVector::~Physics_LargeVector()
{
	if( m_pData )
		delete [] m_pData;
}

int Physics_LargeVector::Size()
{
	return m_iElements;
}

void Physics_LargeVector::Zero()
{
	if( m_iElements && m_pData )
		memset( m_pData, 0, m_iElements * sizeof( Physics_Vector3 ) );
}

void Physics_LargeVector::Resize( int iNewElements )
{
	if( m_iElements > 0 )
	{
		if( iNewElements > 0 )
		{
			Physics_Vector3 *pOld = m_pData;
			m_pData = new Physics_Vector3[iNewElements];

			if( iNewElements > m_iElements )
			{
				memcpy( m_pData, pOld, m_iElements * sizeof( Physics_Vector3 ) );
				memset( m_pData+m_iElements, 0, (iNewElements - m_iElements) * sizeof( Physics_Vector3 ) );
				m_iElements = iNewElements;
				delete [] pOld;
			}
			else
			{
				memcpy( m_pData, pOld, iNewElements * sizeof( Physics_Vector3 ) );
				m_iElements = iNewElements;
				delete [] pOld;
			}
		}
		else
		{
			delete [] m_pData;
			m_pData = NULL;
			m_iElements = 0;
		}
	}
	else if( iNewElements > 0 )
	{
		m_iElements = iNewElements;
		m_pData = new Physics_Vector3[m_iElements];
	}
}

bool Physics_LargeVector::Add( Physics_LargeVector &v, Physics_LargeVector &dst )
{
	if( (m_iElements != v.m_iElements ) || (m_iElements != dst.m_iElements ) )
		return false;

	for( int i=0; i<m_iElements; i++ )
	{
		VECTOR3_ADD( m_pData[i], v.m_pData[i], dst.m_pData[i] );
	}
	return true;
}

bool Physics_LargeVector::Subtract( Physics_LargeVector &v, Physics_LargeVector &dst )
{
	if( (m_iElements != v.m_iElements ) || (m_iElements != dst.m_iElements ) )
		return false;

	for( int i=0; i<m_iElements; i++ )
	{
		VECTOR3_SUB( m_pData[i], v.m_pData[i], dst.m_pData[i] );
	}
	return true;
}

bool Physics_LargeVector::ElementMultiply( Physics_LargeVector &v, Physics_LargeVector &dst )
{
	if( (m_iElements != v.m_iElements ) || (m_iElements != dst.m_iElements ) )
		return false;

	for( int i=0; i<m_iElements; i++ )
	{
		dst.m_pData[i].x = m_pData[i].x * v.m_pData[i].x;
		dst.m_pData[i].y = m_pData[i].y * v.m_pData[i].y;
		dst.m_pData[i].z = m_pData[i].z * v.m_pData[i].z;
	}
	return true;
}

bool Physics_LargeVector::ElementMultiply( Physics_Matrix3x3 S[], Physics_LargeVector &dst )
{
	if( m_iElements != dst.m_iElements )
		return false;

	for( int i=0; i<m_iElements; i++ )
	{
		S[i].PreMultiply( m_pData[i], dst.m_pData[i] );
	}
	return true;
}

bool Physics_LargeVector::Scale( double scale, Physics_LargeVector &dst )
{
	if( m_iElements != dst.m_iElements )
		return false;

	for( int i=0; i<m_iElements; i++ )
	{
		VECTOR3_SCALE( m_pData[i], scale, dst.m_pData[i] );
	}
	return true;
}

bool Physics_LargeVector::Invert( Physics_LargeVector &dst )
{
	if( m_iElements != dst.m_iElements )
		return false;

	for( int i=0; i<m_iElements; i++ )
	{
		dst.m_pData[i].x = (double)1.0 / m_pData[i].x;
		dst.m_pData[i].y = (double)1.0 / m_pData[i].y;
		dst.m_pData[i].z = (double)1.0 / m_pData[i].z;
	}
	return true;
}

double Physics_LargeVector::DotProduct( Physics_LargeVector &v )
{
	double res = (double)0.0;

	for( int i=0; i<m_iElements; i++ )
		res += m_pData[i].x * v.m_pData[i].x +
			   m_pData[i].y * v.m_pData[i].y +
			   m_pData[i].z * v.m_pData[i].z;
	return res;
}

Physics_LargeVector& Physics_LargeVector::operator=( Physics_LargeVector &copy )
{
	if( m_iElements != copy.m_iElements )
	{
		SAFE_DELETE_ARRAY( m_pData );
		m_iElements = 0;
	}
	if( copy.m_iElements )
	{
		m_pData = new Physics_Vector3[copy.m_iElements];
		m_iElements = copy.m_iElements;
		memcpy( m_pData, copy.m_pData, m_iElements * sizeof( Physics_Vector3 ) );
	}
	return *this;
}

void Physics_LargeVector::Dump( char *szTitle )
{
	char szTemp[500];

	if( szTitle )
		cout<< szTitle <<endl;

	sprintf( szTemp, "Elements: %d\r\n", m_iElements );

	for( int i=0; i<m_iElements; i++ )
	{
		sprintf( szTemp, "%3d: %8.4f %8.4f %8.4f\r\n", i, m_pData[i].x, m_pData[i].y, m_pData[i].z );
		cout<< szTemp <<endl;
	}
}
