/*
 * BMPWriter.cc
 *
 *  Created on: Feb 21, 2010
 *      Author: njoubert
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "BMPWriter.h"

typedef struct pixel {
	unsigned char b;
	unsigned char g;
	unsigned char r;
} pixel;

struct Image {
	int w;
	int h;
	pixel* pixels;
};

void saveImage(Image img, const char* filename) {
	FILE *fp = fopen(filename, "w");
	if (fp == NULL) {
	  fprintf(stderr, "Can't open file.\n");
	  exit(1);
	}

	int filesize = 54 + 3*img.w*img.h;
	unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0,0,0, 54,0,0,0};
	unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
	char bmppad[3] = {0,0,0};

	bmpfileheader[ 2] = (unsigned char)(filesize    );
	bmpfileheader[ 3] = (unsigned char)(filesize>> 8);
	bmpfileheader[ 4] = (unsigned char)(filesize>>16);
	bmpfileheader[ 5] = (unsigned char)(filesize>>24);

	bmpinfoheader[ 4] = (unsigned char)(       img.w    );
	bmpinfoheader[ 5] = (unsigned char)(       img.w>> 8);
	bmpinfoheader[ 6] = (unsigned char)(       img.w>>16);
	bmpinfoheader[ 7] = (unsigned char)(       img.w>>24);
	bmpinfoheader[ 8] = (unsigned char)(       img.h    );
	bmpinfoheader[ 9] = (unsigned char)(       img.h>> 8);
	bmpinfoheader[10] = (unsigned char)(       img.h>>16);
	bmpinfoheader[11] = (unsigned char)(       img.h>>24);

	fwrite(&bmpfileheader, sizeof(char), 14, fp);
	fwrite(&bmpinfoheader, sizeof(char), 40, fp);
	int pos = 0;
	unsigned char r, g, b;
	for (int row = 0; row < img.h; row++) {
		for (int col = 0; col < img.w; col++) {
			r = (unsigned char) (img.pixels[pos].r);
			g = (unsigned char) (img.pixels[pos].g);
			b = (unsigned char) (img.pixels[pos].b);
			fwrite(&b, sizeof(unsigned char), 1, fp);
			fwrite(&g, sizeof(unsigned char), 1, fp);
			fwrite(&r, sizeof(unsigned char), 1, fp);
			pos++;
		}
		fwrite(&bmppad, sizeof(char), (4-(img.w*3)%4)%4, fp);
	}
	fclose(fp);

}

void write_bmp(const char *fileName, float *real_image, float *imag_image, int size_x, int size_y) {
	Image img;
	img.w = size_x;
	img.h = size_y;
	img.pixels = (pixel*) malloc(sizeof(pixel) * img.w * img.h);
	for(unsigned int y=0; y<size_x; y++) { //rows
		for(unsigned int x=0; x<size_y; x++) { //cols
			double mag = sqrt(pow(real_image[y*size_x+x],2.0) + pow(imag_image[y*size_x+x],2.0));
			//imagePtr->image_buffer[i*size_x+j] =
			unsigned char val = (unsigned char)(mag);
			img.pixels[x + (img.h-1-y)*img.h].r = val;
			img.pixels[x + (img.h-1-y)*img.h].g = val;
			img.pixels[x + (img.h-1-y)*img.h].b = val;
		}
	}
	saveImage(img, fileName);

	free(img.pixels);
}
