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

#define isThereMore(CURR, MAX, WANT)	((MAX - CURR) > WANT)

using namespace std;

//========================================
// Rendering and Raytracing
//========================================

Film film;
Scene* scene = NULL;

//Trace this ray into the scene, returning its color.
Color raytrace(Ray ray) {
    Color c(1.0f, 0.0f, 0.0f);
    return c;
}

void render() {
    printInfo("Rendering...");
    Point point;
    Sampler sampler(scene, film);
    while (sampler.getPoint(point)) {
        //point.debugMe();
        printInfoChar("#");
        film.expose(raytrace(Ray::getRay(scene->eye.pos, point)), point, scene); //Functional == Beautiful
    }
    printInfoChar("\n");
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

        } else if (!strcmp(argv[i], "-selftest")) {
    
                selftest();
                exit(0);

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
            printDebug(0, "Malformed input arg in parsing command \"" << argv[i] << "\"");
            printUsage = true;
        }
    }
    if (printUsage)
        return 1;
    return 0;

}
void printUsage() {
    cout << "Usage: "<< endl;
    cout << "  raytracer -scene /path/to/file.scene\n   -output /path/to/out.bmp width height\n   [-d n]\n   [-q]\n   [-selftest]" << endl;
}

int main(int argc,char **argv) {
    printInfo("Raytracer Initialized"); 
    scene = NULL;

    if (parseCommandLine(argc, argv) || scene == NULL || film.name.empty()) {
        printUsage();
        exit(1);
    }


    //Testing for Now
    scene->eye.pos.setX(0.0f);
    scene->eye.pos.setY(0.0f);
    scene->eye.pos.setZ(0.0f);
    scene->viewport.setBoundaries(-1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f);

    render();

    printInfo("Raytracer Done");
    return 0;
}
