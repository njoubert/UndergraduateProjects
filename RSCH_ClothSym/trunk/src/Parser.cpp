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

    return myMesh;
}


bool Parser::parseLine(string line, TriangleMesh *myMesh) {
    string operand;
    bool success = true;

    std::cout << "Parsing Line: " << line << endl;
    if (line.empty())
        return true;
    stringstream ss(stringstream::in | stringstream::out);
    ss.str(line);
    ss >> operand;
    //printDebug(4, " Operand: " << operand);
    if (operand[0] == '#') {
        return true;
    } else if (operand.compare("v") == 0) {

        double x, y, z;
        ss >>x >>y >>z;
        myMesh->createVertex(x,y,z);
        //printDebug(3, "Parsed Vertex input to ("<<x<<","<<y<<","<<z<<") into vertice buffer.");


    }
    /*else if (operand.compare("vn") == 0) {

        double x, y, z;
        ss >>x >>y >>z;
        vertexNormalBuffer.push_back(new Vector3d(x,y,z));
        printDebug(3, "Parsed Vertex Normal input to ("<<x<<","<<y<<","<<z<<") into vertice normals buffer.");

    }
    */
    else if (operand.compare("f") == 0) {

    	if (line.find("//") == string::npos) {

	        int v1, v2, v3;
	        ss >>v1 >>v2 >>v3;
	        myMesh->createTriangle(v1, v2, v3);

	        //printDebug(3, "Parsed Face input from vertices "<<v1<<", "<<v2<<" and "<<v3);

    	} else {
    		/*
    		int v1, v2, v3;
    		int n1, n2, n3;
    		ss >>v1;
    		ss.get();
    		ss.get();
    		ss >>n1;

    		ss >>v2;
    		ss.get();
    		ss.get();
    		ss >>n2;

    		ss >>v3;
    		ss.get();
    		ss.get();
			ss >>n3;

  	        Vector3d* ver1 = vertexBuffer[v1-1];
	        Vector3d* ver2 = vertexBuffer[v2-1];
	        Vector3d* ver3 = vertexBuffer[v3-1];
	       	Vector3d* nor1 = vertexNormalBuffer[n1-1];
	        Vector3d* nor2 = vertexNormalBuffer[n2-1];
	        Vector3d* nor3 = vertexNormalBuffer[n3-1];
	        Triangle* tr = new Triangle();
	        tr->v1 = *ver1;
	        tr->v2 = *ver2;
	        tr->v3 = *ver3;
	        tr->n1 = *nor1;
	        tr->n2 = *nor2;
	        tr->n3 = *nor3;

    		printDebug(3, "Parsed Face input from vertices "<<v1<<", "<<v2<<" and "<<v3<<" with normals "<<n1<<", "<<n2<<", "<<n3);

    		output->push_back(tr);
    	    */
    	}


    } else {
        //printError("Unknown operand in scene file, skipping line: " << operand);
    }

    if (ss.fail()) {
        std::cout << "The bad bit of the input file's line's stringstream is set! Couldn't parse!" << endl;
        exit(1);
    }
    return success;
}

#endif /*PARSER_C_*/
