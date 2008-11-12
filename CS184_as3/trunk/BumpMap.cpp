
#include <cmath>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

class BumpMap;

class BumpMap {
private:
	int width;
	int height;
	float map[11][11];
public:
	BumpMap() {

	};
	~BumpMap() {
		
	};
	
	void loadTextFile() {
		filebuf fb;
		fb.open("bmap1.txt",ios::in);
		istream is(&fb);
		is >> width;
		is >> height;
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				is >> map[i][j];
			}
		}
				
	}
	
	float getHorizontalGradient(int x, int y) {
		int i = abs(x)%width;
		int j = abs(y)%height;
		float gradient = map[j][(width+i-1)%width] -map[j][(width+i+1)%width]; 
		return gradient;
	}
	
	float getVerticalGradient(int x, int y) {
		int i = x%width;
		int j = y%height;
		float gradient = map[(height+j-1)%height][i] -map[(height+j+1)%height][i]; 
		return gradient;
	}
};

