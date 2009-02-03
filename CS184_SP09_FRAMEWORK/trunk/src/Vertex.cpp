/*
 * Vertex.cpp
 *
 *  Created on: Feb 2, 2009
 *      Author: njoubert
 */

#include "Vertex.h"

Vertex::Vertex(): _startPos(0.0,0.0), _endPos(0.0,0.0) {
    // TODO Auto-generated constructor stub

}

Vertex::Vertex(vec2 p): _startPos(p), _endPos(0.0,0.0) {
    // TODO Auto-generated constructor stub

}

Vertex::~Vertex() {
    // TODO Auto-generated destructor stub

    //YOUR CODE HERE IF NECESSARY
}


vec2 Vertex::getPos() {
    return _startPos;
}

vec2 Vertex::getPos(int t) {

    //YOUR CODE HERE
    IMPLEMENT_ME(__FILE__,__LINE__);

    return vec2(0.0,0.0);
}

void Vertex::setStartPos(vec2 p) {
    _startPos = p;
}

void Vertex::setEndPos(vec2 p) {
    _endPos = p;
}
