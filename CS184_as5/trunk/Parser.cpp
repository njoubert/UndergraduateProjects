#ifndef PARSER_C_
#define PARSER_C_

/**
 * The aim of this file is to parse input files into Scene objects.
 * Three main categories of objects appear
 * in a scene:
 * -- Camera (Eye and Viewport)
 * -- Lights
 * -- Primitives
 * 
 * Here is the description for my .scene files:
 * 
 * - all inputs are interpreted as floating point.
 * - blank lines are ignored.
 * - lines starting with # are ignored
 * - no in-line comments allowed
 * 
 * All files start with an eye, viewport and boundingbox operative.
 * 
 * File structure:
 * [operative] [operands]\n
 * 
 * operative        operands
 * 
 *  #These are subsets of the actual .obj file implementation. This allows you to draw triangles:
 *  v               x y z           
 *  f               v1 v2 v3
 * 
 */
#include "Parser.h"


/**
 * Attempts to parse an OBJ file. Returns false if we fail.
 */
bool Parser::parseOBJ(string filename, vector<Triangle*> *output) {
    printInfo("Parsing Scene File " << filename);
    char line[1024];
    
    ifstream inFile(filename.c_str(), ifstream::in);
    if (!inFile) {
        printError("Could not open given scene file " << filename);
        return false;
    }
    while (inFile.good()) {
        inFile.getline(line, 1023);
        if (!parseLine(string(line), output))
            return false;
    }
    
    inFile.close();

    return true;
}

    
bool Parser::parseLine(string line, vector<Triangle*> *output) {
    string operand;
    bool success = true;
    
    printDebug(4, "Parsing Line: " << line);
    if (line.empty())
        return true;
    stringstream ss(stringstream::in | stringstream::out);
    ss.str(line);
    ss >> operand;
    printDebug(4, " Operand: " << operand);
    if (operand[0] == '#') {
        return true;    
    } else if (operand.compare("v") == 0) {

        double x, y, z;
        ss >>x >>y >>z;
        vertexBuffer.push_back(new Vector3d(x,y,z));
        printDebug(3, "Parsed Vertex input to ("<<x<<","<<y<<","<<z<<") into vertice buffer.");

    } else if (operand.compare("f") == 0) {

        int v1, v2, v3;
        ss >>v1 >>v2 >>v3;
        Vector3d* ver1 = vertexBuffer[v1-1];
        Vector3d* ver2 = vertexBuffer[v2-1];
        Vector3d* ver3 = vertexBuffer[v3-1];
        
        //success = scene->addTriangle(ver1, ver2, ver3, ks, ka, kd, kr, ksp);
        printDebug(3, "Parsed Face input from vertices "<<v1<<", "<<v2<<" and "<<v3);
    
       
    } else {
        printError("Unknown operand in scene file, skipping line: " << operand);
    }
    
    if (ss.fail()) {
        printError("The bad bit of the input file's line's stringstream is set! Couldn't parse!")
        exit(1);
    }     
    return success;
}

#endif /*PARSER_C_*/
