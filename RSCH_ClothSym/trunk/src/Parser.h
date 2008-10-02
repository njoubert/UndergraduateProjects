#ifndef PARSER_H_
#define PARSER_H_

#include "Mesh.h"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

//#define parseDebug(A) std::cout << __FILE__ << "::" << __LINE__ << "::" << __FUNCTION__ << ":: " << A << std::endl;
#define parseDebug(A) ;

using namespace std;

/**
 * Parser that reads in files and creates Mesh objects.
 */
class Parser {
public:
    Parser();
	TriangleMesh* parseOBJ(string);
private:
    int vertexCount;
    bool parseLine(string, TriangleMesh *);
};

#endif /*PARSER_H_*/
