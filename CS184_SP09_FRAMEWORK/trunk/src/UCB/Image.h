/*
 * Image.h
 *
 *  Created on: Nov 10, 2008
 *      Author: njoubert
 */

#ifndef UCBIMAGE_H_
#define UCBIMAGE_H_

#include <string>
#include <fstream>
#include <iostream>
#include <stdlib.h>

#include "global.h"

namespace UCB {

/**
 * This class represents the basic pixels of an image.
 * Pixels are addressed in the range [0, 0] to [w-1, h-1] inclusive,
 * for bottom left to top right.
 *
 */
class Image {
public:
    struct pixel {
         double b;
         double g;
         double r;
         int sampleCount;
    };
    int w;
    int h;
    pixel* pixels; //This will be a linear array of pixels, row by row from top to bottom

    Image(int w, int h);
    virtual ~Image();

    /**
     * Return the place in the array where the specific x,y pixel lives
     */
    inline int absolutePosition(int x, int y) {
        if (x < 0 || x > (w) || y < 0 || y > (h)) {
            UCBPrint("Image", "Tried to get a pixel outside of valid range!");
            exit(1);
        }
        return x + (y * w);
    }
    void setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);

    bool getPixel(int x, int y, unsigned char* r, unsigned char* g, unsigned char* b);

    void bakeValues(int x, int y);

    /* This routine is an adaptation of the saveBMP routine written by
     ** SuperJer
     ** Email = superjer@superjer.com
     ** Web   = http://www.superjer.com/
     */
    void saveAsBMP(std::string filename);

};

}
#endif /* UCBIMAGE_H_ */
