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
#include "algebra3.h"
#include <iostream>
#include "LargeVector.h"

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
    LargeMat3MatrixRow(int cols, vector<int> & sparsityPattern, bool insertIdentity):
#ifdef CATCHERRORS
        _sparseElements(0), _sparseIndices(0), _colLength(cols), ZERO(0) {
#else
        _sparseElements(sparsityPattern.size()), _sparseIndices(sparsityPattern.size()), _colLength(cols), ZERO(0) {
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
    int getSparseIndexForEntry(int c) const {
        unsigned int i = 0;
        if (0 == _sparseIndices.size()) {
#ifdef CATCHERRORS
            cout << __FILE__ << "::" << __LINE__ << ": ENTRY NOT IN SPARSE MATRIX" << endl;
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
                cout << __FILE__ << "::" << __LINE__ << ": ENTRY NOT IN SPARSE MATRIX" << endl;
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
            _sparseElements[i] += r._sparseElements[i];
        }
        return *this;
    }
    LargeMat3MatrixRow & operator *=(double v) {
        for (unsigned int i = 0; i < _sparseElements.size(); i++) {
            _sparseElements[i] /= v;
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

private:
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
    }
    LargeMat3Matrix(int rows, int cols, vector< vector<int> > & sparsityPattern, bool insertIdentity) :
        _rowData(rows), _rowCount(rows), _colCount(cols), ZERO(0) {
#ifdef CATCHERRORS
        if ((unsigned)rows != sparsityPattern.size()) {
            cout << __FILE__ << "::" << __LINE__ << ": SPARSITY PATTERN NOT SAME DIM AS GIVEN ROW DIMENSIONS\n" << endl;
        }
#endif
        for (int i = 0; i < rows; i++) {
            _rowData[i] = new LargeMat3MatrixRow(cols, sparsityPattern[i], insertIdentity);
        }
    }
    virtual ~LargeMat3Matrix() {
        for (int i = 0; i < _rowCount; i++) {
            delete _rowData[i];
        }
    }
    bool isNonZero(int r, int c) {
        cout << "isNonZero" << endl;
#ifdef CATCHERRORS
        if ((r > _rowCount) || (c >= _colCount)) {
            cout << __FILE__ << "::" << __LINE__ << ": SPARSE MATRIX DIMESIONS OUT OF RANGE\n" << endl;
        }
#endif
        return (_rowData[r]->getSparseIndexForEntry(c) >= 0);
    }
    int getNonzeroSize() {
        int net = 0;
        for (int i = 0; i < _rowCount; i++)
            net += _rowData[i]->getNonzeroSize();
        return net;
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
            (*_rowData[r])[i] = zero;
        }
        if (setIntersectionToOne)
            (*this)(r,c) = identity2D();
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
        for (int iOutRow=0; iOutRow < _rowCount; iOutRow++) {
            _rowData[iOutRow]->preMultiply(in, out, iOutRow);
        }
        return out;
    }
    LargeVec3Vector & postMultiply(LargeVec3Vector & in, LargeVec3Vector & out) {
        for (int iOutRow=0; iOutRow < _rowCount; iOutRow++) {
            _rowData[iOutRow]->postMultiply(in, out, iOutRow);
        }
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

private:
    std::vector<LargeMat3MatrixRow*> _rowData;
    int _rowCount; //The total amount of actual rows.
    int _colCount; //The total amount of theoretical cols.
    mat3 ZERO;
};

ostream & operator <<(ostream & s, const LargeMat3Matrix &m);

ostream & operator <<(ostream & s, const LargeMat3MatrixRow &m);


#endif /* LARGEMATRIX_H_ */
