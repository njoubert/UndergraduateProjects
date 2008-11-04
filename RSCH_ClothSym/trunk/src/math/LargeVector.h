/******************************************************************************
 * LargeVector.h
 *
 *  Created on: Oct 29, 2008
 *      Author: njoubert
 *
 *  PLEASE SEE LARGEMATRIX.H FOR DETAILS!
 *
 *****************************************************************************/

#ifndef LARGEVECTOR_H_
#define LARGEVECTOR_H_



#include <vector>
#include <iostream>
#include "algebra3.h"

class LargeVec3Vector {
public:
    LargeVec3Vector(int n) :
        _size(n), _elements(n) {

    }
    LargeVec3Vector(int n, double v):
        _size(n), _elements(n, vec3(v)){

    }
    LargeVec3Vector(const LargeVec3Vector & v) :
        _size(v.size()), _elements(v.size()) {
        for (int i = 0; i < v.size(); i++) {
            this->_elements[i] = v[i]; //Depends on the copy constructor of its contents
        }
    }
    virtual ~LargeVec3Vector() {
        //Do i need something here?
    }
    void zeroValues() {
        for (int i = 0; i < _size; i++)
            _elements[i] = vec3(0);
    }
    vec3 & operator[](int n) {
        return _elements[n];
    }
    vec3 operator[](int n) const {
        return _elements[n];
    }
    int size() const {
        return _size;
    }
    LargeVec3Vector & operator +=(LargeVec3Vector const &v) {
        for (int i = 0; i < _size; i++) {
            _elements[i] += v[i];
        }
        return *this;
    }
    LargeVec3Vector & operator -=(LargeVec3Vector const &v) {
        for (int i = 0; i < _size; i++) {
            _elements[i] -= v[i];
        }
        return *this;
    }
    LargeVec3Vector & operator *=(double v) {
        for (int i = 0; i < _size; i++) {
            _elements[i] *= v;
        }
        return *this;
    }
    LargeVec3Vector & operator /=(double v) {
        for (int i = 0; i < _size; i++) {
            _elements[i] /= v;
        }
        return *this;
    }
    double Dot(LargeVec3Vector const &v) {
        double ret = 0;
        for (int i = 0; i < _size; i++) {
            ret += (_elements[i] * v._elements[i]);
        }
        return ret;
    }

private:
    int _size;
    std::vector<vec3> _elements;

};

ostream & operator <<(ostream & s, const LargeVec3Vector &v);

#endif /* LARGEVECTOR_H_ */
