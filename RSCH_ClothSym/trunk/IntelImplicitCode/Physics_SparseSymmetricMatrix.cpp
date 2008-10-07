// Physics_SparseSymmetricMatrix.cpp: implementation of the Physics_SparseSymmetricMatrix class.
//
//////////////////////////////////////////////////////////////////////

#include "DRGShell.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Physics_SparseSymmetricMatrix::Physics_SparseSymmetricMatrix( int iRows, int iColumns)
{
	//
	// Do some sanity checking -- probably should throw an exception :-)
	//
	iRows = max( 1, iRows );
	iColumns = max( 1, iColumns );
	m_iRows = iRows;
	m_iColumns = iColumns;
	m_iValues = 0;
	m_pRows = new int[iRows+1];
	memset( m_pRows, 0, (iRows+1) * sizeof( int ) );
	m_pColumns = NULL;
	m_pData = NULL;
	m_dwCheckSum = 0;
}

Physics_SparseSymmetricMatrix::~Physics_SparseSymmetricMatrix()
{
	SAFE_DELETE_ARRAY( m_pColumns );
	SAFE_DELETE_ARRAY( m_pRows );
	SAFE_DELETE_ARRAY( m_pData );
}

void Physics_SparseSymmetricMatrix::Empty()
{
	SAFE_DELETE_ARRAY( m_pColumns );
	SAFE_DELETE_ARRAY( m_pData );

	m_iValues = 0;
	memset( m_pRows, 0, (m_iRows+1) * sizeof( int ) );
	m_dwCheckSum = 0;
}

void Physics_SparseSymmetricMatrix::Zero()
{
	if( m_pData && m_iValues ) 
		memset( m_pData, 0, m_iValues * sizeof( Physics_Matrix3x3 ) );
}

//
// Calculate Dst = this * V
//
bool Physics_SparseSymmetricMatrix::PostMultiply( Physics_LargeVector &V, Physics_LargeVector &Dst )
{
	if( (Dst.m_iElements != m_iRows) || (V.m_iElements != m_iColumns) )
		return false;

	int i, j, ct, idx, iCol;
	Physics_Vector3 tmp;

	Dst.Zero();

	if( m_iValues )
	{
		for( i=0; i<m_iRows; i++ )
		{
			idx = m_pRows[i];
			ct = m_pRows[i+1] - idx;
			for( j = 0; j<ct; j++ )
			{
				iCol = m_pColumns[idx+j];
				Dst.m_pData[i].x += m_pData[idx+j].m_Mx[0] * V.m_pData[iCol].x + m_pData[idx+j].m_Mx[1] * V.m_pData[iCol].y + m_pData[idx+j].m_Mx[2] * V.m_pData[iCol].z;
				Dst.m_pData[i].y += m_pData[idx+j].m_Mx[3] * V.m_pData[iCol].x + m_pData[idx+j].m_Mx[4] * V.m_pData[iCol].y + m_pData[idx+j].m_Mx[5] * V.m_pData[iCol].z;
				Dst.m_pData[i].z += m_pData[idx+j].m_Mx[6] * V.m_pData[iCol].x + m_pData[idx+j].m_Mx[7] * V.m_pData[iCol].y + m_pData[idx+j].m_Mx[8] * V.m_pData[iCol].z;
				if( i != iCol )
				{
					Dst.m_pData[iCol].x += V.m_pData[i].x * m_pData[idx+j].m_Mx[0] + V.m_pData[i].y * m_pData[idx+j].m_Mx[3] + V.m_pData[i].z * m_pData[idx+j].m_Mx[6];
					Dst.m_pData[iCol].y += V.m_pData[i].x * m_pData[idx+j].m_Mx[1] + V.m_pData[i].y * m_pData[idx+j].m_Mx[4] + V.m_pData[i].z * m_pData[idx+j].m_Mx[7];
					Dst.m_pData[iCol].z += V.m_pData[i].x * m_pData[idx+j].m_Mx[2] + V.m_pData[i].y * m_pData[idx+j].m_Mx[5] + V.m_pData[i].z * m_pData[idx+j].m_Mx[8];
				}
			}
		}
	}
	return true;
}

//
// Calculate Dst = V * this
//
bool Physics_SparseSymmetricMatrix::PreMultiply( Physics_LargeVector &V, Physics_LargeVector &Dst )
{
	if( (Dst.m_iElements != m_iColumns) || (V.m_iElements != m_iRows) )
		return false;

	int i, j, ct, idx;

	Dst.Zero();

	if( m_iValues )
	{
		for( i=0; i<m_iRows; i++ )
		{
			idx = m_pRows[i];
			ct = m_pRows[i+1] - idx;
			for( j = 0; j<ct; j++ )
			{
				Dst.m_pData[i].x += V.m_pData[m_pColumns[idx+j]].x * m_pData[idx+j].m_Mx[0] + V.m_pData[m_pColumns[idx+j]].y * m_pData[idx+j].m_Mx[3] + V.m_pData[m_pColumns[idx+j]].z * m_pData[idx+j].m_Mx[6];
				Dst.m_pData[i].y += V.m_pData[m_pColumns[idx+j]].x * m_pData[idx+j].m_Mx[1] + V.m_pData[m_pColumns[idx+j]].y * m_pData[idx+j].m_Mx[4] + V.m_pData[m_pColumns[idx+j]].z * m_pData[idx+j].m_Mx[7];
				Dst.m_pData[i].z += V.m_pData[m_pColumns[idx+j]].x * m_pData[idx+j].m_Mx[2] + V.m_pData[m_pColumns[idx+j]].y * m_pData[idx+j].m_Mx[5] + V.m_pData[m_pColumns[idx+j]].z * m_pData[idx+j].m_Mx[8];
				if( i != m_pColumns[idx+j] )
				{
					Dst.m_pData[m_pColumns[idx+j]].x += V.m_pData[i].x * m_pData[idx+j].m_Mx[0] + V.m_pData[i].y * m_pData[idx+j].m_Mx[1] + V.m_pData[i].z * m_pData[idx+j].m_Mx[2];
					Dst.m_pData[m_pColumns[idx+j]].y += V.m_pData[i].x * m_pData[idx+j].m_Mx[3] + V.m_pData[i].y * m_pData[idx+j].m_Mx[4] + V.m_pData[i].z * m_pData[idx+j].m_Mx[5];
					Dst.m_pData[m_pColumns[idx+j]].z += V.m_pData[i].x * m_pData[idx+j].m_Mx[6] + V.m_pData[i].y * m_pData[idx+j].m_Mx[7] + V.m_pData[i].z * m_pData[idx+j].m_Mx[8];
				}
			}
		}
	}
	return true;
}

//
// Calculate Dst = this + M
//
bool Physics_SparseSymmetricMatrix::Add( Physics_SparseSymmetricMatrix &M, Physics_SparseSymmetricMatrix &Dst )
{
	if( (M.m_iColumns != m_iColumns) || (Dst.m_iColumns != m_iColumns) ||
		(M.m_iRows != m_iRows) || (Dst.m_iRows != m_iRows) )
		return false;

	int i, j, ct, idx;

	if( m_dwCheckSum && (Dst.m_dwCheckSum == m_dwCheckSum) && (M.m_dwCheckSum == m_dwCheckSum) )
	{
		for( i=0; i<m_iValues; i++ )
		{
			m_pData[i].Add( M.m_pData[i], Dst.m_pData[i] );
		}
	}
	else
	{
		Dst.Zero();

		if( m_iValues )
		{
			for( i=0; i<m_iRows; i++ )
			{
				idx = m_pRows[i];
				ct = m_pRows[i+1] - idx;
				for( j = 0; j<ct; j++ )
				{
					Dst(i, m_pColumns[idx+j] ) = m_pData[idx+j];
				}
			}
		}
		if( M.m_iValues )
		{
			for( i=0; i<M.m_iRows; i++ )
			{
				idx = M.m_pRows[i];
				ct = M.m_pRows[i+1] - idx;
				for( j = 0; j<ct; j++ )
				{
					Dst(i, M.m_pColumns[idx+j] ).Add( M.m_pData[idx+j], Dst(i,M.m_pColumns[idx+j]) );
				}
			}
		}
	}
	return true;
}

//
// Calculate Dst = this - M
//
bool Physics_SparseSymmetricMatrix::Subtract( Physics_SparseSymmetricMatrix &M, Physics_SparseSymmetricMatrix &Dst )
{
	if( (M.m_iColumns != m_iColumns) || (Dst.m_iColumns != m_iColumns) ||
		(M.m_iRows != m_iRows) || (Dst.m_iRows != m_iRows) )
		return false;

	int i, j, ct, idx;

	if( m_dwCheckSum && (Dst.m_dwCheckSum == m_dwCheckSum) && (M.m_dwCheckSum == m_dwCheckSum) )
	{
		for( i=0; i<m_iValues; i++ )
		{
			m_pData[i].Subtract( M.m_pData[i], Dst.m_pData[i] );
		}
	}
	else
	{
		Dst.Zero();

		if( m_iValues )
		{
			for( i=0; i<m_iRows; i++ )
			{
				idx = m_pRows[i];
				ct = m_pRows[i+1] - idx;
				for( j = 0; j<ct; j++ )
				{
					Dst(i, m_pColumns[idx+j] ) = m_pData[idx+j];
				}
			}
		}
		if( M.m_iValues )
		{
			for( i=0; i<M.m_iRows; i++ )
			{
				idx = M.m_pRows[i];
				ct = M.m_pRows[i+1] - idx;
				for( j = 0; j<ct; j++ )
				{
					Dst(i, M.m_pColumns[idx+j] ).Subtract( M.m_pData[idx+j], Dst(i,M.m_pColumns[idx+j]) );
				}
			}
		}
	}
	return true;
}

bool Physics_SparseSymmetricMatrix::Scale( Physics_t scale, Physics_SparseSymmetricMatrix &Dst )
{
	int i, j, ct, idx;

	if( m_dwCheckSum && (Dst.m_dwCheckSum == m_dwCheckSum) )
	{
		for( i=0; i<m_iValues; i++ )
		{
			m_pData[i].Multiply( scale, Dst.m_pData[i] );
		}
	}
	else
	{
		Dst.Zero();
		if( m_iValues )
		{
			for( i=0; i<m_iRows; i++ )
			{
				idx = m_pRows[i];
				ct = m_pRows[i+1] - idx;
				for( j = 0; j<ct; j++ )
				{
					m_pData[idx+j].Multiply( scale, Dst(i,m_pColumns[idx+j]) );
				}
			}
		}
	}
	return true;
}


Physics_Matrix3x3 & Physics_SparseSymmetricMatrix::operator() (int row, int col)
{
	int i, k, l, ct;

	if( row > col  )
	{
		k = row;
		row = col;
		col = k;
	}

	k = m_pRows[row];
	ct = m_pRows[row+1] - k;
	for( l=0; l<ct; l++ )
		if( m_pColumns[k+l] == col )
			break;
	if(  (ct != 0) && (l != ct) )
	{
		return m_pData[k+l];
	}
	else
	{
		int *pSave = m_pColumns;
		Physics_Matrix3x3 *pSaveData = m_pData;
		
		m_pColumns = new int[m_iValues+1];
		m_pData = new Physics_Matrix3x3[m_iValues+1];

		if( pSave && k)
		{
			memcpy( m_pColumns, pSave, k * sizeof( int ) );
			memcpy( m_pData, pSaveData, k * sizeof( Physics_Matrix3x3 ) );
		}
		for( l=0; l<ct; l++ )
		{
			if( pSave[k+l] < col )
			{
				m_pColumns[k+l] = pSave[k+l];
				m_pData[k+l] = pSaveData[k+l];
			}
			else
			{
				break;
			}
		}
		m_pColumns[k+l] = col;
		m_pData[k+l].Zero();
		for( i=row+1; i<=m_iRows; i++ )
			m_pRows[i]++;
		if( pSave  )
		{
			if( (m_iValues-k-l) > 0 )
				memcpy( &m_pColumns[k+l+1], &pSave[k+l], (m_iValues-k-l) * sizeof( int ) );
			delete pSave;
		}
		if( pSaveData   )
		{
			if( (m_iValues-k-l) > 0 )
				memcpy( &m_pData[k+l+1], &pSaveData[k+l], (m_iValues-k-l) * sizeof( Physics_Matrix3x3 ) );
			delete [] pSaveData;
		}
		m_iValues++;
		//
		// How well does this work as a check sum?  Who knows :-)
		//
		m_dwCheckSum += (row+1) * 190237 + (col+1);

		return m_pData[k+l];
	}
}

void Physics_SparseSymmetricMatrix::Dump( char *szTitle )
{
	char szTemp[100];
	int i,j, k, idx, ct;

	if( szTitle != NULL )
		OutputDebugString( szTitle );

	sprintf( szTemp, "Rows: %d Columns: %d\r\n", m_iRows, m_iColumns );
	OutputDebugString( szTemp );

	if( m_iValues )
	{
		for( i=0; i<m_iRows; i++ )
		{
			idx = m_pRows[i];
			ct = m_pRows[i+1] - idx;
			for( k=0; k<3; k++ )
			{
				for( j = 0; j<ct; j++ )
				{
					sprintf( szTemp, "(%3d,%3d): %8.4f %8.4f %8.4f ", i, m_pColumns[idx+j], 
						m_pData[idx+j].m_Mx[k*3+0],
						m_pData[idx+j].m_Mx[k*3+1],
						m_pData[idx+j].m_Mx[k*3+2] );
					OutputDebugString( szTemp );
				}
				OutputDebugString( "\r\n" );
			}
		}
	}
}

bool Physics_SparseSymmetricMatrix::IsSymmetric()
{
	int i, j, k, idx, ct, idx2, ct2;

	if( !m_iValues )
		return true;

	for( i=0; i<m_iRows; i++ )
	{
		idx = m_pRows[i];
		ct = m_pRows[i+1] - idx;
		for( j = 0; j<ct; j++ )
		{
			idx2 = m_pRows[m_pColumns[idx+j]];
			ct2 = m_pRows[m_pColumns[idx+j]+1] - idx2;
			for( k = 0; k<ct2; k++ )
			{
				if( m_pColumns[idx2+k] == i )
				{
					if( !m_pData[idx+j].IsTranspose( m_pData[idx2+k] ) )
						return false;
					else
						break;
				}
			}
			if( ct2 && (k == ct2) )
				return false;
		}
	}
	return true;
}

bool Physics_SparseSymmetricMatrix::IsPositiveDefinite()
{
	int i, iRow = 0;
	Physics_t sum = 0;

	if( !m_iValues )
		return true;

	for( i=0; i<m_iValues; i++ )
	{
		while( i > m_pRows[iRow+1] && (m_pRows[iRow+1] > m_pRows[iRow]) )
			iRow++;
		if( (iRow == m_pColumns[i]) && ((m_pData[i].m_Mx[0] <= 0 )||
			(m_pData[i].m_Mx[4] <= 0 ) ||
			(m_pData[i].m_Mx[8] <= 0 ) ))
			return false;
		sum += m_pData[i].m_Mx[0] + m_pData[i].m_Mx[1] + m_pData[i].m_Mx[2]+
			   m_pData[i].m_Mx[3] + m_pData[i].m_Mx[4] + m_pData[i].m_Mx[5]+
			   m_pData[i].m_Mx[6] + m_pData[i].m_Mx[7] + m_pData[i].m_Mx[8];
	}
	return (sum>0);
}

//
// Set this = M
//
void Physics_SparseSymmetricMatrix::Copy( Physics_SparseSymmetricMatrix &M )
{
	SAFE_DELETE_ARRAY( m_pRows );
	SAFE_DELETE_ARRAY( m_pColumns );
	SAFE_DELETE_ARRAY( m_pData );

	m_iRows = M.m_iRows;
	m_iColumns = M.m_iColumns;
	m_iValues = M.m_iValues;
	m_pRows = new int[m_iRows+1];
	memcpy( m_pRows, M.m_pRows, (m_iRows+1) * sizeof( int ) );
	m_pColumns = new int[m_iValues];
	memcpy( m_pColumns, M.m_pColumns, (m_iValues) * sizeof( int ) );
	m_pData = new Physics_Matrix3x3[m_iValues];
	memcpy( m_pData, M.m_pData, (m_iValues) * sizeof( Physics_Matrix3x3 ) );
	m_dwCheckSum = M.m_dwCheckSum;
}
