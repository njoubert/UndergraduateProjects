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

#ifdef ENABLE_OMP_V
    #include <omp.h>
#endif

class LargeVec3Vector {
public:
    LargeVec3Vector(int n) :
        _size(n), _elements(n) {
#ifdef ENABLE_OMP_V
	omp_set_num_threads(ENABLE_OMP_V);
#endif

    }
    LargeVec3Vector(int n, double v):
        _size(n), _elements(n, vec3(v)) {
    }

    LargeVec3Vector(const LargeVec3Vector & v) :
        _size(v.size()), _elements(v.size()) {
#ifdef ENABLE_OMP_V
	#pragma omp parallel
	{
        int blockSize = ceil((double) v.size() / omp_get_num_threads());
        int id = omp_get_thread_num();
        int st = id * blockSize;
        int en = min(((id+1) * blockSize)-1, v.size() -1);
        for (int i=st; i <= en; i++) {
            _elements[i] = v[i];
        }
	}
#else
	  for (int i = 0; i < v.size(); i++) {
		  _elements[i] = v[i]; //Depends on the copy constructor of its contents
	  }
#endif
    }

    virtual ~LargeVec3Vector() {
        //Do i need something here?
    }

    void zeroValues() {
#ifdef ENABLE_OMP_V
	#pragma omp parallel
	{
        int blockSize = ceil((double) _size / omp_get_num_threads());
        int id = omp_get_thread_num();
        int st = id * blockSize;
        int en = min(((id+1) * blockSize)-1, _size -1);
        for (int i=st; i <= en; i++) {
            _elements[i] = vec3(0);
        }
	}
#else
    	for (int i = 0; i < _size; i++)
            _elements[i] = vec3(0);
#endif
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

    LargeVec3Vector & operator =(LargeVec3Vector const &v) {
#ifdef ENABLE_OMP_V
	#pragma omp parallel
	{
        int blockSize = ceil((double) _size / omp_get_num_threads());
        int id = omp_get_thread_num();
        int st = id * blockSize;
        int en = min(((id+1) * blockSize)-1, _size -1);
        for (int i=st; i <= en; i++) {
            _elements[i] = v[i];
        }
	}
#else
    	for (int i = 0; i < _size; i++) {
            _elements[i] = v[i];
        }
#endif
        return *this;
    }
    LargeVec3Vector & operator +=(LargeVec3Vector const &v) {
#ifdef ENABLE_OMP_V
	#pragma omp parallel
	{
        int blockSize = ceil((double) _size / omp_get_num_threads());
        int id = omp_get_thread_num();
        int st = id * blockSize;
        int en = min(((id+1) * blockSize)-1, _size -1);
        for (int i=st; i <= en; i++) {
            _elements[i] += v[i];
        }
	}
#else
    	for (int i = 0; i < _size; i++) {
            _elements[i] += v[i];
        }
#endif
        return *this;
    }
    LargeVec3Vector & operator -=(LargeVec3Vector const &v) {
#ifdef ENABLE_OMP_V
	#pragma omp parallel
	{
        int blockSize = ceil((double) _size / omp_get_num_threads());
        int id = omp_get_thread_num();
        int st = id * blockSize;
        int en = min(((id+1) * blockSize)-1, _size -1);
        for (int i=st; i <= en; i++) {
            _elements[i] -= v[i];
        }
	}
#else
    	for (int i = 0; i < _size; i++) {
            _elements[i] -= v[i];
        }
#endif
    	return *this;
    }
    LargeVec3Vector & operator *=(double v) {
#ifdef ENABLE_OMP_V
	#pragma omp parallel
	{
        int blockSize = ceil((double) _size / omp_get_num_threads());
        int id = omp_get_thread_num();
        int st = id * blockSize;
        int en = min(((id+1) * blockSize)-1, _size -1);
        for (int i=st; i <= en; i++) {
            _elements[i] *= v;
        }
	}
#else
    	for (int i = 0; i < _size; i++) {
            _elements[i] *= v;
        }
#endif
    	return *this;
    }
    LargeVec3Vector & operator /=(double v) {
#ifdef ENABLE_OMP_V
	#pragma omp parallel
	{
        int blockSize = ceil((double) _size / omp_get_num_threads());
        int id = omp_get_thread_num();
        int st = id * blockSize;
        int en = min(((id+1) * blockSize)-1, _size -1);
        for (int i=st; i <= en; i++) {
            _elements[i] /= v;
        }
	}
#else
    	for (int i = 0; i < _size; i++) {
            _elements[i] /= v;
        }
#endif
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
