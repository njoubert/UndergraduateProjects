/*
 * intel_Matrix3x3.cpp
 *
 *  Created on: Oct 7, 2008
 *      Author: silversatin
 */

// Physics_Matrix3x3.cpp: implementation of the Physics_Matrix3x3 class.
//
//////////////////////////////////////////////////////////////////////

//#include "DRGShell.h"
#include "intel_Matrix3x3.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Physics_Matrix3x3::Physics_Matrix3x3()
{

}

Physics_Matrix3x3::~Physics_Matrix3x3()
{

}

void Physics_Matrix3x3::Zero()
{
	for( int i=0; i<9; i++ )
		m_Mx[i] = (Physics_t)0.0;
}

void Physics_Matrix3x3::SetIdentity()
{
	m_Mx[0] = m_Mx[4] = m_Mx[8] = (Physics_t)1.0;
	m_Mx[1] = m_Mx[2] = m_Mx[3] = m_Mx[5] = m_Mx[6] = m_Mx[7] = (Physics_t)0.0;
}

void Physics_Matrix3x3::Multiply( Physics_Matrix3x3 &M, Physics_Matrix3x3 &Dst )
{
	Dst.m_Mx[0] = m_Mx[0] * M.m_Mx[0] + m_Mx[1] * M.m_Mx[3] + m_Mx[2] * M.m_Mx[6];
	Dst.m_Mx[1] = m_Mx[0] * M.m_Mx[1] + m_Mx[1] * M.m_Mx[4] + m_Mx[2] * M.m_Mx[7];
	Dst.m_Mx[2] = m_Mx[0] * M.m_Mx[2] + m_Mx[1] * M.m_Mx[5] + m_Mx[2] * M.m_Mx[8];

	Dst.m_Mx[3] = m_Mx[3] * M.m_Mx[0] + m_Mx[4] * M.m_Mx[3] + m_Mx[5] * M.m_Mx[6];
	Dst.m_Mx[4] = m_Mx[3] * M.m_Mx[1] + m_Mx[4] * M.m_Mx[4] + m_Mx[5] * M.m_Mx[7];
	Dst.m_Mx[5] = m_Mx[3] * M.m_Mx[2] + m_Mx[4] * M.m_Mx[5] + m_Mx[5] * M.m_Mx[8];

	Dst.m_Mx[6] = m_Mx[6] * M.m_Mx[0] + m_Mx[7] * M.m_Mx[3] + m_Mx[8] * M.m_Mx[6];
	Dst.m_Mx[7] = m_Mx[6] * M.m_Mx[1] + m_Mx[7] * M.m_Mx[4] + m_Mx[8] * M.m_Mx[7];
	Dst.m_Mx[8] = m_Mx[6] * M.m_Mx[2] + m_Mx[7] * M.m_Mx[5] + m_Mx[8] * M.m_Mx[8];
}

void Physics_Matrix3x3::Add( Physics_Matrix3x3 &M, Physics_Matrix3x3 &Dst )
{
	for( int i=0; i<9; i++ )
		Dst.m_Mx[i] = m_Mx[i] + M.m_Mx[i];
}

void Physics_Matrix3x3::Subtract( Physics_Matrix3x3 &M, Physics_Matrix3x3 &Dst )
{
	for( int i=0; i<9; i++ )
		Dst.m_Mx[i] = m_Mx[i] - M.m_Mx[i];
}

void Physics_Matrix3x3::PostMultiply( Physics_Vector3 &v, Physics_Vector3 &Dst )
{
	Dst.x = m_Mx[0] * v.x + m_Mx[1] * v.y + m_Mx[2] * v.z;
	Dst.y = m_Mx[3] * v.x + m_Mx[4] * v.y + m_Mx[5] * v.z;
	Dst.z = m_Mx[6] * v.x + m_Mx[7] * v.y + m_Mx[8] * v.z;
}

void Physics_Matrix3x3::PreMultiply( Physics_Vector3 &v, Physics_Vector3 &Dst )
{
	Dst.x = v.x * m_Mx[0] + v.y * m_Mx[3] + v.z * m_Mx[6];
	Dst.y = v.x * m_Mx[1] + v.y * m_Mx[4] + v.z * m_Mx[7];
	Dst.z = v.x * m_Mx[2] + v.y * m_Mx[5] + v.z * m_Mx[8];
}

void Physics_Matrix3x3::Multiply( Physics_t scale, Physics_Matrix3x3 &Dst )
{
	for( int i=0; i<9; i++ )
		Dst.m_Mx[i] = m_Mx[i] * scale;
}

void Physics_Matrix3x3::Invert( Physics_Matrix3x3 &Dst )
{
}

Physics_Matrix3x3& Physics_Matrix3x3::operator=( Physics_Matrix3x3 &other )
{
	for( int i=0; i<9; i++ )
		m_Mx[i] = other.m_Mx[i];
	return *this;
}

void Physics_Matrix3x3::SetFromOuterProduct( Physics_Vector3 &v1, Physics_Vector3 &v2 )
{
	m_Mx[0] = v1.x * v2.x; m_Mx[1] = v1.x * v2.y; m_Mx[2] = v1.x * v2.z;
	m_Mx[3] = v1.y * v2.x; m_Mx[4] = v1.y * v2.y; m_Mx[5] = v1.y * v2.z;
	m_Mx[6] = v1.z * v2.x; m_Mx[7] = v1.z * v2.y; m_Mx[8] = v1.z * v2.z;
}

bool Physics_Matrix3x3::IsSymmetric()
{
	if( (m_Mx[1] != m_Mx[3]) || (m_Mx[2] != m_Mx[6]) || (m_Mx[5] != m_Mx[7]) )
		return false;
	return true;
}

bool Physics_Matrix3x3::IsTranspose( Physics_Matrix3x3 &other )
{
	for( int i=0; i<3; i++ )
		for( int j=0; j<3; j++ )
			if( m_Mx[i*3+j] != other.m_Mx[j*3+i] )
				return false;
	return true;
}

void Physics_Matrix3x3::Set( Physics_t a00, Physics_t a01, Physics_t a02,
		  Physics_t a10, Physics_t a11, Physics_t a12,
		  Physics_t a20, Physics_t a21, Physics_t a22 )
{
	m_Mx[0] = a00;
	m_Mx[1] = a01;
	m_Mx[2] = a02;
	m_Mx[3] = a10;
	m_Mx[4] = a11;
	m_Mx[5] = a12;
	m_Mx[6] = a20;
	m_Mx[7] = a21;
	m_Mx[8] = a22;
}

void Physics_Matrix3x3::Set( Physics_Vector3 &row1, Physics_Vector3 &row2, Physics_Vector3 &row3 )
{
	m_Mx[0] = row1.x; m_Mx[1] = row1.y; m_Mx[2] = row1.z;
	m_Mx[3] = row2.x; m_Mx[4] = row2.y; m_Mx[5] = row2.z;
	m_Mx[6] = row3.x; m_Mx[7] = row3.y; m_Mx[8] = row3.z;
}

void Physics_Matrix3x3::SetTilde( Physics_t x, Physics_t y, Physics_t z )
{
	m_Mx[0] = m_Mx[4] = m_Mx[8] = 0;
	m_Mx[1] = -z; m_Mx[2] = y;
	m_Mx[3] = z; m_Mx[5] = -x;
	m_Mx[6] = -y; m_Mx[7] = x;
}


void Physics_Matrix3x3::Transpose()
{
	Physics_t tmp;

	tmp = m_Mx[1];
	m_Mx[1] = m_Mx[3]; m_Mx[3] = tmp;
	tmp = m_Mx[2];
	m_Mx[2] = m_Mx[6]; m_Mx[6] = tmp;
	tmp = m_Mx[5];
	m_Mx[5] = m_Mx[7]; m_Mx[7] = tmp;
}

//
// Do a cross product of each row with the vector
//
void Physics_Matrix3x3::CrossProduct( Physics_Vector3 &v, Physics_Matrix3x3 &Dst )
{
	Dst.m_Mx[0] = m_Mx[1] * v.z - m_Mx[2] * v.y;
	Dst.m_Mx[1] = m_Mx[2] * v.x - m_Mx[0] * v.z;
	Dst.m_Mx[2] = m_Mx[0] * v.y - m_Mx[1] * v.x;
	Dst.m_Mx[3] = m_Mx[4] * v.z - m_Mx[5] * v.y;
	Dst.m_Mx[4] = m_Mx[5] * v.x - m_Mx[3] * v.z;
	Dst.m_Mx[5] = m_Mx[3] * v.y - m_Mx[4] * v.x;
	Dst.m_Mx[6] = m_Mx[7] * v.z - m_Mx[8] * v.y;
	Dst.m_Mx[7] = m_Mx[8] * v.x - m_Mx[6] * v.z;
	Dst.m_Mx[8] = m_Mx[6] * v.y - m_Mx[7] * v.x;
}


void Physics_Matrix3x3::Negate( )
{
	m_Mx[0] = -m_Mx[0];
	m_Mx[1] = -m_Mx[1];
	m_Mx[2] = -m_Mx[2];
	m_Mx[3] = -m_Mx[3];
	m_Mx[4] = -m_Mx[4];
	m_Mx[5] = -m_Mx[5];
	m_Mx[6] = -m_Mx[6];
	m_Mx[7] = -m_Mx[7];
	m_Mx[8] = -m_Mx[8];
}

void Physics_Matrix3x3::SetTilde( Physics_Vector3 &v )
{
	SetTilde( v.x, v.y, v.z );
}

void Physics_Matrix3x3::Dump( char *szHeading )
{
	char szTemp[50];

	if( szHeading != NULL )
		cout<< szHeading <<endl;

	for( int k=0; k<3; k++ )
	{
		sprintf( szTemp, "%8.4f %8.4f %8.4f\r\n", m_Mx[k*3+0], m_Mx[k*3+1], m_Mx[k*3+2] );
		cout<< szTemp <<endl;
	}

}
