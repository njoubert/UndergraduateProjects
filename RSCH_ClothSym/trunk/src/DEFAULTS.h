#ifndef DEFAULTS_H_
#define DEFAULTS_H_


#define GRAVITY 0,-9.8,0


#define DEFAULT_TIMESTEP	0.02
#define DEFAULT_SYSTEM	System
#define DEFAULT_SOLVER	NewmarkSolver
#define DEFAULT_MATERIAL Material

#define DEFAULT_KS 100
#define DEFAULT_KD 5
#define DEFAULT_KBE 0.1
#define DEFAULT_KBD 0.01

#define DEFAULT_GAMMA 0.6

#define DEFAULT_SPARSEMAT Coord_Mat_double

#define SPARSE_MATRIX LargeMat3Matrix
#define LARGE_VECTOR LargeVec3Vector

#define MAX_CG_ITER 100
#define MAX_CG_ERR 0.00001

#endif /* DEFAULTS */
