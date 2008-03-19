#ifndef SAMPLER_C_
#define SAMPLER_C_

#include "Algebra.cpp"
#include "Film.cpp"
#include "Scene.cpp"

/** Creates samples across the viewport in world space */
class Sampler {
public:
    Sampler(Scene * s, Film & f):scene(s), film(f) {
        incrementu = (1 / (double) film.width);
        incrementv = (1 / (double) film.height);
        v = incrementv/2;
        u = -incrementu/2; //hack to start off correctly
    }

    bool getPoint(Point & p) {
        u += incrementu;
        if (u > 1) {
            u = incrementu / 2;
            v += incrementv;
        }
        if (v > 1)
            return false;
        p.setX((1-u)*((1-v)*scene->viewport.LL.x + v*scene->viewport.UL.x) + u*((1-v)*scene->viewport.LR.x + v*scene->viewport.UR.x));
        p.setY((1-u)*((1-v)*scene->viewport.LL.y + v*scene->viewport.UL.y) + u*((1-v)*scene->viewport.LR.y + v*scene->viewport.UR.y));
        p.setZ((1-u)*((1-v)*scene->viewport.LL.z + v*scene->viewport.UL.z) + u*((1-v)*scene->viewport.LR.z + v*scene->viewport.UR.z));
        p.u = u;
        p.v = v;
        return true;
    }

private:
    Scene * scene;
    Film & film;
    double u, v;
    double incrementu;
    double incrementv;
};

#endif /* SAMPLER_C_ */
