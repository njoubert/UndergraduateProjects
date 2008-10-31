/*
 * LargeVector.h
 *
 *  Created on: Oct 29, 2008
 *      Author: njoubert
 */

#ifndef LARGEVECTOR_H_
#define LARGEVECTOR_H_

#include "vector"

template <class T>
class LargeVector {
public:
    LargeVector(int n);
    virtual ~LargeVector();
    T* operator[](int n);
    T& operator[](int n);
private:
    std::vector<T> elements;
};

template <class T>
LargeVector<T>::LargeVector() {

}

template <class T>
LargeVector<T>::~LargeVector() {

}



#endif /* LARGEVECTOR_H_ */
