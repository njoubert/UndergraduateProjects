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
	void expose(Color color, Vector3d & p, Viewport & vp) {
		//Transform between coordinates!!! 
		
		//This only works for a square or rectangle!
		
		//for space [l,r]x[b,t] for corners (l,b) to (u,r)
		//x = floor((u - l)/(r - l) * width);
		//y = floor((v - b)/(t - b) * height);
		
		//Be sure to round down!
		img->setPixel(,
				,
				color.getBMPR(0,1),
				color.getBMPG(0,1),
				color.getBMPB(0,1));
	}
};

#endif /*FILM_C_*/
