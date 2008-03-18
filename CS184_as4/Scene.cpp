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

/** Collects samples across the viewport in world space
 * into buckets for each pixel 
 * WORLD COORDINATES ===> IMAGE COORDINATES */
class Film {
public:
    int width;
    int height;
    Image * img;
    Film() {
        height = width = -1;
        img = NULL;
    }
    ~Film() {
        if (img != NULL)
            delete img;
    }
    void setDimensions(int w, int h) {
        assert(width == -1); assert(height == -1);
        width = w; height = h;
        img = new Image(w,h);
    }
    void expose(Color color, Vector3d & p, Viewport & vp) {
        //Transform between coordinates!!! 
        
        //This only works for a square or rectangle!
        
        //for space [l,r]x[b,t] for corners (l,b) to (u,r)
        //x = floor((u - l)/(r - l) * width);
        //y = floor((u - l)/(r - l) * width);
        
        //Be sure to round down!
        img->setPixel((int) floor((p.x - vp.LL.x)/(vp.LR.x - vp.LL.x) * width),
                (int) floor((p.y - vp.LL.y)/(vp.UL.y - vp.LL.y) * height),
                color.getBMPR(0,1),
                color.getBMPG(0,1),
                color.getBMPB(0,1));
    }
};


class Scene {
public:
    Eye eye;
    Viewport viewport;
    Film film;
    vector<Light*> lights;
    vector<Primitive*> objects;
};

#endif /*SCENE_C_*/
