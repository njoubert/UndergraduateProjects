/*
 * ImageSaver.h
 *
 *  Created on: Oct 23, 2008
 *      Author: njoubert
 */

#ifndef IMAGESAVER_H_
#define IMAGESAVER_H_

#define BPP 24

#include "../global.h"
#include <sstream>

#ifdef FREEIMAGE
    #include <FreeImage.h>
#else
    #include "Image.h"
#endif


class ImageSaver {
public:
#ifdef FREEIMAGE
    ImageSaver() : frameCount(0), doImageOutput(false) { FreeImage_Initialise(); }
    ~ImageSaver() { FreeImage_DeInitialise(); }
#else
    ImageSaver() : frameCount(0), doImageOutput(false) { ; }
    ~ImageSaver() { ; }
#endif
    void initialize(string directory, double inverseFPS) {
        cout << "Saving frames to directory " << directory << endl;
        imgOutDir = directory;
        if (imgOutDir[imgOutDir.size()-1] != '/')
            imgOutDir.append("/");
        doImageOutput = true;
        lastTime = -1 - inverseFPS;
    }

#ifdef FREEIMAGE

    void saveFrame(double time, bool JustDoIt, double inverseFPS, int w, int h) {
        if (!doImageOutput)
            return;
        if (time >= lastTime + inverseFPS || JustDoIt)
            lastTime = time;
        else
            return;

        frameCount++;
        stringstream filename(stringstream::in | stringstream::out);
        filename << imgOutDir << "sym";
        filename << std::setfill('0') << setw(6) << frameCount << ".png";
        cout << "Save frame " << frameCount << " at "<< setprecision(3) << time <<"s ... \t";

        FIBITMAP* bitmap = FreeImage_Allocate(w, h, BPP);
        if (!bitmap) {
            cout << "Could not create bitmap! Can't save images!" << endl;
            return;
        }

        /******************************
         * Here we draw!
         ******************************/
        unsigned char *image;

        /* Allocate our buffer for the image */
        if ((image = (unsigned char*)malloc(3*w*h*sizeof(char))) == NULL) {
            cout << "Couldn't allocate memory!" << endl;
            free(bitmap);
            return;
        }
        glPixelStorei(GL_PACK_ALIGNMENT,1);
        glReadBuffer(GL_BACK_LEFT);
        glReadPixels(0,0,w,h,GL_RGB,GL_UNSIGNED_BYTE,image);

        RGBQUAD color;
        for (int j=h-1;j>=0;j--) {
           for (int i=0;i<w;i++) {
              color.rgbRed = image[3*j*w+3*i+0];
              color.rgbGreen = image[3*j*w+3*i+1];
              color.rgbBlue = image[3*j*w+3*i+2];
              FreeImage_SetPixelColor(bitmap,i,j,&color);
           }
        }

        if (FreeImage_Save(FIF_PNG, bitmap, filename.str().c_str(), 0)) {
            cout << "succeeded" << endl;
        } else
            cout << "failed!!!" << endl;
        free(image);
        free(bitmap);

    }
#else

    void saveFrame(double time, bool JustDoIt, double inverseFPS, int w, int h) {
        if (!doImageOutput)
            return;
        if (time >= lastTime + inverseFPS || JustDoIt)
            lastTime = time;
        else
            return;

        frameCount++;
        stringstream filename(stringstream::in | stringstream::out);
        filename << imgOutDir << "sym";
        filename << std::setfill('0') << setw(6) << frameCount << ".bmp";
        cout << "Save frame " << frameCount << " at "<< setprecision(3) << time <<"s ..." << endl;

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


#endif

private:
    int frameCount;
    double lastTime;
    string imgOutDir;
    bool doImageOutput;
};

#endif /* IMAGESAVER_H_ */
