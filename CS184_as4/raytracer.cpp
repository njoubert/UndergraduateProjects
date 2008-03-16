//============================================================================
// Name        : CS184_as4.cpp
// Author      : Niels Joubert
// Version     :
// Copyright   : 2008
// Description : Hello World in C, Ansi-style
//============================================================================

#include "Debug.h"
#include "Image.cpp"

using namespace std;

int main(int argc,char **argv)
{
    Image img(200, 320);
    img.saveAsBMP("Test");
    return 0;
}
