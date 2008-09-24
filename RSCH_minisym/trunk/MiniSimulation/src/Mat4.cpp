/*
	File:			Mat4.cpp

	Function:		Implements Mat4.h

	Author(s):		Andrew Willmott

	Copyright:		(c) 1995-2000, Andrew Willmott

	Notes:			

*/
 

#include "vl\Mat4.h"
#include <ctype.h>
#include <iomanip.h>


TMat4::TMat4(TMReal a, TMReal b, TMReal c, TMReal d,
			TMReal e, TMReal f, TMReal g, TMReal h,
			TMReal i, TMReal j, TMReal k, TMReal l,
			TMReal m, TMReal n, TMReal o, TMReal p)
{
	row[0][0] = a;	row[0][1] = b;	row[0][2] = c;	row[0][3] = d;
	row[1][0] = e;	row[1][1] = f;	row[1][2] = g;	row[1][3] = h;
	row[2][0] = i;	row[2][1] = j;	row[2][2] = k;	row[2][3] = l;
	row[3][0] = m;	row[3][1] = n;	row[3][2] = o;	row[3][3] = p;
}

TMat4::TMat4(const TMat4 &m)
{
	row[0] = m[0];
	row[1] = m[1];
	row[2] = m[2];
	row[3] = m[3];
}


TMat4 &TMat4::operator = (const TMat4 &m)
{
	row[0] = m[0];
	row[1] = m[1];
	row[2] = m[2];
	row[3] = m[3];
	
	return(SELF);
}
	  
TMat4 &TMat4::operator += (const TMat4 &m)
{
	row[0] += m[0];
	row[1] += m[1];
	row[2] += m[2];
	row[3] += m[3];
	
	return(SELF);
}

TMat4 &TMat4::operator -= (const TMat4 &m)
{
	row[0] -= m[0];
	row[1] -= m[1];
	row[2] -= m[2];
	row[3] -= m[3];
	
	return(SELF);
}

TMat4 &TMat4::operator *= (const TMat4 &m)
{
	SELF = SELF * m;

	return(SELF);
}

TMat4 &TMat4::operator *= (TMReal s)
{
	row[0] *= s;
	row[1] *= s;
	row[2] *= s;
	row[3] *= s;
	
	return(SELF);
}

TMat4 &TMat4::operator /= (TMReal s)
{
	row[0] /= s;
	row[1] /= s;
	row[2] /= s;
	row[3] /= s;
	
	return(SELF);
}


Bool TMat4::operator == (const TMat4 &m) const
{
	return(row[0] == m[0] && row[1] == m[1] && row[2] == m[2] && row[3] == m[3]);
}

Bool TMat4::operator != (const TMat4 &m) const
{
	return(row[0] != m[0] || row[1] != m[1] || row[2] != m[2] || row[3] != m[3]);
}


TMat4 TMat4::operator + (const TMat4 &m) const
{
	TMat4 result;
	
	result[0] = row[0] + m[0];
	result[1] = row[1] + m[1];
	result[2] = row[2] + m[2];
	result[3] = row[3] + m[3];

	return(result);	
}

TMat4 TMat4::operator - (const TMat4 &m) const
{
	TMat4 result;
	
	result[0] = row[0] - m[0];
	result[1] = row[1] - m[1];
	result[2] = row[2] - m[2];
	result[3] = row[3] - m[3];

	return(result);	
}

TMat4 TMat4::operator - () const
{
	TMat4 result;
	
	result[0] = -row[0];
	result[1] = -row[1];
	result[2] = -row[2];
	result[3] = -row[3];

	return(result);	
}

TMat4 TMat4::operator * (const TMat4 &m) const
{
#define N(x,y) row[x][y]
#define M(x,y) m[x][y]
#define R(x,y) result[x][y]

	TMat4 result;

	R(0,0) = N(0,0) * M(0,0) + N(0,1) * M(1,0) + N(0,2) * M(2,0) + N(0,3) * M(3,0);
	R(0,1) = N(0,0) * M(0,1) + N(0,1) * M(1,1) + N(0,2) * M(2,1) + N(0,3) * M(3,1);
	R(0,2) = N(0,0) * M(0,2) + N(0,1) * M(1,2) + N(0,2) * M(2,2) + N(0,3) * M(3,2);
	R(0,3) = N(0,0) * M(0,3) + N(0,1) * M(1,3) + N(0,2) * M(2,3) + N(0,3) * M(3,3);
	
	R(1,0) = N(1,0) * M(0,0) + N(1,1) * M(1,0) + N(1,2) * M(2,0) + N(1,3) * M(3,0);
	R(1,1) = N(1,0) * M(0,1) + N(1,1) * M(1,1) + N(1,2) * M(2,1) + N(1,3) * M(3,1);
	R(1,2) = N(1,0) * M(0,2) + N(1,1) * M(1,2) + N(1,2) * M(2,2) + N(1,3) * M(3,2);
	R(1,3) = N(1,0) * M(0,3) + N(1,1) * M(1,3) + N(1,2) * M(2,3) + N(1,3) * M(3,3);
	
	R(2,0) = N(2,0) * M(0,0) + N(2,1) * M(1,0) + N(2,2) * M(2,0) + N(2,3) * M(3,0);
	R(2,1) = N(2,0) * M(0,1) + N(2,1) * M(1,1) + N(2,2) * M(2,1) + N(2,3) * M(3,1);
	R(2,2) = N(2,0) * M(0,2) + N(2,1) * M(1,2) + N(2,2) * M(2,2) + N(2,3) * M(3,2);
	R(2,3) = N(2,0) * M(0,3) + N(2,1) * M(1,3) + N(2,2) * M(2,3) + N(2,3) * M(3,3);
	
	R(3,0) = N(3,0) * M(0,0) + N(3,1) * M(1,0) + N(3,2) * M(2,0) + N(3,3) * M(3,0);
	R(3,1) = N(3,0) * M(0,1) + N(3,1) * M(1,1) + N(3,2) * M(2,1) + N(3,3) * M(3,1);
	R(3,2) = N(3,0) * M(0,2) + N(3,1) * M(1,2) + N(3,2) * M(2,2) + N(3,3) * M(3,2);
	R(3,3) = N(3,0) * M(0,3) + N(3,1) * M(1,3) + N(3,2) * M(2,3) + N(3,3) * M(3,3);
	
	return(result);	
	
#undef N
#undef M
#undef R
}

TMat4 TMat4::operator * (TMReal s) const
{
	TMat4 result;
	
	result[0] = row[0] * s;
	result[1] = row[1] * s;
	result[2] = row[2] * s;
	result[3] = row[3] * s;
	
	return(result);
}

TMat4 TMat4::operator / (TMReal s) const
{
	TMat4 result;
	
	result[0] = row[0] / s;
	result[1] = row[1] / s;
	result[2] = row[2] / s;
	result[3] = row[3] / s;
	
	return(result);
}

TMVec4 operator * (const TMat4 &m, const TMVec4 &v)			// m * v
{
	TMVec4 result;
	
	result[0] = v[0] * m[0][0] + v[1] * m[0][1] + v[2] * m[0][2] + v[3] * m[0][3];
	result[1] = v[0] * m[1][0] + v[1] * m[1][1] + v[2] * m[1][2] + v[3] * m[1][3];
	result[2] = v[0] * m[2][0] + v[1] * m[2][1] + v[2] * m[2][2] + v[3] * m[2][3];
	result[3] = v[0] * m[3][0] + v[1] * m[3][1] + v[2] * m[3][2] + v[3] * m[3][3];
	
	return(result);
}

TMVec4 operator * (const TMVec4 &v, const TMat4 &m)			// v * m
{
	TMVec4 result;
	
	result[0] = v[0] * m[0][0] + v[1] * m[1][0] + v[2] * m[2][0] + v[3] * m[3][0];
	result[1] = v[0] * m[0][1] + v[1] * m[1][1] + v[2] * m[2][1] + v[3] * m[3][1];
	result[2] = v[0] * m[0][2] + v[1] * m[1][2] + v[2] * m[2][2] + v[3] * m[3][2];
	result[3] = v[0] * m[0][3] + v[1] * m[1][3] + v[2] * m[2][3] + v[3] * m[3][3];
	
	return(result);
}

TMVec4 &operator *= (TMVec4 &v, const TMat4 &m)				// v *= m
{
	TMReal	t0, t1, t2;

	t0   = v[0] * m[0][0] + v[1] * m[1][0] + v[2] * m[2][0] + v[3] * m[3][0];
	t1   = v[0] * m[0][1] + v[1] * m[1][1] + v[2] * m[2][1] + v[3] * m[3][1];
	t2   = v[0] * m[0][2] + v[1] * m[1][2] + v[2] * m[2][2] + v[3] * m[3][2];
	v[3] = v[0] * m[0][3] + v[1] * m[1][3] + v[2] * m[2][3] + v[3] * m[3][3];
	v[0] = t0;
	v[1] = t1;
	v[2] = t2;
	
	return(v);
}

TMat4 trans(const TMat4 &m)
{
#define M(x,y) m[x][y]
#define R(x,y) result[x][y]

	TMat4 result;

	R(0,0) = M(0,0); R(0,1) = M(1,0); R(0,2) = M(2,0); R(0,3) = M(3,0);
	R(1,0) = M(0,1); R(1,1) = M(1,1); R(1,2) = M(2,1); R(1,3) = M(3,1);
	R(2,0) = M(0,2); R(2,1) = M(1,2); R(2,2) = M(2,2); R(2,3) = M(3,2);
	R(3,0) = M(0,3); R(3,1) = M(1,3); R(3,2) = M(2,3); R(3,3) = M(3,3);
		
	return(result);
	
#undef M
#undef R
}

TMat4 adj(const TMat4 &m)			
{
	TMat4	result;

	result[0] =  cross(m[1], m[2], m[3]);
	result[1] = -cross(m[0], m[2], m[3]);
	result[2] =  cross(m[0], m[1], m[3]);
	result[3] = -cross(m[0], m[1], m[2]);
	
	return(result);
}

TMReal trace(const TMat4 &m)
{
	return(m[0][0] + m[1][1] + m[2][2] + m[3][3]);
}
			
TMReal det(const TMat4 &m)
{
	return(dot(m[0], cross(m[1], m[2], m[3])));
}

TMat4 inv(const TMat4 &m)
{
	TMReal	mDet;
	TMat4	adjoint;
	TMat4	result;
	
	adjoint = adj(m); 				// Find the adjoint
	mDet = dot(adjoint[0], m[0]);
	
	Assert(mDet != 0, "(Mat4::inv) matrix is non-singular");

	result = trans(adjoint);
	result /= mDet;

	return(result);		
}

TMat4 oprod(const TMVec4 &a, const TMVec4 &b)
// returns outerproduct of a and b:  a * trans(b)
{
	TMat4	result;

	result[0] = a[0] * b;
	result[1] = a[1] * b;
	result[2] = a[2] * b;
	result[3] = a[3] * b;

	return(result);
}

Void TMat4::MakeZero()
{
	Int 	i;
	
	for (i = 0; i < 16; i++)
		((TMReal *) row)[i] = vl_zero;
}

Void TMat4::MakeDiag(TMReal k)
{
	Int		i, j;
	
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			if (i == j)
				row[i][j] = k;
			else
				row[i][j] = vl_zero;
}

Void TMat4::MakeBlock(TMReal k)
{
	Int 	i;
	
	for (i = 0; i < 16; i++)
		((TMReal *) row)[i] = k;
}

ostream &operator << (ostream &s, const TMat4 &m)
{
	Int w = s.width();

	return(s << '[' << m[0] << endl << setw(w) << m[1] << endl
		 << setw(w) << m[2] << endl << setw(w) << m[3] << ']' << endl);
}

istream &operator >> (istream &s, TMat4 &m)
{
    TMat4	result;
    Char	c;
	
	// Expected format: [[1 2 3] [4 5 6] [7 8 9]]
	// Each vector is a column of the matrix.
	
    while (s >> c && isspace(c))		// ignore leading white space
		;
		
    if (c == '[')			
    {
		s >> result[0] >> result[1] >> result[2] >> result[3];

		if (!s)
		{
			cerr << "Expected number while reading matrix\n";
			return(s);
		}
			
		while (s >> c && isspace(c))
			;
			
		if (c != ']')
    	{
    		s.clear(ios::failbit);
	    	cerr << "Expected ']' while reading matrix\n";
	    	return(s);
    	}
	}
    else
	{
	    s.clear(ios::failbit);
	    cerr << "Expected '[' while reading matrix\n";
	    return(s);
	}
	
	m = result;
    return(s);
}


TMat4& TMat4::MakeHRot(const TQuaternion &q)					
{	
	TMReal	i2 =  2 * q[0],
			j2 =  2 * q[1],
			k2 =  2 * q[2],
			ij = i2 * q[1],
			ik = i2 * q[2],
			jk = j2 * q[2],
			ri = i2 * q[3],
			rj = j2 * q[3],
			rk = k2 * q[3];

	MakeDiag();
				
	i2 *= q[0];
	j2 *= q[1];
	k2 *= q[2];

#if VL_ROW_ORIENT
	row[0][0] = 1 - j2 - k2;  row[0][1] = ij + rk   ;  row[0][2] = ik - rj;
	row[1][0] = ij - rk    ;  row[1][1] = 1 - i2- k2;  row[1][2] = jk + ri;
	row[2][0] = ik + rj	   ;  row[2][1] = jk - ri   ;  row[2][2] = 1 - i2 - j2;
#else
	row[0][0] = 1 - j2 - k2;  row[0][1] = ij - rk   ;  row[0][2] = ik + rj;
	row[1][0] = ij + rk    ;  row[1][1] = 1 - i2- k2;  row[1][2] = jk - ri;
	row[2][0] = ik - rj	   ;  row[2][1] = jk + ri   ;  row[2][2] = 1 - i2 - j2;
#endif

	return(SELF);
}

TMat4& TMat4::MakeHRot(const TMVec3 &axis, Real theta)	
{
	TMReal		s;
	TMVec4		q;
	
	theta /= 2.0;
	s = sin(theta);
	
	q[0] = s * axis[0];
	q[1] = s * axis[1];
	q[2] = s * axis[2];
	q[3] = cos(theta);
	
	MakeHRot(q);

	return(SELF);
}

TMat4& TMat4::MakeHScale(const TMVec3 &s)	
{
	MakeDiag();
		
	row[0][0] = s[0];
	row[1][1] = s[1];
	row[2][2] = s[2];

	return(SELF);
}

TMat4& TMat4::MakeHTrans(const TMVec3 &t)	
{
	MakeDiag();
		
#ifdef VL_ROW_ORIENT
	row[3][0] = t[0];
	row[3][1] = t[1];
	row[3][2] = t[2];
#else
	row[0][3] = t[0];
	row[1][3] = t[1];
	row[2][3] = t[2];
#endif

	return(SELF);
}

TMat4& TMat4::Transpose()
{
	row[0][1] = row[1][0]; row[0][2] = row[2][0]; row[0][3] = row[3][0];
	row[1][0] = row[0][1]; row[1][2] = row[2][1]; row[1][3] = row[3][1];
	row[2][0] = row[0][2]; row[2][1] = row[1][2]; row[2][3] = row[3][2];
	row[3][0] = row[0][3]; row[3][1] = row[1][3]; row[3][2] = row[2][3];

	return(SELF);
}

TMat4& TMat4::AddShift(const TMVec3 &t)
{
#ifdef VL_ROW_ORIENT
	row[3][0] += t[0];
	row[3][1] += t[1]; 
	row[3][2] += t[2];
#else
	row[0][3] += t[0];
	row[1][3] += t[1]; 
	row[2][3] += t[2];
#endif

	return(SELF);
}
