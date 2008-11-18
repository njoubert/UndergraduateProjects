#ifndef OFFPARSER_H_
#define OFFPARSER_H_

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
class OFFParser {
public:
    OFFParser();
	TriangleMesh* parse(string);
private:
    int vertexCount;
};

#endif /*OFFPARSER_H_*/
