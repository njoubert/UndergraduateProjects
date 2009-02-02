#ifndef DEFAULTS_H_
#define DEFAULTS_H_


#define GRAVITY 0,-9.8,0


#define DEFAULT_TIMESTEP	0.02
#define DEFAULT_SYSTEM	System
#define DEFAULT_SOLVER	NewmarkSolver
//#define DEFAULT_SOLVER	ExplicitSolver
#define DEFAULT_MATERIAL Material

#define DEFAULT_MASS 1.0

#define DEFAULT_KS 100
#define DEFAULT_KD 5
#define DEFAULT_KBE 0.1
#define DEFAULT_KBD 0.01
#define DEFAULT_KCOLL 1000

//#define DEFAULT_COLL_HACK 1.0

#define DEFAULT_MUS 0.0
#define DEFAULT_MUD 0.0

//#define DEFAULT_GAMMA 0.64
//#define DEFAULT_GAMMA 0.9
#define DEFAULT_GAMMA 1.0
#define DEFAULT_BETA 0.25

#define DEFAULT_WIND 0 0 0


#define SPARSE_MATRIX LargeMat3Matrix
#define LARGE_VECTOR LargeVec3Vector

#define DEFAULT_MAX_CG_ITER 1000
#define DEFAULT_MAX_CG_ERR 0.00001

#define DEFAULT_CG SimpleCG

#endif /* DEFAULTS */
