#ifndef __MYSPARSELIB_H__
#define __MYSPARSELIB_H__

#include "globals.h"
#include <vector>

#ifdef __GNUC__
#include <ext/hash_map>
using namespace __gnu_cxx;
#else
#include <hash_map>
using namespace stdext;
#endif


class CUniquePair{
public:
   CUniquePair(long a, long b) {
      x = a; 
      y = b;
   }

   CUniquePair& operator = (const CUniquePair& a) {
      x = a.x;
      y = a.y;

      return *this;
   }
   bool operator == (const CUniquePair& b) const {
      return (x == b.x) && (y == b.y);
   }
   long x, y;
};

class CUniquePairHash{
public:
   size_t operator()(const CUniquePair& a) const{
      return (a.x*73856093) ^ (a.y*19349663);
   }
};

#if (_MSC_VER >= 1300) 
class CUniquePairHasher : public stdext::hash_compare <CUniquePair> {
public:

   size_t operator() (const CUniquePair& a) const
   {
      return (a.x*73856093) ^ (a.y*19349663);
   }

   bool operator() (const CUniquePair& a, const CUniquePair& b) const
   {
      if (a.x < b.x) return true;
      if (a.x > b.x) return false;

      return (a.y < b.y);
   }
};
#endif


class CSparseRow3x3BlockSquareMatrixElement{
public:
   CSparseRow3x3BlockSquareMatrixElement(int ci) {
      // don't bother setting MATRIX3
      c=ci;
   }
   CSparseRow3x3BlockSquareMatrixElement(int ci, MATRIX3& vi) {
      c = ci;
      v = vi;
   }
   int c;
   MATRIX3 v;
};

// Maintain hash table for fast entry locate as well

class CSparseRow3x3BlockSquareMatrix{
public:
   CSparseRow3x3BlockSquareMatrix(int numNodesi, vector<vector<int> >& nonZeroPattern); // Number of nodes
   void AddRowsAndColumns(vector<int>& uniqueSortedCols);
   void ClearElements();
   void Multiply(const vector<VEC3>& in, vector<VEC3>& out) const;
   void RemoveLastRowColumn(int* affected, int numAffected); // List of affected nodes, support only 4 now

   int numNodes; 
   vector<vector<CSparseRow3x3BlockSquareMatrixElement> > elements;

   #if (_MSC_VER >= 1300) 
      void ComputePairHash(hash_map< CUniquePair, long, CUniquePairHasher> &pairHash);
      void UpdatePairHash(hash_map< CUniquePair, long, CUniquePairHasher> &pairHash, vector<int>& uniqueSortedCols);
	#else
		void ComputePairHash(hash_map<CUniquePair, long, CUniquePairHash>& pairHash);
      void UpdatePairHash(hash_map<CUniquePair, long, CUniquePairHash>&, vector<int>& uniqueSortedCols);
	#endif
   
   
   static void FindSortedUniqueElements(vector<int>& cols, vector<int>& uniqueSortedCols);
   
   
   


   
};

#endif