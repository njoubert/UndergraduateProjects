#ifndef IMAGE_H_
#define IMAGE_H_

#include <string>
#include <fstream>
#include "Debug.h"

using namespace std;

class Image {
public:
	typedef struct pixel {
		char r;
		char g;
		char b;
	} pixel;
	int w;
	int h;
	pixel* pixels; //This will be a linear array of pixels, row by row from top to bottom

	Image(int w, int h) {
		this->w = w;
		this->h = h;
		pixels = new pixel[w*h];
	}
	virtual ~Image() {
		delete [] pixels;
	}
	inline int absolutePosition(int x, int y) {
		return 1;
	}
	void setPixel(int x, int y, char r, char g, char b) {
		int pos = absolutePosition(x, y);
		pixels[pos].r = r;
		pixels[pos].g = g;
		pixels[pos].b = b;
	}

	/* This routine is an adaptation of the saveBMP routine written by
	 ** SuperJer
	 ** Email = superjer@superjer.com
	 ** Web   = http://www.superjer.com/
	 */
	void saveAsBMP(string filename) {
		fstream fp_out;
		
		int filesize = 54 + 3*w*h;
		//short type, int size, short res1, short res2, int off
		unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0}; 
		//unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 0,0,0,54}; 
		unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
		//unsigned char bmpinfoheader[40] = {0,0,0,40, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
		char bmppad[3] = {0,0,0};

		bmpfileheader[ 2] = (unsigned char)(filesize    );
		bmpfileheader[ 3] = (unsigned char)(filesize>> 8);
		bmpfileheader[ 4] = (unsigned char)(filesize>>16);
		bmpfileheader[ 5] = (unsigned char)(filesize>>24);

		bmpinfoheader[ 4] = (unsigned char)(       w    );
		bmpinfoheader[ 5] = (unsigned char)(       w>> 8);
		bmpinfoheader[ 6] = (unsigned char)(       w>>16);
		bmpinfoheader[ 7] = (unsigned char)(       w>>24);
		bmpinfoheader[ 8] = (unsigned char)(       h    );
		bmpinfoheader[ 9] = (unsigned char)(       h>> 8);
		bmpinfoheader[10] = (unsigned char)(       h>>16);
		bmpinfoheader[11] = (unsigned char)(       h>>24);

		fp_out.open("myfile.bmp", ios::out | ios::binary);
		fp_out.write((char*)&bmpfileheader, 14);
		fp_out.write((char*)&bmpinfoheader, 40);
		for (int row = 0; row < h; row++) {
			for (int col = 0; col < w; col++) {
				fp_out.write(&(pixels[row*col].r), 1);
				fp_out.write(&(pixels[row*col].g), 1);
				fp_out.write(&(pixels[row*col].b), 1);
			}
			fp_out.write(bmppad, (4-(w*3)%4)%4); //pads it like hellz!
		}
		fp_out.close();
	}

};

#endif /*IMAGE_H_*/
