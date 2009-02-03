/*
 * Polygon.cpp
 *
 *  Created on: Feb 2, 2009
 *      Author: njoubert
 */

#include "Polygon.h"

Polygon::Polygon() {
    // TODO Auto-generated constructor stub

    //YOUR CODE HERE IF NECESSARY

}

Polygon::~Polygon() {
    // TODO Auto-generated destructor stub
    // This is where you free any pointers this class is holding on to.

    //YOUR CODE HERE IF NECESSARY
}

void Polygon::draw() {
    if (1 > _vertices.size())
        return;
    glBegin(GL_LINE_STRIP);
    for (vector<Vertex *>::iterator it = _vertices.begin(); it != _vertices.end(); it++)
        glVertex2d((*it)->getPos()[0],(*it)->getPos()[1]);
    glEnd();
}

void Polygon::drawRubberBand(Vertex * v) {
    if (0 == _vertices.size())
        return;
    glBegin(GL_LINES);
        glVertex2d(_vertices.back()->getPos()[0],_vertices.back()->getPos()[1]);
        glVertex2d(v->getPos()[0],v->getPos()[1]);
    glEnd();
}

void Polygon::draw(int t) {

    //YOUR CODE HERE
    IMPLEMENT_ME(__FILE__,__LINE__);

}

void Polygon::addVertex(Vertex * v) {
    _vertices.push_back(v);
}


void Polygon::writeAsOBJ(string filename) {
    writeAsOBJ(filename,0);
}

void Polygon::writeAsOBJ(string filename, int t) {

    //YOUR CODE HERE
    IMPLEMENT_ME(__FILE__,__LINE__);

}
