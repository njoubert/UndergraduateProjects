#ifndef OBJPARSER_H_
#define OBJPARSER_H_

#include "../Mesh.h"

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
class OBJParser {
public:
    OBJParser();
	TriangleMesh* parseOBJ(string);
private:
    unsigned int vertexCount;
    unsigned int vertexNormalCount;
    unsigned int vertexTextureCount;

    bool parseLine(string, TriangleMesh *);
};

#endif /*OBJPARSER_H_*/
