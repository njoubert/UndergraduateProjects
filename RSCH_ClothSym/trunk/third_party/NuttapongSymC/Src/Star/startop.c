/*****************************************************************************/
/*                                                                           */
/*                      ,d88""\   d8                                         */
/*                      8888    _d88__   o8888o  88o88o                      */
/*                      `Y88b    888         88b 888                         */
/*                       `Y88b,  888    e888o888 888                         */
/*                         8888  888   C888  888 888                         */
/*                      \_o88P'  "88o"  "888"888 888                         */
/*                                                                           */
/*  A Three-Dimensional Delaunay Tetrahedralizer.                            */
/*  (Star.c)                                                                 */
/*                                                                           */
/*  Version 0.11                                                             */
/*  16 February 2008                                                         */
/*  PRE-RELEASE CODE:  DO NOT DISTRIBUTE!!!                                  */
/*  EXPECT THIS CODE TO BE BUGGY.                                            */
/*                                                                           */
/*  Portions of Star written prior to June 30, 1998 are                      */
/*  Copyright 1995, 1996, 1997, 1998                                         */
/*  Jonathan Richard Shewchuk                                                */
/*  965 Sutter Street #815                                                   */
/*  San Francisco, California  94109-6082                                    */
/*  jrs@cs.berkeley.edu                                                      */
/*                                                                           */
/*  Portions of Star written after June 30, 1998 are in the public domain,   */
/*  but Star as a whole is not.                                              */
/*                                                                           */
/*  This program may be freely redistributed under the condition that the    */
/*  copyright notices (including this entire header and the copyright notice */
/*  printed when the `-h' switch is selected) are not removed, and no        */
/*  compensation is received.  Private, research, and institutional use is   */
/*  free.  You may distribute modified versions of this code UNDER THE       */
/*  CONDITION THAT THIS CODE AND ANY MODIFICATIONS MADE TO IT IN THE SAME    */
/*  FILE REMAIN UNDER COPYRIGHT OF THE ORIGINAL AUTHOR, BOTH SOURCE AND      */
/*  OBJECT CODE ARE MADE FREELY AVAILABLE WITHOUT CHARGE, AND CLEAR NOTICE   */
/*  IS GIVEN OF THE MODIFICATIONS.  Distribution of this code as part of a   */
/*  commercial system is permissible ONLY BY DIRECT ARRANGEMENT WITH THE     */
/*  AUTHOR.  (If you are not directly supplying this code to a customer, and */
/*  you are instead telling them how they can obtain it for free, then you   */
/*  are not required to make any arrangement with me.)                       */
/*                                                                           */
/*  Hypertext instructions for Star are available on the Web at              */
/*                                                                           */
/*      http://www.cs.cmu.edu/~quake/star.html                               */
/*                                                                           */
/*  Disclaimer:  Neither I nor any institution I have been associated with   */
/*  warrant this code in any way whatsoever.  This code is provided "as-is". */
/*  Use at your own risk.                                                    */
/*                                                                           */
/*  Some of the references listed below are marked with an asterisk.  [*]    */
/*  These references are available for downloading from the Web page         */
/*                                                                           */
/*      http://www.cs.cmu.edu/~quake/star.research.html                      */
/*                                                                           */
/*  Two papers discussing the mesh generation algorithm used in Star are     */
/*  available.  A short overview is contained in Jonathan Richard Shewchuk,  */
/*  "Tetrahedral Mesh Generation by Delaunay Refinement," Proceedings of the */
/*  Fourteenth Annual Symposium on Computational Geometry (Minneapolis,      */
/*  Minnesota), pp. 86-95, ACM, June 1998.  [*]  A good deal more detail may */
/*  be found in my dissertation:  Jonathan Richard Shewchuk, "Delaunay       */
/*  Refinement Mesh Generation," Ph.D. thesis, Technical Report              */
/*  CMU-CS-97-137, School of Computer Science, Carnegie Mellon University,   */
/*  Pittsburgh, Pennsylvania, 18 May 1997.  [*]                              */
/*                                                                           */
/*  The triangulation data structures are adapted from the star-based        */
/*  simplex dictionary of Daniel K. Blandford, Guy E. Blelloch, David E.     */
/*  Cardoze, and Clemens Kadow, "Compact Representations of Simplicial       */
/*  Meshes in Two and Three Dimensions," International Journal of            */
/*  Computational Geometry and Applications 15(1):3-24, February 2005.       */
/*                                                                           */
/*  The incremental insertion algorithm was proposed simultaneously by       */
/*  Adrian Bowyer, "Computing Dirichlet Tessellations," Computer Journal     */
/*  24(2):162-166, 1981, and David F. Watson, "Computing the n-dimensional   */
/*  Delaunay Tessellation with Applications to Voronoi Polytopes," Computer  */
/*  Journal 24(2):167-172, 1981.  The idea of using a BRIO (biased           */
/*  randomized insertion order) to achieve both worst-case optimal running   */
/*  time and good spatial coherence (memory locality) come from Nina Amenta, */
/*  Sunghee Choi, and Gunter Rote, "Incremental Constructions con BRIO,"     */
/*  Proceedings of the Nineteenth Annual Symposium on Computational Geometry */
/*  (San Diego, California), pages 211-219, Association for Computing        */
/*  Machinery, June 2003.                                                    */
/*                                                                           */
/*  The algorithms for exact computation of the signs of determinants are    */
/*  described in Jonathan Richard Shewchuk, "Adaptive Precision Floating-    */
/*  Point Arithmetic and Fast Robust Geometric Predicates," Discrete &       */
/*  Computational Geometry 18(3):305-363, October 1997.  (Also available as  */
/*  Technical Report CMU-CS-96-140, School of Computer Science, Carnegie     */
/*  Mellon University, Pittsburgh, Pennsylvania, May 1996.)  [*]             */
/*  An abbreviated version appears as Jonathan Richard Shewchuk, "Robust     */
/*  Adaptive Floating-Point Geometric Predicates," Proceedings of the        */
/*  Twelfth Annual Symposium on Computational Geometry, ACM, May 1996. [*]   */
/*  Many of the ideas for my exact arithmetic routines originate with        */
/*  Douglas M. Priest, "Algorithms for Arbitrary Precision Floating Point    */
/*  Arithmetic," Tenth Symposium on Computer Arithmetic, pp. 132-143, IEEE   */
/*  Computer Society Press, 1991.  [*]  Many of the ideas for the correct    */
/*  evaluation of the signs of determinants are taken from Steven Fortune    */
/*  and Christopher J. Van Wyk, "Efficient Exact Arithmetic for              */
/*  Computational Geometry," Proceedings of the Ninth Annual Symposium on    */
/*  Computational Geometry, ACM, pp. 163-172, May 1993, and from Steven      */
/*  Fortune, "Numerical Stability of Algorithms for 2D Delaunay Triangu-     */
/*  lations," International Journal of Computational Geometry & Applications */
/*  5(1-2):193-213, March-June 1995.                                         */
/*                                                                           */
/*  For definitions of and results involving Delaunay tetrahedralizations    */
/*  and other aspects of triangulations and tetrahedral mesh generation, see */
/*  the excellent survey by Marshall Bern and David Eppstein, "Mesh          */
/*  Generation and Optimal Triangulation," in Computing and Euclidean        */
/*  Geometry, Ding-Zhu Du and Frank Hwang, editors, World Scientific,        */
/*  Singapore, pp. 23-90, 1992.  [*]                                         */
/*                                                                           */
/*  My Delaunay refinement algorithm typically generates tetrahedra at a     */
/*  linear rate (constant time per triangle) after the initial triangulation */
/*  is formed.  There may be pathological cases where more time is required, */
/*  but these never arise in practice.                                       */
/*                                                                           */
/*  The geometric predicates (circumcenter calculations, insphere tests,     */
/*  etc.) appear in my "Lecture Notes on Geometric Robustness" at            */
/*  http://www.cs.berkeley.edu/~jrs/mesh .                                   */
/*                                                                           */
/*  If you make any improvements to this code, please please please let me   */
/*  know, so that I may obtain the improvements.  Even if you don't change   */
/*  the code, I'd still love to hear what it's being used for.               */
/*                                                                           */
/*****************************************************************************/


/*  For single precision (which will save some memory and reduce paging),    */
/*  define the symbol SINGLE by using the -DSINGLE compiler switch or by     */
/*  writing "#define SINGLE" below.                                          */
/*                                                                           */
/*  For double precision (which will allow you to refine meshes to a smaller */
/*  edge length), leave SINGLE undefined.                                    */
/*                                                                           */
/*  Double precision uses more memory, but improves the resolution of the    */
/*  meshes you can generate with Star.  It also reduces the likelihood of a  */
/*  floating exception due to overflow.  Finally, it is faster than single   */
/*  precision on some architectures, like the DEC Alpha.  I recommend double */
/*  precision unless you want to generate a triangulation for which you do   */
/*  not have enough memory.                                                  */

/* #define SINGLE */

#ifdef SINGLE
#define starreal float
#else /* not SINGLE */
#define starreal double
#endif /* not SINGLE */

/*  If yours is not a Unix system, define the NO_TIMER compiler switch to    */
/*  remove the Unix-specific timing code.                                    */

/* #define NO_TIMER */

/*  To insert lots of self-checks for internal errors, define the SELF_CHECK */
/*  symbol.  This will slow down the program significantly.  It is best to   */
/*  define the symbol using the -DSELF_CHECK compiler switch, but you could  */
/*  write "#define SELF_CHECK" below.  If you are modifying this code, I     */
/*  recommend you turn self-checks on until your work is debugged.           */

/* #define SELF_CHECK */
/* #define PARANOID */

/*  To compile Star as a callable object library (Star.o), define the        */
/*  STARLIBRARY symbol.  Read the file Star.h for details on how to call the */
/*  procedure triangulate() that results.                                    */

/* #define STARLIBRARY */

/*  It is possible to generate a smaller version of Star using the CDT_ONLY  */
/*  symbol, which eliminates all meshing algorithms above and beyond         */
/*  conforming constrained Delaunay triangulation; specifically, the -r, -q, */
/*  -a, -u, -D, -Y, and -S switches.  These reductions are most likely to be */
/*  useful when generating an object library (Star.o) by defining the        */
/*  STARLIBRARY symbol.                                                      */

/* #define CDT_ONLY */

/*  On some machines, my exact arithmetic routines might be defeated by the  */
/*  use of internal extended precision floating-point registers.  The best   */
/*  way to solve this problem is to set the floating-point registers to use  */
/*  single or double precision internally.  On 80x86 processors, this may be */
/*  accomplished by setting the CPU86 symbol in Microsoft operating systems, */
/*  or the LINUX symbol in Linux.                                            */
/*                                                                           */
/*  An inferior solution is to declare certain values as `volatile', thus    */
/*  forcing them to be stored to memory and rounded off.  Unfortunately,     */
/*  this solution might slow Triangle down quite a bit.  To use volatile     */
/*  values, write "#define INEXACT volatile" below.  Normally, however,      */
/*  INEXACT should be defined to be nothing.  ("#define INEXACT".)           */
/*                                                                           */
/*  For more discussion, see Section 5 of my paper, "Adaptive Precision      */
/*  Floating-Point Arithmetic and Fast Robust Geometric Predicates" (also    */
/*  available as Section 6.6 of my dissertation).                            */

/* #define CPU86 */
/* #define LINUX */

#define INEXACT /* Nothing */
/* #define INEXACT volatile */

/*  Maximum number of characters in a file name (including the null).        */

#define FILENAMESIZE 2048

/*  Maximum number of characters in a line read from a file (including the   */
/*  null).                                                                   */

#define INPUTLINESIZE 1024

/*  A number that speaks for itself, every kissable digit.                   */

#define ST_PI 3.141592653589793238462643383279502884197169399375105820974944592308


#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#ifndef NO_TIMER
#include <sys/time.h>
#endif /* not NO_TIMER */
#ifdef CPU86
#include <float.h>
#endif /* CPU86 */
#ifdef LINUX
#include <fpu_control.h>
#endif /* LINUX */
#ifdef STARLIBRARY
#include "star.h"
#endif /* STARLIBRARY */


/*  `starlong' and `starulong' are the types of integer (signed and          */
/*  unsigned, respectively) of most of the indices used internally and       */
/*  externally by Star, including vertex and element numbers.  They          */
/*  determine the number of internal data structures that can be allocated,  */
/*  so long choices (e.g. ptrdiff_t and size_t, defined in stddef.h) are     */
/*  recommended.  If the number of tetrahedra might be around 2^28 or more,  */
/*  use 64-bit integers.  On a machine with 32-bit pointers (memory          */
/*  addresses), though, there's no point using integers bigger than 32 bits. */
/*  On a machine with limited memory, smaller integers might allow you to    */
/*  create larger meshes.                                                    */

typedef ptrdiff_t starlong;
typedef size_t starulong;


/*  Data structure for command line switches, file names, and operation      */
/*  counts.  Used (instead of global variables) to allow reentrancy.         */

struct behavior {

/*  Switches for the tetrahedralizer.                                        */
/*  Read the instructions to find out the meaning of these switches.         */

  int poly;                                                    /* -p switch. */
  int refine;                                                  /* -r switch. */
  int quality;                                                 /* -q switch. */
  /* Maximum acceptable tetrahedron circumradius-to-shortest edge ratio:     */
  starreal qmeasure;                           /* Specified after -q switch. */
  starreal minangle;          /* Min dehedral angle bound, after -q/ switch. */
  starreal goodangle;                         /* Cosine squared of minangle. */
  int varvolume;                      /* -a switch without number following. */
  int fixedvolume;                       /* -a switch with number following. */
  starreal maxvolume;    /* Maximum volume bound, specified after -a switch. */
  int usertest;                                                /* -u switch. */
  int regionattrib;                                            /* -A switch. */
  int convex;                                                  /* -c switch. */
  int weighted;                         /* 1 for -w switch, 2 for -W switch. */
  int conformdel;                                              /* -D switch. */
  int jettison;                                                /* -j switch. */
  int edgesout;                                                /* -e switch. */
  int facesout;                                                /* -f switch. */
  int voronoi;                                                 /* -v switch. */
  int neighbors;                                               /* -n switch. */
  int geomview;                                                /* -g switch. */
  int nobound;                                                 /* -B switch. */
  int nopolywritten;                                           /* -P switch. */
  int nonodewritten;                                           /* -N switch. */
  int noelewritten;                                            /* -E switch. */
  int noiterationnum;                                          /* -I switch. */
  int noholes;                                                 /* -O switch. */
  int noexact;                                                 /* -X switch. */
  /* All items are numbered starting from `firstnumber':                     */
  starulong firstnumber;                            /* Inverse of -z switch. */
  int order;                    /* Element order, specified after -o switch. */
  int nobisect;                 /* Count of how often -Y switch is selected. */
  starlong steiner;   /* Max # of Steiner points, specified after -S switch. */
  int jumpwalk;                                                /* -J switch. */
  int norandom;                                                /* -k switch. */
  int fullrandom;                                              /* -K switch. */
  int docheck;                                                 /* -C switch. */
  int quiet;                                                   /* -Q switch. */
  int verbose;                  /* Count of how often -V switch is selected. */
  /* Determines whether new vertices will be added, other than the input:    */
  int addvertices;                              /* -p, -q, -a, or -u switch. */
  /* Determines whether segments and facets are used at all:                 */
  int usefacets;                        /* -p, -r, -q, -a, -u, or -c switch. */

  int readnodefileflag;                       /* Has a .node file been read? */

/*  Variables for file names.                                                */

#ifndef STARLIBRARY
  char innodefilename[FILENAMESIZE];                    /* Input .node file. */
  char inelefilename[FILENAMESIZE];                      /* Input .ele file. */
  char inpolyfilename[FILENAMESIZE];                    /* Input .poly file. */
  char areafilename[FILENAMESIZE];                      /* Input .area file. */
  char outnodefilename[FILENAMESIZE];                  /* Output .node file. */
  char outelefilename[FILENAMESIZE];                    /* Output .ele file. */
  char outpolyfilename[FILENAMESIZE];                  /* Output .poly file. */
  char edgefilename[FILENAMESIZE];                     /* Output .edge file. */
  char facefilename[FILENAMESIZE];                     /* Output .face file. */
  char vnodefilename[FILENAMESIZE];                  /* Output .v.node file. */
  char vpolyfilename[FILENAMESIZE];                  /* Output .v.poly file. */
  char neighborfilename[FILENAMESIZE];                /* Output .neigh file. */
  char offfilename[FILENAMESIZE];                       /* Output .off file. */
#endif /* not STARLIBRARY */

/*  Counts of operations performed.                                          */

  starulong inspherecount;            /* Number of insphere tests performed. */
  starulong orientcount;        /* Number of 3D orientation tests performed. */
  starulong orient4dcount;      /* Number of 4D orientation tests performed. */
  starulong tetcircumcentercount;/* Number of tet circumcenter calculations. */
  starulong tricircumcentercount;    /* Triangular face circumcenter calc's. */

};                                              /* End of `struct behavior'. */

/*  Global constants.                                                        */

starreal splitter;   /* Used to split real factors for exact multiplication. */
starreal epsilon;                         /* Floating-point machine epsilon. */
starreal resulterrbound;
starreal o2derrboundA, o2derrboundB, o2derrboundC;
starreal o3derrboundA, o3derrboundB, o3derrboundC;
starreal isperrboundA, isperrboundB, isperrboundC;


/********* Memory allocation and program exit wrappers begin here    *********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  starexit()   Exits the program Star.                                     */
/*                                                                           */
/*  Used to give Star a single point of exit whose contents can easily be    */
/*  replaced to help interface with client programs.                         */
/*                                                                           */
/*  status:  Should be zero on normal termination; one if an error occurs.   */
/*                                                                           */
/*****************************************************************************/

void starexit(int status)
{
  exit(status);
}

/*****************************************************************************/
/*                                                                           */
/*  starmalloc()   Allocates memory from the operating system.               */
/*                                                                           */
/*  Used to give Star a single point of memory allocation whose contents can */
/*  easily be replaced to help interface with client programs.               */
/*                                                                           */
/*  size:  The number of contiguous bytes of memory to allocate.             */
/*                                                                           */
/*  Returns a pointer to the allocated memory.                               */
/*                                                                           */
/*****************************************************************************/

void *starmalloc(size_t size)
{
  void *memptr;

  memptr = malloc(size);
  if (memptr == (void *) NULL) {
    printf("Error:  Out of memory.\n");
    starexit(1);
  }
  return(memptr);
}

/*****************************************************************************/
/*                                                                           */
/*  starfree()   Returns previously allocated memory to the operating system.*/
/*                                                                           */
/*  Used to give Star a single point of memory freeing whose contents can    */
/*  easily be replaced to help interface with client programs.               */
/*                                                                           */
/*  memptr:  A pointer to the block of memory that should be freed so that   */
/*    it is available to be reallocated.                                     */
/*                                                                           */
/*****************************************************************************/

void starfree(void *memptr)
{
  free(memptr);
}

/**                                                                         **/
/**                                                                         **/
/********* Memory allocation and program exit wrappers end here      *********/


