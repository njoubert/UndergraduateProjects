/******************************************************************************
 * LargeMatrix.h
 *
 *  Created on: Oct 29, 2008
 *      Author: njoubert
 *
 *
 * This matrix class is stores sparse matrices in a block compressed format.
 *
 * DESIGN:
 * - We do not distinguish between row and column vectors, thus pre and post
 *   multiply assumes the vector is of such that you get a vector back.
 *
 * - We do not support matrix multiplication at this time. Too expensive!
 *
 * - We never want to create temporary memory. Thus, we pass back references,
 *   and code using this library can write directly into our memory.
 *
 * - We never create copies of the structure. Thus, the operators and methods
 *   implemented forces you to make a copy of it yourself with the copy
 *   constructor
 *
 * - All our functions return the result, so you can string operations
 *   together. We also use lots of operator overloads.
 *
 *****************************************************************************/

#ifndef LARGEMATRIX_H_
#define LARGEMATRIX_H_

#include <vector>
#include <iostream>
#include <algorithm>
#include "algebra3.h"
#include "LargeVector.h"

#ifdef ENABLE_OMP
    #include <omp.h>
#endif

class LargeMat3Matrix;
class LargeMat3MatrixRow;

/******************************************************************************
 *                                                                             *
 *                 LargeMatrixRow                                              *
 *                                                                             *
 *******************************************************************************/
class LargeMat3MatrixRow {
public:
    LargeMat3MatrixRow(int cols) :
        _sparseElements(0), _sparseIndices(0), _colLength(cols), ZERO(0) {

    }
    //The behaviour is undefined if there's duplicates in the sparsity pattern
    //and you do not compile with CATCHERRORS
    LargeMat3MatrixRow(int cols, vector<int> sparsityPattern, bool insertIdentity):
#ifdef CATCHERRORS
        _sparseElements(0), _sparseIndices(0), _colLength(cols), ZERO(0) {
#else
        _sparseElements(sparsityPattern.size()), _sparseIndices(sparsityPattern.size()), _colLength(cols), ZERO(0) {
#endif

#ifdef ENABLE_OMP
	omp_set_num_threads(ENABLE_OMP);
#endif

        mat3 toInsert(0);
        if (insertIdentity)
            toInsert = identity2D();
        sort(sparsityPattern.begin(), sparsityPattern.end());
        for (unsigned int i = 0; i < sparsityPattern.size(); i++) {
#ifdef CATCHERRORS
            if (i > 0 && (sparsityPattern[i] == sparsityPattern[i-1])) {
                //cout << __FILE__ << "::" << __LINE__ << ": DUPLICATE ELEMENTS IN SPARSE PATTERN!" << endl;
                //Don't add it.
            } else {
                _sparseIndices.push_back(sparsityPattern[i]);
                _sparseElements.push_back(toInsert);
            }
#else
            _sparseIndices[i] = sparsityPattern[i];
            _sparseElements[i] = toInsert;
#endif
        }
#ifdef CATCHERRORS
        if (_sparseIndices.size() != _sparseElements.size())
            cout << __FILE__ << "::" << __LINE__ << ": SPARSE ENTRIES AND ELEMENTS SIZES DOESN'T MATCH!" << endl;
#endif
    }
    virtual ~LargeMat3MatrixRow() {

    }
    int getLength() {
        return _colLength;
    }
    int getNonzeroSize() {
        return _sparseElements.size();
    }
    bool isNonZero(int c) const {
        unsigned int i = 0;
        while (i < _sparseIndices.size()) {
            if (_sparseIndices[i] < c) {
                i++;
            } else if (_sparseIndices[i] == c) {
                return true;
            } else
                return false;
        }
        return false;
    }
    int getSparseIndexForEntry(int c) const {
        unsigned int i = 0;
        if (0 == _sparseIndices.size()) {
#ifdef CATCHERRORS
            cout << __FILE__ << "::" << __LINE__ << ": ENTRY NOT IN SPARSE MATRIX: c=" << c << endl;
#endif
            return -1;
        }
        while (i < _sparseIndices.size()) {
            if (_sparseIndices[i] < c) {
                i++;
            } else if (_sparseIndices[i] == c)
                return i;
            else {
#ifdef CATCHERRORS
                cout << __FILE__ << "::" << __LINE__ << ": ENTRY NOT IN SPARSE MATRIX: c=" << c << endl;
#endif
                return -1;
            }
        }
#ifdef CATCHERRORS
                cout << __FILE__ << "::" << __LINE__ << ": ENTRY NOT FOUND IN ROW AT ALL" << endl;
#endif
        return -1;
    }
    void zeroValues() {
        mat3 zero(0);
        for (unsigned int i = 0; i < _sparseElements.size(); i++)
            _sparseElements[i] = zero;
    }
    void removeCol(int c) {
    	if (isNonZero(c)) {
			int index = getSparseIndexForEntry(c);
			vector<mat3>::iterator Eit = _sparseElements.begin();
			vector<int>::iterator Iit = _sparseIndices.begin();
			for (int i = 0; i != index; i++) {
				Eit++;
				Iit++;
			}
			_sparseElements.erase(Eit);
			_sparseIndices.erase(Iit);
    	}
		_colLength--;
    }
    void insertRowIntoDenserRow(LargeMat3MatrixRow const &r) {
        for (unsigned int i = 0; i < r._sparseElements.size(); i++) {
            (*this)[r._sparseIndices[i]] = r._sparseElements[i];
        }
    }
    mat3 & operator[](int c) {
        int ind = getSparseIndexForEntry(c);
        if (ind < 0) {
            return ZERO;
        } else {
            return _sparseElements[ind];
        }
    }
    mat3 operator[](int c) const {
        int ind = getSparseIndexForEntry(c);
        if (ind < 0) {
        	return ZERO;
        } else {
            return _sparseElements[ind];
        }
    }
    LargeMat3MatrixRow & operator =(LargeMat3MatrixRow const &r) {
#ifdef CATCHERRORS
        if ((_colLength != r._colLength) || (_sparseIndices.size() != r._sparseIndices.size())) {
            cout << __FILE__ << "::" << __LINE__ << ": SPARSE ROW DIMESIONS DOESNT MATCH\n" << endl;
        }
#endif
        for (unsigned int i = 0; i < _sparseElements.size(); i++) {
            _sparseElements[i] = r._sparseElements[i];
        }
        return *this;
    }
    LargeMat3MatrixRow & operator +=(LargeMat3MatrixRow const &r) {
#ifdef CATCHERRORS
        if ((_colLength != r._colLength) || (_sparseIndices.size() != r._sparseIndices.size())) {
            cout << __FILE__ << "::" << __LINE__ << ": SPARSE ROW DIMESIONS DOESNT MATCH\n" << endl;
        }
#endif
        for (unsigned int i = 0; i < _sparseElements.size(); i++) {
            _sparseElements[i] += r._sparseElements[i];
        }
        return *this;
    }
    LargeMat3MatrixRow & operator -=(LargeMat3MatrixRow const &r) {
#ifdef CATCHERRORS
        if ((_colLength != r._colLength) || (_sparseIndices.size() != r._sparseIndices.size())) {
            cout << __FILE__ << "::" << __LINE__ << ": SPARSE ROW DIMESIONS DOESNT MATCH\n" << endl;
        }
#endif
        for (unsigned int i = 0; i < _sparseElements.size(); i++) {
#ifdef CATCHERRORS
            if (_sparseIndices[i] != r._sparseIndices[i]) {
                cout << __FILE__ << "::" << __LINE__ << ": SPARSE ROW INDICES DOESNT MATCH\n" << endl;
            }
#endif
            _sparseElements[i] -= r._sparseElements[i];
        }
        return *this;
    }
    LargeMat3MatrixRow & operator *=(double v) {
        for (unsigned int i = 0; i < _sparseElements.size(); i++) {
            _sparseElements[i] *= v;
        }
        return *this;
    }
    LargeMat3MatrixRow & operator /=(double v) {
        for (unsigned int i = 0; i < _sparseElements.size(); i++) {
            _sparseElements[i] /= v;
        }
        return *this;
    }
    void preMultiply(LargeVec3Vector & in, LargeVec3Vector & out, int inIndex) {
        for (unsigned int i = 0; i < _sparseIndices.size(); i++) {
            out[_sparseIndices[i]] += _sparseElements[i] * in[inIndex];
        }
    }
    void postMultiply(LargeVec3Vector & in, LargeVec3Vector & out, int outIndex) {
        out[outIndex] = vec3(0,0,0);
        for (unsigned int i = 0; i < _sparseIndices.size(); i++) {
            out[outIndex] += _sparseElements[i] * in[_sparseIndices[i]];
        }
    }
    friend ostream & operator <<(ostream & s, const LargeMat3Matrix &m);

public:
    std::vector<mat3> _sparseElements;
    std::vector<int> _sparseIndices; //We probably want some kind of fast lookup too.
    int _colLength; //The theoretical amount of cols in this row.
    mat3 ZERO;
};

/******************************************************************************
 *                                                                             *
 *                 LargeMatrix                                                 *
 *                                                                             *
 *******************************************************************************/
class LargeMat3Matrix {
public:
    LargeMat3Matrix(int rows, int cols) :
        _rowData(rows), _rowCount(rows), _colCount(cols), ZERO(0) {
        for (int i = 0; i < rows; i++) {
            _rowData[i] = new LargeMat3MatrixRow(cols);
        }
        _matrixScale = 1;
    }
    LargeMat3Matrix(int rows, int cols, vector< vector<int> > sparsityPattern, bool insertIdentity) :
        _rowData(rows), _rowCount(rows), _colCount(cols), ZERO(0) {
#ifdef CATCHERRORS
        if ((unsigned)rows != sparsityPattern.size()) {
            cout << __FILE__ << "::" << __LINE__ << ": SPARSITY PATTERN NOT SAME DIM AS GIVEN ROW DIMENSIONS\n" << endl;
        }
#endif
        for (int i = 0; i < rows; i++) {
            _rowData[i] = new LargeMat3MatrixRow(cols, sparsityPattern[i], insertIdentity);
        }
        _matrixScale = 1;
    }

    virtual ~LargeMat3Matrix() {
        for (int i = 0; i < _rowCount; i++) {
            delete _rowData[i];
        }
    }
    bool isNonZero(int r, int c) {
#ifdef CATCHERRORS
        if ((r > _rowCount) || (c >= _colCount)) {
            cout << __FILE__ << "::" << __LINE__ << ": SPARSE MATRIX DIMESIONS OUT OF RANGE\n" << endl;
        }
#endif
        return (_rowData[r]->isNonZero(c));
    }
    int getNonzeroSize() {
        int net = 0;
        for (int i = 0; i < _rowCount; i++)
            net += _rowData[i]->getNonzeroSize();
        return net;
    }
    int getRowSize() {
        return _rowCount;
    }
    int getColSize() {
        return _colCount;
    }
    void zeroValues() {
        for (int i = 0; i < _rowCount; i++)
            _rowData[i]->zeroValues();
    }
    void zeroRowCol(int r, int c, bool setIntersectionToOne) {
#ifdef CATCHERRORS
        if ((r > _rowCount) || (c >= _colCount)) {
            cout << __FILE__ << "::" << __LINE__ << ": SPARSE MATRIX DIMESIONS OUT OF RANGE\n" << endl;
        }
#endif
        mat3 zero(0);
        _rowData[r]->zeroValues();
        for (int i = 0; i < _rowCount; i++) {
            if (_rowData[i]->isNonZero(c))
                (*_rowData[i])[c] = zero;
        }
        if (setIntersectionToOne) {

        	if (_rowData[r]->getSparseIndexForEntry(c) < 0) {
        		cout << __FILE__ << "::" << __LINE__ << ": ATTEMPTED TO WRITE DATA TO SPARSE ELEMENT!\n" << endl;
        	} else {
        		(*this)(r,c) = identity2D();
        	}

        }
    }

    /**
     * Zero's the rows and cols in the r row and c col, and modifies the b vector to
     * take into account the change in this system.
     * This function should be used on the A matrix in Ax=b, and given b and the value k
     * that x_i should be. It sets both A and b correctly.
     *
     * Returns true on success.
     * b needs to be the same length as this matrix.
     */
    bool constrainSystem(int r, int c, LargeVec3Vector & b, vec3 k) {
#ifdef CATCHERRORS
        if ((r > _rowCount) || (c >= _colCount)) {
            cout << __FILE__ << "::" << __LINE__ << ": SPARSE MATRIX DIMESIONS OUT OF RANGE\n" << endl;
        }
       if (b.size() != getColSize()) {
    	   cout << __FILE__ << "::" << __LINE__ << ": VECTOR AND MATRIX DIMENSIONS DOES NOT MATCH!\n" << endl;
    	   return false;
       }
       if (_rowData[r]->getSparseIndexForEntry(c) < 0) {
    	   cout << __FILE__ << "::" << __LINE__ << ": ATTEMPTED TO WRITE DATA TO SPARSE ELEMENT!\n" << endl;
    	   return false;
       }
#endif
        mat3 zero(0);
        _rowData[r]->zeroValues();
        for (int i = 0; i < _rowCount; i++) {
            if (_rowData[i]->isNonZero(c)) {
                //careful, order is important. M*v
                (*_rowData[i])[c] = zero;
            }
        }

        //now enforce constraint
        //updateConstraintIntersectionValue does not need to be called every step this could be called periodically elsewhere
        //updateConstraintIntersectionValue();
      	(*this)(r,c) = identity2D()*_matrixScale;
      	b[r] = k*_matrixScale;

      	return true;
    }

    bool printAvgStdDevMinMax() {
		mat3 m;
		double val = 0;
		double avg = 0;
		double count = 1;
		double max = -10e10;
		double min = 10e10;
		double avgOfBlocks = 0;
		double avgOfStdDev = 0;
		double squaredDiff = 0, aveDiffs = 0;
		double valAbs = 0;
		double avgAbs = 0;
		double maxAbs = 10e-30;
		double minAbs = 10e10;
		double avgOfBlocksAbs = 0;
		double avgOfStdDevAbs = 0;
		double blockCounter = 0;
		double squaredDiffAbs = 0, aveDiffsAbs = 0;
		for (int i = 0; i < _rowCount; i++) {
			for (int j = 0; j < _colCount; j++) {
				if (_rowData[i]->isNonZero(j)) {
					m = (*_rowData[i])[j];
					for (int i = 0; i < 3; i++) {
						for (int j = 0; j < 3; j++) {
							val = m[i][j];
							valAbs = fabs(val);
							avg += val;
							avgAbs += valAbs;
							count++;

							if (val > max)
								max = val;
							if (val < min)
								min = val;

							if (valAbs > maxAbs)
								maxAbs = valAbs;
							if (valAbs < minAbs && valAbs > 0)
								minAbs = valAbs;
						}
					}
					avg /= count;
					avgAbs /= count;

					squaredDiff = 0;
					aveDiffs = 0;
					squaredDiffAbs = 0;
					aveDiffsAbs = 0;
					//CALCULATE THE STDDEV:
					for (int i = 0; i < 3; i++) {
						for (int j = 0; j < 3; j++) {
							val = m[i][j];
							valAbs = fabs(val);
							squaredDiff = (avg - val);
							squaredDiff *= squaredDiff;
							aveDiffs += squaredDiff;

							squaredDiffAbs = (avgAbs - valAbs);
							squaredDiffAbs *= squaredDiffAbs;
							aveDiffsAbs += squaredDiffAbs;
						}
					}
					double stdDev = sqrt(aveDiffs / count);
					double stdDevAbs = sqrt(aveDiffsAbs / count);

					avgOfBlocks += avg;
					avgOfStdDev += stdDev;
					avgOfBlocksAbs += avgAbs;
					avgOfStdDevAbs += stdDevAbs;
					blockCounter++;
				}
			}
		}
		avgOfBlocks /= blockCounter;
		avgOfStdDev /= blockCounter;
		avgOfBlocksAbs /= blockCounter;
		avgOfStdDevAbs /= blockCounter;
		cout << "{REAL ANALYSIS ->     AVG: " << avgOfBlocks << ", STDDEV: " << avgOfStdDev
				<< ", MAX: " << max << ", MIN: " << min << "}" << endl;
		cout << "{ABSOLUTE ANALYSIS -> AVG: " << avgOfBlocksAbs << ", STDDEV: " << avgOfStdDevAbs
						<< ", MAX: " << maxAbs << ", MIN: " << minAbs << "}" << endl;
	}

    bool constrainSystem(int r, int c, LargeVec3Vector * b, vec3 k) {
    	return constrainSystem(r,c,*b,k);
    }

    bool updateConstraintIntersectionValue() {
		double avg = 0;
		double count = 0;
		mat3 m;
    	for (int i = 0; i < _rowCount; i++) {
			for (int j = 0; j < _colCount; j++) {
				if (_rowData[i]->isNonZero(j)) {
					m = (*_rowData[i])[j];
					for (int i = 0; i < 3; i++) {
						for (int j = 0; j < 3; j++) {
							avg += m[i][j];
						    count++;
						}
					}
				}
			}
		}
    	avg /= count;
//*
    	//Round the average to nearest decimal place so CG has easier time solving
    	double decimalCount = 1;
    	if(avg < 1)
    		while(avg < 1) {
    			avg *= 10;
    			decimalCount *= 10;
    		}
    	else
    		while(avg > 1) {
    			avg /= 10;
    			decimalCount /= 10;
    		}
    	avg = floor(avg);
    	if(decimalCount > 0)
    		avg /= decimalCount;
    	else
    		avg *= -1*decimalCount;
//*/
    	_matrixScale = avg;
	}
    /**
     * This is an extremely slow method that rebuilds the matrix without the specified row and column. It
     * is a counterpart to the constrainSystem methods that attempts to do the same thing, but
     * with removing rows and cols rather than zeroing out rows and columns.
     */
   bool removeRowCol(int r, int c, LargeVec3Vector & b, vec3 k) {
#ifdef CATCHERRORS
        if ((r > _rowCount) || (c >= _colCount)) {
            cout << __FILE__ << "::" << __LINE__ << ": SPARSE MATRIX DIMESIONS OUT OF RANGE\n" << endl;
        }
       if (b.size() != getColSize()) {
    	   cout << __FILE__ << "::" << __LINE__ << ": VECTOR AND MATRIX DIMENSIONS DOES NOT MATCH!\n" << endl;
    	   return false;
       }
       if (_rowData[r]->getSparseIndexForEntry(c) < 0) {
    	   cout << __FILE__ << "::" << __LINE__ << ": ATTEMPTED TO WRITE DATA TO SPARSE ELEMENT!\n" << endl;
    	   return false;
       }
#endif
        for (int i = 0; i < _rowCount; i++) {
            if (_rowData[i]->isNonZero(c)) {
                b[i] -= (*_rowData[i])[c]*k; //careful, order is important. M*v
            }
        }
        b[r] = k;

        //remove the column
        for (int i = 0; i < _rowCount; i++)
        	_rowData[i]->removeCol(c);
        _colCount--;

        //remove the row
        vector<LargeMat3MatrixRow*>::iterator it = _rowData.begin();
        for (int i = 0; i != r; i++)
        	it++;	//zomg this is fucking lame. c++ sux
        delete *it;
        _rowData.erase(it);
        _rowCount--;

        b.removeElement(r);

        return true;
    }

   bool removeRowCol(int r, int c, LargeVec3Vector * b, vec3 k) {
	   return removeRowCol(r,c,*b,k);
   }

    void insertMatrixIntoDenserMatrix(LargeMat3Matrix const &m) {
        for (int i = 0; i < m._rowCount; i++)
            (*(_rowData[i])).insertRowIntoDenserRow(*(m._rowData[i]));
    }
    mat3 & operator()(int r, int c) {
#ifdef CATCHERRORS
        if ((r > _rowCount) || (c >= _colCount)) {
            cout << __FILE__ << "::" << __LINE__ << ": SPARSE MATRIX DIMESIONS OUT OF RANGE\n" << endl;
        }
#endif
        return (*(_rowData[r]))[c];
    }
    mat3 operator()(int r, int c) const {
#ifdef CATCHERRORS
        if ((r > _rowCount) || (c >= _colCount)) {
            cout << __FILE__ << "::" << __LINE__ << ": SPARSE MATRIX DIMESIONS OUT OF RANGE\n" << endl;
        }
#endif
        return (*(_rowData[r]))[c];
    }
    LargeMat3Matrix & operator =(LargeMat3Matrix const &m) {
        for (int i = 0; i < _rowCount; i++)
            *(_rowData[i]) = *(m._rowData[i]);
        cout << "COPYING LARGE MATRIX!" << endl;
        return *this;
    }
    LargeMat3Matrix & operator +=(LargeMat3Matrix const &m) {
        for (int i = 0; i < _rowCount; i++)
            *(_rowData[i]) += *(m._rowData[i]);
        return *this;
    }
    LargeMat3Matrix & operator -=(LargeMat3Matrix const &m) {
        for (int i = 0; i < _rowCount; i++)
            *(_rowData[i]) -= *(m._rowData[i]);
        return *this;
    }
    LargeMat3Matrix & operator *=(double v) {
        for (int i = 0; i < _rowCount; i++)
            *(_rowData[i]) *= v;
        return *this;
    }
    LargeMat3Matrix & operator /=(double v) {
        for (int i = 0; i < _rowCount; i++)
            *(_rowData[i]) /= v;
        return *this;
    }
    //This turns the normal matrix-vector multiply into row-based rather than
    //column based through the observation that loop[out[i] = loop[(vector.*col)]]
    //is the same as loop[out += (vector.*row)]
    LargeVec3Vector & preMultiply(LargeVec3Vector & in, LargeVec3Vector & out, bool clearOutFirst) {
        if (clearOutFirst)
            out.zeroValues();

	/*
    //Unfortunately my cool trick for doing preMultiply doesn't parallelize as easily.
    //We're not using this at the moment, but it's worth working on this if necessary in the future.
#ifdef ENABLE_OMP
      #pragma omp parallel
      {
	int blockSize = ceil((double) _rowCount / omp_get_num_threads());
	int id = omp_get_thread_num();
	int st = id * blockSize;
	int en = min(((id+1) * blockSize)-1, _rowCount -1);
	//cout << "I am thread number " << id << endl;
	for (int iOutRow = st; iOutRow <= en; iOutRow++) {
	  for (unsigned int i = 0; i < _rowData[iOutRow]->_sparseIndices.size(); i++) {
	    out[_rowData[iOutRow]->_sparseIndices[i]] += _rowData[iOutRow]->_sparseElements[i] * in[iOutRow];
	  }
	}
      }
#else
	*/

        for (int iOutRow=0; iOutRow < _rowCount; iOutRow++) {
            _rowData[iOutRow]->preMultiply(in, out, iOutRow);
        }

	/*
#endif
	*/
	return out;
    }
    LargeVec3Vector & postMultiply(LargeVec3Vector & in, LargeVec3Vector & out) {
#ifdef ENABLE_OMP
      #pragma omp parallel
      {

          int blockSize = ceil((double) _rowCount / omp_get_num_threads());
          int id = omp_get_thread_num();
          int st = id * blockSize;
          int en = min(((id+1) * blockSize)-1, _rowCount -1);
          for (int iOutRow=st; iOutRow <= en; iOutRow++) {
              out[iOutRow] = vec3(0,0,0);
              for (unsigned int i = 0; i < _rowData[iOutRow]->_sparseIndices.size(); i++) {
                  out[iOutRow] += _rowData[iOutRow]->_sparseElements[i] * in[_rowData[iOutRow]->_sparseIndices[i]];
              }
          }

      }
#else
        for (int iOutRow=0; iOutRow < _rowCount; iOutRow++) {
            _rowData[iOutRow]->postMultiply(in, out, iOutRow);
        }
#endif
        return out;
    }
    ostream & outAsMatlab(ostream & s) {
        s << "[";
        for (int rowi = 0; rowi < _rowCount; rowi++) {
            for (int i = 0; i < 3; i++) {
                for (int coli = 0; coli < _colCount; coli++) {
                    s << (*_rowData[rowi])[coli][i][0] << " ";
                    s << (*_rowData[rowi])[coli][i][1] << " ";
                    s << (*_rowData[rowi])[coli][i][2] << " ";
                }
                s << ";" << endl;
            }
        }
        s << "]";
        return s;
    }
    friend ostream & operator <<(ostream & s, const LargeMat3Matrix &m);

    ostream & outAsMatlabDataFile(ostream & s) {
            s << "Matrix Dimensions: " << _rowCount << " X " << _colCount << endl;
            for (int rowi = 0; rowi < _rowCount; rowi++) {
                for (int i = 0; i < 3; i++) {
                    for (int coli = 0; coli < _colCount; coli++) {
                        s << (*_rowData[rowi])[coli][i][0] << " ";
                        s << (*_rowData[rowi])[coli][i][1] << " ";
                        s << (*_rowData[rowi])[coli][i][2] << " ";
                    }
                    s << endl;
                }
            }
            return s;
        }

private:
    std::vector<LargeMat3MatrixRow*> _rowData;
    int _rowCount; //The total amount of actual rows.
    int _colCount; //The total amount of theoretical cols.
    mat3 ZERO;
    double _matrixScale;
};

ostream & operator <<(ostream & s, const LargeMat3Matrix &m);

ostream & operator <<(ostream & s, const LargeMat3MatrixRow &m);


#endif /* LARGEMATRIX_H_ */
