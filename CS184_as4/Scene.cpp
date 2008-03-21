#ifndef SCENE_C_
#define SCENE_C_

#include <assert.h>

#include <vector>
#include "Debug.cpp"
#include "Algebra.cpp"
#include "Primitives.cpp"
#include "Image.cpp"

#define SHADOWBIAS 0.0001

class Light;
class PointLight;
class DirectionalLight;

class Eye {
public:
    Vector3d pos;
};

class Viewport {
public:
    Vector3d LL; //lower left world coordinates
    Vector3d LR; //lower right world coordinates
    Vector3d UL; //lower left world coordinates
    Vector3d UR; //lower right world coordinates
    Viewport() {};
    void setBoundaries(float llx, float lly, float llz, float lrx, float lry, float lrz, float urx, float ury, float urz, float ulx, float uly, float ulz) {
        LL.setX(llx); LL.setY(lly); LL.setZ(llz);
        LR.setX(lrx); LR.setY(lry); LR.setZ(lrz);
        UL.setX(ulx); UL.setY(uly); UL.setZ(ulz);
        UR.setX(urx); UR.setY(ury); UR.setZ(urz);
    }
};

class Light {
public:
    Light() { }
    virtual ~Light() { }
    
    Light(float x,float y,float z,float r,float g,float b) {
        illumination.setColor(r,g,b);
        pos.setX(x); pos.setY(y); pos.setZ(z);
    }
    virtual void getIncidenceNormal(Vector3d & point, Vector3d & incidence) = 0;
    
    virtual void getShadowRay(Vector3d & point, Ray & shadow) = 0;
    
    Color illumination;
    Vector3d pos;
};

class PointLight: public Light {
public:
    PointLight(float x,float y,float z,float r,float g,float b): Light(x,y,z,r,g,b) {
        printDebug(3, "Created new PointLight!");
    }
    inline void getIncidenceNormal(Vector3d & point, Vector3d & incidence) {
        incidence.calculateFromPositions(&point,&pos);
        incidence.normalize();
    }
    inline void getShadowRay(Vector3d & point, Ray & shadow) {
        shadow = Ray::getRay(point, pos, SHADOWBIAS);
    }
};

class DirectionalLight: public Light {
public:
    DirectionalLight(float x,float y,float z,float r,float g,float b): Light(x,y,z,r,g,b) {
        printDebug(3, "Created new DirectionalLight!");
    }
    inline void getIncidenceNormal(Vector3d & point, Vector3d & incidence) {
        Vector3d zero(0,0,0);
        incidence.calculateFromPositions(&pos,&zero);
        incidence.normalize();
    }
    inline void getShadowRay(Vector3d & point, Ray & shadow) {
        shadow.e = point;
        shadow.d = (pos)*(-1);
        shadow.min_t = SHADOWBIAS;
    }
};


class Scene {
    vector<Primitive*> primitives;
public:
    bool addSphere(float radius, float x, float y, float z, 
            float ksr, float ksg, float ksb, float ksp,
            float kar, float kag, float kab,
            float kdr, float kdg, float kdb,
            float rr, float rg, float rb) {
        primitives.push_back(new Sphere(radius,x,y,z,ksr,ksg,ksb,ksp,kar,kag,kab,kdr,kdg,kdb,rr,rg,rb));
        return true;
    }
    
    bool addTriangle(float x1, float y1, float z1, 
            float x2, float y2, float z2, 
            float x3, float y3, float z3, 
            float ksr, float ksg, float ksb, float ksp,
            float kar, float kag, float kab, 
            float kdr, float kdg, float kdb,
            float rr, float rg, float rb) {
        
        primitives.push_back(new Triangle(x1, y1, z1, x2, y2, z2, x3, y3, z3, ksr, ksg, ksb, ksp, kar, kag, kab, kdr, kdg, kdb,rr,rg,rb));
        return true;
    }
    
    bool addTriangle(Vector3d* v1, Vector3d* v2, Vector3d* v3, Color ks, Color ka, Color kd, Color kr, float ksp) {
        primitives.push_back(new Triangle(v1,v2,v3,ks,ka,kd,kr,ksp));
        return true;
    }
    
   bool addDirectionLight(float x, float y, float z, float r, float g, float b) {
       lights.push_back(new DirectionalLight(x,y,z,r,g,b));
        return true;
    }
    bool addPointLight(float x, float y, float z, float r, float g, float b) {
        lights.push_back(new PointLight(x,y,z,r,g,b));
        return true;
    }
    
    /** Returns the closest object that the given ray intersects, or null */
    Primitive* intersect(Ray & ray, double* t_ptr) {
        double t;
        *t_ptr = numeric_limits<double>::infinity();
        Primitive* p = NULL;
        for (unsigned int i = 0; i < primitives.size(); i++) {
            t = primitives[i]->intersect(ray);  
            if ((t < *t_ptr) && (t > ray.min_t)) {          //checking here works...
                *t_ptr = t;
                p = primitives[i];
            }
        }
        return p;
        
    }
    
    Eye eye;
    Viewport viewport;
    vector<Light*> lights;
};

#endif /*SCENE_C_*/
