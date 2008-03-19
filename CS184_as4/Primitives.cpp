#ifndef PRIMITIVES_C_
#define PRIMITIVES_C_

#include "Algebra.cpp"

class Primitive {
public:
    Color ks, ka, kd, r;
    float sp;
    virtual ~Primitive();
    virtual bool intersect(Ray & ray)=0;
};

class Sphere : public Primitive {
public:
    float r;
    Vector3d pos;
    Sphere(float radius, float x, float y, float z, 
            float ksr, float ksg, float ksb, 
            float kar, float kag, float kab,
            float kdr, float kdg, float kdb,
            float rr, float rg, float rb) {
        
    }
    
    bool intersect(Ray & ray) {
        return false;
    }
    
};

#endif /*PRIMITIVES_C_*/
