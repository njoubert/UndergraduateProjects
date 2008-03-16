#ifndef IMAGE_H_
#define IMAGE_H_

#include <string>
#include <fstream>
#include "Debug.cpp"

using namespace std;

/**
 * This class represents the basic pixels of an image.
 * Pixels are addressed in the range [0, 0] to [w-1, h-1] inclusive,
 * for bottom left to top right.
 * 
 */
class Image {
public:
	typedef struct pixel {
		char b;
		char g;
		char r;
	} pixel;
	int w;
	int h;
	pixel* pixels; //This will be a linear array of pixels, row by row from top to bottom

	Image(int w, int h) {
		this->w = w;
		this->h = h;
		pixels = new pixel[w*h];
		for (int i = 0; i < w*h; i++) {
			pixels[i].r = 0;
			pixels[i].g = 0;
			pixels[i].b = 0;
		}
	}
	
	virtual ~Image() {
		delete [] pixels;
	}
	/**
	 * Return the place in the array where the specific x,y pixel lives
	 */
	inline int absolutePosition(int x, int y) {
		if (x < 0 || x > (w) || y < 0 || y > (h)) {
			printDebug("Tried to get a pixel outside of valid range!")
		}
		return x + (y * w);
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
		
		int filesize = 54 + 3*w*h;// + h*((4-(w*3)%4)%4); //Account for padding
		unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};  
		unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
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

		fp_out.open(filename.c_str(), ios::out | ios::binary);
		fp_out.write((char*)&bmpfileheader, 14);
		fp_out.write((char*)&bmpinfoheader, 40);
		for (int row = 0; row < h; row++) {
			for (int col = 0; col < w; col++) {
				fp_out.write((char*)&(pixels[absolutePosition(col, row)]), 3);
			}
			fp_out.write(bmppad, (4-(w*3)%4)%4); //pads it like hellz!
		}
		fp_out.close();
	}
	
	static void selfTest() {
		Image img1(1, 1);
		    img1.setPixel(0,0,255,0,0);
		    img1.saveAsBMP("tests/Test1.bmp");
		    
		    Image img2(20, 32);
		    img2.setPixel(0,0,255,0,0);
		    img2.setPixel(19,0,255,0,0);
		    img2.setPixel(0,31,255,0,0);
		    img2.setPixel(19,31,255,0,0);
		    img2.saveAsBMP("tests/Test2.bmp");
		    
		    Image img3(100,100);
		    img3.setPixel(0,0,255,0,0);
		    img3.setPixel(1,0,255,0,0);
		    img3.setPixel(2,0,255,0,0);
		    img3.saveAsBMP("tests/Test3.bmp");
		    
		    Image img4(255, 255);
		    for (int x = 0; x < 255; x++) {
		    	for (int y = 0; y < 255; y++) {
		    		img4.setPixel(x, y, x, y, 0);
		    	}
		    }
		    img4.saveAsBMP("tests/Test4-redgreen.bmp");
		    Image img5(255, 255);
		    for (int x = 0; x < 255; x++) {
		    	for (int y = 0; y < 255; y++) {
		    		img5.setPixel(x, y, 0, y, x);
		    	}
		    }
		    img5.saveAsBMP("tests/Test5-bluegreen.bmp");
		    Image img6(255, 255);
		    for (int x = 0; x < 255; x++) {
		    	for (int y = 0; y < 255; y++) {
		    		img6.setPixel(x, y, x, 0, y);
		    	}
		    }
		    img6.saveAsBMP("tests/Test5-bluered.bmp");
		    Image img7(255, 255);
		    for (int x = 0; x < 255; x++) {
		    	for (int y = 0; y < 255; y++) {
		    		img7.setPixel(x, y, (255 - (x+y)/2), x, y);
		    	}
		    }
		    img7.saveAsBMP("tests/Test5-redgreenblue.bmp");
		    
		    int i = 255;
		    Image img8(1680, 1050);
		    for (int x = 0; x < 1680; x++) {
		    	for (int y = 0; y < 1050; y++) {
		    		i++;
		    		img8.setPixel(x, y, i,(i>>8),(i>>16));
		    	}
		    }
		    img8.saveAsBMP("tests/Test6-fullscreen.bmp");
	}

};

#endif /*IMAGE_H_*/
