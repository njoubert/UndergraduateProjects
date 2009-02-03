/*
 * global.h
 *
 *  Created on: Jan 26, 2009
 *      Author: njoubert
 */

#ifndef UCBGLOBAL_H_
#define UCBGLOBAL_H_

#define UCBPrint(title, msg) std::cout << "UCB::" << title << " - " << msg << std::endl;

#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#endif /* UCBGLOBAL_H_ */
