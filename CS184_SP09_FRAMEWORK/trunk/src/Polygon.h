/*
 * Polygon.h
 *
 *  Created on: Feb 2, 2009
 *      Author: njoubert
 */

#ifndef POLYGON_H_
#define POLYGON_H_

#include "global.h"
#include "Vertex.h"

class Polygon {
public:
    Polygon();
    virtual ~Polygon();

    /* Draws the given polygon using OpenGL. */
    void draw();
    /* Draws the given polygon, linearly interpolated at t in interval [0,1] */
    void draw(int t);

    /* Adds a vertex to the end of this polygon */
    void addVertex(Vertex * v);


    /* Writes out the polygon as an OBJ file. */
    void writeAsOBJ(string filename);
    /* Writes out the polygon interpolated at t in inverval [0,1] as an OBJ. */
    void writeAsOBJ(string filename, int t);

private:
    vector<Vertex *> _vertices;
};

#endif /* POLYGON_H_ */
