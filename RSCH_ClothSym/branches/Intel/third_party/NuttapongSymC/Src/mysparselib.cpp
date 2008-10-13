#include "mysparselib.h"

CSparseRow3x3BlockSquareMatrix::CSparseRow3x3BlockSquareMatrix(int numNodesi, vector<vector<int> >& nonZeroPattern) { 
   numNodes = numNodesi;
   for (int i = 0; i < numNodes; i++) {
      elements.push_back(vector<CSparseRow3x3BlockSquareMatrixElement> ());
      vector<CSparseRow3x3BlockSquareMatrixElement>& row = elements[i];
      vector<int>& nz = nonZeroPattern[i];
      sort(nz.begin(), nz.end());
      unsigned numEntry = nz.size();
      int t = 0;
		while (t < numEntry) {
			int cc = nz[t];
			while ((t < numEntry) && (nz[t] == cc)) {
				t++;
			}
         row.push_back(CSparseRow3x3BlockSquareMatrixElement(cc));
		}
   }
}
// Add both a row and a column, the rowCols include itself (the last one is itself)
void CSparseRow3x3BlockSquareMatrix::AddRowsAndColumns(vector<int>& uniqueSortedCols) {
   int nr = numNodes;
   numNodes++;
   elements.push_back(vector<CSparseRow3x3BlockSquareMatrixElement> ());
   vector<CSparseRow3x3BlockSquareMatrixElement>& newRow = elements[nr];
   unsigned numEntrym1 = uniqueSortedCols.size() - 1;

   // The last one must be itself..
   for (int i = 0; i < numEntrym1; i++) {
		int cc = uniqueSortedCols[i];
      newRow.push_back(CSparseRow3x3BlockSquareMatrixElement(cc));
      elements[cc].push_back(CSparseRow3x3BlockSquareMatrixElement(nr));
	}

   // Put in itself
   newRow.push_back(CSparseRow3x3BlockSquareMatrixElement(nr));
}
void CSparseRow3x3BlockSquareMatrix::ClearElements() {
   CSparseRow3x3BlockSquareMatrixElement* ptr;
   for (int i = 0; i < numNodes; i++) {
      int nnz = elements[i].size();
      ptr = &elements[i][0];
      for (int j = 0; j < nnz; j++) {
         ptr->v.MakeZero();
         ptr++;
      }    
   }
}
void CSparseRow3x3BlockSquareMatrix::RemoveLastRowColumn(int* affected, int numAffected) {
   // List of affected nodes
   elements.pop_back();
   for (int i = 0; i < numAffected; i++) {
      elements[affected[i]].pop_back();
   }
   numNodes--;
}
void CSparseRow3x3BlockSquareMatrix::Multiply(const vector<VEC3>& in, vector<VEC3>& out) const {
   //SOMETHING WRONG IN HERE
   const CSparseRow3x3BlockSquareMatrixElement* ptr;
   for (int i = 0; i < numNodes; i++) {
      int nnz = elements[i].size();
      ptr = &elements[i][0];
      VEC3 vec(0.0, 0.0, 0.0);
      for (int j = 0; j < nnz; j++) {
          
         if (ptr->c >= in.size()) cout<<"EXCEED INDEX!!!!!"<<endl;
         vec += ptr->v * in[ptr->c];
         ptr++;
      }   
      out[i] = vec;
   }
}

#if (_MSC_VER >= 1300) 
   void CSparseRow3x3BlockSquareMatrix::ComputePairHash(hash_map< CUniquePair, long, CUniquePairHasher> &pairHash) {
#else
   void CSparseRow3x3BlockSquareMatrix::ComputePairHash(hash_map<CUniquePair, long, CUniquePairHash>& pairHash) {
#endif
   pairHash.clear();
   for (int i = 0; i < numNodes; i++) {
      vector<CSparseRow3x3BlockSquareMatrixElement>& vec = elements[i];
      int ne = vec.size();
      for (int j = 0; j < ne; j++) {
         pairHash[CUniquePair(i, vec[j].c)] = j;
      }
   }   
}

#if (_MSC_VER >= 1300) 
   void CSparseRow3x3BlockSquareMatrix::UpdatePairHash(hash_map< CUniquePair, long, CUniquePairHasher> &pairHash, vector<int>& uniqueSortedCols) {
#else
   void CSparseRow3x3BlockSquareMatrix::UpdatePairHash(hash_map<CUniquePair, long, CUniquePairHash> &pairHash, vector<int>& uniqueSortedCols) {
#endif
   // Assume AddRowsAndColumns was called already, so this is the last node
   assert(uniqueSortedCols.back() == (numNodes - 1));
   int newRow = numNodes - 1;
   int numAddedm1 = uniqueSortedCols.size()-1;
   for (int i = 0; i < numAddedm1; i++) {
      int e = uniqueSortedCols[i];
      pairHash[CUniquePair(e, newRow)] = elements[e].size() - 1;
      pairHash[CUniquePair(newRow, e)] = i;
   }
   pairHash[CUniquePair(newRow, newRow)] = numAddedm1;
}

void CSparseRow3x3BlockSquareMatrix::FindSortedUniqueElements(vector<int>& cols, vector<int>& uniqueSortedCols) {
   sort(cols.begin(), cols.end());
   uniqueSortedCols.clear();
   unsigned numEntry = cols.size();
   int t = 0;
	while (t < numEntry) {
      int cc = cols[t];
		while ((t < numEntry) && (cols[t] == cc)) {
			t++;
		}
      uniqueSortedCols.push_back(cc);
   }
}
