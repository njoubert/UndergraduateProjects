#ifndef PARSER_H_
#define PARSER_H_

#include "Mesh.h"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

using namespace std;

/**
 * Parser that reads in files and creates Mesh objects.
 */
class Parser {
public:
	TriangleMesh* parseOBJ(string);
private:
    bool parseLine(string, TriangleMesh *);
};

#endif /*PARSER_H_*/
