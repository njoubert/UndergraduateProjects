/*
 * Image.cpp
 *
 *  Created on: Nov 10, 2008
 *      Author: njoubert
 */

#include "Image.h"

namespace UCB {

Image::Image(int w, int h) {
    this->w = w;
    this->h = h;
    pixels = new pixel[w * h];
    for (int i = 0; i < w * h; i++) {
        pixels[i].r = 0;
        pixels[i].g = 0;
        pixels[i].b = 0;
        pixels[i].sampleCount = 0;
    }
}

Image::~Image() {
    delete[] pixels;
}

void Image::setPixel(int x, int y, unsigned char r, unsigned char g,
        unsigned char b) {
    int pos = absolutePosition(x, y);
    pixels[pos].r += r;
    pixels[pos].g += g;
    pixels[pos].b += b;
    pixels[pos].sampleCount += 1;
}

bool Image::getPixel(int x, int y, unsigned char* r, unsigned char* g,
        unsigned char* b) {
    int pos = absolutePosition(x, y);
    *r = (unsigned char) pixels[pos].r;
    *g = (unsigned char) pixels[pos].g;
    *b = (unsigned char) pixels[pos].b;
    return true;
}

void Image::bakeValues(int x, int y) {
    int pos = absolutePosition(x, y);
    pixels[pos].r /= pixels[pos].sampleCount;
    pixels[pos].g /= pixels[pos].sampleCount;
    pixels[pos].b /= pixels[pos].sampleCount;
    pixels[pos].sampleCount = 1;
}

void Image::saveAsBMP(std::string filename) {
    std::fstream fp_out;

    int filesize = 54 + 3 * w * h;// + h*((4-(w*3)%4)%4); //Account for padding
    unsigned char bmpfileheader[14] = { 'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54,
            0, 0, 0 };
    unsigned char bmpinfoheader[40] = { 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            0, 24, 0 };
    char bmppad[3] = { 0, 0, 0 };

    bmpfileheader[2] = (unsigned char) (filesize);
    bmpfileheader[3] = (unsigned char) (filesize >> 8);
    bmpfileheader[4] = (unsigned char) (filesize >> 16);
    bmpfileheader[5] = (unsigned char) (filesize >> 24);

    bmpinfoheader[4] = (unsigned char) (w);
    bmpinfoheader[5] = (unsigned char) (w >> 8);
    bmpinfoheader[6] = (unsigned char) (w >> 16);
    bmpinfoheader[7] = (unsigned char) (w >> 24);
    bmpinfoheader[8] = (unsigned char) (h);
    bmpinfoheader[9] = (unsigned char) (h >> 8);
    bmpinfoheader[10] = (unsigned char) (h >> 16);
    bmpinfoheader[11] = (unsigned char) (h >> 24);

    fp_out.open(filename.c_str(), std::ios::out | std::ios::binary);
    fp_out.write((char*) &bmpfileheader, 14);
    fp_out.write((char*) &bmpinfoheader, 40);
    for (int row = 0; row < h; row++) {
        for (int col = 0; col < w; col++) {
            bakeValues(col, row);
            char r = (char) pixels[absolutePosition(col, row)].r;
            char g = (char) pixels[absolutePosition(col, row)].g;
            char b = (char) pixels[absolutePosition(col, row)].b;
            fp_out.write(&b, 1);
            fp_out.write(&g, 1);
            fp_out.write(&r, 1);
        }
        fp_out.write(bmppad, (4 - (w * 3) % 4) % 4); //pad it like hellz!
    }
    fp_out.close();
}

}
