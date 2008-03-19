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

class Triangle : public Primitive {
public:
    Vector3d v1;
    Vector3d v2;
    Vector3d v3;
    Triangle(float x1, float y1, float z1, 
            float x2, float y2, float z2, 
            float x3, float y3, float z3, 
            float ksr, float ksg, float ksb, float ksp,
            float kar, float kag, float kab, 
            float kdr, float kdg, float kdb,
            float rr, float rg, float rb) {
        
        v1.setPos(x1, y1, z1);
        v2.setPos(x2, y2, z2);
        v3.setPos(x3, y3, z3);
        sp = ksp;
        ks.setColor(ksr, ksg, ksb);
        ka.setColor(kar, kag, kab);
        kd.setColor(kdr, kdg, kdb);
        kr.setColor(rr, rg, rb);
    }
    
    bool intersect(Ray & ray) {
        return false;
    }
};

#endif /*PRIMITIVES_C_*/
