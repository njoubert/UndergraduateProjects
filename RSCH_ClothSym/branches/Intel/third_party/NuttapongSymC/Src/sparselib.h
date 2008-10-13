/*--------------------------------------------------------------------------*\
 |                                                                          |
 |  Sparse Matrix Manager                                                   |
 |                                                                          |
 |  date         : May 30, 2005                                             |
 |  release      : 10.3                                                     |
 |  release_date : 1999, May 22                                             |
 |  file         : sparselib.hh                                             |
 |  author       : Enrico Bertolazzi                                        |
 |  email        : enrico.bertolazzi@ing.unitn.it                           |
 |                                                                          |
 |  This program is free software; you can redistribute it and/or modify    |
 |  it under the terms of the GNU General Public License as published by    |
 |  the Free Software Foundation; either version 2, or (at your option)     |
 |  any later version.                                                      |
 |                                                                          |
 |  This program is distributed in the hope that it will be useful,         |
 |  but WITHOUT ANY WARRANTY; without even the implied warranty of          |
 |  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           |
 |  GNU General Public License for more details.                            |
 |                                                                          |
 |  You should have received a copy of the GNU General Public License       |
 |  along with this program; if not, write to the Free Software             |
 |  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.               |
 |                                                                          |
 |  Copyright (C) 1999                                                      |
 |                                                                          |
 |            ___    ____  ___   _   _        ___    ____  ___   _   _      |
 |           /   \  /     /   \  \  /        /   \  /     /   \  \  /       |
 |          /____/ /__   /____/   \/        /____/ /__   /____/   \/        |
 |         /   \  /     /  \      /        /   \  /     /  \      /         |
 |        /____/ /____ /    \    /        /____/ /____ /    \    /          |
 |                                                                          |
 |      Enrico Bertolazzi                                                   |
 |      Dipartimento di Ingegneria Meccanica e Strutturale                  |
 |      Universita` degli Studi di Trento                                   |
 |      Via Mesiano 77, I-38050 Trento, Italy                               |
 |                                                                          |
\*--------------------------------------------------------------------------*/

//
// Possible customization
// # define SPARSELIB_DEBUG
// # define SPARSELIB_UNROLL
//

# ifndef SPARSELIB_HH
# define SPARSELIB_HH

# include <cmath>

// standard includes I/O
# include <iostream>
# include <iomanip>

// STL lib
# include <algorithm>
# include <functional>
#include <vector>
using namespace std;

/*
// #    #    ##     ####   #####    ####    ####
// ##  ##   #  #   #    #  #    #  #    #  #
// # ## #  #    #  #       #    #  #    #   ####
// #    #  ######  #       #####   #    #       #
// #    #  #    #  #    #  #   #   #    #  #    #
// #    #  #    #   ####   #    #   ####    ####
*/

# define SPARSELIB_ERR(W)                             \
  { using namespace ::std ;                           \
    cerr << "\n" << W  << "\nin file `" << __FILE__   \
         << "' on line " << __LINE__ << "\n" ;        \
    exit(0) ; }

# define SPARSELIB_TEST0(X,W) if ( !(X) ) SPARSELIB_ERR(W)

# ifdef SPARSELIB_DEBUG
  # define SPARSELIB_INLINE
  # define SPARSELIB_TEST(X,W) SPARSELIB_TEST0(X,W)
# else
  # define SPARSELIB_INLINE inline
  # define SPARSELIB_TEST(X,W)
# endif

// loops
# ifndef SPARSELIB_UNROLL

  # define SPARSELIB_MLOOP(N,DO)                                      \
    { index_type i_loop_var = N ; while ( i_loop_var-- > 0 ) { DO ;} }

  # define SPARSELIB_VLOOP(N,DO) \
   { for ( index_type i = 0 ; i < N ; ++i ) { DO ; } }

# else

  # define SPARSELIB_MLOOP(N,DO)                                      \
    { index_type i_loop_var = N ;                                     \
      while ( i_loop_var >= 4 )                                       \
        { DO ; DO ; DO ; DO ; i_loop_var -= 4 ; }                     \
      while ( i_loop_var-- > 0 ) { DO ; }                             \
    }

  # define SPARSELIB_VLOOP(N,DO)                                      \
    { index_type i = N ;                                              \
      while ( i >= 4 )                                                \
        { --i ; DO ; --i ; DO ; --i ; DO ; --i ; DO ; }               \
      while ( i-- > 0 ) { DO ; }                                      \
    }

# endif

namespace sparselib_fun {
  static inline float       absval(float       const & a) { return a > 0 ? a : -a ; }
  static inline double      absval(double      const & a) { return a > 0 ? a : -a ; }
  static inline long double absval(long double const & a) { return a > 0 ? a : -a ; }

  static inline float const &       maxval(float       const & a, float       const & b) { return a > b ? a : b ; }
  static inline double  const &     maxval(double      const & a, double      const & b) { return a > b ? a : b ; }
  static inline long double const & maxval(long double const & a, long double const & b) { return a > b ? a : b ; }

  static inline float const &       minval(float       const & a, float       const & b) { return a < b ? a : b ; }
  static inline double  const &     minval(double      const & a, double      const & b) { return a < b ? a : b ; }
  static inline long double const & minval(long double const & a, long double const & b) { return a < b ? a : b ; }

  using namespace ::std ;

}
#ifdef WIN32
#include <float.h>
#else
#ifndef _isnan(x)
	#define _isnan(x) isnan(x)
#endif
#ifndef _finite(x)
	#define _finite(x) finite(x)
#endif
#endif

/////////////////////////////////////////////////////////////////////
namespace sparselib {
/////////////////////////////////////////////////////////////////////

  //using namespace ::std ;
  
  using ::std::istream ;
  using ::std::ostream ;
  using ::std::cin  ;
  using ::std::cout ;
  using ::std::cerr ;
  using ::std::setw ;
  using ::std::endl ;
  using ::std::greater ;
  using ::std::less ;

  // FROM BLITZ++

  /*
   * This compiler supports partial specialization, so type promotion
   * can be done the elegant way.  This implementation is after ideas
   * by Jean-Louis Leroy.
   */

  template<typename T>
  struct precision_trait {
    static unsigned const precisionRank     = 0 ;
    static bool     const knowPrecisionRank = false ;
  } ;

  # define SPARSELIB_DECLARE_PRECISION(T,rank)         \
    template<>                                         \
    struct precision_trait< T > {                      \
      static unsigned const precisionRank     = rank ; \
      static bool     const knowPrecisionRank = true ; \
    } ;

  SPARSELIB_DECLARE_PRECISION(int,100)
  SPARSELIB_DECLARE_PRECISION(unsigned int,200)
  SPARSELIB_DECLARE_PRECISION(long,300)
  SPARSELIB_DECLARE_PRECISION(unsigned long,400)
  SPARSELIB_DECLARE_PRECISION(float,500)
  SPARSELIB_DECLARE_PRECISION(double,600)
  SPARSELIB_DECLARE_PRECISION(long double,700)

  #ifdef SPARSELIB_HAVE_COMPLEX
  SPARSELIB_DECLARE_PRECISION(complex<float>,800)
  SPARSELIB_DECLARE_PRECISION(complex<double>,900)
  SPARSELIB_DECLARE_PRECISION(complex<long double>,1000)
  #endif

  template<typename T>
  struct autopromote_trait { typedef T T_numtype ; } ;

  // These are the odd cases where small integer types are
  // automatically promoted to int or unsigned int for arithmetic.
  template<> struct autopromote_trait<bool> { typedef int T_numtype ; } ;
  template<> struct autopromote_trait<char> { typedef int T_numtype ; } ;
  template<> struct autopromote_trait<unsigned char> { typedef int T_numtype ; };
  template<> struct autopromote_trait<short> { typedef int T_numtype ; } ;
  template<> struct autopromote_trait<unsigned short>
  { typedef unsigned T_numtype ; } ;

  template<typename T1, typename T2, bool promoteToT1>
  struct promote2 { typedef T1 T_promote ; };

  template<typename T1, typename T2>
  struct promote2<T1,T2,false> { typedef T2 T_promote ; };

  template<typename T1_orig, typename T2_orig>
  struct promote_trait {
    // Handle promotion of small integers to int/unsigned int
    typedef typename autopromote_trait<T1_orig>::T_numtype T1 ;
    typedef typename autopromote_trait<T2_orig>::T_numtype T2 ;

    // True if T1 is higher ranked
    static bool const T1IsBetter = precision_trait<T1>::precisionRank >
                                   precision_trait<T2>::precisionRank ;

    // True if we know ranks for both T1 and T2
    static bool const knowBothRanks = precision_trait<T1>::knowPrecisionRank
                                   && precision_trait<T2>::knowPrecisionRank ;

    // True if we know T1 but not T2
    static bool const knowT1butNotT2 = precision_trait<T1>::knowPrecisionRank &&
                                      !precision_trait<T2>::knowPrecisionRank ;

    // True if we know T2 but not T1
    static bool const knowT2butNotT1 = precision_trait<T2>::knowPrecisionRank &&
                                      !precision_trait<T1>::knowPrecisionRank ;

    // True if T1 is bigger than T2
    static bool const T1IsLarger = sizeof(T1) >= sizeof(T2) ;

    // We know T1 but not T2: true
    // We know T2 but not T1: false
    // Otherwise, if T1 is bigger than T2: true
    static bool const defaultPromotion =
      knowT1butNotT2 ? false : (knowT2butNotT1 ? true : T1IsLarger) ;

    // If we have both ranks, then use them.
    // If we have only one rank, then use the unknown type.
    // If we have neither rank, then promote to the larger type.

    static bool const promoteToT1 =
      knowBothRanks ? T1IsBetter : defaultPromotion ;

    typedef typename promote2<T1,T2,promoteToT1>::T_promote T_promote;
  } ;
  
  template<typename T1_orig, typename T2_orig, typename T3_orig>
  struct promote_trait3 {
    // Handle promotion of small integers to int/unsigned int
    typedef typename promote_trait<T1_orig,T2_orig>::T_promote T12_promote ;
    typedef typename promote_trait<T12_promote,T3_orig>::T_promote T_promote ;
  } ;

  # ifndef SPARSELIB_FUNCTION_TYPE
  # define SPARSELIB_FUNCTION_TYPE float
  # endif

  # ifndef SPARSELIB_ACCUMULATOR_TYPE
  # define SPARSELIB_ACCUMULATOR_TYPE double
  # endif

  # define SPARSELIB_PROMOTE(Ta,Tb) promote_trait<Ta,Tb>::T_promote
  # define SPARSELIB_PROMOTE3(Ta,Tb,Tc) \
    promote_trait<Ta,typename promote_trait<Tb,Tc>::T_promote>::T_promote

  # define SPARSELIB_ACCUMULATOR_PROMOTE(T) \
    SPARSELIB_PROMOTE(SPARSELIB_ACCUMULATOR_TYPE,T)

  # define SPARSELIB_ACCUMULATOR_PROMOTE2(Ta,Tb) \
    SPARSELIB_PROMOTE3(SPARSELIB_ACCUMULATOR_TYPE,Ta,Tb)

  # define SPARSELIB_TYPES_FROM_TYPENAME(A)                                  \
    typedef typename A::value_type        value_type ;                       \
    typedef typename A::pointer           pointer ;                          \
    typedef typename A::const_pointer     const_pointer ;                    \
    typedef typename A::reference         reference ;                        \
    typedef typename A::const_reference   const_reference

  # define SPARSELIB_TYPES_FROM_TYPENAME2(A,B)                               \
    typedef typename                                                         \
      promote_trait<typename A::value_type,typename B::value_type>::T_promote\
      promoted_value_type ;                                                  \
    SPARSELIB_TYPES(promoted_value_type)

  # define SPARSELIB_TYPES_FROM_TYPENAME3(S,A)                               \
    typedef typename promote_trait<S,typename A::value_type>::T_promote      \
      promoted_value_type ;                                                  \
    SPARSELIB_TYPES(promoted_value_type)

  # define SPARSELIB_TYPES_FROM_TYPENAME_FUN1(A) \
  SPARSELIB_TYPES_FROM_TYPENAME3(SPARSELIB_FUNCTION_TYPE,A)

  # define SPARSELIB_TYPES_FROM_TYPENAME_FUN2(A,B)                           \
    typedef typename                                                         \
      promote_trait<typename A::value_type,typename B::value_type>::T_promote\
      promoted_value_type ;                                                  \
    typedef typename promote_trait<SPARSELIB_FUNCTION_TYPE,                  \
                                   promoted_value_type>::T_promote           \
      promoted_fun_type ;                                                    \
    SPARSELIB_TYPES(promoted_fun_type)

  template <typename T> class Vector ;

  typedef unsigned          index_type ;
  typedef index_type*       index_pointer ;
  typedef const index_type* index_const_pointer ;
  typedef index_type&       index_reference ;
  typedef const index_type& index_const_reference ;

  inline index_const_reference
  min_index(index_const_reference a, index_const_reference b)
  { return a < b ? a : b ; }

  inline index_const_reference
  max_index(index_const_reference a, index_const_reference b)
  { return a > b ? a : b ; }

  struct all_ok {
    bool operator () (index_type const, index_type const) const
      { return true ; }
  } ;
  
  # define SPARSELIB_TYPES(T)                 \
    typedef T                 value_type ;    \
    typedef value_type*       pointer ;       \
    typedef const value_type* const_pointer ; \
    typedef value_type&       reference ;     \
    typedef const value_type& const_reference

  # define SPARSELIB_VECTOR_TYPES(T)                    \
    typedef Vector<T>            vector_type ;          \
    typedef vector_type*         vector_pointer ;       \
    typedef vector_type const *  vector_const_pointer ; \
    typedef vector_type&         vector_reference ;     \
    typedef vector_type const &  vector_const_reference

  /*
  //  ####   #####     ##    #####    ####   ######
  // #       #    #   #  #   #    #  #       #
  //  ####   #    #  #    #  #    #   ####   #####
  //      #  #####   ######  #####        #  #
  // #    #  #       #    #  #   #   #    #  #
  //  ####   #       #    #  #    #   ####   ######
  //
  //  #####  #####   ######  ######
  //    #    #    #  #       #
  //    #    #    #  #####   #####
  //    #    #####   #       #
  //    #    #   #   #       #
  //    #    #    #  ######  ######
  */

  // a / M
  template <typename VECTOR, typename MATRIX>
  struct Vector_V_div_M {
    VECTOR const & a ;
    MATRIX const & M ;
    Vector_V_div_M(VECTOR const & _a, MATRIX const & _M) : a(_a), M(_M) {}
  } ;

  // M * a
  template <typename MATRIX, typename VECTOR>
  struct Vector_M_mul_V {
    MATRIX const & M ;
    VECTOR const & a ;
    Vector_M_mul_V(MATRIX const & _M, VECTOR const & _a) : M(_M), a(_a) {}
  } ;

  // s * M * a
  template <typename SCALAR, typename MATRIX, typename VECTOR>
  struct Vector_S_mul_M_mul_V {
    SCALAR const & s ;
    MATRIX const & M ;
    VECTOR const & a ;
    Vector_S_mul_M_mul_V(SCALAR const & _s,
                         MATRIX const & _M,
                         VECTOR const & _a) : s(_s), M(_M), a(_a) {}
  } ;

  // a + M * b
  template <typename MATRIX, typename VECTOR>
  struct Vector_V_sum_M_mul_V {
    VECTOR const & a ;
    MATRIX const & M ;
    VECTOR const & b ;
    Vector_V_sum_M_mul_V(VECTOR const & _a, MATRIX const & _M, VECTOR const & _b)
      : a(_a), M(_M), b(_b) {}
  } ;

  // a - M * b
  template <typename MATRIX, typename VECTOR>
  struct Vector_V_sub_M_mul_V {
    VECTOR const & a ;
    MATRIX const & M ;
    VECTOR const & b ;
    Vector_V_sub_M_mul_V(VECTOR const & _a, MATRIX const & _M, VECTOR const & _b)
      : a(_a), M(_M), b(_b) {}
  } ;
  
  // a + s * M * b
  template <typename SCALAR, typename MATRIX, typename VECTOR>
  struct Vector_V_sum_S_mul_M_mul_V {
    VECTOR const & a ;
    SCALAR const & s ;
    MATRIX const & M ;
    VECTOR const & b ;
    Vector_V_sum_S_mul_M_mul_V(VECTOR const & _a,
                               SCALAR const & _s,
                               MATRIX const & _M,
                               VECTOR const & _b)
      : a(_a), s(_s), M(_M), b(_b) {}
  } ;

  // a - s * M * b
  template <typename SCALAR, typename MATRIX, typename VECTOR>
  struct Vector_V_sub_S_mul_M_mul_V {
    VECTOR const & a ;
    SCALAR const & s ;
    MATRIX const & M ;
    VECTOR const & b ;
    Vector_V_sub_S_mul_M_mul_V(VECTOR const & _a,
                               SCALAR const & _s,
                               MATRIX const & _M,
                               VECTOR const & _b)
      : a(_a), s(_s), M(_M), b(_b) {}
  } ;

  // M^-1 * a Preconditioner
  template <typename PRECO, typename VECTOR>
  struct Vector_P_mul_V {
    PRECO  const & P ;
    VECTOR const & a ;
    Vector_P_mul_V(PRECO const & PP, VECTOR const & aa) : P(PP), a(aa) {}
  } ;

  //

  template <typename T, typename A>
  class VectorE {
  public:
    SPARSELIB_TYPES(T) ;

  private:
    A value ;

  public:
    VectorE(A const & a) : value(a) { }
    value_type operator [] (index_type i) const { return value[i] ; }
    index_type size(void) const { return value . size() ; }
  } ;

  template <typename T>
  class Vector {
  public:
    SPARSELIB_TYPES(T) ;
    SPARSELIB_VECTOR_TYPES(T) ;
    Vector(T* ppp, index_type numElements) {
      SetPtr(ppp, numElements);
    }

    void SetPtr(T* ppp, index_type numElements) {
       iallocate = false;
       _begin = ppp;
       _end = _begin + numElements;
       _size = numElements;


    }
  private:
    bool iallocate;
    index_type _size ;
    pointer    _begin, _end ;

    void allocate(index_type sz) {
      _size = sz ;
      try {
        _begin = new value_type[_size] ;
      }
      catch (...) {
        SPARSELIB_ERR("Vector<T>::allocate(" << sz <<
                      ") FAILED TO ALLOCATE" ) ;
      }
      _end = _begin + _size ;
    }

  public:

    Vector() : _begin(0), _end(0) { iallocate = false; } ;
    Vector(index_type sz) { allocate(sz) ; iallocate = true; } ;
    virtual ~Vector(void) { free() ; }

    void new_dim(index_type sz)
    { if ( _size != sz ) { free() ; allocate(sz); iallocate = true; } }

    index_type size(void) const { return _size ; }

    const_reference operator[] (index_type i) const {
      SPARSELIB_TEST( i < _size,
                      "Vector[" << i << "] max size " << _size << " bad index")
      return _begin[i] ;
    }
    reference operator[] (index_type i) {
      SPARSELIB_TEST( i < _size,
                      "Vector[" << i << "] max size " << _size << " bad index")
      return _begin[i] ;
    }

    const_pointer begin(void) const { return _begin ; }
    pointer       begin(void)       { return _begin ; }
    const_pointer end(void)   const { return _end ; }
    pointer       end(void)         { return _end ; }

    void free(void) {
       if (iallocate) {
          if ( _begin != 0 ) delete [] _begin ;
          _begin = _end = 0 ;
          _size = 0 ;
       }
    }

    void fill(index_type b, index_type e, const_reference v) {
      SPARSELIB_TEST( b <= e, "Vector::fill(" << b << "," << e << ") bad range")
      SPARSELIB_TEST( e < _size,
                      "Vector::fill(" << b << "," << e << ") out of range")
      pointer p = _begin + b ;
      SPARSELIB_VLOOP( e-b, *p++ = v) ;
    }

    # define SPARSELIB_VASSIGN(OP)                            \
    vector_const_reference                                    \
    operator OP (vector_const_reference v) {                  \
      index_type sz = min_index( size(), v.size() ) ;         \
      SPARSELIB_VLOOP(sz, (*this)[i] OP v[i]) ;               \
      return *this ;                                          \
    }                                                         \
    template <typename R> inline                              \
    vector_const_reference                                    \
    operator OP (VectorE<T,R> const & e) {                    \
      index_type sz = min_index( size(), e.size() ) ;         \
      SPARSELIB_VLOOP(sz, (*this)[i] OP e[i]) ;               \
      return *this ;                                          \
    }                                                         \
    template <typename S> inline                              \
    vector_const_reference                                    \
    operator OP (S const & s) {                               \
      SPARSELIB_VLOOP(size(), (*this)[i] OP value_type(s)) ;  \
      return *this ;                                          \
    }

    SPARSELIB_VASSIGN(=)
    SPARSELIB_VASSIGN(+=)
    SPARSELIB_VASSIGN(-=)
    SPARSELIB_VASSIGN(*=)
    SPARSELIB_VASSIGN(/=)

    # undef SPARSELIB_VASSIGN

    // ------------ COMPLEX MATRIX OPERATIONS ---------------------
    // a / M
    template <typename MATRIX> inline
    vector_const_reference
    operator = (Vector_V_div_M<vector_type,MATRIX> const & op)
    { op.M.ass_V_div_M(*this, op.a) ; return *this ; }

    // M * a
    template <typename MATRIX> inline
    vector_const_reference
    operator = (Vector_M_mul_V<MATRIX,vector_type> const & op)
    { op.M.ass_M_mul_V(*this, op.a) ; return *this ; }

    // s * M * a
    template <typename SCALAR, typename MATRIX> inline
    vector_const_reference
    operator = (Vector_S_mul_M_mul_V<SCALAR,MATRIX,vector_type> const & op)
    { op.M.ass_S_mul_M_mul_V(*this, op.s, op.a) ; return *this ; }

    // a + M * b
    template <typename MATRIX> inline
    vector_const_reference
    operator = (Vector_V_sum_M_mul_V<MATRIX,vector_type> const & op)
    { op.M.ass_V_sum_M_mul_V(*this, op.a, op.b) ; return *this ; }

    // a - M * b
    template <typename MATRIX> inline
    vector_const_reference
    operator = (Vector_V_sub_M_mul_V<MATRIX,vector_type> const & op)
    { op.M.ass_V_sub_M_mul_V(*this, op.a, op.b) ; return *this ; }

    // a + s * M * b
    template <typename SCALAR, typename MATRIX> inline
    vector_const_reference
    operator = (Vector_V_sum_S_mul_M_mul_V<SCALAR,MATRIX,vector_type> const & op)
    { op.M.ass_V_sum_S_mul_M_mul_V(*this, op.a, op.s, op.b) ; return *this ; }

    // P^-1 * a Preconditioner
    template <typename PRECO> inline
    vector_const_reference
    operator = (Vector_P_mul_V<PRECO,vector_type> const & op)
    { op.P.ass_preco(*this, op.a) ; return *this ; }

  } ;

  ///////////////////////////////////////////////////////////////////////////////

  # define SPARSELIB_V(OP,OP_V)                                               \
  template<typename A>                                                        \
  class OP_V {                                                                \
  public:                                                                     \
    SPARSELIB_TYPES_FROM_TYPENAME(A) ;                                        \
  private:                                                                    \
    A const & a ;                                                             \
  public:                                                                     \
    OP_V(A const & aa) : a(aa) { }                                            \
    value_type operator [] (index_type i) const { return OP a[i] ; }          \
    index_type size(void) const { return a . size() ; }                       \
  };                                                                          \
                                                                              \
  template<typename T> inline                                                 \
  VectorE<T,OP_V<Vector<T> > >                                                \
  operator OP (Vector<T> const & a) {                                         \
    typedef OP_V<Vector<T> > op ;                                             \
    return VectorE<T,op>(op(a));                                              \
  }                                                                           \
                                                                              \
  template <typename T, typename A> inline                                    \
  VectorE<T,OP_V<VectorE<T,A> > >                                             \
  operator OP (VectorE<T,A> const & a) {                                      \
    typedef OP_V<VectorE<T,A> > op ;                                          \
    return VectorE<T,op>(op(a));                                              \
  }

  # define SPARSELIB_VV(OP,V_OP_V)                                            \
  template<typename A, typename B>                                            \
  class V_OP_V {                                                              \
  public:                                                                     \
    SPARSELIB_TYPES_FROM_TYPENAME2(A,B) ;                                     \
  private:                                                                    \
    A const & a ;                                                             \
    B const & b ;                                                             \
  public:                                                                     \
    V_OP_V(A const & aa, B const & bb) : a(aa), b(bb) { }                     \
    value_type operator [] (index_type i) const { return a[i] OP b[i] ; }     \
    index_type size(void) const { return min_index(a.size(), b.size()) ; }    \
  } ;                                                                         \
                                                                              \
  template<typename T> inline                                                 \
  VectorE<T,V_OP_V<Vector<T>,Vector<T> > >                                    \
  operator OP (Vector<T> const & a, Vector<T> const & b) {                    \
    typedef V_OP_V<Vector<T>,Vector<T> > op ;                                 \
    return VectorE<T,op>(op(a,b)) ;                                           \
  }                                                                           \
                                                                              \
  template <typename T, typename A> inline                                    \
  VectorE<T,V_OP_V<VectorE<T,A>,Vector<T> > >                                 \
  operator OP (VectorE<T,A> const & a, Vector<T> const & b) {                 \
    typedef V_OP_V<VectorE<T,A>,Vector<T> > op ;                              \
    return VectorE<T,op>(op(a,b));                                            \
  }                                                                           \
                                                                              \
  template <typename T, typename B> inline                                    \
  VectorE<T,V_OP_V<Vector<T>,VectorE<T,B> > >                                 \
  operator OP (Vector<T> const & a, VectorE<T,B> const & b) {                 \
    typedef V_OP_V<Vector<T>,VectorE<T,B> > op ;                              \
    return VectorE<T,op>(op(a,b));                                            \
  }                                                                           \
                                                                              \
  template <typename T, typename A, typename B> inline                        \
  VectorE<T,V_OP_V<VectorE<T,A>, VectorE<T,B> > >                             \
  operator OP (VectorE<T,A> const & a, VectorE<T,B> const & b) {              \
    typedef V_OP_V<VectorE<T,A>,VectorE<T,B> > op ;                           \
    return VectorE<T,op>(op(a,b));                                            \
  }

  # define SPARSELIB_SV(OP,S_OP_V)                                            \
  template<typename S, typename B>                                            \
  class S_OP_V {                                                              \
  public:                                                                     \
    SPARSELIB_TYPES_FROM_TYPENAME3(S,B) ;                                     \
  private:                                                                    \
    S const & a ;                                                             \
    B const & b ;                                                             \
  public:                                                                     \
    S_OP_V(S const & aa, B const & bb) : a(aa), b(bb) { }                     \
    value_type operator [] (index_type i) const { return a OP b[i] ; }        \
    index_type size(void) const { return b . size() ; }                       \
  } ;                                                                         \
                                                                              \
  template <typename T, typename S> inline                                    \
  VectorE<T,S_OP_V<S,Vector<T> > >                                            \
  operator OP (S const & s, Vector<T> const & v) {                            \
    typedef S_OP_V<S,Vector<T> > op ;                                         \
    return VectorE<T,op>(op(s,v));                                            \
  }                                                                           \
                                                                              \
  template <typename T, typename S, typename A> inline                        \
  VectorE<T,S_OP_V<S,VectorE<T,A> > >                                         \
  operator OP (S const & s, VectorE<T,A> const & v) {                         \
    typedef S_OP_V<S,VectorE<T,A> > op ;                                      \
    return VectorE<T,op>(op(s,v));                                            \
  }

  # define SPARSELIB_VS(OP,V_OP_S)                                            \
  template<typename S, typename A>                                            \
  class V_OP_S {                                                              \
  public:                                                                     \
    SPARSELIB_TYPES_FROM_TYPENAME3(S,A) ;                                     \
  private:                                                                    \
    A const & a ;                                                             \
    S const & b ;                                                             \
  public:                                                                     \
    V_OP_S(A const & aa, S const & bb) : a(aa), b(bb) { }                     \
    value_type operator [] (index_type i) const { return a[i] OP b ; }        \
    index_type size(void) const { return a . size() ; }                       \
  } ;                                                                         \
                                                                              \
  template <typename T, typename S> inline                                    \
  VectorE<T,V_OP_S<S,Vector<T> > >                                            \
  operator OP (Vector<T> const & v, S const & s) {                            \
    typedef V_OP_S<S,Vector<T> > op ;                                         \
    return VectorE<T,op>(op(v,s)) ;                                           \
  }                                                                           \
                                                                              \
  template <typename T, typename S, typename A> inline                        \
  VectorE<T, V_OP_S<S,VectorE<T,A> > >                                        \
  operator OP (VectorE<T,A> const & v, S const & s) {                         \
    typedef V_OP_S<S,VectorE<T,A> > op ;                                      \
    return VectorE<T,op>(op(v,s)) ;                                           \
  }


  SPARSELIB_V(-,Vector_neg_V)

  SPARSELIB_VV(+,Vector_V_sum_V)
  SPARSELIB_VV(-,Vector_V_sub_V)
  SPARSELIB_VV(*,Vector_V_mul_V)
  SPARSELIB_VV(/,Vector_V_div_V)

  SPARSELIB_SV(+,Vector_S_sum_V)
  SPARSELIB_SV(-,Vector_S_sub_V)
  SPARSELIB_SV(*,Vector_S_mul_V)
  SPARSELIB_SV(/,Vector_S_div_V)

  SPARSELIB_VS(+,Vector_V_sum_S)
  SPARSELIB_VS(-,Vector_V_sub_S)
  SPARSELIB_VS(*,Vector_V_mul_S)
  SPARSELIB_VS(/,Vector_V_div_S)

  # define SPARSELIB_F_V(FUN)                                                 \
  template<typename A>                                                        \
  class Vector_##FUN {                                                        \
  public:                                                                     \
    SPARSELIB_TYPES_FROM_TYPENAME_FUN1(A) ;                                   \
  private:                                                                    \
    A const & a ;                                                             \
  public:                                                                     \
    Vector_##FUN(A const & aa) : a(aa) { }                                    \
    value_type operator [] (index_type i) const                               \
      { return ::sparselib_fun::FUN(a[i]) ; }                                 \
    index_type size(void) const { return a . size() ; }                       \
  } ;

  # define SPARSELIB_F_V_TREE(FUN)                                            \
  template<typename T> inline                                                 \
  VectorE<T,Vector_##FUN<Vector<T> > >                                        \
  FUN(Vector<T> const & a) {                                                  \
    typedef Vector_##FUN<Vector<T> > op ;                                     \
    return VectorE<T,op>(op(a));                                              \
  }                                                                           \
                                                                              \
  template <typename T, typename A> inline                                    \
  VectorE<T,Vector_##FUN<VectorE<T,A> > >                                     \
  FUN(VectorE<T,A> const & a) {                                               \
    typedef Vector_##FUN<VectorE<T,A> > op ;                                  \
    return VectorE<T,op>(op(a));                                              \
  }

  # define SPARSELIB_F_VV(FUN)                                                \
  template<typename A, typename B>                                            \
  class Vector_##FUN {                                                        \
  public:                                                                     \
    SPARSELIB_TYPES_FROM_TYPENAME_FUN2(A,B) ;                                 \
  private:                                                                    \
    A const & a ;                                                             \
    B const & b ;                                                             \
  public:                                                                     \
    Vector_##FUN(A const & aa, B const & bb) : a(aa), b(bb) { }               \
    value_type operator [] (index_type i) const                               \
      { return ::sparselib_fun::FUN(a[i], b[i]) ; }                           \
    index_type size(void) const { return min_index(a.size(), b.size()) ; }    \
  } ;

  # define SPARSELIB_F_VV_TREE(FUN)                                           \
  template<typename T> inline                                                 \
  VectorE<T,Vector_##FUN<Vector<T>,Vector<T> > >                              \
  FUN(Vector<T> const & a, Vector<T> const & b) {                             \
    typedef Vector_##FUN<Vector<T>,Vector<T> > op ;                           \
    return VectorE<T,op>(op(a,b)) ;                                           \
  }                                                                           \
                                                                              \
  template <typename T, typename A> inline                                    \
  VectorE<T,Vector_##FUN<VectorE<T,A>,Vector<T> > >                           \
  FUN(VectorE<T,A> const & a, Vector<T> const & b) {                          \
    typedef Vector_##FUN<VectorE<T,A>,Vector<T> > op ;                        \
    return VectorE<T,op>(op(a,b));                                            \
  }                                                                           \
                                                                              \
  template <typename T, typename B> inline                                    \
  VectorE<T,Vector_##FUN<Vector<T>,VectorE<T,B> > >                           \
  FUN(Vector<T> const & a, VectorE<T,B> const & b) {                          \
    typedef Vector_##FUN<Vector<T>,VectorE<T,B> > op ;                        \
    return VectorE<T,op>(op(a,b));                                            \
  }                                                                           \
                                                                              \
  template <typename T, typename A, typename B> inline                        \
  VectorE<T,Vector_##FUN<VectorE<T,A>, VectorE<T,B> > >                       \
  FUN(VectorE<T,A> const & a, VectorE<T,B> const & b) {                       \
    typedef Vector_##FUN<VectorE<T,A>,VectorE<T,B> > op ;                     \
    return VectorE<T,op>(op(a,b));                                            \
  }

  SPARSELIB_F_V(absval)
  SPARSELIB_F_V(sin)
  SPARSELIB_F_V(cos)
  SPARSELIB_F_V(tan)
  SPARSELIB_F_V(asin)
  SPARSELIB_F_V(acos)
  SPARSELIB_F_V(atan)
  SPARSELIB_F_V(cosh)
  SPARSELIB_F_V(sinh)
  SPARSELIB_F_V(tanh)

  SPARSELIB_F_V(sqrt)
  SPARSELIB_F_V(ceil)
  SPARSELIB_F_V(floor)
  SPARSELIB_F_V(log)
  SPARSELIB_F_V(log10)

  SPARSELIB_F_VV(pow)
  SPARSELIB_F_VV(atan2)

  SPARSELIB_F_VV(minval)
  SPARSELIB_F_VV(maxval)

  template<typename A>
  class F1 {
  public:
    typedef typename A::value_type      value_type ;
    typedef typename A::pointer         pointer ;
    typedef typename A::const_pointer   const_pointer ;
    typedef typename A::reference       reference ;
    typedef typename A::const_reference const_reference ;

    static inline
    value_type norm1(A const & a) {
      value_type res = 0 ;
      SPARSELIB_VLOOP( a.size(), res = res + ::sparselib_fun::absval(a[i]) ) ;
      return res ;
    }

    static inline
    value_type normi(A const & a) {
      value_type res = 0 ;
      SPARSELIB_VLOOP( a.size(), value_type bf = ::sparselib_fun::absval(a[i]) ;
                                 if ( bf > res ) res = bf) ;
      return res ;
    }

    static inline
    value_type norm2(A const & a) {
      value_type res = 0 ;
      SPARSELIB_VLOOP( a.size(), res = res + a[i] * a[i]) ;
      return ::sparselib_fun::sqrt(res) ;
    }

    static inline
    value_type normp(A const & a, const_reference p) {
      value_type res = 0 ;
      SPARSELIB_VLOOP( a.size(), res = res + ::sparselib_fun::pow(::sparselib_fun::absval(a[i]),p) ) ;
      return ::sparselib_fun::pow(res,1/p) ;
    }

    static inline
    value_type maxval(A const & a) {
      value_type res = a[0] ;
      SPARSELIB_VLOOP( a.size(), if ( a[i] > res ) res = a[i] ) ;
      return res ;
    }

    static inline
      value_type minval(A const & a) {
      value_type res = a[0] ;
      SPARSELIB_VLOOP( a.size(), if ( a[i] < res ) res = a[i] ) ;
      return res ;
    }

  };

  template<typename A, typename B>
  class F2 {
  public:

    SPARSELIB_TYPES_FROM_TYPENAME2(A,B) ;

    static inline
    value_type dot(A const & a, B const & b) {
      value_type res = 0 ;
      SPARSELIB_VLOOP( min_index(a.size(),b.size()), res = res + a[i] * b[i]) ;
      return res ;
    }

    static inline
    value_type dot_div(A const & a, B const & b) {
      value_type res = 0 ;
      SPARSELIB_VLOOP( min_index(a.size(),b.size()), res = res + a[i] / b[i]) ;
      return res ;
    }

    static inline
    value_type dist2(A const & a, B const & b) {
      value_type res = 0 ;
      SPARSELIB_VLOOP( min_index( a.size(), b.size() ),
                       value_type bf = a[i] - b[i] ; res = res + bf * bf) ;
      return res ;
    }

    static inline
    value_type dist(A const & a, B const & b) {
      value_type res = 0 ;
      SPARSELIB_VLOOP( min_index( a.size(), b.size() ),
                       value_type bf = a[i] - b[i] ; res = res + bf * bf) ;
      return ::sparselib_fun::sqrt(res) ;
    }

  } ;

  SPARSELIB_F_V_TREE(absval)
  SPARSELIB_F_V_TREE(sin)
  SPARSELIB_F_V_TREE(cos)
  SPARSELIB_F_V_TREE(tan)
  SPARSELIB_F_V_TREE(asin)
  SPARSELIB_F_V_TREE(acos)
  SPARSELIB_F_V_TREE(atan)
  SPARSELIB_F_V_TREE(cosh)
  SPARSELIB_F_V_TREE(sinh)
  SPARSELIB_F_V_TREE(tanh)

  SPARSELIB_F_V_TREE(sqrt)
  SPARSELIB_F_V_TREE(ceil)
  SPARSELIB_F_V_TREE(floor)
  SPARSELIB_F_V_TREE(log)
  SPARSELIB_F_V_TREE(log10)

  SPARSELIB_F_VV_TREE(pow)
  SPARSELIB_F_VV_TREE(atan2)
  SPARSELIB_F_VV_TREE(minval)
  SPARSELIB_F_VV_TREE(maxval)

  # undef SPARSELIB_V
  # undef SPARSELIB_VV
  # undef SPARSELIB_SV
  # undef SPARSELIB_VS
  # undef SPARSELIB_F_V
  # undef SPARSELIB_F_V_TREE
  # undef SPARSELIB_F_VV
  # undef SPARSELIB_F_VV_TREE

  // N O R M 1

  template <typename T> inline
  T norm1(Vector<T> const & v)
  { return F1<Vector<T> >::norm1(v) ; }

  template <typename T, typename A> inline
  T norm1(VectorE<T,A> const & v)
  { return F1<VectorE<T,A> >::norm1(v) ; }

  // N O R M 2

  template <typename T> inline
  T norm2(Vector<T> const & v)
  { return F1<Vector<T> >::norm2(v) ; }

  template <typename T, typename A> inline
  T norm2(VectorE<T,A> const & v)
  { return F1<VectorE<T,A> >::norm2(v) ; }

  // N O R M I

  template <typename T> inline
  T normi(Vector<T> const & v)
  { return F1<Vector<T> >::normi(v) ; }

  template <typename T, typename A> inline
  T normi(VectorE<T,A> const & v)
  { return F1<VectorE<T,A> >::normi(v) ; }

  // N O R M P

  template <typename T, typename S> inline
  T normp(Vector<T> const & v, S const & p)
  { return F1<Vector<T> >::normp(v,T(p)) ; }

  template <typename T, typename A, typename S> inline
  T normp(VectorE<T,A> const & v, S const & p)
  { return F1<VectorE<T,A> >::normp(v,T(p)) ; }

  // M A X

  template <typename T> inline
  T maxval(Vector<T> const & v)
  { return F1<Vector<T> >::maxval(v) ; }

  template <typename T, typename A> inline
  T maxval(VectorE<T,A> const & v)
  { return F1<VectorE<T,A> >::maxval(v) ; }

  // M I N

  template <typename T> inline
  T minval(Vector<T> const & v)
  { return F1<Vector<T> >::minval(v) ; }

  template <typename T, typename A> inline
  T minval(VectorE<T,A> const & v)
  { return F1<VectorE<T,A> >::minval(v) ; }

  // D O T

  template <typename T> inline
  T dot(Vector<T> const & a, Vector<T> const & b)
  { return F2<Vector<T>,Vector<T> >::dot(a,b) ; }

  template <typename T, typename A> inline
  T dot(VectorE<T,A> const & a, Vector<T> const & b)
  { return F2<VectorE<T,A>,Vector<T> >::dot(a,b) ; }

  template <typename T, typename A> inline
  T dot(Vector<T> const & a, VectorE<T,A> const & b)
  { return F2<Vector<T>,VectorE<T,A> >::dot(a,b) ; }

  template <typename T, typename A, typename B> inline
  T dot(VectorE<T,A> const & a, VectorE<T,B> const & b)
  { return F2<VectorE<T,A>,VectorE<T,B> >::dot(a,b) ; }

  // D O T _ D I V

  template <typename T> inline
  T dot_div(Vector<T> const & a, Vector<T> const & b)
  { return F2<Vector<T>,Vector<T> >::dot_div(a,b) ; }

  template <typename T, typename A> inline
  T dot_div(VectorE<T,A> const & a, Vector<T> const & b)
  { return F2<VectorE<T,A>,Vector<T> >::dot_div(a,b) ; }

  template <typename T, typename B> inline
  T dot_div(Vector<T> const & a, VectorE<T,B> const & b)
  { return F2<Vector<T>,VectorE<T,B> >::dot_div(a,b) ; }

  template <typename T, typename A, typename B> inline
  T dot_div(VectorE<T,A> const & a, VectorE<T,B> const & b)
  { return F2<VectorE<T,A>,VectorE<T,B> >::dot_div(a,b) ; }

  // D I S T

  template <typename T> inline
  T dist(Vector<T> const & a, Vector<T> const & b)
  { return F2<Vector<T>,Vector<T> >::dist(a,b) ; }

  template <typename T, typename A> inline
  T dist(VectorE<T,A> const & a, Vector<T> const & b)
  { return F2<VectorE<T,A>,Vector<T> >::dist(a,b) ; }

  template <typename T, typename A> inline
  T dist(Vector<T> const & a, VectorE<T,A> const & b)
  { return F2<Vector<T>,VectorE<T,A> >::dist(a,b) ; }

  template <typename T, typename A, typename B> inline
  T dist(VectorE<T,A> const & a, VectorE<T,B> const & b)
  { return F2<VectorE<T,A>,VectorE<T,B> >::dist(a,b) ; }

  // D I S T 2

  template <typename T> inline
  T dist2(Vector<T> const & a, Vector<T> const & b)
  { return F2<Vector<T>,Vector<T> >::dist2(a,b) ; }

  template <typename T, typename A> inline
  T dist2(VectorE<T,A> const & a, Vector<T> const & b)
  { return F2<VectorE<T,A>,Vector<T> >::dist2(a,b) ; }

  template <typename T, typename A> inline
  T dist2(Vector<T> const & a, VectorE<T,A> const & b)
  { return F2<Vector<T>,VectorE<T,A> >::dist2(a,b) ; }

  template <typename T, typename A, typename B> inline
  T dist2(VectorE<T,A> const & a, VectorE<T,B> const & b)
  { return F2<VectorE<T,A>,VectorE<T,B> >::dist2(a,b) ; }

  /*
   *  ###       # #######
   *   #       #  #     #
   *   #      #   #     #
   *   #     #    #     #
   *   #    #     #     #
   *   #   #      #     #
   *  ### #       #######
   */

  template<typename A>
  static inline
  void
  print(ostream & s, A const & v) {
    index_type sz1 = v . size() - 1 ;
    s << "[" ;
    for ( index_type i = 0 ; i < sz1 ; ++i ) s << v[i] << " , " ;
    s << v[sz1] << "]" ;
  }

  template <typename T> inline
  ostream & operator << (ostream & s, Vector<T> const & v) {
    print(s,v) ;
    return s ;
  }

  template <typename T, typename A>
  inline
  ostream & operator << (ostream & s, VectorE<T,A> const & v) {
    print(s,v) ;
    return s ;
  }

  template <typename T> inline
  istream & operator >> (istream & s, Vector<T> & v) {
    typename Vector<T>::pointer pv=v.begin() ;
    while ( pv != v.end() ) s >> *pv++ ;
    return s ;
  }

  /*
  //  #####  #     #  ###   #####  #    #        #####  ####### ######  #######
  // #     # #     #   #   #     # #   #        #     # #     # #     #    #
  // #     # #     #   #   #       #  #         #       #     # #     #    #
  // #     # #     #   #   #       ###           #####  #     # ######     #
  // #   # # #     #   #   #       #  #               # #     # #   #      #
  // #    #  #     #   #   #     # #   #        #     # #     # #    #     #
  //  #### #  #####   ###   #####  #    #        #####  ####### #     #    #
  */
  
  typedef struct { index_type lo, hi ; } stack_node ;

  static inline
  bool GT(index_type const i, index_type const i1)
  { return i>i1 ; }
    
  static inline
  bool GT(index_type const i,  index_type const j,
          index_type const i1, index_type const j1)
  { return i==i1 ? j>j1 : i>i1 ; }

  template <typename T>
  static void
  QuickSortI(index_type I[], T A[],
             index_type const total_elems,
             index_type const MAX_THRESH = 4) {
  
    using ::std::swap ;

    if ( total_elems <= 1 ) return ;
    if ( total_elems <= MAX_THRESH ) goto insert_sort ;
    {
      index_type lo = 0 ;
      index_type hi = total_elems - 1 ;

      stack_node stack[sizeof(index_type) * CHAR_BIT] ;
      stack_node *top = stack ;

      while ( top >= stack ) { // Stack not empty

        /* Select median value from among LO, MID, and HI. Rearrange *\
         * LO and HI so the three values are sorted. This lowers the *
         * probability of picking a pathological pivot value and     *
        \* skips a comparison for both the LEFT_PTR and RIGHT_PTR.   */

        index_type & I_hi  = I[hi] ;
        index_type & I_lo  = I[lo] ;
        index_type & I_mid = I[ (hi + lo) / 2 ] ;

        T & A_hi  = A[hi] ;
        T & A_lo  = A[lo] ;
        T & A_mid = A[ (hi + lo) / 2 ] ;

        if ( GT(I_lo, I_mid) ) {
          swap(I_mid, I_lo) ;
          swap(A_mid, A_lo) ;
        }
        if ( GT(I_mid, I_hi) ) {
          swap(I_hi, I_mid) ;
          swap(A_hi, A_mid) ;
          if ( GT(I_lo,I_mid) ) {
            swap(I_mid, I_lo) ;
            swap(A_mid, A_lo) ;
          }
        }

        index_type Pivot     = I_mid ;
        index_type left_ptr  = lo + 1;
        index_type right_ptr = hi - 1;

        /* Here's the famous ``collapse the walls'' section of quicksort. *\
         * Gotta like those tight inner loops!  They are the main reason  *
        \* that this algorithm runs much faster than others.              */

        do {

          while ( GT(Pivot, I[left_ptr])  ) ++left_ptr ;
          while ( GT(I[right_ptr], Pivot) ) --right_ptr ;

          if ( left_ptr < right_ptr ) {
            swap(I[left_ptr], I[right_ptr]);
            swap(A[left_ptr], A[right_ptr]);
            ++left_ptr ;
            --right_ptr ;
          } else if ( left_ptr == right_ptr ) {
            ++left_ptr ;
            --right_ptr ;
            break;
          }

        } while ( left_ptr <= right_ptr );

        /* Set up pointers for next iteration.  First determine whether   *\
         * left and right partitions are below the threshold size. If so, *
         * ignore one or both.  Otherwise, push the larger partition's    *
        \* bounds on the stack and continue sorting the smaller one.      */

        if ( (right_ptr - lo) <= MAX_THRESH ) {
          if ((hi - left_ptr) <= MAX_THRESH ) {
            --top ;
            lo = top -> lo ;
            hi = top -> hi ;
          } else {
            lo = left_ptr ;
          }
        } else if ((hi - left_ptr) <= MAX_THRESH) {
          hi = right_ptr;
        } else if ((right_ptr - lo) > (hi - left_ptr)) {
          top -> lo = lo ;
          top -> hi = right_ptr ;
          ++top ;
          lo = left_ptr;
        } else {
          top -> lo = left_ptr ;
          top -> hi = hi ;
          ++top ;
          hi = right_ptr;
        }
      }

      /* Once the BASE_PTR array is partially sorted by quicksort the rest  *\
       * is completely sorted using insertion sort, since this is efficient *
      \* for partitions below MAX_THRESH size.                              */
    }

  insert_sort:

    for ( index_type i = 1 ; i < total_elems ; ++i ) {
      for ( index_type j = i ; j > 0 ; --j ) {
        if ( GT(I[j], I[j-1]) ) break ;
        swap( I[j], I[j-1] ) ;
        swap( A[j], A[j-1] ) ;
      }
    }
  }

  static void
  QuickSortIJ(index_type I[], index_type J[],
              index_type const total_elems,
              index_type const MAX_THRESH = 4) {
    using ::std::swap ;

    if ( total_elems <= 1 ) return ;
    if ( total_elems <= MAX_THRESH ) goto insert_sort ;

    {
      index_type lo = 0 ;
      index_type hi = total_elems - 1 ;

      stack_node stack[sizeof(index_type) * CHAR_BIT] ;
      stack_node *top = stack ;

      while ( top >= stack ) { // Stack not empty

        /* Select median value from among LO, MID, and HI. Rearrange *\
         * LO and HI so the three values are sorted. This lowers the *
         * probability of picking a pathological pivot value and     *
        \* skips a comparison for both the LEFT_PTR and RIGHT_PTR.   */

        index_type & I_hi  = I[hi] ;
        index_type & I_lo  = I[lo] ;
        index_type & I_mid = I[ (hi + lo) / 2 ] ;

        index_type & J_hi  = J[hi] ;
        index_type & J_lo  = J[lo] ;
        index_type & J_mid = J[ (hi + lo) / 2 ] ;

        if ( GT(I_lo, J_lo, I_mid, J_mid) ) {
          swap(I_mid, I_lo) ;
          swap(J_mid, J_lo) ;
        }
        if ( GT(I_mid, J_mid, I_hi, J_hi) ) {
          swap(I_hi, I_mid) ;
          swap(J_hi, J_mid) ;
          if ( GT(I_lo, J_lo, I_mid, J_mid) ) {
            swap(I_mid, I_lo) ;
            swap(J_mid, J_lo) ;
          }
        }

        index_type IPivot    = I_mid ;
        index_type JPivot    = J_mid ;
        index_type left_ptr  = lo + 1;
        index_type right_ptr = hi - 1;

        /* Here's the famous ``collapse the walls'' section of quicksort. *\
         * Gotta like those tight inner loops!  They are the main reason  *
        \* that this algorithm runs much faster than others.              */

        do {

          while ( GT(IPivot, JPivot, I[left_ptr], J[left_ptr] )  ) ++left_ptr ;
          while ( GT(I[right_ptr], J[right_ptr], IPivot, JPivot) ) --right_ptr;

          if ( left_ptr < right_ptr ) {
            swap(I[left_ptr], I[right_ptr]);
            swap(J[left_ptr], J[right_ptr]);
            ++left_ptr ;
            --right_ptr ;
          } else if ( left_ptr == right_ptr ) {
            ++left_ptr ;
            --right_ptr ;
            break;
          }

        } while ( left_ptr <= right_ptr );

        /* Set up pointers for next iteration.  First determine whether   *\
         * left and right partitions are below the threshold size. If so, *
         * ignore one or both.  Otherwise, push the larger partition's    *
        \* bounds on the stack and continue sorting the smaller one.      */

        if ( (right_ptr - lo) <= MAX_THRESH ) {
          if ((hi - left_ptr) <= MAX_THRESH ) {
            --top ;
            lo = top -> lo ;
            hi = top -> hi ;
          } else {
            lo = left_ptr ;
          }
        } else if ((hi - left_ptr) <= MAX_THRESH) {
          hi = right_ptr;
        } else if ((right_ptr - lo) > (hi - left_ptr)) {
          top -> lo = lo ;
          top -> hi = right_ptr ;
          ++top ;
          lo = left_ptr;
        } else {
          top -> lo = left_ptr ;
          top -> hi = hi ;
          ++top ;
          hi = right_ptr;
        }
      }

      /* Once the BASE_PTR array is partially sorted by quicksort the rest   *\
       * is completely sorted using insertion sort, since this is efficient  *
       * for partitions below MAX_THRESH size.                               */
    }

  insert_sort:

    for ( index_type i = 1 ; i < total_elems ; ++i ) {
      for ( index_type j = i ; j > 0 ; --j ) {
        if ( GT(I[j], J[j], I[j-1], J[j-1]) ) break ;
        swap( I[j], I[j-1] ) ;
        swap( J[j], J[j-1] ) ;
      }
    }
  }

  template <typename T>
  static void
  QuickSortIJ(index_type I[], index_type J[], T A[],
              index_type const total_elems,
              index_type const MAX_THRESH = 4) {
 
    using ::std::swap ;

    if ( total_elems <= 1 ) return ;
    if ( total_elems <= MAX_THRESH ) goto insert_sort ;

    {
      index_type lo = 0 ;
      index_type hi = total_elems - 1 ;

      stack_node stack[sizeof(index_type) * CHAR_BIT] ;
      stack_node *top = stack ;

      while ( top >= stack ) { // Stack not empty

        /* Select median value from among LO, MID, and HI. Rearrange *\
         * LO and HI so the three values are sorted. This lowers the *
         * probability of picking a pathological pivot value and     *
        \* skips a comparison for both the LEFT_PTR and RIGHT_PTR.   */

        index_type & I_hi  = I[hi] ;
        index_type & I_lo  = I[lo] ;
        index_type & I_mid = I[ (hi + lo) / 2 ] ;

        index_type & J_hi  = J[hi] ;
        index_type & J_lo  = J[lo] ;
        index_type & J_mid = J[ (hi + lo) / 2 ] ;

        T & A_hi  = A[hi] ;
        T & A_lo  = A[lo] ;
        T & A_mid = A[ (hi + lo) / 2 ] ;

        if ( GT(I_lo, J_lo, I_mid, J_mid) ) {
          swap(I_mid, I_lo) ;
          swap(J_mid, J_lo) ;
          swap(A_mid, A_lo) ;
        }
        if ( GT(I_mid, J_mid, I_hi, J_hi) ) {
          swap(I_hi, I_mid) ;
          swap(J_hi, J_mid) ;
          swap(A_hi, A_mid) ;
          if ( GT(I_lo, J_lo, I_mid, J_mid) ) {
            swap(I_mid, I_lo) ;
            swap(J_mid, J_lo) ;
            swap(A_mid, A_lo) ;
          }
        }

        index_type IPivot    = I_mid ;
        index_type JPivot    = J_mid ;
        index_type left_ptr  = lo + 1;
        index_type right_ptr = hi - 1;

        /* Here's the famous ``collapse the walls'' section of quicksort. *\
         * Gotta like those tight inner loops!  They are the main reason  *
        \* that this algorithm runs much faster than others.              */

        do {

          while ( GT(IPivot, JPivot, I[left_ptr], J[left_ptr] )  ) ++left_ptr ;
          while ( GT(I[right_ptr], J[right_ptr], IPivot, JPivot) ) --right_ptr;

          if ( left_ptr < right_ptr ) {
            swap(I[left_ptr], I[right_ptr]);
            swap(J[left_ptr], J[right_ptr]);
            swap(A[left_ptr], A[right_ptr]);
            ++left_ptr ;
            --right_ptr ;
          } else if ( left_ptr == right_ptr ) {
            ++left_ptr ;
            --right_ptr ;
            break;
          }

        } while ( left_ptr <= right_ptr );

        /* Set up pointers for next iteration.  First determine whether   *\
         * left and right partitions are below the threshold size. If so, *
         * ignore one or both.  Otherwise, push the larger partition's    *
        \* bounds on the stack and continue sorting the smaller one.      */

        if ( (right_ptr - lo) <= MAX_THRESH ) {
          if ((hi - left_ptr) <= MAX_THRESH ) {
            --top ;
            lo = top -> lo ;
            hi = top -> hi ;
          } else {
            lo = left_ptr ;
          }
        } else if ((hi - left_ptr) <= MAX_THRESH) {
          hi = right_ptr;
        } else if ((right_ptr - lo) > (hi - left_ptr)) {
          top -> lo = lo ;
          top -> hi = right_ptr ;
          ++top ;
          lo = left_ptr;
        } else {
          top -> lo = left_ptr ;
          top -> hi = hi ;
          ++top ;
          hi = right_ptr;
        }
      }

      /* Once the BASE_PTR array is partially sorted by quicksort the rest  *\
       * is completely sorted using insertion sort, since this is efficient *
       * for partitions below MAX_THRESH size.                              */
    }

  insert_sort:

    for ( index_type i = 1 ; i < total_elems ; ++i ) {
      for ( index_type j = i ; j > 0 ; --j ) {
        if ( GT(I[j], J[j], I[j-1], J[j-1]) ) break ;
        swap( I[j], I[j-1] ) ;
        swap( J[j], J[j-1] ) ;
        swap( A[j], A[j-1] ) ;
      }
    }
  }

  /*
  //  ####   #####     ##    #####    ####   ######
  // #       #    #   #  #   #    #  #       #
  //  ####   #    #  #    #  #    #   ####   #####
  //      #  #####   ######  #####        #  #
  // #    #  #       #    #  #   #   #    #  #
  //  ####   #       #    #  #    #   ####   ######
  */

  # define SPARSELIB_MUL_STRUCTURES(MATRIX)                                   \
  template <typename TM, typename T> inline                                   \
  Vector_M_mul_V<MATRIX<TM>, Vector<T> >                                      \
  operator * (MATRIX<TM> const & M, Vector<T> const & a) {                    \
    return Vector_M_mul_V<MATRIX<TM>, Vector<T> >(M,a) ;                      \
  }                                                                           \
                                                                              \
  template <typename S, typename TM, typename T> inline                       \
  Vector_S_mul_M_mul_V<S, MATRIX<TM>, Vector<T> >                             \
  operator * (S const & s,                                                    \
              Vector_M_mul_V<MATRIX<TM>, Vector<T> > const & Mv) {            \
    return Vector_S_mul_M_mul_V<S, MATRIX<TM>, Vector<T> >(s,Mv.M,Mv.a) ;     \
  }                                                                           \
                                                                              \
  template <typename TM, typename T> inline                                   \
  Vector_V_sum_M_mul_V<MATRIX<TM>,Vector<T> >                                 \
  operator + (Vector<T> const & a,                                            \
              Vector_M_mul_V<MATRIX<TM>, Vector<T> > const & Mv) {            \
    return Vector_V_sum_M_mul_V<MATRIX<TM>,Vector<T> >(a,Mv.M,Mv.a) ;         \
  }                                                                           \
                                                                              \
  template <typename TM, typename T> inline                                   \
  Vector_V_sub_M_mul_V<MATRIX<TM>,Vector<T> >                                 \
  operator - (Vector<T> const & a,                                            \
              Vector_M_mul_V<MATRIX<TM>, Vector<T> > const & Mv) {            \
    return Vector_V_sub_M_mul_V<MATRIX<TM>,Vector<T> >(a,Mv.M,Mv.a) ;         \
  }                                                                           \
                                                                              \
  template <typename S, typename TM, typename T> inline                       \
  Vector_V_sum_S_mul_M_mul_V<S, MATRIX<TM>,Vector<T> >                        \
  operator + (Vector<T> const & a,                                            \
              Vector_S_mul_M_mul_V<S, MATRIX<TM>, Vector<T> > const & sMv) {  \
    return Vector_V_sum_S_mul_M_mul_V<S, MATRIX<TM>,Vector<T> >               \
           (a,sMv.s,sMv.M,sMv.a) ;                                            \
  }                                                                           \


  template <typename M>
  class Sparse {
  public:
    typedef M Matrix ;

  protected:

    index_type sp_nrows, sp_ncols, sp_min_size, sp_max_size ;
    index_type sp_nnz, sp_max_nnz, sp_lower_nnz, sp_upper_nnz, sp_diag_nnz ;
    bool       sp_is_ordered ;

    # ifdef SPARSELIB_DEBUG
    void test_nnz(index_type const idx) const {
      SPARSELIB_TEST0( idx < sp_nnz,
                       "Sparse::operator [" << idx << "] index out of range")
    }
    void test_index(index_type const i, index_type const j) const {
      SPARSELIB_TEST0( i < sp_nrows && j < sp_ncols,
                       "Sparse::test_index(" << i << "," << j <<
                       ") index out of range")
    }
    void test_row(index_type const i) const {
      SPARSELIB_TEST0( i < sp_nrows,
                       "Sparse::test_row(" << i << ") index out of range")
    }
    void test_col(index_type const j) const {
      SPARSELIB_TEST0( j < sp_ncols,
                       "Sparse::test_col(" << j << ") index out of range" )
    }
    # else
    void test_nnz(index_type const) const {}
    void test_index(index_type const, index_type const) const {}
    void test_row(index_type const) const {}
    void test_col(index_type const) const {}
    # endif

    void
    set_up(index_type const nr, index_type const nc, index_type const mnnz) {
      sp_nrows      = nr ;
      sp_ncols      = nc ;
      sp_min_size   = std::min(nr,nc) ;
      sp_max_size   = std::max(nr,nc) ;
      sp_nnz        = 0 ;
      sp_max_nnz    = mnnz ;
      sp_lower_nnz  = 0 ;
      sp_upper_nnz  = 0 ;
      sp_diag_nnz   = 0 ;
      sp_is_ordered = true ;
    }

    void
    ldu_count(index_type const i, index_type const j) {
      if ( j < i ) ++sp_lower_nnz ;
      else if ( j > i ) ++sp_upper_nnz ;
      else ++sp_diag_nnz ;
    }
    
  public:

    Sparse(void) { set_up(0, 0, 0) ; }

    virtual ~Sparse(void) {} ;

    // common data
    inline index_const_reference nrows     (void) const { return sp_nrows ; }
    inline index_const_reference ncols     (void) const { return sp_ncols ; }
    inline index_const_reference min_size  (void) const { return sp_min_size ; }
    inline index_const_reference max_size  (void) const { return sp_max_size ; }
    inline index_const_reference nnz       (void) const { return sp_nnz ; }
    inline index_const_reference max_nnz   (void) const { return sp_max_nnz ; }
    inline index_const_reference lower_nnz (void) const { return sp_lower_nnz  ; }
    inline index_const_reference diag_nnz  (void) const { return sp_diag_nnz  ; }
    inline index_const_reference upper_nnz (void) const { return sp_upper_nnz  ; }
    inline bool                  is_ordered(void) const { return sp_is_ordered ; }

    // forwarding
    inline void Begin(void) const { static_cast<Matrix const *>(this) -> Begin() ; }
    inline void Next(void)  const { static_cast<Matrix const *>(this) -> Next()  ; }
    inline bool End(void)   const { return static_cast<Matrix const *>(this) -> End() ;}

    // forwardind actual values
    inline index_type i_index(void) const
    { return static_cast<Matrix const *>(this) -> i_index() ; }
    inline index_type j_index(void) const
    { return static_cast<Matrix const *>(this) -> j_index() ; }

    template <typename T> 
    inline
    T value(T const *) const {
      typedef T const * TT ; 
      return static_cast<Matrix const *>(this) ->value(TT(0) ) ; 
    }
  } ;

  /*
   *  ###       # #######
   *   #       #  #     #
   *   #      #   #     #
   *   #     #    #     #
   *   #    #     #     #
   *   #   #      #     #
   *  ### #       #######
   */

  template <typename MAT> inline
  ostream & operator << (ostream & s, Sparse<MAT> const & M) {
    s <<   "nrows      = " << M . nrows()
      << "\nncols      = " << M . ncols()
      << "\nnnz        = " << M . nnz()
      << "\nmax_nnz    = " << M . max_nnz()
      << "\ndiag_nnz   = " << M . diag_nnz()
      << "\nlower_nnz  = " << M . lower_nnz()
      << "\nupper_nnz  = " << M . upper_nnz()
      << "\nis_ordered = " << M . is_ordered()
      << "\n" ;
    typedef typename MAT::value_type value_type ;
    for ( M.Begin() ; M.End() ; M.Next() ) {
      s << "("  << setw(10) << M . i_index()
        << ","  << setw(10) << M . j_index()
        << ")=" << setw(10) << M . value((value_type*)(0))
        << "\n" ;
    }
    return s ;
  }

  //* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

  template <typename PRECO>
  class Preco {
  protected:
    index_type pr_size ;
  public:
    Preco(void) : pr_size(0) {}
    virtual ~Preco(void) {} ;
    index_const_reference size(void) const { return pr_size ; }
  } ;

  /*
  //  #####
  // #     #  #####     ##    #####    ####   ######
  // #        #    #   #  #   #    #  #       #
  //  #####   #    #  #    #  #    #   ####   #####
  //       #  #####   ######  #####        #  #
  // #     #  #       #    #  #   #   #    #  #
  //  #####   #       #    #  #    #   ####   ######
  //
  // ######
  // #     #    ##    #####  #####  ######  #####   #    #
  // #     #   #  #     #      #    #       #    #  ##   #
  // ######   #    #    #      #    #####   #    #  # #  #
  // #        ######    #      #    #       #####   #  # #
  // #        #    #    #      #    #       #   #   #   ##
  // #        #    #    #      #    ######  #    #  #    #
  */

  class SparsePattern : public Sparse<SparsePattern> {
  public:
    typedef SparsePattern  MATRIX ;
    typedef Sparse<MATRIX> SPARSE ;

  private:

    Vector<index_type> I, J ;
    mutable index_type ipos ;

    template <typename MAT, typename Compare>
    void
    convert(Sparse<MAT> const & M, Compare cmp) {
      // count nonzero
      index_type nz = 0 ;
      for ( M . Begin() ; M . End() ; M . Next() )
        if ( cmp( M . i_index(), M . j_index() ) ) ++nz ;

      new_dim( M . nrows(), M . ncols(), nz) ;
      for ( M . Begin() ; M . End() ; M . Next() ) {
        index_type i = M . i_index() ;
        index_type j = M . j_index() ;
        if ( cmp(i,j) ) insert(i,j) ;
      }
      internal_order() ;
    }

  public:
    virtual ~SparsePattern() { free() ; }
    SparsePattern(void) { }

    SparsePattern(index_type nr, index_type nc, index_type mnnz)
    { new_dim(nr, nc, mnnz) ; }

    template <typename Compare>
    SparsePattern(SparsePattern const & sp, Compare cmp)
    { convert(sp,cmp) ; }

    SparsePattern(SparsePattern const & sp) : SPARSE() 
    { convert(sp,all_ok()) ; }

    // convert => SparsePattern
    template <typename MAT, typename Compare>
    SparsePattern(Sparse<MAT> const & M, Compare cmp)
    { convert(M,cmp) ; }

    template <typename MAT>
    SparsePattern(Sparse<MAT> const & M)
    { convert(M,all_ok()) ; }

    SparsePattern const & operator = (SparsePattern const & SP) {
      if ( &SP != this ) { // avoid copy to itself
        new_dim( SP . nrows(), SP . ncols(), SP . max_nnz() ) ;
        SPARSE::sp_nnz        = SP . nnz() ;
        SPARSE::sp_is_ordered = SP . is_ordered() ;
        I = SP . I ;
        J = SP . J ;
        if ( !SPARSE::sp_is_ordered ) internal_order() ;
      }
      return *this ;
    }

    // convert => SparsePattern
    template <typename MAT>
    SparsePattern const & operator = (Sparse<MAT> const & M)
    { convert(M,all_ok()) ; return *this ; }

    void new_dim(index_type nr, index_type nc, index_type mnnz) {
      SPARSE::set_up(nr, nc, mnnz) ;
      I . new_dim( mnnz ) ;
      J . new_dim( mnnz ) ;
    }

    template <typename MAT>
    void
    new_dim(Sparse<MAT> const & M) { convert(M,all_ok()) ; }

    template <typename MAT, typename Compare>
    void
    new_dim(Sparse<MAT> const & M, Compare cmp) { convert(M.cmp) ; }

    void free(void) {
      I . free() ;
      J . free() ;
      SPARSE::set_up(0, 0, 0) ;
    }

    void internal_order() {
      QuickSortIJ(I . begin(), J . begin(), SPARSE::sp_nnz) ;
      // eliminate duplicate elements
      index_type i1 = 0, i = 0 ;
      SPARSE::sp_lower_nnz =
      SPARSE::sp_diag_nnz  =
      SPARSE::sp_upper_nnz = 0 ;
      while ( i1 < SPARSE::sp_nnz ) {
        // copy i1 => i
        I[i] = I[i1] ; J[i] = J[i1] ;
        // setup statistic
        SPARSE::ldu_count(I[i],J[i]) ;
        // find i1 != i
        for ( ++i1 ; i1 < SPARSE::sp_nnz && I[i1] == I[i] && J[i1] == J[i] ; ++i1 ) {};
        ++i ;
      }
      SPARSE::sp_nnz = i ;
      SPARSE::sp_is_ordered = true ;
    }

    void empty(void) { SPARSE::sp_nnz = 0 ; }

    void insert(index_type const i, index_type const j) {
      SPARSELIB_TEST( SPARSE::sp_nnz < SPARSE::sp_max_nnz,
                      "SparsePattern::insert(" <<i<< "," <<j<< ") matrix full")
      SPARSE::test_index(i,j) ;
      I[SPARSE::sp_nnz] = i ;
      J[SPARSE::sp_nnz] = j ;
      ++SPARSE::sp_nnz ;
      SPARSE::sp_is_ordered = false ;
    }

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    index_const_pointer getI(void) const { return I . begin() ; }
    index_const_pointer getJ(void) const { return J . begin() ; }
    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    // ITERATORS
    void Begin(void) const { ipos = 0 ; }
    void Next(void)  const {
      SPARSELIB_TEST( ipos < SPARSE::sp_nnz, "SparsePattern::Next()")
      ++ipos ;
    }
    bool End(void) const { return ipos < SPARSE::sp_nnz ; }

    index_const_reference i_index(void) const { return I[ipos] ; }
    index_const_reference j_index(void) const { return J[ipos] ; }

    template <typename T> inline
    T value(T const * const) const { return T(0) ; }

  } ;

  /*
   *  ###       # #######
   *   #       #  #     #
   *   #      #   #     #
   *   #     #    #     #
   *   #    #     #     #
   *   #   #      #     #
   *  ### #       #######
   */

  inline
  ostream &
  operator << (ostream & s, SparsePattern const & M) {
    for ( M . Begin() ; M . End() ; M . Next() )
      s << M . i_index() << " " << M . j_index() << "\n" ;
    return s ;
  }

  /*
  //  #####   #####
  // #     # #     #   ####    ####   #####
  // #       #        #    #  #    #  #    #
  // #       #        #    #  #    #  #    #
  // #       #        #    #  #    #  #####
  // #     # #     #  #    #  #    #  #   #
  //  #####   #####    ####    ####   #    #
  //
  // #     #
  // ##   ##    ##    #####  #####   #  #    #
  // # # # #   #  #     #    #    #  #   #  #
  // #  #  #  #    #    #    #    #  #    ##
  // #     #  ######    #    #####   #    ##
  // #     #  #    #    #    #   #   #   #  #
  // #     #  #    #    #    #    #  #  #    #
  //
  */

  template <typename T>
  class CCoorMatrix : public Sparse<CCoorMatrix<T> > {
  public:
    typedef CCoorMatrix<T> MATRIX ;
    typedef Sparse<MATRIX> SPARSE ;

    SPARSELIB_TYPES(T) ;
    SPARSELIB_VECTOR_TYPES(T) ;

  private:

    Vector<index_type> I, J, PR ;
    Vector<value_type> A ;

    mutable index_type ipos ;

    void build_row_pointers() {
      SPARSELIB_TEST0( SPARSE::sp_is_ordered,
                       "CCoorMatrix::build_row_pointers() matrix must be ordered")
      index_type nr = 0 ;
      PR[0] = 0 ;
      for ( index_type k = 1 ; k < SPARSE::sp_nnz ; ++k ) {
        SPARSELIB_TEST( I[k-1] <= I[k],
                        "CCoorMatrix::build_row_pointers() internal error")
        index_type kk = I[k] - I[k-1] ;
        while ( kk-- > 0 ) PR[++nr] = k ;
      }
      SPARSELIB_TEST( nr < SPARSE::sp_nrows,
                      "CCoorMatrix::build_row_pointers() internal error")
      while ( ++nr <= SPARSE::sp_nrows ) PR[nr] = SPARSE::sp_nnz ;
    }

    template <typename MAT, typename Compare> inline
    void convert(Sparse<MAT> const & M, Compare cmp) {
      // count nonzero
      index_type nz = 0 ;
      for ( M . Begin() ; M . End() ; M . Next() )
        if ( cmp( M . i_index(), M . j_index() ) ) ++nz ;

      new_dim( M . nrows(), M . ncols(), nz ) ;

      for ( M . Begin() ; M . End() ; M . Next() ) {
        index_type i = M . i_index() ;
        index_type j = M . j_index() ;
        if ( cmp(i,j) ) insert(i, j) = M . value((value_type*)(0)) ;
      }
      internal_order() ;
    }

  public:

    virtual ~CCoorMatrix(void) { free() ; }

    CCoorMatrix(void) {} ;

    CCoorMatrix(index_type nr, index_type nc, index_type mnnz)
    { new_dim(nr, nc, mnnz) ; }

    template <typename Compare>
    CCoorMatrix(CCoorMatrix<T> const & M, Compare cmp)
    { convert(M,cmp); }

    CCoorMatrix(CCoorMatrix<T> const & M)
    { convert(M,all_ok()); }

    template <typename MAT, typename Compare>
    CCoorMatrix(Sparse<MAT> const & M, Compare cmp)
    { convert(M,cmp) ; }

    template <typename MAT>
    CCoorMatrix(Sparse<MAT> const & M)
    { convert(M,all_ok()) ; }

    CCoorMatrix<T> const & operator = (CCoorMatrix<T> const & M) {
      if ( &M == this ) return *this ; // avoid copy to itself
      new_dim( M . nrows(), M . ncols(), M . max_nnz() ) ;
      SPARSE::sp_nnz        = M . nnz() ;
      SPARSE::sp_is_ordered = M . is_ordered() ;
      A  = M . A ;
      I  = M . I ;
      J  = M . J ;
      PR = M . PR ;
      if ( !SPARSE::sp_is_ordered ) internal_order() ;
      return *this ;
    }

    // convert => CCoorMatrix
    template <typename MAT>
    CCoorMatrix<T> const & operator = (Sparse<MAT> const & M)
    { convert(M,all_ok()) ; return *this ; }

    void new_dim(index_type const nr,
                 index_type const nc,
                 index_type const mnnz) {
      SPARSE::set_up(nr, nc, mnnz) ;
      A  . new_dim( SPARSE::sp_max_nnz + 1 ) ;
      I  . new_dim( SPARSE::sp_max_nnz )     ;
      J  . new_dim( SPARSE::sp_max_nnz )     ;
      PR . new_dim( SPARSE::sp_nrows + 1 )   ;
      A[ SPARSE::sp_max_nnz ] = 0 ;
    }

    template <typename MAT, typename Compare> inline
    void new_dim(Sparse<MAT> const & M, Compare cmp) { convert(M,cmp) ; }

    template <typename MAT>
    void new_dim(Sparse<MAT> const & M) { convert(M,all_ok()) ; }

    void free(void) {
      I  . free( ) ;
      J  . free( ) ;
      A  . free( ) ;
      PR . free( ) ;
      SPARSE::set_up(0, 0, 0) ;
    }

    void internal_order() {
      QuickSortIJ<T>(I . begin() ,
                     J . begin() ,
                     A . begin() ,
                     SPARSE::sp_nnz) ;
      // eliminate duplicate elements
      index_type i1 = 0, i = 0 ;
      SPARSE::sp_lower_nnz =
      SPARSE::sp_diag_nnz  =
      SPARSE::sp_upper_nnz = 0 ;
      while ( i1 < SPARSE::sp_nnz ) {
        // copy i1 => i
        I[i] = I[i1] ; J[i] = J[i1] ; A[i] = A[i1] ;
        // setup statistic
        SPARSE::ldu_count(I[i],J[i]) ;
        // find i1 != i
        for ( ++i1 ; i1 < SPARSE::sp_nnz && I[i1] == I[i] && J[i1] == J[i] ; ++i1 )
          A[i] += A[i1] ;
        ++i ;
      }
      SPARSE::sp_nnz = i ;
      SPARSE::sp_is_ordered = true ;
      build_row_pointers() ;
    }

    void set_row(index_type const nr, const_reference val) {
      SPARSE::test_row(nr) ;
      if ( SPARSE::sp_is_ordered ) {
        A . fill( PR[nr], PR[nr+1], val) ;
      } else {
        for ( index_type k = 0 ; k < SPARSE::sp_nnz ; ++k )
          if ( I[k] == nr ) A[k] = val ;
      }
    }

    void set_column(index_type const nc, const_reference val) {
      SPARSE::test_col(nc) ;
      for ( index_type k = 0 ; k < SPARSE::sp_nnz ; ++k )
        if ( J[k] == nc ) A[k] = val ;
    }

    void scale_row   (index_type nr, const_reference val) ;
    void scale_column(index_type nc, const_reference val) ;

    CCoorMatrix<T> const & operator = (const_reference s) {
      A = 0 ;
      if ( s != 0 )
        for ( index_type k = 0 ; k < SPARSE::sp_min_size ; ++k )
          ref(k,k) = s ;
      return *this ;
    }

    CCoorMatrix<T> const & operator += (const_reference s) {
      for ( index_type k = 0 ; k < SPARSE::sp_min_size ; ++k )
        ref(k,k) += s ;
      return *this ;
    }

    CCoorMatrix<T> const & operator -= (const_reference s) {
      for ( index_type k = 0 ; k < SPARSE::sp_min_size ; ++k )
        ref(k,k) -= s ;
      return *this ;
    }
    
    CCoorMatrix<T> const & operator *= (const_reference s)
    { A *= s ; return * this ; }
    
    CCoorMatrix<T> const & operator /= (const_reference s)
    { A /= s ; return * this ; }

    CCoorMatrix<T> const & operator = (vector_const_reference v) {
      index_type n = min_index(v.size(), SPARSE::sp_min_size ) ;
      A = 0 ;
      for ( index_type k = 0 ; k < n ; ++k )
        ref(k,k) = v[k] ;
      return *this ;
    }

    CCoorMatrix<T> const & operator += (vector_const_reference v) {
      index_type n = min_index(v.size(), SPARSE::sp_min_size ) ;
      for ( index_type k = 0 ; k < n ; ++k )
        ref(k,k) += v[k] ;
      return *this ;
    }

    CCoorMatrix<T> const & operator -= (vector_const_reference v) {
      index_type n = min_index(v.size(), SPARSE::sp_min_size) ;
      for ( index_type k = 0 ; k < n ; ++k )
        ref(k,k) -= v[k] ;
      return *this ;
    }

    template <typename R> inline
    CCoorMatrix<T> const & operator = (VectorE<T,R> const & e) {
      index_type n = min_index(e.size(), SPARSE::sp_min_size ) ;
      A = 0 ;
      for ( index_type k = 0 ; k < n ; ++k )
        ref(k,k) = e[k] ;
      return *this ;
    }

    template <typename R> inline
    CCoorMatrix<T> const & operator += (VectorE<T,R> const & e) {
      index_type n = min_index(e.size(), SPARSE::sp_min_size ) ;
      for ( index_type k = 0 ; k < n ; ++k )
        ref(k,k) += e[k] ;
      return *this ;
    }

    template <typename R> inline
    CCoorMatrix<T> const & operator -= (VectorE<T,R> const & e) {
      index_type n = min_index(e.size(), SPARSE::sp_min_size ) ;
      for ( index_type k = 0 ; k < n ; ++k )
        ref(k,k) -= e[k] ;
      return *this ;
    }

    void empty(void) { SPARSE::sp_nnz = 0 ; }

    index_type position(index_type i, index_type j) const {
      SPARSE::test_index(i,j) ;
      index_type lo  = PR[i] ;
      index_type hi  = PR[i+1] ;
      index_type len = hi - lo ;

      while ( len > 0 ) {
        index_type half = len / 2 ;
        index_type mid  = lo + half ;
        if ( J[mid] < j ) {
          lo = mid + 1 ;
          len -= half + 1 ;
        } else len = half;
      }
      if ( lo == hi || J[lo] != j ) lo = SPARSE::sp_max_nnz ;
      return lo ;
    }

    const_reference operator () (index_type i, index_type j) const
    { return A[position(i,j)] ; }

    reference ref(index_type i, index_type j) {
      index_type pos = position(i,j) ;
      SPARSELIB_TEST(pos != SPARSE::sp_max_nnz,
                     "CCoorMatrix::ref(" << i << "," << j <<
                     ") referring to non existent element")
      return A[pos] ;
    }

    reference insert(index_type i, index_type j) {
      SPARSELIB_TEST( SPARSE::sp_nnz < SPARSE::sp_max_nnz,
                      "CCoorMatrix::insert(" <<i<< "," <<j<< ") matrix full")
      SPARSE::test_index(i,j) ;
      I[SPARSE::sp_nnz] = i ;
      J[SPARSE::sp_nnz] = j ;
      SPARSE::sp_is_ordered = false ;
      return A[SPARSE::sp_nnz++] ;
    }

    const_reference operator [] (index_type idx) const {
      SPARSE::test_nnz(idx) ;
      return A[idx] ;
    }

    reference operator [] (index_type idx) {
      SPARSE::test_nnz(idx) ;
      return A[idx] ;
    }

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

    const_pointer       getA(void) const { return A . begin() ; }
    pointer             getA(void)       { return A . begin() ; }
    index_const_pointer getI(void) const { return I . begin() ; }
    index_const_pointer getJ(void) const { return J . begin() ; }

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    // ITERATOR
    void Begin(void) const { ipos = 0 ; }
    void Next (void) const {
      SPARSELIB_TEST( ipos < SPARSE::sp_nnz, "CCoorMatrix::Next()")
      ++ipos ;
    }
    bool End(void) const { return ipos < SPARSE::sp_nnz ; }
    index_const_reference i_index(void) const { return I[ipos] ; }
    index_const_reference j_index(void) const { return J[ipos] ; }
    const_reference value(T const * const) const { return A[ipos] ; }

    // A * x
    template <typename VECTOR> inline
    void ass_M_mul_V(VECTOR & res, VECTOR const & v) const {
      SPARSELIB_TEST(&res != &v, "CCoorMatrix::ass_M_mul_V equal pointer")
      index_const_pointer II = I . begin() ;
      index_const_pointer JJ = J . begin() ;
      const_pointer       AA = A . begin() ;
      res = 0 ;
      SPARSELIB_MLOOP( SPARSE::sp_nnz, res[*II] = res[*II] + *AA * v[*JJ] ;
                                       ++II ; ++JJ ; ++AA) ;
    }

    // s * A * x
    template <typename SCALAR, typename VECTOR> inline
    void ass_S_mul_M_mul_V(VECTOR & res, SCALAR const & s, VECTOR const & v) const {
      SPARSELIB_TEST(&res != &v, "CCoorMatrix::ass_S_mul_M_mul_V equal pointer")
      index_const_pointer II = I . begin() ;
      index_const_pointer JJ = J . begin() ;
      const_pointer       AA = A . begin() ;
      res = 0 ;
      SPARSELIB_MLOOP( SPARSE::sp_nnz, res[*II] = res[*II] + s * *AA * v[*JJ] ;
                                       ++II ; ++JJ ; ++AA) ;
    }

    // res = a + A * b
    template <typename VECTOR> inline
    void ass_V_sum_M_mul_V(VECTOR       & res,
                           VECTOR const & a,
                           VECTOR const & b) const {
      SPARSELIB_TEST( &res != &b,
                      "CCoorMatrix::ass_V_sum_M_mul_V equal pointer")
      index_const_pointer II = I . begin() ;
      index_const_pointer JJ = J . begin() ;
      const_pointer       AA = A . begin() ;
      res = a ;
      SPARSELIB_MLOOP( SPARSE::sp_nnz, res[*II] = res[*II] + *AA * b[*JJ] ;
                                       ++II ; ++JJ ; ++AA) ;
    }

    // res = a - A * b
    template <typename VECTOR> inline
    void ass_V_sub_M_mul_V(VECTOR       & res,
                           VECTOR const & a,
                           VECTOR const & b) const {
      SPARSELIB_TEST( &res != &b,
                     "CCoorMatrix::ass_V_sub_M_mul_V equal pointer")
      index_const_pointer II = I . begin() ;
      index_const_pointer JJ = J . begin() ;
      const_pointer       AA = A . begin() ;
      res = a ;
      SPARSELIB_MLOOP( SPARSE::sp_nnz, res[*II] = res[*II] - *AA * b[*JJ] ;
                                       ++II ; ++JJ ; ++AA) ;
    }
    
    // res = a + s * A * b
    template <typename SCALAR, typename VECTOR> inline
    void ass_V_sum_S_mul_M_mul_V(VECTOR       & res,
                                 VECTOR const & a,
                                 SCALAR const & s,
                                 VECTOR const & b) const {
      SPARSELIB_TEST( &res != &b,
                      "CCoorMatrix::ass_V_sum_S_mul_M_mul_V equal pointer")
      index_const_pointer II = I . begin() ;
      index_const_pointer JJ = J . begin() ;
      const_pointer       AA = A . begin() ;
      res = a ;
      SPARSELIB_MLOOP( SPARSE::sp_nnz, res[*II] = res[*II] + s * *AA * b[*JJ] ;
                                       ++II ; ++JJ ; ++AA) ;
    }

  } ;

  SPARSELIB_MUL_STRUCTURES(CCoorMatrix)

  /*
  //
  //  #####  ######
  // #     # #     #   ####   #    #
  // #       #     #  #    #  #    #
  // #       ######   #    #  #    #
  // #       #   #    #    #  # ## #
  // #     # #    #   #    #  ##  ##
  //  #####  #     #   ####   #    #
  //
  // #     #
  // ##   ##    ##    #####  #####   #  #    #
  // # # # #   #  #     #    #    #  #   #  #
  // #  #  #  #    #    #    #    #  #    ##
  // #     #  ######    #    #####   #    ##
  // #     #  #    #    #    #   #   #   #  #
  // #     #  #    #    #    #    #  #  #    #
  */

  template <typename T>
  class CRowMatrix : public Sparse<CRowMatrix<T> > {
  public:
    typedef CRowMatrix<T>  MATRIX ;
    typedef Sparse<MATRIX> SPARSE ;

    SPARSELIB_TYPES(T) ;
    SPARSELIB_VECTOR_TYPES(T) ;

  private:


    mutable index_type iter_row ;
    mutable index_type iter_ptr ;



    template <typename MAT, typename Compare>
    void
    convert(Sparse<MAT> const & M, Compare cmp) {
       
      // step 0: Count nonzero
      index_type nz = 0 ;
      for ( M . Begin() ; M . End() ;  M . Next() )
        if ( cmp(M . i_index(), M . j_index() ) ) ++nz ;

      SPARSE::set_up( M . nrows(), M . ncols(), nz ) ;
      SPARSE::sp_nnz = SPARSE::sp_max_nnz ;

      A . new_dim( SPARSE::sp_max_nnz + 1 ) ; A[ SPARSE::sp_max_nnz ] = 0 ;
      J . new_dim( SPARSE::sp_max_nnz )     ;
      R . new_dim( SPARSE::sp_nrows + 1 )   ; R = 0 ;

      // step 1: Evaluate not zero pattern
      for ( M . Begin() ; M . End() ;  M . Next() ) {
        index_type i = M . i_index() ;
        index_type j = M . j_index() ;
        if ( cmp(i, j) ) ++R[i] ;
      }
      for ( index_type k = 0 ; k < SPARSE::sp_nrows ; ++k ) R[k+1] += R[k] ;

      // step 2: Fill matrix
      for ( M . Begin() ; M . End() ; M . Next() ) {
        index_type i = M . i_index() ;
        index_type j = M . j_index() ;
        if ( cmp(i,j) ) {
          index_type ii = --R[i] ;
          A[ ii ] = M . value((pointer)(0)) ;
          J[ ii ] = j ;
        }
      }

      SPARSELIB_TEST( R[0] == 0 && R[SPARSE::sp_nrows] == nz,
                      "CRowMatrix::new_dim(Sparse & A) failed" )
      // step 3: internal_order matrix
      internal_order() ;
    }

  public:

    Vector<value_type> A ;
    Vector<index_type> R, J ;


	void internal_order_need_no_sort() {
      index_type ii, kk, rk, rk1 ;
      SPARSE::sp_lower_nnz =
      SPARSE::sp_diag_nnz  =
      SPARSE::sp_upper_nnz = 0 ;
      for ( ii = 0, rk = R[0] ; ii < SPARSE::sp_nrows ; ++ii, rk = rk1 ) {
        rk1 = R[ii+1] ;
        /*QuickSortI<T>(J . begin() + rk,
                      A . begin() + rk,
                      rk1 - rk) ;*/
        // setup statistic
        for ( kk = rk ; kk < rk1 ; ++kk ) SPARSE::ldu_count(ii,J[kk]) ;
  # ifdef SPARSELIB_DEBUG
        for ( kk = rk+1 ; kk < rk1 ; ++kk )
          SPARSELIB_TEST0( J[kk-1] < J[kk], "CRowMatrix::internal_order() failed" )
  # endif
      }
      SPARSE::sp_nnz = R[SPARSE::sp_nrows] ;
	}
    void internal_order() {
      index_type ii, kk, rk, rk1 ;
      SPARSE::sp_lower_nnz =
      SPARSE::sp_diag_nnz  =
      SPARSE::sp_upper_nnz = 0 ;
      for ( ii = 0, rk = R[0] ; ii < SPARSE::sp_nrows ; ++ii, rk = rk1 ) {
        rk1 = R[ii+1] ;
        QuickSortI<T>(J . begin() + rk,
                      A . begin() + rk,
                      rk1 - rk) ;
        // setup statistic
        for ( kk = rk ; kk < rk1 ; ++kk ) SPARSE::ldu_count(ii,J[kk]) ;
  # ifdef SPARSELIB_DEBUG
        for ( kk = rk+1 ; kk < rk1 ; ++kk )
          SPARSELIB_TEST0( J[kk-1] < J[kk], "CRowMatrix::internal_order() failed" )
  # endif
      }
      SPARSE::sp_nnz = R[SPARSE::sp_nrows] ;
    }

    virtual ~CRowMatrix(void) { free() ; }
    CRowMatrix(void) {} ;

    template <typename Compare>
    CRowMatrix(CRowMatrix<T> const & M, Compare cmp)
    { convert(M,cmp) ; }
 
	CRowMatrix(index_type nRows, index_type nCols, index_type nnz, index_type* rows, index_type* cols, value_type* val) {
		SPARSE::set_up( nRows, nCols, nnz ) ;    
		SPARSE::sp_nnz = nnz;
		A.new_dim(nnz + 1); A[nnz] = 0;
		J.new_dim(nnz);
		R.new_dim(nRows + 1);
		memcpy(A.begin(), val, sizeof(value_type)*nnz);
		memcpy(J.begin(), cols, sizeof(index_type)*nnz);
		memcpy(R.begin(), rows, sizeof(index_type)*(nRows+1));
		internal_order_need_no_sort();
	}
    
	CRowMatrix(index_type nRows, index_type nCols, index_type nnz) {
		SPARSE::set_up( nRows, nCols, nnz ) ;    
		SPARSE::sp_nnz = nnz;
		A.new_dim(nnz + 1); A[nnz] = 0;
		J.new_dim(nnz);
		R.new_dim(nRows + 1);
		// internal_order();
		// internal_order must be called before it's usable
	}
    


    CRowMatrix(CRowMatrix<T> const & M)
    { convert(M,all_ok()) ; }

    template <typename MAT, typename Compare>
    CRowMatrix(Sparse<MAT> const & M, Compare cmp)
    { convert(M,cmp) ; }

    template <typename MAT>
    CRowMatrix(Sparse<MAT> const & M)
    { convert(M,all_ok()) ; }

    CRowMatrix<T> const & operator = (CRowMatrix<T> const & M) {
      if ( &M == this ) return *this ; // avoid copy to itself
      SPARSE::set_up( M . nrows(), M . ncols(), M . nnz() ) ;
      SPARSE::sp_nnz        = M . nnz() ;
      SPARSE::sp_is_ordered = M . is_ordered() ;
      A . new_dim( SPARSE::sp_nnz   + 1 ) ; A = M . A ;
      R . new_dim( SPARSE::sp_nrows + 1 ) ; R = M . R ;
      J . new_dim( SPARSE::sp_nnz       ) ; J = M . J ;
      if ( !SPARSE::sp_is_ordered ) internal_order() ;
      return *this ;
    }

    // convert => CRowMatrix
    template <typename MAT>
    CRowMatrix<T> const & operator = (Sparse<MAT> const & M)
    { convert(M,all_ok()) ; return *this ; }

    template <typename MAT, typename Compare>
    void new_dim(Sparse<MAT> const & M, Compare cmp) { convert(M,cmp) ; }

    template <typename MAT>
    void new_dim(Sparse<MAT> const & M) { new_dim(M,all_ok()) ; }

    void free(void) {
      A . free() ;
      J . free() ;
      R . free() ;
      SPARSE::set_up(0,0,0) ;
    }

    void set_row(index_type const nr, const_reference val)
    { SPARSE::test_row(nr) ; A . fill( R[nr], R[nr+1], val ) ; }

    void set_column(index_type const nc, const_reference val) {
      SPARSE::test_col(nc) ;
      for ( index_type i = 0 ; i < SPARSE::sp_nrows ; ++i ) {
        index_type pos = position(i,nc,true) ;
        if ( pos != SPARSE::sp_max_nnz ) A[pos] = val ;
     }
    }

    void scale_row(index_type const nr, const_reference val) {
      SPARSE::test_row(nr) ;
      pointer pA = A . begin() + R[nr] ;
      pointer pB = A . begin() + R[nr+1] ;
      while ( pA < pB ) *pA++ *= val ;
    }

    void scale_column(index_type const nc, const_reference val) {
      SPARSE::test_col(nc) ;
      for ( index_type i = 0 ; i < SPARSE::sp_nrows ; ++i ) {
        index_type pos = position(i,nc,true) ;
        if ( pos != SPARSE::sp_max_nnz ) A[pos] *= val ;
      }
    }

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    const_pointer       getA(void) const { return A . begin() ; }
    pointer             getA(void)       { return A . begin() ; }
    index_const_pointer getR(void) const { return R . begin() ; }
    index_const_pointer getJ(void) const { return J . begin() ; }
	index_pointer getR(void)  { return R . begin() ; }
    index_pointer getJ(void)  { return J . begin() ; }

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    CRowMatrix<T> const & operator = (const_reference s) {
      A = 0 ;
      if ( s != 0 )
        for ( index_type i = 0 ; i < SPARSE::sp_min_size ; ++i )
          ref(i,i) = s ;
      return *this ;
    }

    CRowMatrix<T> const & operator += (const_reference s) {
      for ( index_type i = 0 ; i < SPARSE::sp_min_size ; ++i )
        ref(i,i) += s ;
      return *this ;
    }

    CRowMatrix<T> const & operator -= (const_reference s) {
      for ( index_type i = 0 ; i < SPARSE::sp_min_size ; ++i )
        ref(i,i) -= s ;
      return *this ;
    }

    CRowMatrix<T> const & operator *= (const_reference s)
    { A *= s ; return * this ; }

    CRowMatrix<T> const & operator /= (const_reference s)
    { A /= s ; return * this ; }

    CRowMatrix<T> const & operator = (vector_const_reference v) {
      index_type n = min_index(v.size(), SPARSE::sp_min_size) ;
      A = 0 ;
      for ( index_type i = 0 ; i < n ; ++i )
        ref(i,i) = v[i] ;
      return *this ;
    }

    CRowMatrix<T> const & operator += (vector_const_reference v) {
      index_type n = min_index(v.size(), SPARSE::sp_min_size) ;
      for ( index_type i = 0 ; i < n ; ++i )
        ref(i,i) += v[i] ;
      return *this ;
    }

    CRowMatrix<T> const & operator -= (vector_const_reference v) {
      index_type n = min_index(v.size(), SPARSE::SPARSE::sp_min_size) ;
      for ( index_type i = 0 ; i < n ; ++i )
        ref(i,i) -= v[i] ;
      return *this ;
    }
    
    template <typename R> inline
    CRowMatrix<T> const & operator = (VectorE<T,R> const & e) {
      index_type n = min_index(e.size(), SPARSE::sp_min_size ) ;
      A = 0 ;
      for ( index_type k = 0 ; k < n ; ++k )
        ref(k,k) = e[k] ;
      return *this ;
    }
    
    template <typename R> inline
    CRowMatrix<T> const & operator += (VectorE<T,R> const & e) {
      index_type n = min_index(e.size(), SPARSE::sp_min_size ) ;
      for ( index_type k = 0 ; k < n ; ++k )
        ref(k,k) += e[k] ;
      return *this ;
    }

    template <typename R> inline
    CCoorMatrix<T> const & operator -= (VectorE<T,R> const & e) {
      index_type n = min_index(e.size(), SPARSE::sp_min_size ) ;
      for ( index_type k = 0 ; k < n ; ++k )
        ref(k,k) -= e[k] ;
      return *this ;
    }

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    index_type position(index_type const i, index_type const j) const {
      SPARSE::test_index(i,j) ;
      index_type lo  = R[i] ;
      index_type hi  = R[i+1] ;
      index_type len = hi - lo ;
      while (len > 0) {
        index_type half = len / 2 ;
        index_type mid = lo + half ;
        if ( J[mid] < j ) {
          lo = mid + 1 ;
          len -= half + 1 ;
        } else len = half;
      }
      if ( lo == hi || J[lo] != j ) lo = SPARSE::sp_max_nnz ;
      return lo ;
    }

    const_reference operator () (index_type i, index_type j) const
    { return A[position(i,j)] ; }

    reference ref(index_type i, index_type j) {
      index_type pos = position(i,j) ;
      SPARSELIB_TEST(pos != SPARSE::sp_max_nnz,
                     "CRowMatrix::set(" << i << "," << j <<
                     ") referring to non existent element")
      return A[pos] ;
    }

    const_reference operator [] (index_type idx) const
    { SPARSE::test_nnz(idx) ; return A[idx] ; }

    reference operator [] (index_type idx)
    { SPARSE::test_nnz(idx) ; return A[idx] ; }

    // ****************************************************************
    // ITERATOR
    void Begin(void) const { iter_row = iter_ptr = 0 ; }
    void Next (void) const {
      SPARSELIB_TEST( iter_ptr < SPARSE::sp_nnz, "CRowMatrix::Next()")
      ++iter_ptr ;
      while ( iter_ptr >= R[iter_row+1] && iter_row+1 < SPARSE::sp_nrows ) ++iter_row ;
    }
    bool End(void) const { return iter_ptr < SPARSE::sp_nnz ; }
    index_const_reference i_index(void) const { return iter_row ; }
    index_const_reference j_index(void) const { return J[iter_ptr] ; }
    const_reference value(T const * const) const { return A[iter_ptr] ; }

    // mul row
    template <typename VECTOR> inline
    typename VECTOR::value_type
    mul_row(VECTOR const & v, index_type nr) const {
      SPARSELIB_TEST(nr < SPARSE::sp_nrows, "CRowMatrix::row_M_mul_V  bad row number")
      index_const_pointer RR = R . begin() + nr ;
      index_const_pointer JJ = J . begin() + R[0] ;
      const_pointer       AA = A . begin() + R[0] ;
      typename VECTOR::value_type tmp(0) ;
      SPARSELIB_MLOOP( RR[1] - RR[0], tmp = tmp + *AA++ * v[ *JJ++ ] ) ;
      return tmp ;
    }

    // A * x
    template <typename VECTOR> inline
    void ass_M_mul_V(VECTOR & res, VECTOR const & b) const {
      SPARSELIB_TEST( &res != &b, "CRowMatrix::ass_M_mul_V equal pointer")
      index_const_pointer RR = R . begin() ;
      index_const_pointer JJ = J . begin() ;
      const_pointer       AA = A . begin() ;

      index_type ir = 0 ;
      do {
        typename VECTOR::value_type tmp(0) ;
        SPARSELIB_MLOOP( RR[1] - RR[0], tmp = tmp + *AA++ * b[ *JJ++ ] ) ;
        ++RR ;
        res[ir] = tmp ;
      } while ( ++ir < SPARSE::sp_nrows ) ;

    }
    
    // s * A * x
    template <typename SCALAR, typename VECTOR> inline
    void ass_S_mul_M_mul_V(VECTOR & res, SCALAR const & s, VECTOR const & b) const {
      SPARSELIB_TEST( &res != &b, "CRowMatrix::ass_S_mul_M_mul_V equal pointer")
      index_const_pointer RR = R . begin() ;
      index_const_pointer JJ = J . begin() ;
      const_pointer       AA = A . begin() ;

      index_type ir = 0 ;
      do {
        typename VECTOR::value_type tmp(0) ;
        SPARSELIB_MLOOP( RR[1] - RR[0], tmp = tmp + *AA++ * b[ *JJ++ ] ) ;
        ++RR ;
        res[ir] = s * tmp ;
      } while ( ++ir < SPARSE::sp_nrows ) ;

    }

    // a + A * b
    template <typename VECTOR> inline
    void ass_V_sum_M_mul_V(VECTOR       & res,
                           VECTOR const & a,
                           VECTOR const & b) const {
      SPARSELIB_TEST( &res != &b,
                      "CRowMatrix::ass_V_sum_M_mul_V equal pointer")
      index_const_pointer RR = R . begin() ;
      index_const_pointer JJ = J . begin() ;
      const_pointer       AA = A . begin() ;

      index_type ir = 0 ;
      do {
        typename VECTOR::value_type tmp(0) ;
        SPARSELIB_MLOOP( RR[1] - RR[0], tmp = tmp + *AA++ * b[ *JJ++ ] ) ;
        res[ir] = a[ir] + tmp ;
        ++RR ;
      } while ( ++ir < SPARSE::sp_nrows ) ;
    }

    // a - A * b
    template <typename VECTOR> inline
    void ass_V_sub_M_mul_V(VECTOR       & res,
                           VECTOR const & a,
                           VECTOR const & b) const {
      SPARSELIB_TEST(&res != &b,
                     "CRowMatrix::ass_V_sub_M_mul_V equal pointer")
      index_const_pointer RR = R . begin() ;
      index_const_pointer JJ = J . begin() ;
      const_pointer       AA = A . begin() ;

      index_type ir = 0 ;
      do {
        typename VECTOR::value_type tmp(0) ;
        SPARSELIB_MLOOP( RR[1] - RR[0], tmp = tmp + *AA++ * b[ *JJ++ ] ) ;
        res[ir] = a[ir] - tmp ;
        ++RR ;
      } while ( ++ir < SPARSE::sp_nrows ) ;
    }

    // a + s * A * b
    template <typename SCALAR, typename VECTOR> inline
    void ass_V_sum_S_mul_M_mul_V(VECTOR       & res,
                                 VECTOR const & a,
                                 SCALAR const & s,
                                 VECTOR const & b) const {
      SPARSELIB_TEST( &res != &b,
                      "CRowMatrix::ass_V_sum_M_mul_V equal pointer")
      index_const_pointer RR = R . begin() ;
      index_const_pointer JJ = J . begin() ;
      const_pointer       AA = A . begin() ;

      index_type ir = 0 ;
      do {
        typename VECTOR::value_type tmp(0) ;
        SPARSELIB_MLOOP( RR[1] - RR[0], tmp = tmp + *AA++ * b[ *JJ++ ] ) ;
        res[ir] = a[ir] + s * tmp ;
        ++RR ;
      } while ( ++ir < SPARSE::sp_nrows ) ;
    }

  } ;

  SPARSELIB_MUL_STRUCTURES(CRowMatrix)

  /*
  //  #####   #####
  // #     # #     #   ####   #
  // #       #        #    #  #
  // #       #        #    #  #
  // #       #        #    #  #
  // #     # #     #  #    #  #
  //  #####   #####    ####   ######
  //
  // #     #
  // ##   ##    ##    #####  #####   #  #    #
  // # # # #   #  #     #    #    #  #   #  #
  // #  #  #  #    #    #    #    #  #    ##
  // #     #  ######    #    #####   #    ##
  // #     #  #    #    #    #   #   #   #  #
  // #     #  #    #    #    #    #  #  #    #
  */

  template <typename T>
  class CColMatrix : public Sparse<CColMatrix<T> > {
  public:
    typedef CColMatrix<T>  MATRIX ;
    typedef Sparse<MATRIX> SPARSE ;

    SPARSELIB_TYPES(T) ;
    SPARSELIB_VECTOR_TYPES(T) ;
    Vector<value_type> A ;
    Vector<index_type> C, I ;

  private:

    mutable index_type iter_col ;
    mutable index_type iter_ptr ;

    void internal_order() {
      index_type jj, kk, ck, ck1 ;
      SPARSE::sp_lower_nnz =
      SPARSE::sp_diag_nnz  =
      SPARSE::sp_upper_nnz = 0 ;
      for ( jj = 0, ck = C[0] ; jj < SPARSE::sp_ncols ; ++jj, ck = ck1 ) {
        ck1 = C[jj+1] ;
        QuickSortI<T>(I . begin() + ck,
                      A . begin() + ck,
                      ck1 - ck ) ;
        // setup statistic
        for ( kk = ck ; kk < ck1 ; ++kk ) SPARSE::ldu_count(I[kk],jj) ;
  # ifdef SPARSELIB_DEBUG
        for ( kk = ck+1 ; kk < ck1 ; ++kk )
          SPARSELIB_TEST0(I[kk-1] < I[kk], "CColMatrix::internal_order() failed")
  # endif
      }
      SPARSE::sp_nnz = C[SPARSE::sp_ncols] ;
    }

    template <typename MAT, typename Compare>
    void convert(Sparse<MAT> const & M, Compare cmp) {
      // step 0: Count nonzero
      index_type nz = 0 ;
      for ( M . Begin() ; M . End() ;  M . Next() )
        if ( cmp(M . i_index(), M . j_index() ) ) ++nz ;

      SPARSE::set_up( M . nrows(), M . ncols(), nz ) ;
      SPARSE::sp_nnz = SPARSE::sp_max_nnz ;
      A . new_dim( SPARSE::sp_max_nnz + 1)  ; A[ SPARSE::sp_max_nnz ] = 0 ;
      I . new_dim( SPARSE::sp_max_nnz )     ;
      C . new_dim( SPARSE::sp_ncols + 1 )   ; C = 0 ;

      // step 1: Evaluate not zero
      for ( M . Begin() ; M . End() ; M . Next() ) {
        index_type i = M . i_index() ;
        index_type j = M . j_index() ;
        if ( cmp(i,j) ) ++C[j] ;
      }
      for ( index_type k = 0 ; k < SPARSE::sp_ncols ; ++k ) C[k+1] += C[k] ;

      // step 2: Fill matrix
      for ( M . Begin() ; M . End() ; M . Next() ) {
        index_type i = M . i_index() ;
        index_type j = M . j_index() ;
        if ( cmp(i,j) ) {
          index_type jj = --C[ j ] ;
          A[ jj ] = M . value((pointer)(0)) ;
          I[ jj ] = i ;
        }
      }
      SPARSELIB_TEST( C[0] == 0 && C[SPARSE::sp_ncols] == nz,
                      "CColMatrix::new_dim(Sparse & A) failed" )
      // step 3: internal_order matrix
      internal_order() ;
    }

  public:

    virtual ~CColMatrix(void) { free() ; }
    CColMatrix(void) {} ;

    template <typename Compare>
    CColMatrix(CColMatrix<T> const & M, Compare cmp)
    { convert(M,cmp) ; }

    CColMatrix(CColMatrix<T> const & M)
    { convert(M,all_ok()) ; }

    template <typename MAT, typename Compare>
    CColMatrix(Sparse<MAT> const & M, Compare cmp)
    { convert(M,cmp) ; }

    template <typename MAT>
    CColMatrix(Sparse<MAT> const & M)
    { convert(M,all_ok()) ; }

    // copy
    CColMatrix<T> const & operator = (CColMatrix<T> const & M) {
      if ( &M == this ) return *this ; // avoid copy to itself
      new_dim( M . nrows(), M . ncols(), M . nnz() ) ;
      SPARSE::sp_nnz        = M . nnz() ;
      SPARSE::sp_is_ordered = M . is_ordered() ;
      A . new_dim( SPARSE::sp_nnz   + 1 ) ; A = M . A ;
      C . new_dim( SPARSE::sp_ncols + 1 ) ; C = M . C ;
      I . new_dim( SPARSE::sp_nnz       ) ; I = M . I ;
      if ( !SPARSE::sp_is_ordered ) internal_order() ;
      return *this ;
    }

    // convert => CRowMatrix
    template <typename MAT>
    CColMatrix<T> const & operator = (Sparse<MAT> const & M)
    { convert(M,all_ok()) ; return * this ; }

    template <typename MAT, typename Compare>
    void new_dim(Sparse<MAT> const & M, Compare cmp) { convert(M,cmp) ; }

    template <typename MAT>
    void new_dim(Sparse<MAT> const & M) { new_dim(M,all_ok()) ; }

    void free() {
      A . free() ;
      I . free() ;
      C . free() ;
      SPARSE::set_up(0,0,0) ;
    }

    void set_row(index_type const nr, const_reference val) {
      SPARSE::test_row(nr) ;
      for ( index_type j = 0 ; j < SPARSE::sp_ncols ; ++j ) {
        index_type pos = position(nr,j,true) ;
        if ( pos != SPARSE::sp_max_nnz ) A[pos] = val ;
      }
    }

    void set_column(index_type const nc, const_reference val) {
      SPARSE::test_col(nc) ;
      A . fill( C[nc], C[nc+1], val ) ;
    }

    void scale_row(index_type const nr, const_reference val) {
      SPARSE::test_row(nr) ;
      for ( index_type j = 0 ; j < SPARSE::sp_ncols ; ++j ) {
        index_type pos = position(nr,j,true) ;
        if ( pos != SPARSE::sp_max_nnz ) A[pos] *= val ;
      }
    }

    void scale_column(index_type const nc, const_reference val) {
      SPARSE::test_col(nc) ;
      pointer pA = A.begin() + C[nc] ;
      pointer pB = A.begin() + C[nc+1] ;
      while ( pA < pB ) *pA++ *= val ;
    }


    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    const_pointer       getA(void) const { return A . begin() ; }
    pointer             getA(void)       { return A . begin() ; }
    index_const_pointer getI(void) const { return I . begin() ; }
    index_const_pointer getC(void) const { return C . begin() ; }

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

    CColMatrix<T> const & operator = (const_reference s) {
      A = 0 ;
      for ( index_type i = 0 ; i < SPARSE::sp_min_size ; ++i )
        ref(i,i) = s ;
      return *this ;
    }

    CColMatrix<T> const & operator += (const_reference s) {
      for ( index_type i = 0 ; i < SPARSE::sp_min_size ; ++i )
        ref(i,i) += s ;
      return *this ;
    }

    CColMatrix<T> const & operator -= (const_reference s) {
      for ( index_type i = 0 ; i < SPARSE::sp_min_size ; ++i )
        ref(i,i) -= s ;
      return *this ;
    }

    CColMatrix<T> const & operator *= (const_reference s)
    { A *= s ; return * this ; }

    CColMatrix<T> const & operator /= (const_reference s)
    { A /= s ; return * this ; }


    CColMatrix<T> const & operator = (vector_const_reference v) {
      index_type n = min_index(v.size(), SPARSE::sp_min_size) ;
      A = 0 ;
      for ( index_type i = 0 ; i < n ; ++i )
        ref(i,i) = v[i] ;
      return *this ;
    }

    CColMatrix<T> const & operator += (vector_const_reference v) {
      index_type n = min_index(v.size(), SPARSE::sp_min_size) ;
      for ( index_type i = 0 ; i < n ; ++i )
        ref(i,i) += v[i] ;
      return *this ;
    }

    CColMatrix<T> const & operator -= (vector_const_reference v) {
      index_type n = min_index(v.size(), SPARSE::sp_min_size) ;
      for ( index_type i = 0 ; i < n ; ++i )
        ref(i,i) -= v[i] ;
      return *this ;
    }
    
    template <typename R> inline
    CColMatrix<T> const & operator = (VectorE<T,R> const & e) {
      index_type n = min_index(e.size(), SPARSE::sp_min_size ) ;
      A = 0 ;
      for ( index_type k = 0 ; k < n ; ++k )
        ref(k,k) = e[k] ;
      return *this ;
    }

    template <typename R> inline
    CColMatrix<T> const & operator += (VectorE<T,R> const & e) {
      index_type n = min_index(e.size(), SPARSE::sp_min_size ) ;
      for ( index_type k = 0 ; k < n ; ++k )
        ref(k,k) += e[k] ;
      return *this ;
    }

    template <typename R> inline
    CColMatrix<T> const & operator -= (VectorE<T,R> const & e) {
      index_type n = min_index(e.size(), SPARSE::sp_min_size ) ;
      for ( index_type k = 0 ; k < n ; ++k )
        ref(k,k) -= e[k] ;
      return *this ;
    }

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

    index_type position(index_type i, index_type j) const {
      SPARSE::test_index(i,j) ;
      index_type lo  = C[j] ;
      index_type hi  = C[j+1] ;
      index_type len = hi - lo ;
      while (len > 0) {
        index_type half = len / 2 ;
        index_type mid = lo + half ;
        if ( I[mid] < i ) {
          lo = mid + 1 ;
          len -= half + 1 ;
        } else len = half;
      }
      if ( lo == hi || I[lo] != i ) lo = SPARSE::sp_max_nnz ;
      return lo ;
    }

    const_reference operator () (index_type i, index_type j) const
    { return A[position(i,j)] ; }

    reference ref(index_type i, index_type j) {
      index_type pos = position(i,j) ;
      SPARSELIB_TEST( pos != SPARSE::sp_max_nnz,
                      "CColMatrix::set(" << i << "," << j <<
                      ") referring to non existent element")
      return A[pos] ;
    }

    const_reference operator [] (index_type idx) const
    { SPARSE::test_nnz(idx) ; return A[idx] ; }

    reference operator [] (index_type idx)
    { SPARSE::test_nnz(idx) ; return A[idx] ; }

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    void Begin(void) const { iter_col = iter_ptr = 0 ; }
    void Next (void) const {
      SPARSELIB_TEST( iter_ptr < SPARSE::sp_nnz, "CColMatrix::Next()")
      ++iter_ptr ;
      while ( iter_ptr >= C[iter_col+1] && iter_col+1 < SPARSE::sp_ncols ) ++iter_col ;
    }
    bool End(void) const { return iter_ptr < SPARSE::sp_nnz ; }
    index_const_reference i_index(void) const { return I[iter_ptr] ; }
    index_const_reference j_index(void) const { return iter_col ; }
    const_reference value(T const * const) const { return A[iter_ptr] ; }

    // A * a
    template <typename VECTOR> inline
    void ass_M_mul_V(VECTOR & res, VECTOR const & a) const {
      SPARSELIB_TEST(&res != &a, "CColMatrix::ass_M_mul_V equal pointer")
      index_const_pointer II = I . begin() ;
      index_const_pointer CC = C . begin() ;
      index_const_pointer CE = C . begin() + SPARSE::sp_ncols ;
      const_pointer       AA = A . begin() ;
      res = 0 ;
      typename VECTOR::const_pointer pa = a . begin() ;
      do {
         SPARSELIB_MLOOP( CC[1] - CC[0], res[*II] = res[*II] + *AA++ * *pa ; ++II ) ;
         ++pa ;
      } while ( ++CC < CE ) ;
    }

    // s * A * a
    template <typename SCALAR, typename VECTOR> inline
    void ass_S_mul_M_mul_V(VECTOR & res, SCALAR const & s, VECTOR const & a) const {
      SPARSELIB_TEST(&res != &a, "CColMatrix::ass_S_mul_M_mul_V equal pointer")
      index_const_pointer II = I . begin() ;
      index_const_pointer CC = C . begin() ;
      index_const_pointer CE = C . begin() + SPARSE::sp_ncols ;
      const_pointer       AA = A . begin() ;
      res = 0 ;
      typename VECTOR::const_pointer pa = a . begin() ;
      do {
         SPARSELIB_MLOOP( CC[1] - CC[0], res[*II] = res[*II] + s * *AA++ * *pa ; ++II ) ;
         ++pa ;
      } while ( ++CC < CE ) ;
    }

    // a + A * b
    template <typename VECTOR> inline
    void ass_V_sum_M_mul_V(VECTOR       & res,
                           VECTOR const & a,
                           VECTOR const & b) const {
      SPARSELIB_TEST(&res != &b, "CColMatrix::ass_V_sum_M_mul_V equal pointer")
      index_const_pointer II = I . begin() ;
      index_const_pointer CC = C . begin() ;
      index_const_pointer CE = C . begin() + SPARSE::sp_ncols ;
      const_pointer       AA = A . begin() ;
      res = a ;
      typename VECTOR::const_pointer pb = b . begin() ;
      do {
         SPARSELIB_MLOOP( CC[1] - CC[0], res[*II] = res[*II] + *AA++ * *pb ; ++II) ;
         ++pb ;
      } while ( ++CC < CE ) ;
    }

    // a - A * b
    template <typename VECTOR> inline
    void ass_V_sub_M_mul_V(VECTOR       & res,
                           VECTOR const & a,
                           VECTOR const & b) const {
      SPARSELIB_TEST(&res != &b, "CColMatrix::ass_V_sub_M_mul_V equal pointer")
      index_const_pointer II = I . begin() ;
      index_const_pointer CC = C . begin() ;
      index_const_pointer CE = C . begin() + SPARSE::sp_ncols ;
      const_pointer       AA = A . begin() ;
      res = a ;
      typename VECTOR::const_pointer pb = b . begin() ;
      do {
         SPARSELIB_MLOOP( CC[1] - CC[0], res[*II] = res[*II] - *AA++ * *pb ; ++II) ;
         ++pb ;
      } while ( ++CC < CE ) ;
    }

    // a + s * A * b
    template <typename SCALAR, typename VECTOR> inline
    void ass_V_sum_S_mul_M_mul_V(VECTOR       & res,
                                 VECTOR const & a,
                                 SCALAR const & s,
                                 VECTOR const & b) const {
      SPARSELIB_TEST(&res != &b, "CColMatrix::ass_V_sum_M_mul_V equal pointer")
      index_const_pointer II = I . begin() ;
      index_const_pointer CC = C . begin() ;
      index_const_pointer CE = C . begin() + SPARSE::sp_ncols ;
      const_pointer       AA = A . begin() ;
      res = a ;
      typename VECTOR::const_pointer pb = b . begin() ;
      do {
         SPARSELIB_MLOOP( CC[1] - CC[0], res[*II] = res[*II] + s * *AA++ * *pb ; ++II) ;
         ++pb ;
      } while ( ++CC < CE ) ;
    }

  } ;

  SPARSELIB_MUL_STRUCTURES(CColMatrix)

  /*
  // #######
  //    #     #####   #  #####
  //    #     #    #  #  #    #
  //    #     #    #  #  #    #
  //    #     #####   #  #    #
  //    #     #   #   #  #    #
  //    #     #    #  #  #####
  //
  // #     #
  // ##   ##    ##    #####  #####   #  #    #
  // # # # #   #  #     #    #    #  #   #  #
  // #  #  #  #    #    #    #    #  #    ##
  // #     #  ######    #    #####   #    ##
  // #     #  #    #    #    #   #   #   #  #
  // #     #  #    #    #    #    #  #  #    #
  */

  template <typename T>
  class TridMatrix : public Sparse<TridMatrix<T> > {
  public:

    typedef TridMatrix<T>  MATRIX ;
    typedef Sparse<MATRIX> SPARSE ;

    SPARSELIB_TYPES(T) ;
    SPARSELIB_VECTOR_TYPES(T) ;

  private:

    template <typename VECTOR>
    void
    solve(VECTOR const & b, VECTOR & x) const {
      VECTOR ll(SPARSE::sp_nrows-1), dd(SPARSE::sp_nrows) ;

      // LU DEC
      index_type k = 0 ;
      dd[0] = d[0] ;
      for ( k = 1 ; k < SPARSE::sp_nrows ; ++k ) {
        ll[k-1] = l[k-1] / dd[k-1] ;
        dd[k] = d[k] - ll[k-1] * u[k-1] ;
      }

      // SOLVE
      k = 0 ;
      x[0] = b[0] ;
      while ( ++k < SPARSE::sp_nrows ) x[k] = b[k] - ll[k-1] * x[k-1] ;
      --k ;
      x[k] /= dd[k] ;
      while ( k > 0 ) {
        --k ;
        x[k] = (x[k] - u[k] * x[k+1]) / dd[k] ;
      } ;

      ll . free() ;
      dd . free() ;
    }

    vector_type l, d, u ;

    mutable index_type iter_counter, iter_row, iter_col ;

  public:

    virtual ~TridMatrix(void) { free() ; }

    TridMatrix(void) {}
    TridMatrix(index_type const ns) { new_dim(ns) ; }
    TridMatrix(TridMatrix<T> const & A) { new_dim(A) ; }

    TridMatrix<T> const & operator = (TridMatrix<T> const & A) {
      if ( &A == this ) return *this ; // avoid copy to itself
      new_dim( A .nrows() ) ;
      l = A . l ;
      d = A . d ;
      u = A . u ;
      return *this ;
    }

    void new_dim(index_type const ns) {
      l . new_dim(ns-1) ;
      d . new_dim(ns)   ;
      u . new_dim(ns-1) ;
      SPARSE::set_up(ns,ns,3*ns-1) ;
      SPARSE::sp_nnz = SPARSE::sp_max_nnz ;
    }

    void free(void) {
      l . free() ;
      d . free() ;
      u . free() ;
      SPARSE::set_up(0,0,0) ;
    }

    void set_row(index_type const nr, const_reference val) {
      SPARSE::test_row(nr) ;
      d[nr] = val ;
      if ( nr < SPARSE::sp_now ) u[nr]   = val ;
      if ( nr > 0              ) l[nr-1] = val ;
    }

    void set_column(index_type const nc, const_reference val) {
      SPARSE::test_col(nc) ;
      d[nc] = val ;
      if ( nc > 0               ) u[nc-1] = val ;
      if ( nc < SPARSE::sp_nrow ) l[nc]   = val ;
    }

    void scale_row(index_type const nr, const_reference val) {
      SPARSE::test_row(nr) ;
      d[nr] *= val ;
      if ( nr < SPARSE::sp_now ) u[nr]   *= val ;
      if ( nr > 0              ) l[nr-1] *= val ;
    }

    void scale_column(index_type const nc, const_reference val) {
      SPARSE::test_col(nc) ;
      d[nc] *= val ;
      if ( nc > 0               ) u[nc-1] *= val ;
      if ( nc < SPARSE::sp_nrow ) l[nc]   *= val ;
    }

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

    TridMatrix<T> const & operator = (const_reference s)
    { l = 0 ; u = 0 ; d = s ; return *this ; }

    TridMatrix<T> const & operator += (const_reference s)
    { d = d + s ; return *this ; }

    TridMatrix<T> const & operator -= (const_reference s)
    { d = d - s ; return *this ; }
    
    TridMatrix<T> const & operator *= (const_reference s)
    { l = l * s ; d = d * s ; u = u * s ; return *this ; }
    
    TridMatrix<T> const & operator /= (const_reference s)
    { l = l / s ; d = d / s ; u = u / s ; return *this ; }

    TridMatrix<T> const & operator = (vector_const_reference v)
    { l = 0 ; u = 0 ; d = v ; return *this ; }

    TridMatrix<T> const & operator += (vector_const_reference v)
    { d = d + v ; return *this ; }

    TridMatrix<T> const & operator -= (vector_const_reference v)
    { d = d - v ; return *this ; }

    template <typename R> inline
    TridMatrix<T> const & operator = (VectorE<T,R> const & e)
    { l = 0 ; u = 0 ; d = e ; ; return *this ; }

    template <typename R> inline
    TridMatrix<T> const & operator += (VectorE<T,R> const & e)
    { d += e ; ; return *this ; }

    template <typename R> inline
    TridMatrix<T> const & operator -= (VectorE<T,R> const & e)
    { d -= e ; ; return *this ; }

    const_reference operator () (index_type const i, index_type const j) const {
      switch ( i+1-j ) {
      case 0: return u[i] ;
      case 1: break ;
      case 2: return l[i-1] ;
      default:SPARSELIB_ERR( "TridMatrix(" << i << "," << j <<
                             ") referring to non existent element")
      } ;
      return d[i] ;
    }

    reference operator () (index_type const i, index_type const j) {
      switch ( i+1-j ) {
      case 0: return u[i] ;
      case 1: break ;
      case 2: return l[i-1] ;
      default: SPARSELIB_ERR("TridMatrix(" << i << "," << j <<
                             ") referring to non existent element")
      } ;
      return d[i] ;
    }

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

    void Begin(void) const {
      iter_counter = 0 ;
      iter_row     = 0 ;
      iter_col     = 0 ;
    }

    void Next (void) const {
      ++iter_counter ;
      if ( iter_counter < SPARSE::sp_nrows ) {
        iter_row = iter_col = iter_counter ;
      } else if ( iter_counter < 2*SPARSE::sp_nrows-1 ) {
        iter_row = iter_counter - SPARSE::sp_nrows + 1 ;
        iter_col = iter_counter - SPARSE::sp_nrows ;
      } else {
        iter_row = iter_counter - (2*SPARSE::sp_nrows-1) ;
        iter_col = iter_counter - (2*SPARSE::sp_nrows-2) ;;
      }
    }

    bool End(void) const
    { return iter_counter < 3*SPARSE::sp_nrows-2 ; }

    index_const_reference i_index(void) const { return iter_row ; }
    index_const_reference j_index(void) const { return iter_col ; }
    const_reference value(const_pointer const) const
    { return (*this)(iter_row,iter_col) ; }

    // A * a
    template <typename VECTOR> inline
    void ass_M_mul_V(VECTOR & res, VECTOR const & a) const {
      SPARSELIB_TEST(&res != &a, "TridMatrix::ass_M_mul_V equal pointer")

      typename VECTOR::pointer       pr = res . begin() ;
      typename VECTOR::const_pointer pa = a   . begin() ;
      const_pointer                  pl = l   . begin() ;
      const_pointer                  pd = d   . begin() ;
      const_pointer                  pu = u   . begin() ;
      
      *pr = *pd * *pa ;
      SPARSELIB_MLOOP( SPARSE::sp_nrows - 1,
                       pr[0] = pr[0] + pu[0] * pa[1] ;
                       pr[1] = pd[1] * pa[1] + pl[0] * pa[0] ;
                       ++pr; ++pl; ++pd; ++pu; ++pa ) ;

    }

    // a + A * b
    template <typename VECTOR> inline
    void ass_V_sum_M_mul_V(VECTOR       & res,
                           VECTOR const & a,
                           VECTOR const & b) const {
      SPARSELIB_TEST(&res != &b, "TridMatrix::ass_V_sum_M_mul_V equal pointer")
                      
      typename VECTOR::pointer       pr = res . begin() ;
      typename VECTOR::const_pointer pa = a   . begin() ;
      typename VECTOR::const_pointer pb = b   . begin() ;
      const_pointer                  pl = l   . begin() ;
      const_pointer                  pd = d   . begin() ;
      const_pointer                  pu = u   . begin() ;
      
      *pr = *pa + *pd * *pb ;
      SPARSELIB_MLOOP( SPARSE::sp_nrows - 1,
                       pr[0] = pr[0] + pu[0] * pb[1] ;
                       pr[1] = pa[1] + pd[1] * pb[1] + pl[0] * pb[0] ;
                       ++pr; ++pl; ++pd; ++pu; ++pa; ++pb ) ;

    }

    // a - A * b
    template <typename VECTOR> inline
    void ass_V_sub_M_mul_V(VECTOR       & res,
                           VECTOR const & a,
                           VECTOR const & b) const {
      SPARSELIB_TEST(&res != &b, "TridMatrix::ass_V_sub_M_mul_V equal pointer")
      
      typename VECTOR::pointer       pr = res . begin() ;
      typename VECTOR::const_pointer pa = a   . begin() ;
      typename VECTOR::const_pointer pb = b   . begin() ;
      const_pointer                  pl = l   . begin() ;
      const_pointer                  pd = d   . begin() ;
      const_pointer                  pu = u   . begin() ;
      
      *pr = *pa - *pd * *pb ;
      SPARSELIB_MLOOP( SPARSE::sp_nrows - 1,
                       pr[0] = pr[0] - pu[0] * pb[1] ;
                       pr[1] = pa[1] - pd[1] * pb[1] - pl[0] * pb[0] ;
                       ++pr; ++pl; ++pd; ++pu; ++pa; ++pb ) ;
    }

    // s * A * a
    template <typename SCALAR, typename VECTOR> inline
    void ass_S_mul_M_mul_V(VECTOR & res, SCALAR const & s, VECTOR const & a) const {
      SPARSELIB_TEST(&res != &v, "TridMatrix::ass_S_mul_M_mul_V equal pointer")

      typename VECTOR::pointer       pr = res . begin() ;
      typename VECTOR::const_pointer pa = a   . begin() ;
      const_pointer                  pl = l   . begin() ;
      const_pointer                  pd = d   . begin() ;
      const_pointer                  pu = u   . begin() ;
      
      *pr = s * *pd * *pa ;
      SPARSELIB_MLOOP( SPARSE::sp_nrows - 1,
                       pr[0] = pr[0] - pu[0] * pa[1] ;
                       pr[1] = s * pd[1] * pa[1] + s * pl[0] * pa[0] ;
                       ++pr; ++pl; ++pd; ++pu; ++pa ) ;

    }
    // a + s * A * b
    template <typename SCALAR, typename VECTOR> inline
    void ass_V_sum_S_mul_M_mul_V(VECTOR       & res,
                                 VECTOR const & a,
                                 SCALAR const & s,
                                 VECTOR const & b) const {
      SPARSELIB_TEST(&res != &b, "TridMatrix::ass_V_sum_M_mul_V equal pointer")

      typename VECTOR::pointer       pr = res . begin() ;
      typename VECTOR::const_pointer pa = a . begin() ;
      typename VECTOR::const_pointer pb = b . begin() ;
      const_pointer                  pl = l . begin() ;
      const_pointer                  pd = d . begin() ;
      const_pointer                  pu = u . begin() ;
      
      *pr = *pa + s * *pd * *pb ;
      SPARSELIB_MLOOP( SPARSE::sp_nrows - 1,
                       pr[0] = pr[0] + pu[0] * pb[1] ;
                       pr[1] = pa[1] + s * pd[1] * pb[1] + s * pl[0] * pb[0] ;
                       ++pr; ++pl; ++pd; ++pu; ++pa; ++pb ) ;
    }

    // x / A
    template <typename VECTOR> inline
    void ass_V_div_M(VECTOR & res, VECTOR const & v) const {
      solve(v, res) ;
    }
  } ;

  SPARSELIB_MUL_STRUCTURES(TridMatrix)

  template <typename TM, typename T> inline
  Vector_V_div_M<Vector<T>,TridMatrix<TM> >
  operator / (Vector<T> const & a, TridMatrix<TM> const & M) {
    return Vector_V_div_M<Vector<T>,TridMatrix<TM> >(a,M) ;
  }
  
  
  /*
  // ######  ######  ######  #######  #####  #######
  // #     # #     # #     # #       #     # #     #
  // #     # #     # #     # #       #       #     #
  // #     # ######  ######  #####   #       #     #
  // #     # #       #   #   #       #       #     #
  // #     # #       #    #  #       #     # #     #
  // ######  #       #     # #######  #####  #######
  */

  template <typename T>
  class IdPreco : public Preco<IdPreco<T> > {

    typedef IdPreco<T>     IDPRECO ;
    typedef Preco<IDPRECO> PRECO ;

    SPARSELIB_TYPES(T) ;
    SPARSELIB_VECTOR_TYPES(T) ;

  public:

    IdPreco(void) : Preco<IdPreco<T> >() {}

    template <typename MAT>
    void build(Sparse<MAT> const & A) {}

    template <typename VECTOR> inline
    void ass_preco(VECTOR & res, VECTOR const & v) const { res = v ; }

  } ;

  template <typename T, typename TP> inline
  Vector_P_mul_V<IdPreco<TP>,Vector<T> >
  operator * (IdPreco<TP> const & P, Vector<T> const & v)
  { return Vector_P_mul_V<IdPreco<TP>,Vector<T> >(P,v) ; }

  /*
  // ######  ######  ######  #######  #####  #######
  // #     # #     # #     # #       #     # #     #
  // #     # #     # #     # #       #       #     #
  // #     # ######  ######  #####   #       #     #
  // #     # #       #   #   #       #       #     #
  // #     # #       #    #  #       #     # #     #
  // ######  #       #     # #######  #####  #######
  */

  template <typename T>
  class DPreco : public Preco<DPreco<T> > {

    typedef DPreco<T>     DPRECO ;
    typedef Preco<DPRECO> PRECO ;

    SPARSELIB_TYPES(T) ;
    SPARSELIB_VECTOR_TYPES(T) ;

    vector_type D ;

  public:

    DPreco(void) : Preco<DPreco<T> >() {}
    virtual ~DPreco(void) { D.free() ; }

    template <typename MAT>
    void build(Sparse<MAT> const & A) {
      D . new_dim( A . nrows() ) ;
      D = 1 ;
      for ( A.Begin() ; A.End() ; A.Next() ) {
        index_type i = A . i_index() ;
        index_type j = A . j_index() ;
        if ( i == j ) D[i] = A . value((pointer)(0)) ;
      }
    }

    vector_type const & GetD(void) const { return D ; }

    template <typename VECTOR> inline
    void ass_preco(VECTOR & res, VECTOR const & v) const {
      for ( index_type i = 0 ; i < D . size() ; ++i ) res[i] = v[i] / D[i] ;
    }

  } ;

  template <typename T, typename TP> inline
  Vector_P_mul_V<DPreco<TP>,Vector<T> >
  operator * (DPreco<TP> const & P, Vector<T> const & v)
  { return Vector_P_mul_V<DPreco<TP>,Vector<T> >(P,v) ; }

  /*
  //  ###  #       ######  #     #
  //   #   #       #     # #     #
  //   #   #       #     # #     #
  //   #   #       #     # #     #
  //   #   #       #     # #     #
  //   #   #       #     # #     #
  //  ###  ####### ######   #####
  */

  template <typename T>
  class ILDUPreco : public Preco<ILDUPreco<T> > {
  public:

    typedef ILDUPreco<T>     ILDUPRECO ;
    typedef Preco<ILDUPRECO> PRECO ;

    SPARSELIB_TYPES(T) ;
    SPARSELIB_VECTOR_TYPES(T) ;

	vector<int> zeroDiag;
    vector_type   D ;
    CRowMatrix<T> L ;
    CColMatrix<T> U ;

  private:

    template <typename MAT, typename PAT>
    void build_ILDU(MAT const & A, PAT const & P) {
      SPARSELIB_TEST0( P . is_ordered(),
                      "ILDUPreco::build_LDU pattern must be ordered before use")
      SPARSELIB_TEST0( P . nrows() == A . nrows() &&  P . ncols() == A . ncols(),
                      "ILDUPreco::build_LDU pattern do not match matrix size")
      SPARSELIB_TEST0( P . nrows() == P . ncols(),
                      "ILDUPreco::build_LDU only square matrix allowed")
      SPARSELIB_TEST0( P . nrows() > 0,
                      "ILDUPreco::build_LDU empty matrix")

      // step 1: allocate_memory
      PRECO::pr_size = P . nrows() ;

      L . new_dim( P, greater<index_type>() ) ;
      U . new_dim( P, less<index_type>() ) ;
      D . new_dim( PRECO::pr_size ) ;
      D = 1 ;

      // insert valueues
      for ( P . Begin() ; P . End() ; P . Next() ) {
        index_type i   = P . i_index() ;
        index_type j   = P . j_index() ;
        value_type val = A(i,j) ;
        if ( i > j )      L . ref(i,j) = val ;
        else if ( j > i ) U . ref(i,j) = val ;
		else              {
			if (fabs(val) < 1e-10) {
			    // Make sure 0 diag causes no harm
				cout<<"0 diagonal at "<<i<<endl;
				val = 1;
				zeroDiag.push_back(i);
			}
			if (_isnan(val) || !_finite(val)) {
				cout<<"Dias is "<<val<<endl;
				val = 1;
				zeroDiag.push_back(i);
			}
			D[i]         = val ;
		}
      }

      index_const_pointer pR  = L . getR() ;
      index_const_pointer pJ  = L . getJ() ;
      pointer             pLA = L . getA() ;

      index_const_pointer pC  = U . getC() ;
      index_const_pointer pI  = U . getI() ;
      pointer             pUA = U . getA() ;

      // build LDU decomposition
      for ( index_type i = 1 ; i < PRECO::pr_size ; ++i ) {
        index_type LRi  = pR[i] ;
        index_type LRi1 = pR[i+1] ;

        for ( index_type kk = LRi ; kk < LRi1 ; ++kk ) {
          index_type k = pJ[kk] ;
          value_type bf = 0 ;
          for ( index_type jj = LRi ; jj < kk ; ++jj ) {
            index_type j = pJ[jj] ;
            value_type bf1 = pLA[jj] * D[j ] ;
            if ( j == k ) { bf += bf1 ; break ; }
            bf += bf1 * U(j,k) ;
          }
          pLA[kk] = ( pLA[kk] - bf ) / D[k] ;
        }

        index_type UCi  = pC[i] ;
        index_type UCi1 = pC[i+1] ;

        for ( index_type kk = UCi ; kk < UCi1 ; ++kk ) {
          index_type k = pI[kk] ;
          value_type bf = 0 ;
          for ( index_type jj = UCi ; jj < kk ; ++jj ) {
            index_type j = pI[jj] ;
            value_type bf1 = D[j] * pUA[jj] ;
            if ( j == k ) { bf += bf1 ; break ; }
            bf += L(k,j) * bf1 ;
          }
          pUA[kk] = ( pUA[kk] - bf ) / D[k] ;
        }

        value_type bf = 0 ;
        for ( index_type kk = LRi ; kk < LRi1 ; ++kk ) {
          index_type k = pJ[kk] ;
          bf += pLA[kk] * D[k] * U(k,i) ;
        }
        D[i] -= bf ;

			if (fabs(D[i]  ) < 1e-10) {
			    // Make sure 0 diag causes no harm
				cout<<"a 0 diagonal at "<<i<<endl;
				D[i]   = 1;
				zeroDiag.push_back(i);
			}
			if (_isnan(D[i]  ) || !_finite(D[i]  )) {
				cout<<"a Diag is "<<D[i]  <<endl;
				D[i]   = 1;
				zeroDiag.push_back(i);
			}
      }

    }

  public:

    ILDUPreco(void) : Preco<ILDUPRECO>() {}
    virtual ~ILDUPreco() { D.free() ; L.free() ; U.free() ; }

    template <typename MAT>
    void build(MAT const & M)
    { build_ILDU(M,M) ; }

    template <typename MAT, typename PRE>
    void build(MAT const & M, PRE const & P)
    { build_ILDU(M,P) ; }

    Vector<T>     const & GetD(void) const { return D ; }
    CRowMatrix<T> const & GetL(void) const { return L ; }
    CColMatrix<T> const & GetU(void) const { return U ; }

    template <typename VECTOR> inline
    void ass_preco(VECTOR & res, VECTOR const & v) const {
      res = v ;

      // solve L
      index_const_pointer pR  = L . getR() ;
      index_const_pointer pJ  = L . getJ() ;
      const_pointer       pLA = L . getA() ;
      index_type          i ;

      for ( i=1 ; i < PRECO::pr_size ; ++i ) {
        ++pR ;
        typename VECTOR::value_type tmp(0) ;
        for ( index_type i_cnt = pR[1] - pR[0] ; i_cnt > 0 ; --i_cnt)
          tmp = tmp + *pLA++ * res[ *pJ++ ] ;
        res[i] -= tmp ;
      } ;

      // solve D
      for ( index_type ii = 0 ; ii < PRECO::pr_size ; ++ii ) res[ii] = res[ii] / D[ii] ;

      // solve U
      index_const_pointer pC  = U . getC() + PRECO::pr_size ;
      index_const_pointer pI  = U . getI() + *pC  ;
      const_pointer       pUA = U . getA() + *pC  ;

      do {
        typename VECTOR::value_type vJ = res[--i] ;
        --pC ;
        for ( index_type i_cnt = pC[1] - pC[0] ; i_cnt > 0 ; --i_cnt )
          res[ *--pI ] -= *--pUA * vJ ;
      } while ( i > 0 ) ;
	  
	  // Make sure 0 diag causes no harm
	  for (i = 0; i < zeroDiag.size(); i++) {
		  res[zeroDiag[i]] = v[zeroDiag[i]];
	  }

    }

  };

  template <typename T, typename TP> inline
  Vector_P_mul_V<ILDUPreco<TP>,Vector<T> >
  operator * (ILDUPreco<TP> const & P, Vector<T> const & v)
  { return Vector_P_mul_V<ILDUPreco<TP>,Vector<T> >(P,v) ; }


/////////////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////////////

namespace sparselib {


  /*
  //  #####   #####
  // #     # #     #
  // #       #
  // #       #  ####
  // #       #     #
  // #     # #     #
  //  #####   #####
  */

  template <typename value_type,
            typename index_type,
            typename matrix_type,
            typename vector_type,
            typename preco_type>
  value_type cg(matrix_type const & A,
                vector_type const & b,
                vector_type       & x,
                preco_type  const & P,
                value_type  const & epsi,
                index_type  const   max_iter,
                index_type        & iter) {

    SPARSELIB_TEST0( A . nrows() == b . size() &&
                     A . ncols() == x . size() &&
                     A . nrows() == A . ncols(),
                    "Bad system in cg" <<
                    "dim matrix  = " << A.nrows() <<
                    " x " << A.ncols() <<
                    "\ndim r.h.s.  = " << b.size() <<
                    "\ndim unknown = " << x.size() )

    index_type  neq = b.size() ;
    vector_type p(neq), q(neq), r(neq), Ap(neq) ;
    value_type  resid, rho, rho_1 ;

    r   = b - A * x ;

		//for (int qq = 0; qq < r.size(); qq++) {
		//	if (_isnan(r[qq]) || !_finite(r[qq])) cout<<"r is bad at entry "<<qq<<" with val "<<r[qq]<<endl;
		//}

    p   = P * r ;
    rho = dot(p,r) ;

    value_type normb = normi(b) ;
    if ( normb == 0 ) normb = 1;

    iter = 1 ;
    do {
	//	for (int qq = 0; qq < p.size(); qq++) {
	//		if (_isnan(p[qq]) || !_finite(p[qq])) cout<<"p is bad at entry "<<qq<<" with val "<<p[qq]<<endl;
	//	}
	//	cout<<"normb == "<<normb<<endl;
      resid = normi(r) / normb ;

      if ( resid <= epsi ) break ;

      Ap = A * p ;

	//	for (int qq = 0; qq < Ap.size(); qq++) {
	//		if (_isnan(Ap[qq]) || !_finite(Ap[qq])) cout<<"Ap is bad at entry "<<qq<<" with val "<<Ap[qq]<<endl;
	//	}

	 //cout<<"dot(Ap,p) == "<<dot(Ap,p)<<endl;
      value_type alpha = rho/dot(Ap,p) ;
      x = x + alpha * p ;
      r = r - alpha * Ap ;
      q = P * r ;

      rho_1 = rho ;
      rho   = dot(q,r) ;

	  //cout<<"rho_1 == "<<rho_1<<endl;
      p = q + (rho / rho_1) * p ;

    }  while ( ++iter <= max_iter ) ;

    p  . free() ;
    q  . free() ;
    r  . free() ;
    Ap . free() ;

    return resid ;
  }

  /*
  // ######       #####   #####   #####  #######    #    #####
  // #     #  #  #     # #     # #     #    #      # #   #     #
  // #     #  #  #       #       #          #     #   #  #     #
  // ######   #  #       #  ####  #####     #    #     # ######
  // #     #  #  #       #     #       #    #    ####### #     #
  // #     #  #  #     # #     # #     #    #    #     # #     #
  // ######   #   #####   #####   #####     #    #     # ######
  */

  template <typename value_type,
            typename index_type,
            typename matrix_type,
            typename vector_type,
            typename preco_type>
  value_type bicgstab(matrix_type const & A,
                      vector_type const & b,
                      vector_type       & x,
                      preco_type  const & P,
                      value_type  const & epsi,
                      index_type  const   max_iter,
                      index_type        & iter) {

    SPARSELIB_TEST0( A . nrows() == b . size() &&
                     A . ncols() == x . size() &&
                     A . nrows() == A . ncols(),
                     "Bad system in bicgstab" <<
                     "dim matrix  = " << A.nrows() << 
                     " x " << A.ncols() <<
                     "\ndim r.h.s.  = " << b.size() <<
                     "\ndim unknown = " << x.size() )

    index_type neq = b . size() ;
    vector_type p(neq), s(neq), t(neq), v(neq), r(neq), rtilde(neq) ;

    iter = 1 ;

    value_type rhom1 = 1 ;
    value_type alpha = 1 ;
    value_type omega = 1 ;

    r = b - A * x ;
    r = P * r ;

    value_type normb = normi(b) ;
    if ( normb == 0.0 ) normb = 1;

    value_type resid = normi(r) / normb ;
    if ( resid <= epsi ) goto fine ;

    rtilde = r ;
    p      = 0 ;
    v      = 0 ;

    do {

      value_type rho  = dot(rtilde, r) ;
      SPARSELIB_TEST0( rho != 0, "BiCGSTAB failed" )

      value_type beta = (rho/rhom1) * (alpha/omega) ;
 
      p = r + beta  * (p - omega * v) ; 
      v = A * p ;
      v = P * v ;
      alpha = dot(rtilde,v) ;
      if ( alpha == 0.0 ) { cerr << "found alpha == 0\n" ; goto fine ; }
      alpha = rho / alpha ;

      s = r - alpha * v ;

      resid = normi(s) / normb ;
      if ( resid <= epsi) { x = x + alpha * p ; goto fine ; }

      t = A * s ;
      t = P * t ;

      omega = dot(t,t) ;
      if ( omega == 0 ) { cerr << "found omega == 0\n" ; goto fine ; }
      omega = dot(t,s) / omega ;

      x = x + alpha * p + omega * s ;
      r = s - omega * t ;

      resid = normi(r) / normb ;
      rhom1 = rho ;

    } while ( ++iter <= max_iter && resid > epsi && omega != 0 ) ;

  fine:

    p      . free() ;
    s      . free() ;
    t      . free() ;
    v      . free() ;
    r      . free() ;
    rtilde . free() ;

    return resid ;
  }

  namespace sparselib_fun {

    template <class T>
    inline
    void
    GeneratePlaneRotation(T const & dx, T const & dy, T & cs, T & sn) {
      if ( dy == 0.0 ) {
        cs = 1.0;
        sn = 0.0;
      } else if ( absval(dy) > absval(dx) ) {
        T temp = dx / dy;
        sn = 1.0 / sqrt( 1.0 + temp*temp );
        cs = temp * sn;
      } else {
        T temp = dy / dx;
        cs = 1.0 / sqrt( 1.0 + temp*temp );
        sn = temp * cs;
      }
    }

    template <class T>
    inline
    void
    ApplyPlaneRotation(T & dx, T & dy, T const & cs, T const & sn) {
      T temp = cs * dx + sn * dy;
      dy = -sn * dx + cs * dy;
      dx = temp;
    }
  }
  
  template <typename value_type,
            typename index_type,
            typename matrix_type,
            typename vector_type,
            typename preco_type>
  value_type gmres(matrix_type const & A,
                   vector_type const & b,
                   vector_type       & x,
                   preco_type  const & P,
                   value_type  const & epsi,
                   index_type  const   m, // max_sub_iter
                   index_type  const   max_iter,
                   index_type        & iter) {


    SPARSELIB_TEST0( A . nrows() == b . size() &&
                     A . ncols() == x . size() &&
                     A . nrows() == A . ncols(),
                     "Bad system in gmres" <<
                     "dim matrix  = " << A.nrows() <<
                     " x " << A.ncols() <<
                     "\ndim r.h.s.  = " << b.size() <<
                     "\ndim unknown = " << x.size() )

    index_type m1  = m+1 ;
    index_type neq = b . size() ;
    vector_type w(neq), r(neq), H(m1*m1), s(m1), cs(m1), sn(m1) ;
    
    Vector<vector_type> v(m1) ;
    for ( index_type nv = 0 ; nv <= m ; ++nv ) v[nv] . new_dim(neq) ;

    iter = 1 ; 
    value_type normb = norm2(b), resid ;
    if ( normb == 0.0 ) normb = 1.0 ;

    do {

      r = b - A * x ;
      r = P * r ;
      value_type beta = norm2(r) ;
      resid = beta / normb ;

      if ( resid <= epsi ) goto fine ;

      v[0] = r / beta ;
      s[0] = beta ;
      for ( index_type k = 1 ; k <= m ; ++k ) s[k] = 0 ;

      index_type i = 0 ;
      do {

        w = A * v[i] ;
        w = P * w ;

        index_type k ;
        for ( k = 0; k <= i; ++k ) {
          H[k*m1+i] = dot(w, v[k]) ;
          w -= H[k*m1+i] * v[k] ;
        }

        H[(i+1)*m1+i] = norm2(w) ;
        v[i+1] = w / H[(i+1)*m1+i] ;

        for ( k = 0 ; k < i ; ++k )
          sparselib_fun::ApplyPlaneRotation(H[k*m1+i], H[(k+1)*m1+i],
                                            cs[k],     sn[k]) ;

        sparselib_fun::GeneratePlaneRotation(H[i*m1+i], H[(i+1)*m1+i],
                                             cs[i],     sn[i]) ;

        sparselib_fun::ApplyPlaneRotation(H[i*m1+i], H[(i+1)*m1+i],
                                          cs[i],     sn[i]) ;
        
        sparselib_fun::ApplyPlaneRotation(s[i],  s[i+1],
                                          cs[i], sn[i]) ;
      
        ++i ; ++iter ;
        resid = s[i] / normb ;
        if ( resid < 0 ) resid = - resid ;

      } while ( i < m && iter <= max_iter && resid > epsi ) ;

      // Backsolve:  
      for ( int ii = i-1 ; ii >= 0 ; --ii ) {
        s[ii] /= H[ii*m1+ii] ;
        for ( int jj = ii - 1 ; jj >= 0 ; --jj )
          s[jj] -= H[jj*m1+ii] * s[ii] ;
      }

      for ( index_type jj = 0 ; jj < i ; ++jj )
        x += v[jj] * s[jj] ;

    } while ( iter <= max_iter ) ;

  fine:

    w  . free() ;
    r  . free() ;
    H  . free() ;
    s  . free() ;
    cs . free() ;
    sn . free() ;
    for ( index_type nv = 0 ; nv <= m ; ++nv ) v[nv] . free() ;
    v . free() ;

    return resid ;
  }
}

namespace sparselib_load {

  using ::sparselib::Vector ;
  using ::sparselib::Sparse ;
  using ::sparselib::SparsePattern ;
  using ::sparselib::CCoorMatrix ;
  using ::sparselib::CRowMatrix ;
  using ::sparselib::CColMatrix ;
  using ::sparselib::TridMatrix ;
  
  using ::sparselib::ILDUPreco ;
  using ::sparselib::DPreco ;
  using ::sparselib::IdPreco ;

  using ::sparselib::absval ;
  using ::sparselib::sin ;
  using ::sparselib::cos ;
  using ::sparselib::tan ;
  using ::sparselib::asin ;
  using ::sparselib::acos ;
  using ::sparselib::atan ;
  using ::sparselib::cosh ;
  using ::sparselib::sinh ;
  using ::sparselib::tanh ;

  using ::sparselib::sqrt ;
  using ::sparselib::ceil ;
  using ::sparselib::floor ;
  using ::sparselib::log ;
  using ::sparselib::log10 ;

  using ::sparselib::pow ;
  using ::sparselib::atan2 ;

  using ::sparselib::norm1 ;
  using ::sparselib::norm2 ;
  using ::sparselib::normi ;
  using ::sparselib::normp ;
  using ::sparselib::maxval ;
  using ::sparselib::minval ;
  using ::sparselib::dot ;
  using ::sparselib::dot_div ;
  using ::sparselib::dist ;
  using ::sparselib::dist2 ;

  // I/O
  using ::sparselib::operator >> ;
  using ::sparselib::operator << ;

  // solvers
  using ::sparselib::cg ;
  using ::sparselib::bicgstab ;
  using ::sparselib::gmres ;
}

/*
// ####### ####### #######
// #       #     # #
// #       #     # #
// #####   #     # #####
// #       #     # #
// #       #     # #
// ####### ####### #
*/

#include <vector>
#include <algorithm>
using namespace sparselib_load;

template <class T1, class T2, class T3>
void SparseMatMul(CRowMatrix<T1>*& result, CRowMatrix<T2>& A, CRowMatrix<T3>& B) {
	// Will create sparse matrix that is the result of multiplying A and B
	// You need to free the memory manually after finish using it
	if (A.ncols() != B.nrows()) {
		Error("Matrix dimension incompatible, can't multiply");
		return;
	}
	const bool IS_MEMORY_A_CONCERN = true;
	
	if (!IS_MEMORY_A_CONCERN) {
		// This will have duplicate memory
		// Generate rows of the result one by one
		int nrows = A.nrows();
		int ncols = B.ncols();
		unsigned* ra = A.getR();
		unsigned* ja = A.getJ();
		T2* va = A.getA();

		unsigned* rb = B.getR();
		unsigned* jb = B.getJ();
		T3* vb = B.getA();

		vector<int> lastSeen(ncols, -1);
		vector<int> index(ncols, 0);
		vector<pair<unsigned, T1> > resultCol;
		
		vector<unsigned> resultR(nrows+1);
		vector<unsigned> resultJ;
		vector<T1> resultA;
		uint acc = 0;
		for (int i = 0; i < nrows; i++) {		
			resultCol.clear();
			
			unsigned nnzInRowA = ra[i+1]-ra[i];		
			unsigned* ja_ptr = &ja[ra[i]];
			T2*	va_ptr = &va[ra[i]];
			for (int j = 0; j < nnzInRowA; j++) {
				unsigned colA = ja_ptr[j];
				T2 valA = va_ptr[j];
				unsigned nnzInRowB = rb[colA+1]-rb[colA];
				unsigned* jb_ptr = &jb[rb[colA]];
				T3* vb_ptr = &vb[rb[colA]];
				for (int k = 0; k < nnzInRowB; k++) {
					unsigned jpk = jb_ptr[k];
					if (lastSeen[jpk] < i) {
						lastSeen[jpk] = i;
						index[jpk] = resultCol.size();
						resultCol.push_back(pair<unsigned, T1>(jpk, vb_ptr[k]*valA));
					} else {
						resultCol[index[jpk]].second += vb_ptr[k]*valA;

					}
				}
			}

			uint rcSize = resultCol.size();
			sort(resultCol.begin(), resultCol.end(), CompareUintTPair<T1>());
			resultR[i] = acc;
			for (int j = 0; j < rcSize; j++) {
				resultJ.push_back(resultCol[j].first);
				resultA.push_back(resultCol[j].second);
			}
			acc += rcSize;
		}
		resultR[nrows] = acc;
		result = new CRowMatrix<T1>(nrows, ncols, resultA.size(), &resultR[0], &resultJ[0], &resultA[0]);
	} else {
		// No memory duplication but may be slower, but may be not, I don't know :P

		int nrows = A.nrows();
		int ncols = B.ncols();
		unsigned* ra = A.getR();
		unsigned* ja = A.getJ();
		T2* va = A.getA();

		unsigned* rb = B.getR();
		unsigned* jb = B.getJ();
		T3* vb = B.getA();

		vector<int> lastSeen(ncols, -1);
		vector<int> index(ncols, 0);
		vector<pair<unsigned, T1> > resultCol;
		

		uint nnz = 0;
		// Analyze the non-zero pattern first
		for (int i = 0; i < nrows; i++) {		
			unsigned nnzInRowA = ra[i+1]-ra[i];		
			unsigned* ja_ptr = &ja[ra[i]];
			for (int j = 0; j < nnzInRowA; j++) {
				unsigned colA = ja_ptr[j];
				unsigned nnzInRowB = rb[colA+1]-rb[colA];
				unsigned* jb_ptr = &jb[rb[colA]];
				for (int k = 0; k < nnzInRowB; k++) {
					unsigned jpk = jb_ptr[k];
					if (lastSeen[jpk] < i) {
						lastSeen[jpk] = i;
						nnz++;
					} 
				}
			}
		}


		result = new CRowMatrix<T1>(nrows, ncols, nnz);
		uint acc = 0;
		T1* resultA = result->A.begin();
		unsigned* resultJ = result->J.begin();	
		unsigned* resultR = result->R.begin();
		
		int sindex = nrows;
		// Perform actual computation
		for (int i = 0; i < nrows; i++) {		
			resultCol.clear();
			
			unsigned nnzInRowA = ra[i+1]-ra[i];		
			unsigned* ja_ptr = &ja[ra[i]];
			T2*	va_ptr = &va[ra[i]];
			for (int j = 0; j < nnzInRowA; j++) {
				unsigned colA = ja_ptr[j];
				T2 valA = va_ptr[j];
				unsigned nnzInRowB = rb[colA+1]-rb[colA];
				unsigned* jb_ptr = &jb[rb[colA]];
				T3* vb_ptr = &vb[rb[colA]];
				for (int k = 0; k < nnzInRowB; k++) {
					unsigned jpk = jb_ptr[k];
					if (lastSeen[jpk] < sindex) {
						lastSeen[jpk] = sindex;
						index[jpk] = resultCol.size();
						resultCol.push_back(pair<unsigned, T1>(jpk, vb_ptr[k]*valA));
					} else {
						resultCol[index[jpk]].second += vb_ptr[k]*valA;

					}
				}
			}

			uint rcSize = resultCol.size();
			sort(resultCol.begin(), resultCol.end(), CompareUintTPair<T1>());
			resultR[i] = acc;
			for (int j = 0; j < rcSize; j++) {
				*(resultJ++) = resultCol[j].first;
				*(resultA++) = resultCol[j].second;
			}
			acc += rcSize;
			sindex++;
		}
		resultR[nrows] = acc;
		result->internal_order_need_no_sort();
	}

}

template <class T>
class CSparseElement{
	public:
	CSparseElement(unsigned row, T val) {

		r = row;
		v = val;
	}
	int r;
	T v;
};
template <class T>
class CSparseElementCompare{
	public:
	bool operator() (const CSparseElement<T>& a, const CSparseElement<T>& b) {

		return a.r < b.r;
	}
};


template <class T>
class CSparseMatrixHelper{
public:
	unsigned numRows, numCols;
	vector<CSparseElement<T> >* columns;
	
	CSparseMatrixHelper(unsigned r, unsigned c) {
		numRows = r;
		numCols = c;
		
		columns = new vector<CSparseElement<T> >[numCols];
		
	}
	void CreateMatrix(CRowMatrix<T>*& mat) {

		unsigned* rows = new unsigned[numRows + 1];
		unsigned totalIndex = 0;
		vector<CSparseElement<T> > allElements;
		for (unsigned i = 0; i < numRows; i++) {
			//mexPrintf("%d\n",i);
			vector<CSparseElement<T> >* col = &columns[i];

			rows[i] = totalIndex;
			if (col->size() > 0) {
				sort(col->begin(), col->end(), CSparseElementCompare<T>());
				unsigned rr;
				unsigned t = 0;
				unsigned numEntry = col->size();
				while (t < numEntry) {
					double sum = 0.0;
					rr = col->at(t).r;
					while ((t < numEntry) && (col->at(t).r == rr)) {
						sum += col->at(t).v;
						t++;
					}

					allElements.push_back(CSparseElement<T>(rr, sum));
					totalIndex++;
				}
			}

		}
		rows[numRows] = totalIndex;
		
		// Create actual sparse matrix structure
		mat = new CRowMatrix<T>(numRows, numCols, allElements.size());
		
		T* pvals = mat->getA();
		unsigned* prows = mat->getR();
    	unsigned* pcols = mat->getJ();		
		memcpy(prows, rows, sizeof(unsigned)*(numRows+1));
		for (unsigned i = 0; i < allElements.size(); i++) {
			*(pvals++) = allElements[i].v;
			*(pcols++) = allElements[i].r;
		}
		mat->internal_order_need_no_sort();		
	}
	
	~CSparseMatrixHelper() {
		delete [] columns;
	}
};



#endif
