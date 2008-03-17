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


using namespace std;

class Eye {
	
};

class Viewport {
	
};

/** Creates samples across the viewport in world space */
class Sampler {
	
};

/** Collects samples across the viewport in world space
 * into buckets for each pixel 
 * WORLD COORDINATES ===> IMAGE COORDINATES */
class Film {
	
};

class Scene {
	
};

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
				
				DEBUG = 1;
				
			}  else if (!strcmp(argv[i], "-q")) {
				
				
				INFO = 0;
				
			} else if (!strcmp(argv[i], "-selftest")) {
				
				
				selftest();
				exit(0);
				
			} else {
				malformedArg = true;
			}
			
			if (malformedArg) {
				printDebug("Malformed input arg in parsing command \"" << argv[i] << "\"");
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
	printInfo("Raytraycer Initialized");
	
	
	
	
    return 0;
}
