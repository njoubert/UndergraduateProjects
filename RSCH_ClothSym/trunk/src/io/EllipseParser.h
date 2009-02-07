/*
 * EllipseParser.h
 *
 *  Created on: Oct 27, 2008
 *      Author: silversatin
 */

#ifndef ELLIPSEPARSER_H_
#define ELLIPSEPARSER_H_

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
//*
class ellipseParser {
public:
	ellipseParser();
	std::pair<  vector < vector <mat4> > , vector < vector < std::pair < int, int > > > >
		parseEllipsoids(string, int, int);
private:
	std::pair< std::pair< bool, mat4 >, std::pair< bool, int > > parseLine(string);
};
//*/


#endif /* ELLIPSEPARSER_H_ */
