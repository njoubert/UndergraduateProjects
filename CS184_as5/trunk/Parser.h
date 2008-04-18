#ifndef PARSER_H_
#define PARSER_H_

#include "as5.h"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

using namespace std;

class Parser {
public:
	bool parseOBJ(string, vector<Triangle*> *);		
private:
    vector<Vector3d*> vertexBuffer;
    vector<Vector3d*> vertexNormalBuffer;
    bool parseLine(string, vector<Triangle*> *);
};

#endif /*PARSER_H_*/
