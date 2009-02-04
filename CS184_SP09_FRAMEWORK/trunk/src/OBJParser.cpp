/*
 * OBJParser.cpp
 *
 *  Created on: Feb 4, 2009
 *      Author: njoubert
 */

#include "OBJParser.h"

OBJParser::OBJParser(): _vertices() {
    //empty
}

/*
 * Parses an OBJ file containing one face. Returns NULL if unsuccessfull.
 */
Polygon * OBJParser::parseOBJ(string filename) {
    char line[4096];

    std::cout << "Parsing OBJ file " << filename << std::endl;

    Polygon * poly = new Polygon();
    ifstream inFile(filename.c_str(), ifstream::in);
    if (!inFile) {
        std::cout << "Could not open given obj file " << filename << std::endl;
        delete poly;
        return NULL;
    }
    while (inFile.good()) {
        inFile.getline(line, 1023);
        if (!_parseLine(string(line), poly)) {
            std::cout << "Failed to parse OBJ file." << std::endl;
            delete poly;
            return NULL;
        }
    }
    inFile.close();
    std::cout << "Parsed an OBJ file with " << _vertices.size() << " vertices." << endl;
    return poly;
}

bool OBJParser::_parseLine(string line, Polygon * poly) {
    string operand;
    bool success = true;

    if (line.empty()) {
        std::cout << "Done parsing!" << std::endl;
        return true;
    }
    stringstream ss(stringstream::in | stringstream::out);
    ss.str(line);
    ss >> operand;
    //printDebug(4, " Operand: " << operand);
    if (operand[0] == '#') {
        return true;
    } else if (operand.compare("v") == 0) {

        double x, y;
        ss >>x >>y;
        _vertices.push_back(new Vertex(x,y));

    }
    else if (operand.compare("f") == 0) {

        while (!ss.eof()) {
            int i;
            ss >> i;
            poly->addVertex(_vertices[i-1]);
        }

    } else {
        cout << "Unknown operand in scene file, skipping line: " << operand << endl;
    }

    if (ss.fail()) {
        std::cout << "The bad bit of the input file's line's stringstream is set! Couldn't parse:" << std::endl;
        std::cout << "  " << line << std::endl;
    }
    return success;
}
