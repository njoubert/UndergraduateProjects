/*
	File:			Solve.cpp

	Function:		Implements Solve.h

	Author(s):		Andrew Willmott

	Copyright:		(c) 1995-2000, Andrew Willmott

	Notes:			

*/


#include "vl\Solve.h"

/**	Solves Ax = b via gaussian elimination.

	Each iteration modifies the current approximate solution x.
	Omega controls overrelaxation: omega = 1 gives Gauss-Seidel,
	omega somewhere beteen 1 and 2 gives the fastest convergence.

	x is the initial guess on input, and solution vector on output.

	If steps is zero, the routine iterates until the residual is
	less than epsilon. Otherwise, it performs at most *steps 
	iterations, and returns the actual number of iterations performed
	in *steps.

	Returns approximate squared length of residual vector: |Ax-b|^2.

	[Strang, "Introduction to Applied Mathematics", 1986, p. 407]
*/

TMReal SolveOverRelax(
			const TMat		&A,
			TVec			&x,
			const TVec		&b,
			TMReal			epsilon,
			TMReal			omega,
			Int				*steps
		)
{
	Int		i, j, jMax;
	TMReal	sum;
	TMReal	diagonal, xOld, error;

	Assert(A.IsSquare(), "(SolveOverRelax) Matrix not square");
	j = 0;
	if (steps)
		jMax = *steps;
	else
		jMax = VL_MAX_STEPS;
		
	do
	{
		error = 0.0;
		for (i = 0; i < A.Rows(); i++)
		{
			sum = b[i] - dot(A[i], x);		
			diagonal = A[i][i];
			sum += diagonal * x[i];		
			// I.e., sum = b[i] - (A[i] * x - A[i,i])
			xOld = x[i];
			
			if (diagonal == 0)
				_Warning("(SolveOverRelax) diagonal element = 0");
			else if (omega == 1.0)	// Gauss-Seidel 
				x[i] = sum / diagonal;
			else					// Overrelax
				x[i] = Mix(xOld, sum / diagonal, (Real) omega); 
				
			sum -= diagonal * xOld;
			error += sqr(sum);
		}
		j++;
	}
	while (error > epsilon && j < jMax);

	if (steps)
		*steps = j;

	return(error);
}

/**	Solves Ax = b via gaussian elimination for a sparse matrix.

	See the dense version above for details.
*/

TMReal SolveOverRelax(
			const TSparseMat	&A,
			TVec				&x,
			const TVec			&b,
			TMReal				epsilon,
			TMReal				omega,
			Int					*steps
		)
{
	Int		i, j, jMax;
	TMReal	sum;
	TMReal	diagonal, xOld, error;

	Assert(A.IsSquare(), "(SolveOverRelax) Matrix not square");
	j = 0;
	if (steps)
		jMax = *steps;
	else
		jMax = VL_MAX_STEPS;
		
	do
	{
		error = 0.0;
		for (i = 0; i < A.Rows(); i++)
		{
			// sum = b[i] - (A[i] dot x - A[i,i])			 
		
			sum = b[i] - dot(A[i], x);		
			diagonal = A[i].Get(i);
			sum += diagonal * x[i];		
			
			xOld = x[i];
			
			if (diagonal == 0)
				_Warning("(SolveOverRelax) diagonal element = 0");
			else if (omega == 1)
				x[i] = sum / diagonal;	// Gauss-Seidel 
			else
				x[i] = Mix(xOld, sum / diagonal, (Real) omega); 
				
			sum -= diagonal * xOld;
			error += sqr(sum);
		}
		j++;
	}
	while (error > epsilon && j < jMax);

	if (steps)
		*steps = j;
		
	return(error);
}


/**
	Solve Ax = b by conjugate gradient method, for symmetric, positive
	definite A.
	
	x is the initial guess on input, and solution vector on output.

	Returns squared length of residual vector.

	If A is not symmetric, this will solve the system (A + At)x/2 = b

	[Strang, "Introduction to Applied Mathematics", 1986, p. 422]
*/


TMReal SolveConjGrad(
				const TMat		&A,				// Solve Ax = b.
				TVec			&x,
				const TVec		&b,
				TMReal			epsilon,		// how low should we go?
				Int				*steps			// iterations to converge.
			)
{
	Int		i, iMax;
	TMReal	alpha, beta, rSqrLen, rSqrLenOld, u;
	TVec	r(A.Rows());		// Residual vector, b - Ax 
	TVec	d(A.Rows());		// Direction to move x at each step 
	TVec	t(A.Rows());		// temp!

	Assert(A.IsSquare(), "(SolveConjGrad) Matrix not square");
	
	r = b;
	r -= A * x;				
	rSqrLen = sqrlen(r);
	d = r;						
	i = 0;
	if (steps)
		iMax = *steps;
	else
		iMax = VL_MAX_STEPS;
		
	if (rSqrLen < epsilon)
		while (i < iMax)	
		{	
			i++;
			t = A * d;		
			u = dot(d, t); 
			
			if (u == 0)
			{
				_Warning("(SolveConjGrad) d'Ad = 0");
				break;
			}
			
			alpha = rSqrLen / u;		// How far should we go?			
			x += alpha * d;				// Take a step along direction d
			if (i & 0x3F)
				r -= alpha * t; 
			else
			{
				r = b;		// For stability, correct r every 64th iteration
				r -= A * x;
			}
				
			rSqrLenOld = rSqrLen;
			rSqrLen = sqrlen(r); 
			
			if (rSqrLen <= epsilon)
				break;					// Converged! Let's get out of here
			
			beta = rSqrLen/rSqrLenOld;
			d *= beta;					// Change direction: d = r + beta * d
			d += r;
		}
		
	if (steps)
		*steps = i;
	
	return(rSqrLen);
}

/**	Solves Ax = b via conjugate gradient for a sparse matrix.

	See the dense version above for details.
*/

TMReal SolveConjGrad(
				const TSparseMat	&A,
				TVec				&x,
				const TVec			&b,
				TMReal				epsilon,
				Int					*steps
			)
{
	Int		i, iMax;
	TMReal	alpha, beta, rSqrLen, rSqrLenOld, u;
	TVec	r(b.Elts());		// Residual vector, b - Ax 
	TVec	d(b.Elts());		// Direction to move x at each step 
	TVec	t(b.Elts());

	Assert(A.IsSquare(), "(SolveConjGrad) Matrix not square");
	r = b;
	r -= A * x;				
	rSqrLen = sqrlen(r);
	d = r;						
	i = 0;
	if (steps)
		iMax = *steps;		
	else
		iMax = VL_MAX_STEPS;
		
	if (rSqrLen > epsilon)				// If we haven't already converged...
		while (i < iMax)
		{	
			i++;
			t = A * d;		
			u = dot(d, t);
			
			if (u == 0.0)
			{
				_Warning("(SolveConjGrad) d'Ad = 0");
				break;
			}
			
			alpha = rSqrLen / u;		// How far should we go?
			x += alpha * d;				// Take a step along direction d
			r -= alpha * t; 
			rSqrLenOld = rSqrLen;
			rSqrLen = sqrlen(r); 
			
			if (rSqrLen <= epsilon)
				break;					// Converged! Let's get out of here
			
			beta = rSqrLen / rSqrLenOld;
			d = r + beta * d;			//	Change direction
		}

	if (steps)
		*steps = i;
	
	return(rSqrLen);
}

