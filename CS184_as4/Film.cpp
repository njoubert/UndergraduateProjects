#ifndef FILM_C_
#define FILM_C_

#include "Algebra.cpp"
#include "Scene.cpp"
#include <string>

/** Collects samples across the viewport in world space
 * into buckets for each pixel 
 * WORLD COORDINATES ===> IMAGE COORDINATES */
class Film {
    int oldpercent;
public:
    int width;
    int height;
    string name;
    Image * img;
    Film() {
        oldpercent = height = width = -1;
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
    void expose(Color color, Point & p, Scene * sc) {
        if (img == NULL) {
            printError("Attempted to expose a film before image was initialized! call setDimensions first!");
            exit(1);
        }
        int x = (int) floor(p.u * width);
        int y = (int) floor(p.v * height);
        printDebug(5, "Saving to pixel ("<<x<<","<<y<<") color ("<<(int)color.getBMPR(0,1)<<","<<(int)color.getBMPG(0,1)<<","<<(int)color.getBMPB(0,1)<<")");
        //Be sure to round down!
        int newpercent = ((x + (y * width) + 1)*100)/(width*height);
        if (oldpercent != newpercent) {
            oldpercent = newpercent;
            printDebug(2, "Exposing " << newpercent << "%");
        }

        img->setPixel(x,y,
                color.getBMPR(0,1),
                color.getBMPG(0,1),
                color.getBMPB(0,1));

    }
};

#endif /*FILM_C_*/
