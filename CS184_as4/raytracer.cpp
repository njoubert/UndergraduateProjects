//============================================================================
// Name        : CS184_as4.cpp
// Author      : Niels Joubert
// Version     :
// Copyright   : 2008
// Description : Hello World in C, Ansi-style
//============================================================================

#include <vector>

#include "Debug.cpp"
#include "Primitives.cpp"
#include "Image.cpp"
#include "Algebra.cpp"
#include "Film.cpp"
#include "Sampler.cpp"
#include "Parser.cpp"
#include "Scene.cpp"
#include "assert.h"
#include <limits>

#define isThereMore(CURR, MAX, WANT)	((MAX - CURR) > WANT)

using namespace std;

//========================================
// Rendering and Raytracing
//========================================

Film film;
Scene* scene = NULL;
int maxDepth = 2;
int raysPerPixel = 1;
double dof = 0;

//Trace this ray into the scene, returning its color.
Color raytrace(Ray & ray, int depth) {
    Vector3d normal, incidence, reflectance, view;
    Color retColor;
       double t = 0, tl=0;
    printDebug(5, "Tracing Ray!");

    if (depth == 0)
        return retColor;

    Primitive* prim = scene->intersect(ray, &t);
    if (prim == NULL)
        return retColor;

    Vector3d p = ray.getPos(t);
    retColor += prim->ka;
    view.calculateFromPositions(&p,&(ray.e));
    view.normalize();                                               //VIEW VECTOR
    prim->calculateNormal(p, normal);                         //SURFACE NORMAL

    //chck normal
    normal.makeSameDirection(view);

    for (unsigned int li = 0; li < scene->lights.size(); li++) {

            Ray shadow;
            scene->lights[li]->getShadowRay(p, shadow);

            Primitive* occluder;
            if ((occluder = scene->intersect(shadow, &tl)) != NULL) {
                if (tl > 0) {
                    printDebug(5, "Shadow ray hit an object at t="<<tl);
                    occluder->debugMe(5);
                    continue;
                }
            }

            printDebug(5, "Calculating lighting on object for light " << li);

            scene->lights[li]->getIncidenceNormal(p, incidence);    //INCIDENCE
            reflectance.calculateReflective(incidence, normal);     //REFLECTIVE
            reflectance.normalize();

            //difuse term
            retColor += (prim->kd * scene->lights[li]->illumination) * max(incidence.dot(&normal), 0.0);
            //specular term
            retColor += (prim->ks * scene->lights[li]->illumination) * pow(max(reflectance.dot(&view), 0.0), (double) prim->sp);
    }

    retColor.clip();

    if (prim->kr.r > 0.0 || prim->kr.b > 0.0 || prim->kr.g > 0.0) {

            printDebug(2, "Calculating Reflective!");

            Ray refl;
            Vector3d reflp;
            reflp.calculateReflective(view, normal);
            reflp.normalize();
            refl.e = p;
            refl.d = reflp;
            refl.min_t = 0.0001;
            retColor += prim->kr * raytrace(refl, depth - 1);

    }

    retColor.clip();

    return retColor;

}

void render() {
    printInfo("Rendering...");
    Point point;
    Vector3d focalPlanePos;
    Ray ray;
    Sampler sampler(scene, film, raysPerPixel);
    while (sampler.getPoint(point)) {
        Vector3d eye = scene->eye.pos;
        ray = Ray::getRay(eye, point,0);

        if (dof != 0) {
            //find intersection with focal plane
            double z = 10;
            Triangle* focalPlane = new Triangle(0, 1000, z,
                    -1000, -1000, z,
                    1000, -1000, z,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            double t = focalPlane->intersect(ray);
            if (t == numeric_limits<double>::infinity()) {
                cout << "DID NOT HIT THE FOCAL PLANE!!!" << endl;
                exit(1);
            }
            focalPlanePos = ray.getPos(t);
            //jitter eye
            Vector3d d = ray.d;
            d.normalize();
            eye.x += (rand() % 100)/(100.0*dof)*d.x;
            eye.y += (rand() % 100)/(100.0*dof)*d.y;
            eye.z += (rand() % 100)/(100.0*dof)*d.z;
            // */
            //calc ray from new eye to focal plane pos
            ray = Ray::getRay(eye, focalPlanePos , 0);
        }

        film.expose(raytrace(ray, maxDepth), point, scene); //Functional == Beautiful
    }
    printInfo("Saving output file " << film.name);
    film.img->saveAsBMP(film.name);

}

//========================================
// Setup and Teardown Follows
//========================================

void selftest() {
    int ret=0;
    printInfo("Selftest Started!");
    ret += Image::selfTest();
    ret += Sphere::selfTest();
    ret += Triangle::selfTest();

    if (ret == 0) {
        printInfo("Selftest Completed!");
    } else
        printInfo("Selftest Failed!");
}
int parseCommandLine(int argc, char *argv[]) {

    bool malformedArg;
    bool printUsage = false;
    int i;
    for (i = 1; i < argc; i++) {

        malformedArg = false;

        if (strcmp(argv[i],"-d") == 0) {

                if (isThereMore(i, argc, 1)) {
                    if (atoi(argv[i + 1]) < 6 && atoi(argv[i + 1]) > 0)
                        DEBUG = atoi(argv[++i]);
                } else {
                    DEBUG = 1;
                }

        }  else if (!strcmp(argv[i], "-q")) {

                INFO = 0;

        } else if (!strcmp(argv[i], "-raysPerPixel")) {

            if (isThereMore(i, argc, 1)) {
                raysPerPixel = atoi(argv[++i]);
            } else {
                malformedArg = true;
            }

        } else if (!strcmp(argv[i], "-dof")) {

            if (isThereMore(i, argc, 1)) {
                dof = atof(argv[++i]);
            } else {
                malformedArg = true;
            }

        } else if (!strcmp(argv[i], "-selftest")) {

                selftest();
                exit(0);

        } else if (!strcmp(argv[i], "-maxdepth")) {

            if (isThereMore(i, argc, 1)) {
                maxDepth = atoi(argv[++i]);
            } else {
                malformedArg = true;
            }

        } else if (!strcmp(argv[i], "-scene")) {

                if (isThereMore(i, argc, 1)) {
                    Parser p;
                    scene = p.parseScene(string(argv[++i]));
                    if (scene == NULL || !p.isDone())
                        malformedArg = true;

                } else {
                    malformedArg = true;
                }

        } else if (!strcmp(argv[i], "-output")) {

                if (isThereMore(i, argc, 3)) {
                    film.name = string(argv[++i]);
                    int w = atoi(argv[++i]);
                    int h = atoi(argv[++i]);
                    film.setDimensions(w,h);
                } else {
                    malformedArg = true;
                }

        }  else {
            malformedArg = true;
        }

        if (malformedArg) {
            printError("Malformed input arg in parsing command \"" << argv[i] << "\"");
            printUsage = true;
        }
    }
    if (printUsage)
        return 1;
    return 0;

}
void printUsage() {
    cout << "Usage: "<< endl;
    cout << "  raytracer \n";
    cout << "   -scene /path/to/file.scene\n";
    cout << "   -output /path/to/out.bmp width height\n";
    cout << "   [-maxdepth n]      changes depth of ray recursion\n";
    cout << "   [-raysPerPixel n]  allows for supersampling.\n";
    cout << "   [-dof n]           allows for lens depth of field.\n";
    cout << "   [-d n]             for different levels of debug output\n";
    cout << "   [-q]               forces quiet operation\n";
    cout << "   [-selftest]" << endl;
}

int main(int argc,char **argv) {
    printInfo("Raytracer Initialized");
    scene = NULL;

    if (parseCommandLine(argc, argv) || scene == NULL || film.name.empty()) {
        printUsage();
        exit(1);
    }

    render();

    printInfo("Raytracer Done");
    return 0;
}
