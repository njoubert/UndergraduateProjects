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
    virtual double intersect(Ray & ray)=0;

    /** Returns a normalized normal for the given point on the object. */
    virtual void calculateNormal(Vector3d & point, Vector3d & normal)=0;

    virtual void debugMe(int level)=0;
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

    double intersect(Ray & ray) {
        double dd = ray.d.dot(&ray.d);
        Vector3d e_c = ray.e - c;
        double desc = ray.d.dot(&e_c);
        desc = desc*desc;
        desc = desc - dd*(e_c.dot(&e_c) - r*r);
        if (desc < 0)
            return numeric_limits<double>::infinity(); //no hit!
            desc = sqrt(desc);
            double minde_c = -1*(ray.d.dot(&e_c));
            double t1 = (minde_c - desc)/dd;
            double t2 = (minde_c + desc)/dd;
            printDebug(4, "Hit a sphere!");
            if (t1 < t2)
                return t1;
            return t2;
    }

    inline void calculateNormal(Vector3d & point, Vector3d & normal) {
        normal.calculateFromPositions(&c, &point);
        normal.normalize();
    }

    void debugMe(int level) {
        if (DEBUG >= level)
            cout << "Sphere at position ("<<c.x<<","<<c.y<<","<<c.z<<") radius="<<r << endl;
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
        if (t != 0)
            return 0;
        return 1;
    }
};

class Vertex {
public:
    Vertex() {hasVN = false;}
    Vector3d v;  //vertex position
    Vector3d vn; //vertex normal
    bool hasVN;
};

class Triangle : public Primitive {
public:
    Vertex a;
    Vertex b;
    Vertex c;
    Triangle(float x1, float y1, float z1, 
            float x2, float y2, float z2, 
            float x3, float y3, float z3, 
            float ksr, float ksg, float ksb, float ksp,
            float kar, float kag, float kab, 
            float kdr, float kdg, float kdb,
            float rr, float rg, float rb) {

        a.v.setPos(x1, y1, z1);
        b.v.setPos(x2, y2, z2);
        c.v.setPos(x3, y3, z3);

      printDebug(2, "New triangle with vertices a=("<<a.v.x<<","<<a.v.y<<","<<a.v.z<<") b=("<<b.v.x<<","<<b.v.y<<","<<b.v.z<<") c=("<<c.v.x<<","<<c.v.y<<","<<c.v.z<<")");

        sp = ksp;
        ks.setColor(ksr, ksg, ksb);
        ka.setColor(kar, kag, kab);
        kd.setColor(kdr, kdg, kdb);
        kr.setColor(rr, rg, rb);
    }
 
    Triangle(Vector3d* v1, Vector3d* v2, Vector3d* v3, Color ks, Color ka, Color kd, Color kr, float ksp) {
      a.v.setPos(v1->x,v1->y,v1->z);
      b.v.setPos(v2->x,v2->y,v2->z);
      c.v.setPos(v3->x,v3->y,v3->z);

      printDebug(2, "New triangle with vertices a=("<<a.v.x<<","<<a.v.y<<","<<a.v.z<<") b=("<<b.v.x<<","<<b.v.y<<","<<b.v.z<<") c=("<<c.v.x<<","<<c.v.y<<","<<c.v.z<<")");

      sp = ksp;
      this->ks = ks;
      this->ka = ka;
      this->kd = kd;
      this->kr = kr;
    }

    double intersect(Ray & ray) {
        double t = numeric_limits<double>::infinity();
        Vector3d a_b = a.v - b.v;
        Vector3d a_c = a.v - c.v;
        Vector3d a_e = a.v - ray.e;
        double ei_min_hf = (a_c.y)*(ray.d.z) - (ray.d.y)*(a_c.z); 
        double gf_min_di = (ray.d.x)*(a_c.z) - (a_c.x)*(ray.d.z);
        double dh_min_eg = (a_c.x)*(ray.d.y) - (a_c.y)*(ray.d.x);
        double ak_min_jb = (a_b.x)*(a_e.y) - (a_e.x)*(a_b.y);
        double jc_min_al = (a_e.x)*(a_b.z) - (a_b.x)*(a_e.z);
        double bl_min_kc = (a_b.y)*(a_e.z) - (a_e.y)*(a_b.z);

        double M = a_b.x*(ei_min_hf) + a_b.y*(gf_min_di) + a_b.z*(dh_min_eg);
//        if (M < 0.001 && M > -0.001) {
//            printError("M is within [-0.001, 0.001] range!");
//            return numeric_limits<double>::infinity();
//        }   
        t = -1*(a_c.z*(ak_min_jb) + a_c.y*(jc_min_al) + a_c.z*(bl_min_kc))/M;
        if (t <= 0)
            return numeric_limits<double>::infinity();
         
        printDebug(5,"Intersecting Triangle with M="<<M<<" and t="<<t);

        double gamma = (ray.d.z*(ak_min_jb) + ray.d.y*(jc_min_al) + ray.d.x*(bl_min_kc))/M;
        if (gamma < 0 || gamma > 1)
            return numeric_limits<double>::infinity();
        double beta = (a_e.x*(ei_min_hf) + a_e.y*(gf_min_di) + a_e.z*(dh_min_eg))/M;
        if (beta < 0 || beta > 1)
            return numeric_limits<double>::infinity();    
        if ((gamma + beta) > 1)
            return numeric_limits<double>::infinity();
        return t;
    }

    inline void calculateNormal(Vector3d & point, Vector3d & normal) {
//        if (a.hasVN)
//            normal = a.vn;
//        else if (b.hasVN)
//            normal = b.vn;
//        else if (c.hasVN)
//            normal = c.vn;
        
        Vector3d a_b = b.v - a.v;
        Vector3d a_c = c.v - a.v;
//        Vector3d vn1(-1*(a_b.y*a_c.z - a_b.z*a_c.y),
//                -1*(a_b.z*a_c.x - a_b.x*a_c.z),
//                -1*(a_b.x*a_c.y - a_b.y*a_c.x));
        Vector3d vn1(a_b.y*a_c.z - a_b.z*a_c.y,
                a_b.z*a_c.x - a_b.x*a_c.z,
                a_b.x*a_c.y - a_b.y*a_c.x);
        vn1.normalize();
        a.hasVN = true;
        a.vn = vn1;
        normal = vn1;
    }

    void debugMe(int level) {
        if (DEBUG >= level)
            cout << "Triangle";
    }

    static int selfTest() {
        printInfo("Testing Triangle Intersection...");
        Triangle tr(10, 0, -10,-10, 0, -10, 0, 10 ,-10 , 0,0,0,0,0,0,0,0,0,0,0,0,0);
        Vector3d start(0,5,0);
        Vector3d end(0,5,-2);
        Ray r = Ray::getRay(start,end);
        double t = tr.intersect(r);
        Vector3d i = r.getPos(t);
        printInfo("Intersect at t="<<t<<" pos=("<<i.x<<","<<i.y<<","<<i.z<<")");
        if (t != 0)
            return 0;
        return 1;
    }

};

#endif /*PRIMITIVES_C_*/
