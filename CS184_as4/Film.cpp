#ifndef FILM_C_
#define FILM_C_

#include "Image.cpp"
#include "Algebra.cpp"
#include <assert.h>

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
	void expose(Color color, Vector3d & point, Viewport & vp) {
		//Transform between coordinates!!! 
		img->setPixel(   ,
				,
				color.getBMPR(0,1),
				color.getBMPG(0,1),
				color.getBMPB(0,1));
	}
};

#endif /*FILM_C_*/
