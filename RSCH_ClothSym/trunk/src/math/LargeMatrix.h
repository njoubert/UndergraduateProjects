/*
 * LargeMatrix.h
 *
 *  Created on: Oct 29, 2008
 *      Author: njoubert
 */

#ifndef LARGEMATRIX_H_
#define LARGEMATRIX_H_

template <class T> class LargeMatrix;
template <class T> class LargeMatrixRow;

template <class T> class LargeMatrix {
public:
    LargeMatrix(int rows, int cols) {

    }
    //LargeMatrix(int rows, int cols, sparityPattern) {

    //}
    virtual ~LargeMatrix();
private:
    std::vector<LargeMatrixRow<T> > rows;
};

template <class T> class LargeMatrixRow {
    LargeMatrixRow(int cols);
    //LargeMatrixRow(int cols, sparsityPattern);
    virtual ~LargeMatrixRow();
public:
    std::vector<T> elements;
};

#endif /* LARGEMATRIX_H_ */
