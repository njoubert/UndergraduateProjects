/*
 * Vertex.h
 *
 *  Created on: Feb 2, 2009
 *      Author: njoubert
 */

#ifndef VERTEX_H_
#define VERTEX_H_

#include "global.h"

class Vertex {
public:
    Vertex();
    Vertex(vec2 p);
    virtual ~Vertex();

    /* Returns the position vector of this Vertex. */
    vec2 getPos();
    /* Returns the position vector of this Vertex interpolated at t. */
    vec2 getPos(int t);

    /* Sets the original position of this vertex. */
    void setStartPos(vec2 p);
    /* Sets the modified position of this vertes. */
    void setEndPos(vec2 p);

private:
    vec2 _startPos;
    vec2 _endPos;
};

#endif /* VERTEX_H_ */
