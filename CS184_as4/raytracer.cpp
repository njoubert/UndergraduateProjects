//============================================================================
// Name        : CS184_as4.cpp
// Author      : Niels Joubert
// Version     :
// Copyright   : 2008
// Description : Hello World in C, Ansi-style
//============================================================================

#include "Debug.cpp"
#include "Image.cpp"

using namespace std;

void selftest() {
    Image img1(200, 320);
    img1.saveAsBMP("Test1.bmp");
    
    Image img2(100,100);
    img2.saveAsBMP("Test2.bmp");
}

int main(int argc,char **argv) {
	selftest();
    return 0;
}
