/*
 * OBJParser.h
 *
 *  Created on: Feb 4, 2009
 *      Author: njoubert
 */

#ifndef OBJPARSER_H_
#define OBJPARSER_H_

#include "global.h"
#include "Polygon.h"

/**
 * Responsible for parsing OBJ files.
 * Instantiate a parser for each file you want to parse.
 */
class OBJParser {
public:
    OBJParser();
    Polygon * parseOBJ(string filename);
private:
    vector<Vertex * > _vertices;
    bool _parseLine(string, Polygon *);

};

#endif /* OBJPARSER_H_ */
