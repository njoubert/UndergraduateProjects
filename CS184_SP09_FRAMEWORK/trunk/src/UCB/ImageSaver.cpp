/*
 * ImageSaver.cpp
 *
 *  Created on: Oct 23, 2008
 *      Author: njoubert
 */

#include "ImageSaver.h"

namespace UCB {

    ImageSaver::ImageSaver(std::string directory, std::string prefix) : _frameCount(0) {
        UCBPrint("ImageSaver", "Saving frames to directory " << directory);
        _imgOutDir = directory;
        if (_imgOutDir[_imgOutDir.size()-1] != '/')
            _imgOutDir.append("/");
        _prefix = prefix;
    }

    ImageSaver::~ImageSaver() {
        //intentionally left blank
        ;
    }

    void ImageSaver::saveFrame(int w, int h) {
        _frameCount++;
        stringstream filename(stringstream::in | stringstream::out);
        filename << _imgOutDir << _prefix;
        filename << std::setfill('0') << setw(6) << _frameCount << ".bmp";
        cout << "Save frame " << _frameCount << endl;

        Image* img = new Image(w, h);

        /******************************
         * Here we draw!
         ******************************/
        unsigned char *image;

        /* Allocate our buffer for the image */
        if ((image = (unsigned char*)malloc(3*w*h*sizeof(char))) == NULL) {
            cout << "Couldn't allocate memory!" << endl;
            delete img;
            return;
        }
        glPixelStorei(GL_PACK_ALIGNMENT,1);
        glReadBuffer(GL_BACK_LEFT);
        glReadPixels(0,0,w,h,GL_RGB,GL_UNSIGNED_BYTE,image);

        unsigned char r, g, b;
        for (int j=h-1;j>=0;j--) {
           for (int i=0;i<w;i++) {
              r = image[3*j*w+3*i+0];
              g = image[3*j*w+3*i+1];
              b = image[3*j*w+3*i+2];
              img->setPixel(i, j, r, g, b);
           }
        }
        img->saveAsBMP(filename.str());
        delete image;
        delete img;

    }

}
