#ifndef PRIMITIVES_C_
#define PRIMITIVES_C_

#include "Algebra.cpp"

class Primitive {
public:
    Color ks, ka, kd, kr;
    float sp;
    virtual ~Primitive() {
        ;
    }
    virtual bool intersect(Ray & ray)=0;
};

class Sphere : public Primitive {
public:
    float r;
    Vector3d pos;
    Sphere(float radius, float x, float y, float z, 
            float ksr, float ksg, float ksb, float ksp,
            float kar, float kag, float kab,
            float kdr, float kdg, float kdb,
            float rr, float rg, float rb) {
        
        r = radius;
        pos.setX(x);
        pos.setY(y);
        pos.setZ(z);
        sp = ksp;
        ks.setColor(ksr, ksg, ksb);
        ka.setColor(kar, kag, kab);
        kd.setColor(kdr, kdg, kdb);
        kr.setColor(rr, rg, rb);
        printDebug(4, "Created Sphere!");
    }
    
    bool intersect(Ray & ray) {
        return false;
    }
    
};

#endif /*PRIMITIVES_C_*/
