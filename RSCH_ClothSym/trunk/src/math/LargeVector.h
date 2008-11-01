/*
 * LargeVector.h
 *
 *  Created on: Oct 29, 2008
 *      Author: njoubert
 */

#ifndef LARGEVECTOR_H_
#define LARGEVECTOR_H_

#include "vector"
#include <iostream>

template <class T>
class LargeVector {
public:
    LargeVector(int n) : size(n), elements(n) {

    }
    LargeVector(const &LargeVector<T> v) {

    }
    virtual ~LargeVector() {

    }
    T& operator[](int n) {
        return elements[n];
    }
    T operator[](int n) const {
        return elements[n];
    }
    int getSize() const {
        return size;
    }
    LargeVector<T>& operator +=(const LargeVector<T> & v) {
        for (int i = 0; i < size; i++) {
            elements[i] += v[i];
        }
        return *this;
    }
    LargeVector<T>& operator -=(const LargeVector<T> & v) {
        for (int i = 0; i < size; i++) {
            elements[i] -= v[i];
        }
        return *this;
    }
    LargeVector<T>& operator *=(const LargeVector<T> & v) {
        for (int i = 0; i < size; i++) {
            elements[i] *= v[i];
        }
        return *this;
    }
    LargeVector<T>& operator /=(const LargeVector<T> & v) {
        for (int i = 0; i < size; i++) {
            elements[i] /= v[i];
        }
        return *this;
    }

private:
    int size;
    std::vector<T> elements;

};

template <class T> ostream& operator << (ostream& s, const LargeVector<T>& v) {
    s << "(";
    for (int i = 0; i < v.getSize()-1; i++) {
        s << v[i] << ",";
    }
    if (v.getSize() > 1)
        s << v[v.getSize()-1];
    s << ")";
    return s;
}


#endif /* LARGEVECTOR_H_ */
