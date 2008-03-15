//============================================================================
// Name        : CS184_as4.cpp
// Author      : Niels Joubert
// Version     :
// Copyright   : 2008
// Description : Hello World in C, Ansi-style
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <Magick++.h>
#include <iostream>
using namespace std;
using namespace Magick; 

int main(int argc,char **argv)
{
    Image image( "100x100", "white" );
    image.pixelColor( 49, 49, "red" );
    image.write( "red_pixel.png" );
    return 0;
}
