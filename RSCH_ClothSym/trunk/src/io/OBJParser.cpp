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
#include "OBJParser.h"

OBJParser::OBJParser() {
    vertexCount = 0;
}

/**
 * Attempts to parse an OBJ file. Returns false if we fail.
 */
TriangleMesh* OBJParser::parseOBJ(string filename) {
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

    myMesh->setGlobalMassPerParticle(1.0 / vertexCount);

    inFile.close();
    parseDebug("Parser exiting...");
    return myMesh;
}


bool OBJParser::parseLine(string line, TriangleMesh *myMesh) {
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
        vertexCount++;
        //**
        ss >>ws;
        if (!ss.eof()) {
            string args;
            ss >> args;
            //cout << "reading MORE! " << args;
            if (args.compare("fixed") == 0) {
                parseDebug("Constraining particle " << i);
                myMesh->getVertex(i)->setConstraint(true);
            }
        }
           // */
        //printDebug(3, "Parsed Vertex input to ("<<x<<","<<y<<","<<z<<") into vertice buffer.");


    }
    else if (operand.compare("vn") == 0) {

        //double x, y, z;
        //ss >>x >>y >>z;
        //vertexNormalBuffer.push_back(new Vector3d(x,y,z));
        //printDebug(3, "Parsed Vertex Normal input to ("<<x<<","<<y<<","<<z<<") into vertice normals buffer.");

    }
    else if (operand.compare("vt") == 0) {

        //double x, y, z;
        //ss >>x >>y >>z;
        //vertexNormalBuffer.push_back(new Vector3d(x,y,z));
        //printDebug(3, "Parsed Vertex Normal input to ("<<x<<","<<y<<","<<z<<") into vertice normals buffer.");

    }
    else if (operand.compare("m") == 0) {

    }
    else if (operand.compare("f") == 0) {

    	if (line.find("//") != string::npos) {

            int v1, v2, v3;
            int n1 = -1, n2 = -1, n3 = -1;
            ss >>v1;
            assert(ss.peek() == '/');
            ss.get();
            assert(ss.peek() == '/');
            ss.get();
            //if next is not space
                ss >>n1;

            ss >>v2;
            assert(ss.peek() == '/');
            ss.get();
            assert(ss.peek() == '/');
            ss.get();
            //if next is not space
                    ss >>n2;

            ss >>v3;
            assert(ss.peek() == '/');
            ss.get();
            assert(ss.peek() == '/');
            ss.get();
            //if next is not space
                ss >>n3;

            myMesh->createTriangle(v1-1,v2-1,v3-1);

    	} else if (line.find("/") != string::npos) {

    	    int v1, v2, v3;
            int n1 = -1, n2 = -1, n3 = -1;
            int t1 = -1, t2 = -1, t3 = -1;
            ss >>v1;
            assert(ss.peek() == '/');
            ss.get();
            //if next is not space
            ss >>n1;
            //if next is not space
            assert(ss.peek() == '/');
            ss.get();
            ss >>t1;

            ss >>v2;
            assert(ss.peek() == '/');
            ss.get();
            ss >>n2;
            assert(ss.peek() == '/');
            ss.get();
            ss >>t2;

            ss >>v3;
            assert(ss.peek() == '/');
            ss.get();
            //if next is not space
            ss >>n3;
            //if next is not space
            assert(ss.peek() == '/');
            ss.get();
            ss >>t3;


            if (v1 < 0)
                v1 = vertexCount + v1 + 1;
            if (v2 < 0)
                v2 = vertexCount + v2 + 1;
            if (v3 < 0)
                v3 = vertexCount + v3 + 1;
            //cout << "New triangle " << v1 << " " << v2 << " " << v3 << " out of " << vertexCount << endl;
            myMesh->createTriangle(v1-1,v2-1,v3-1);


    	} else {

            int v1, v2, v3;
            ss >>v1 >>v2 >>v3;
            myMesh->createTriangle(v1-1, v2-1, v3-1);

    	}


    } else {
        cout << "Unknown operand in scene file, skipping line: " << operand << endl;
    }

    if (ss.fail()) {
        std::cout << "The bad bit of the input file's line's stringstream is set! Couldn't parse:" << endl;
        std::cout << "  " << line << endl;
        exit(1);
    }
    return success;
}

#endif /*PARSER_C_*/
