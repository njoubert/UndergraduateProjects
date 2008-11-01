/*
 * LargeVector.h
 *
 *  Created on: Oct 29, 2008
 *      Author: njoubert
 */

#ifndef LARGEVECTOR_H_
#define LARGEVECTOR_H_

#define ERRORS

#include <vector>
#include <iostream>
#include "algebra3.h"

class LargeVec3Vector {
public:
    LargeVec3Vector(int n) :
        size(n), elements(n) {

    }
    LargeVec3Vector(const LargeVec3Vector & v) :
        size(v.getSize()), elements(v.getSize()) {
        for (int i = 0; i < v.getSize(); i++) {
            this->elements[i] = v[i]; //Depends on the copy constructor of its contents
        }
    }
    virtual ~LargeVec3Vector() {
        //Do i need something here?
    }
    vec3 & operator[](int n) {
        return elements[n];
    }
    vec3 operator[](int n) const {
        return elements[n];
    }
    int getSize() const {
        return size;
    }
    LargeVec3Vector & operator +=(LargeVec3Vector const &v) {
        for (int i = 0; i < size; i++) {
            elements[i] += v[i];
        }
        return *this;
    }
    LargeVec3Vector & operator -=(LargeVec3Vector const &v) {
        for (int i = 0; i < size; i++) {
            elements[i] -= v[i];
        }
        return *this;
    }
    LargeVec3Vector & operator *=(double v) {
        for (int i = 0; i < size; i++) {
            elements[i] *= v;
        }
        return *this;
    }
    LargeVec3Vector & operator /=(double v) {
        for (int i = 0; i < size; i++) {
            elements[i] /= v;
        }
        return *this;
    }

private:
    int size;
    std::vector<vec3> elements;

};

ostream & operator <<(ostream & s, const LargeVec3Vector &v);

#endif /* LARGEVECTOR_H_ */
