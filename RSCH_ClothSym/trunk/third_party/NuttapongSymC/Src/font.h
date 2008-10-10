//==========================================================================
//
//	FILE:	Font.h
//	AUTHOR:	Mike Bonnington & Rob Bateman
//	DATE:	Somewhen last century
//	NOTE:
//
//
//	This file defines a small bitmap font for use in openGL
//
//	USAGE:
//
//	You should call FontInit() upon the initialisation of your application.
//
//	Call the function FontDraw() specifying the text to write, and the 2D
//	screen co-ordinates of where you will be drawing the font.
//
//	Use glColor3f() or glColor4f() to specify the colour of the font to be
//	drawn.
//
//	To cleanup any data allocated by the FontInit() function, call the
//	FontCleanup() function.
//
//==========================================================================

#ifdef WIN32
#pragma once
#endif

#ifndef SMALL_FONT_H
#define SMALL_FONT_H


// this is required for C compatibility with C++
#ifdef __cplusplus
extern "C" {
#endif



//==========================================================================
//
//	note:	This function intialises the font for use in OpenGL. You must call
//			this function before drawing any text...
//
//==========================================================================

extern void FontInit();


//==========================================================================
//
//	note:	call this function to delete all old display lists etc.
//
//==========================================================================

extern void FontCleanup();


//==========================================================================
//
//	note:	This function allows you to draw the text string in the
//			position (x,y).
//	param:	txt	-	the text to write
//	param:	x	-	x coordinate of text
//	param:	y	-	y coordinate of text
//
//==========================================================================

extern void FontDraw(char *txt,int x,int y);




// this is required for C compatibility with C++
#ifdef __cplusplus
};
#endif



#endif
