#ifndef PRIMITIVES_C_
#define PRIMITIVES_C_

#include "Algebra.cpp"

class Primitive {
public:
    virtual ~Primitive();
    virtual bool intersect(Ray & ray)=0;
};

class Sphere : public Primitive {
public:
    bool intersect(Ray & ray) {
        return false;
    }
    
};

#endif /*PRIMITIVES_C_*/
