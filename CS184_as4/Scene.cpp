#ifndef SCENE_C_
#define SCENE_C_

#include <assert.h>

#include <vector>
#include "Debug.cpp"
#include "Algebra.cpp"
#include "Primitives.cpp"
#include "Image.cpp"


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
    
protected:
    Color illumination;
    Vector3d pos;
};

class PointLight: public Light {
public:
    
};

class DirectionalLight: public Light {
public:

private:
    
};


class Scene {
    vector<Light*> lights;
    vector<Primitive*> objects;

public:
    
    bool addSphere(float radius, float x, float y, float z, 
            float ksr, float ksg, float ksb, 
            float kar, float kag, float kab,
            float kdr, float kdg, float kdb) {
        return true;
    }
    
    void addTriangle() {
        
    }
    
    Eye eye;
    Viewport viewport;
};

#endif /*SCENE_C_*/
