//============================================================================
// Name        : CS184_as4.cpp
// Author      : Niels Joubert
// Version     :
// Copyright   : 2008
// Description : Hello World in C, Ansi-style
//============================================================================

#include "Debug.cpp"
#include "Image.cpp"
#include "Algebra.cpp"
#include "assert.h"

#define isThereMore(CURR, MAX, WANT)	((MAX - CURR) > WANT)

using namespace std;

class Viewport;
class Sampler;
class Film;
class Scene;

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
		
	}
};

class Viewport {
public:
	Vector3d LL; //lower left world coordinates
	Vector3d LR; //lower right world coordinates
	Vector3d UL; //lower left world coordinates
	Vector3d UR; //lower right world coordinates
	Viewport() {
		
	};
	void setBoundaries(float llx, float lly, float llz, float lrx, float lry, float lrz, float urx, float ury, float urz, float ulx, float uly, float ulz) {
		LL.setX(llx); LL.setY(lly); LL.setZ(llz);
		LR.setX(lrx); LR.setY(lry); LR.setZ(lrz);
		UL.setX(ulx); UL.setY(uly); UL.setZ(ulz);
		UR.setX(urx); UR.setY(ury); UR.setZ(urz);
	}
};

/** Creates samples across the viewport in world space */
class Sampler {
public:
	Sampler(Viewport & viewport, Film & f):view(viewport), film(f) {
		incrementu = 1 / (double) film.width;
		incrementv = 1 / (double) film.height;
		v = incrementv/2;
		u = -incrementu/2; //hack to start off correctly
	}
	
	bool getPoint(Vector3d & p) {
		u += incrementu;
		if (u > 1) {
			u = incrementu / 2;
			v += incrementv;
		}
		if (v > 1)
			return false;
		p.setX((1-u)*((1-v)*view.LL.x + v*view.UL.x) + u*((1-v)*view.LR.x + v*view.UR.x));
		p.setY((1-u)*((1-v)*view.LL.y + v*view.UL.y) + u*((1-v)*view.LR.y + v*view.UR.y));
		p.setZ((1-u)*((1-v)*view.LL.z + v*view.UL.z) + u*((1-v)*view.LR.z + v*view.UR.z));
		return true;
	}
	
private:
	Viewport & view;
	Film & film;
	double u, v;
	double incrementu;
	double incrementv;
};

class Scene {
	
};


//========================================
// Rendering and Raytracing
//========================================

Film film;
Vector3d eye;
Viewport viewport;
Scene scene;

Color raytrace(Ray ray) {
	Color c(1, 0, 0);
	return c;
}
	
void render() {
	Vector3d point;
	Sampler sampler(viewport, film);
	while (sampler.getPoint(point)) {
		point.debugMe();
		film.expose(raytrace(Ray::getRay(eye, point)), point, viewport); //Functional == Beautiful
	}
}

//========================================
// Setup and Teardown Follows
//========================================

void selftest() {
	int ret=0;
	printInfo("Selftest Started!");
    ret += Image::selfTest();
    
    
    if (ret == 0) {
    	printInfo("Selftest Completed!");
    } else
    	printInfo("Selftest Failed!");
}
int parseCommandLine(int argc, char *argv[]) {
	
	bool malformedArg;
	bool printUsage = false;
	int i;
	for (i = 1; i < argc; i++) {
		
			malformedArg = false;
		
			if (strcmp(argv[i],"-d") == 0) {
				
				if (isThereMore(i, argc, 1)) {
					if (atoi(argv[i + 1]) < 6 && atoi(argv[i + 1]) > 0)
						DEBUG = atoi(argv[++i]);
				} else {
					DEBUG = 1;
				}
				
			}  else if (!strcmp(argv[i], "-q")) {
				
				
				INFO = 0;
				
			} else if (!strcmp(argv[i], "-selftest")) {
				
				
				selftest();
				exit(0);
				
			} else {
				malformedArg = true;
			}
			
			if (malformedArg) {
				printDebug(0, "Malformed input arg in parsing command \"" << argv[i] << "\"");
				printUsage = true;
			}
	}
	if (printUsage)
		return 1;
	return 0;
	
}
void printUsage() {
	cout << "Usage: "<< endl;
	cout << "  raytracer [-d] [-q] [-selftest] \\" << endl;
}

int main(int argc,char **argv) {
	
	if (parseCommandLine(argc, argv)) {
		printUsage();
		exit(1);
	}
	printInfo("Raytracer Initialized");
	
	//Testing for Now
	film.width = 5;
	film.height = 5;
	eye.setX(0.0f);
	eye.setY(0.0f);
	eye.setZ(0.0f);
	viewport.setBoundaries(-1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f);
	
	render();
	
	printInfo("Raytraycer Done");
    return 0;
}
