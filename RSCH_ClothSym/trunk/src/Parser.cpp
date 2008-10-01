#ifndef PARSER_C_
#define PARSER_C_

/**
 * The aim of this file is to parse input files into OBJ files
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
 *
 */
#include "Parser.h"


/**
 * Attempts to parse an OBJ file. Returns false if we fail.
 */
TriangleMesh* Parser::parseOBJ(string filename) {
    //printInfo("Parsing Scene File " << filename);
    char line[1024];
    TriangleMesh* myMesh = new TriangleMesh();
    ifstream inFile(filename.c_str(), ifstream::in);
    if (!inFile) {
        std::cout << "Could not open given obj file " << filename << endl;
        delete myMesh;
        return NULL;
    }
    while (inFile.good()) {
        inFile.getline(line, 1023);
        if (!parseLine(string(line), myMesh)) {
            delete myMesh;
            return NULL;
        }
    }

    inFile.close();
    parseDebug("Parser exiting...");
    return myMesh;
}


bool Parser::parseLine(string line, TriangleMesh *myMesh) {
    string operand;
    bool success = true;

    parseDebug("Parsing Line: " << line);
    if (line.empty()) {
        parseDebug("Done parsing!");
        return true;
    }
    stringstream ss(stringstream::in | stringstream::out);
    ss.str(line);
    ss >> operand;
    //printDebug(4, " Operand: " << operand);
    if (operand[0] == '#') {
        return true;
    } else if (operand.compare("v") == 0) {

        double x, y, z;
        ss >>x >>y >>z;
        int i = myMesh->createVertex(x,y,z);
        //**
        if (!ss.eof()) {
            string args;
            ss >> args;
            //cout << "reading MORE! " << args;
            if (args.compare("fixed") == 0) {
                parseDebug("Constraining particle " << i);
                myMesh->getVertex(i)->setConstraint(identity2D());
            }
        }
           // */
        //printDebug(3, "Parsed Vertex input to ("<<x<<","<<y<<","<<z<<") into vertice buffer.");


    }
    /*else if (operand.compare("vn") == 0) {

        double x, y, z;
        ss >>x >>y >>z;
        vertexNormalBuffer.push_back(new Vector3d(x,y,z));
        printDebug(3, "Parsed Vertex Normal input to ("<<x<<","<<y<<","<<z<<") into vertice normals buffer.");

    }
    */
    else if (operand.compare("c") == 0) {

    }
    else if (operand.compare("f") == 0) {

    	if (line.find("//") == string::npos) {

	        int v1, v2, v3;
	        ss >>v1 >>v2 >>v3;
	        myMesh->createTriangle(v1-1, v2-1, v3-1);

    	} else {

    		int v1, v2, v3;
    		int n1 = -1, n2 = -1, n3 = -1;
    		ss >>v1;
    		ss.get();
    		ss.get();
    		//if next is not space
                ss >>n1;

    		ss >>v2;
    		ss.get();
    		ss.get();
    		//if next is not space
                    ss >>n2;

    		ss >>v3;
    		ss.get();
    		ss.get();
    		//if next is not space
                ss >>n3;

  	        myMesh->createTriangle(v1-1,v2-1,v3-1);

    	}


    } else {
        cout << "Unknown operand in scene file, skipping line: " << operand << endl;
    }

    if (ss.fail()) {
        //std::cout << "The bad bit of the input file's line's stringstream is set! Couldn't parse!" << endl;
        //exit(1);
    }
    return success;
}

#endif /*PARSER_C_*/
