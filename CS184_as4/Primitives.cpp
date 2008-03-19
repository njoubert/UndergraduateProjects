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
    Vector3d c;
    Sphere(float radius, float x, float y, float z, 
            float ksr, float ksg, float ksb, float ksp,
            float kar, float kag, float kab,
            float kdr, float kdg, float kdb,
            float rr, float rg, float rb) {
        
        r = radius;
        c.setX(x);
        c.setY(y);
        c.setZ(z);
        sp = ksp;
        ks.setColor(ksr, ksg, ksb);
        ka.setColor(kar, kag, kab);
        kd.setColor(kdr, kdg, kdb);
        kr.setColor(rr, rg, rb);
        printDebug(4, "Created Sphere!");
    }
    
    float intersect(Ray & ray) {
        float dd = ray.d.dot(&ray.d);
        Vector3d e_c = ray.e - c;
        double desc = ray.d.dot(&e_c);
        desc = desc*desc;
        desc = desc - dd*(e_c.dot(&e_c) - r*r);
        if (desc < 0)
            return numeric_limits<float>::infinity(); //no hit!
        desc = sqrt(desc);
        double minde_c = -1*(ray.d.dot(&e_c));
        double t1 = (minde_c - desc)/(double)dd;
        double t2 = (minde_c + desc)/(double)dd;
        printDebug(3, "Hit a sphere!");
        if (t1 < t2)
        	return t1;
        return t2;
    }
    
    static int selfTest() {
   		printInfo("Testing Sphere Intersection...");
        Sphere sp(5, 0, 0, 0, 0,0,0,0,0,0,0,0,0,0,0,0,0);
        Ray r;
        r.e.setPos(-10, 0, 0);
        r.d.setPos(2,0.2,0.2);
        float t = sp.intersect(r);
        Vector3d i = r.getPos(t);
        printInfo("Intersect at t="<<t<<" pos=("<<i.x<<","<<i.y<<","<<i.z<<")");
        return t;
    }
};

class Vertex {
public:
	Vertex() {hasVN = false;};
	Vector3d v;  //vertex position
	Vector3d vn; //vertex normal
	bool hasVN;
}

class Triangle : public Primitive {
public:
    Vertex v1;
    Vertex v2;
    Vertex v3;
    Triangle(float x1, float y1, float z1, 
            float x2, float y2, float z2, 
            float x3, float y3, float z3, 
            float ksr, float ksg, float ksb, float ksp,
            float kar, float kag, float kab, 
            float kdr, float kdg, float kdb,
            float rr, float rg, float rb) {
        
        v1.v.setPos(x1, y1, z1);
        v2.v.setPos(x2, y2, z2);
        v3.v.setPos(x3, y3, z3);
        sp = ksp;
        ks.setColor(ksr, ksg, ksb);
        ka.setColor(kar, kag, kab);
        kd.setColor(kdr, kdg, kdb);
        kr.setColor(rr, rg, rb);
    }
    
    Triangle(Vector3d* v1, Vector3d* v2, Vector3d* v3, Color ks, Color ka, Color kd, Color kr, float ksp) {
        this->v1.v = *v1;
        this->v2.v = *v2;
        this->v3.v = *v3;
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
