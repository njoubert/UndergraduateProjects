/*
 * LargeMatrix.h
 *
 *  Created on: Oct 29, 2008
 *      Author: njoubert
 */

#ifndef LARGEMATRIX_H_
#define LARGEMATRIX_H_

#define CATCHERRORS

#include <vector>
#include "algebra3.h"
#include <iostream>

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
        _sparseElements(0), _sparseIndices(0), _colLength(cols) {

    }
    LargeMat3MatrixRow(int cols, vector<int> & sparsityPattern) {
        //TODO: Fill in
    }
    virtual ~LargeMat3MatrixRow() {

    }
    int getLength() {
        return _colLength;
    }
    int getSparseIndexForEntry(int c) const {
        int i = 0;
        while (i < _colLength) {
            if (c < _sparseIndices[i])
                i++;
            if (c == _sparseIndices[i])
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

        return _sparseElements[getSparseIndexForEntry(c)];
    }
    mat3 operator[](int c) const {
        return _sparseElements[getSparseIndexForEntry(c)];
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
private:
    std::vector<mat3> _sparseElements;
    std::vector<int> _sparseIndices; //We probably want some kind of fast lookup too.
    int _colLength; //The theoretical amount of cols in this row.
};

/******************************************************************************
 *                                                                             *
 *                 LargeMatrix                                                 *
 *                                                                             *
 *******************************************************************************/
class LargeMat3Matrix {
public:
    LargeMat3Matrix(int rows, int cols) :
        _rowData(rows), _rowLength(rows) {
        for (int i = 0; i < rows; i++) {
            _rowData[i] = new LargeMat3MatrixRow(cols);
        }
    }
    LargeMat3Matrix(int rows, int cols, vector< vector<int> > sparityPattern) {
        //TODO: Fill in
    }
    virtual ~LargeMat3Matrix() {
        for (int i = 0; i < _rowLength; i++) {
            delete _rowData[i];
        }
    }
    bool isNonZero(int r, int c) {
        cout << "isNonZero" << endl;
#ifdef CATCHERRORS
        if ((r > _rowLength) || (c > (_rowData[r])->getLength())) {
            cout << __FILE__ << "::" << __LINE__ << ": SPARSE MATRIX DIMESIONS OUT OF RANGE\n" << endl;
        }
#endif
        return (_rowData[r]->getSparseIndexForEntry(c) >= 0);
    }
    void zeroValues() {
        for (int i = 0; i < _rowLength; i++)
            _rowData[i]->zeroValues();
    }
    void zeroRowCol(int r, int c, bool setIntersectionToOne) {
#ifdef CATCHERRORS
        if ((r > _rowLength) || (c > (_rowData[r])->getLength())) {
            cout << __FILE__ << "::" << __LINE__ << ": SPARSE MATRIX DIMESIONS OUT OF RANGE\n" << endl;
        }
#endif
        mat3 zero(0);
        _rowData[r]->zeroValues();
        for (int i = 0; i < _rowLength; i++) {
            (*_rowData[r])[i] = zero;
        }
        if (setIntersectionToOne)
            (*this)(r,c) = identity2D();
    }
    mat3 & operator()(int r, int c) {
#ifdef CATCHERRORS
        if ((r > _rowLength) || (c > (_rowData[r])->getLength())) {
            cout << __FILE__ << "::" << __LINE__ << ": SPARSE MATRIX DIMESIONS OUT OF RANGE\n" << endl;
        }
#endif
        return (*(_rowData[r]))[c];
    }
    mat3 operator()(int r, int c) const {
        return (*(_rowData[r]))[c];
    }
    LargeMat3Matrix & operator +=(LargeMat3Matrix const &m) {
        for (int i = 0; i < _rowLength; i++)
            *(_rowData[i]) += *(m._rowData[i]);
        return *this;
    }
    LargeMat3Matrix & operator -=(LargeMat3Matrix const &m) {
        for (int i = 0; i < _rowLength; i++)
            *(_rowData[i]) -= *(m._rowData[i]);
        return *this;
    }
    LargeMat3Matrix & operator *=(double v) {
        for (int i = 0; i < _rowLength; i++)
            *(_rowData[i]) *= v;
        return *this;
    }
    LargeMat3Matrix & operator /=(double v) {
        for (int i = 0; i < _rowLength; i++)
            *(_rowData[i]) /= v;
        return *this;
    }
private:
    std::vector<LargeMat3MatrixRow*> _rowData;
    int _rowLength; //The total amount of actual rows.
};

#endif /* LARGEMATRIX_H_ */
