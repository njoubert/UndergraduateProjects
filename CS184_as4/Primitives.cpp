#ifndef PRIMITIVES_C_
#define PRIMITIVES_C_

#include "Algebra.cpp"
#include <limits>

class Primitive {
public:
    Color ks, ka, kd, kr;
    float sp;
    virtual ~Primitive() {
        ;
    }
    /** 
     * Returns the point along the ray parameter t 
     * where the ray intersects this primitive.
     * returns numeric_limits<float>::infinity();
     * if no intersection occurs. */
    virtual float intersect(Ray & ray)=0;
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
    
    float intersect(Ray & ray) {
        return numeric_limits<float>::infinity();
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
    
    Triangle(Vector3d* v1, Vector3d* v2, Vector3d* v3, Color ks, Color ka, Color kd, Color kr, float ksp) {
        this->v1 = *v1;
        this->v2 = *v2;
        this->v3 = *v3;
        sp = ksp;
        this->ks = ks;
        this->ka = ka;
        this->kd = kd;
        this->kr = kr;
        
    }
    float intersect(Ray & ray) {
        return numeric_limits<float>::infinity();
    }
};

#endif /*PRIMITIVES_C_*/
