// Physics_SymmetricMatrix.cpp: implementation of the Physics_SymmetricMatrix class.
//
//////////////////////////////////////////////////////////////////////

#include "DRGShell.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Physics_SymmetricMatrix::Physics_SymmetricMatrix( int iRows )
{
	//
	// Do some sanity checking -- probably should throw an exception :-)
	//
	iRows = max( 1, iRows );
	m_iRows = iRows;
	m_pData = new Physics_t[ m_iRows * (m_iRows+1) / 2 ];
	Zero();
}

Physics_SymmetricMatrix::~Physics_SymmetricMatrix()
{
	SAFE_DELETE_ARRAY( m_pData );
}

void Physics_SymmetricMatrix::Zero()
{
	memset( m_pData, 0, m_iRows * (m_iRows+1) / 2 * sizeof( Physics_t ) );
}

//
// Calculate Dst = this * V
//
bool Physics_SymmetricMatrix::PostMultiply( Physics_LargeVector &V, Physics_LargeVector &Dst )
{
	int i, j;

	Dst.Zero();

	for( i=0; i<m_iRows; i++ )
	{
		for( j = i; j<m_iRows; j++ )
		{
			Dst.m_pData[i].x += m_pData[i*m_iRows - i*(i-1)/2+j-i] * V.m_pData[i].x;
			Dst.m_pData[i].y += m_pData[i*m_iRows - i*(i-1)/2+j-i] * V.m_pData[i].y;
			Dst.m_pData[i].z += m_pData[i*m_iRows - i*(i-1)/2+j-i] * V.m_pData[i].z;
			if( i != j )
			{
				Dst.m_pData[j].x += m_pData[i*m_iRows - i*(i-1)/2+j-i] * V.m_pData[i].x;
				Dst.m_pData[j].y += m_pData[i*m_iRows - i*(i-1)/2+j-i] * V.m_pData[i].y;
				Dst.m_pData[j].z += m_pData[i*m_iRows - i*(i-1)/2+j-i] * V.m_pData[i].z;
			}
		}
	}
	return true;
}

//
// Calculate Dst = V * this
//
bool Physics_SymmetricMatrix::PreMultiply( Physics_LargeVector &V, Physics_LargeVector &Dst )
{
	int i, j;

	Dst.Zero();

	for( i=0; i<m_iRows; i++ )
	{
		for( j = i; j<m_iRows; j++ )
		{
			Dst.m_pData[i].x += m_pData[i*m_iRows - i*(i-1)/2+j-i] * V.m_pData[i].x;
			Dst.m_pData[i].y += m_pData[i*m_iRows - i*(i-1)/2+j-i] * V.m_pData[i].y;
			Dst.m_pData[i].z += m_pData[i*m_iRows - i*(i-1)/2+j-i] * V.m_pData[i].z;
			if( i != j )
			{
				Dst.m_pData[j].x += m_pData[i*m_iRows - i*(i-1)/2+j-i] * V.m_pData[i].x;
				Dst.m_pData[j].y += m_pData[i*m_iRows - i*(i-1)/2+j-i] * V.m_pData[i].y;
				Dst.m_pData[j].z += m_pData[i*m_iRows - i*(i-1)/2+j-i] * V.m_pData[i].z;
			}
		}
	}
	return true;
}

//
// Calculate Dst = this + M
//
bool Physics_SymmetricMatrix::Add( Physics_SymmetricMatrix &M, Physics_SymmetricMatrix &Dst )
{
	if( (M.m_iRows != m_iRows) || (Dst.m_iRows != m_iRows) )
		return false;

	int i, ct;

	ct = m_iRows * (m_iRows+1) / 2;
	for( i=0; i<ct; i++ )
		Dst.m_pData[i] = m_pData[i] + M.m_pData[i];

	return true;
}

//
// Calculate Dst = this - M
//
bool Physics_SymmetricMatrix::Subtract( Physics_SymmetricMatrix &M, Physics_SymmetricMatrix &Dst )
{
	if( (M.m_iRows != m_iRows) || (Dst.m_iRows != m_iRows) )
		return false;

	int i, ct;

	ct = m_iRows * (m_iRows+1) / 2;
	for( i=0; i<ct; i++ )
		Dst.m_pData[i] = m_pData[i] - M.m_pData[i];

	return true;
}

bool Physics_SymmetricMatrix::Scale( Physics_t scale, Physics_SymmetricMatrix &Dst )
{
	if( Dst.m_iRows != m_iRows )
		return false;

	int i, ct;

	ct = m_iRows * (m_iRows+1) / 2;
	for( i=0; i<ct; i++ )
		Dst.m_pData[i] = scale * m_pData[i];

	return true;
}


Physics_t & Physics_SymmetricMatrix::operator() (int row, int col)
{
	int k;

	if( row > col  )
	{
		k = row;
		row = col;
		col = k;
	}

	return m_pData[ row*m_iRows - row*(row-1)/2 + col - row ];
}

void Physics_SymmetricMatrix::Dump( char *szTitle )
{
	char szTemp[100];
	int i,j;

	if( szTitle != NULL )
		OutputDebugString( szTitle );

	sprintf( szTemp, "Rows: %d Columns: %d\r\n", m_iRows, m_iRows );
	OutputDebugString( szTemp );

	for( i=0; i<m_iRows; i++ )
	{
		sprintf( szTemp, "%3d: ", i );
		OutputDebugString( szTemp );
		for( j = i; j<m_iRows; j++ )
		{
			sprintf( szTemp, "%8.4f ", m_pData[i*m_iRows - i*(i-1)/2+j-i] );
			OutputDebugString( szTemp );
		}
		OutputDebugString( "\r\n" );
	}
}

//
// Set this = M
//
void Physics_SymmetricMatrix::Copy( Physics_SymmetricMatrix &M )
{
	SAFE_DELETE_ARRAY( m_pData );

	m_iRows = M.m_iRows;
	m_pData = new Physics_t[m_iRows * (m_iRows+1) / 2];
	memcpy( m_pData, M.m_pData, (m_iRows * (m_iRows+1)/2) * sizeof( Physics_t ) );
}

#define SWAP(a,b) {temp=(a);(a)=(b);(b)=temp;}

bool Physics_SymmetricMatrix::Invert()
{
	int i, j, l, ll;
	int irow, icol;
	Physics_t dum, pivinv;
	Physics_t *pData;


	//
	// Make a temp copy of the data in a fully populated fashion
	//
	
	pData = new Physics_t[m_iRows * m_iRows];

	//
	// Populate the dense matrix
	//
	for( i=0; i<m_iRows; i++ )
	{
		for( j=0; j<m_iRows; j++ )
		{
			if( j<i )
			{
				pData[ i*m_iRows+j ] = m_pData[ j*m_iRows - j * (j-1) / 2 + i-j ];
			}
			else
			{
				pData[ i*m_iRows+j ] = m_pData[ i*m_iRows - i * (i-1) / 2 + j-i ];
			}
		}
	}


	//
	// Use Gauss-Jordan elimination without pivoting (because my matrices have their
	// appropiate pivot values already on the diagonal).  Code from "Numerical Recipes in C"
	//
	
	for( i=0; i<m_iRows; i++ )
	{
		icol = irow = i;
		
		if( pData[icol*m_iRows+icol] == 0.0) 
		{
			OutputDebugString( "Singular Matrix\r\n" );
			return false;
		}
		pivinv=(Physics_t)1.0/pData[icol*m_iRows+icol];
		pData[icol*m_iRows+icol] = 1.0;
		for( l=0;l<m_iRows;l++)
			pData[icol*m_iRows+l] *= pivinv;
		for( ll=0; ll<m_iRows; ll++ )
		{
			if( ll != icol )
			{
				dum=pData[ll*m_iRows+icol];
				pData[ll*m_iRows+icol]=0.0;
				for( l=0; l<m_iRows; l++) 
					pData[ll*m_iRows+l] -= pData[icol*m_iRows+l]*dum;
			}			
		}
	}
	
	//
	// Okay, now we can store the data as the upper triangle only because of the symmetric
	// assumption this class uses -- I'm sure their are many better ways to invert a 
	// symmetric matrix, but I just wanted something quick
	//

	//
	// Create some temporary pointers and then reallocate the data
	//

	m_pData = new Physics_t[m_iRows * (m_iRows+1) / 2];

	for( i=0; i<m_iRows; i++ )
	{
		for( j=i; j<m_iRows; j++ )
		{
			m_pData[i*m_iRows - i * (i-1) / 2 +j-i] = pData[i*m_iRows+j];
		}
	}

	//
	// delete the old data
	//
	SAFE_DELETE_ARRAY( pData );

	//
	// Success
	//
	return true;
}

void Physics_SymmetricMatrix::SetIdentity()
{
	int i;

	Zero();
	for( i=0; i<m_iRows; i++ )
	{
		m_pData[i*m_iRows - i * (i-1) / 2] = (Physics_t)1.0;
	}
}

