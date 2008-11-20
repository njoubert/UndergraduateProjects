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
    vertexNormalCount = 0;
    vertexTextureCount = 0;
}

/**
 * Attempts to parse an OBJ/OFF file. Returns false if we fail.
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

    assert(vertexCount == myMesh->countVertices());
    assert(vertexNormalCount == myMesh->countNormalVertices());
    assert(vertexTextureCount == myMesh->countTextureVertices());

    inFile.close();
    parseDebug("Parser exiting...");
    cout << "Parsed an OBJ mesh with " << myMesh->countVertices() << " vertices, "
		<< vertexNormalCount << " vertex normals, " << vertexTextureCount << " texture vertices, "
		<< myMesh->countTriangles() << " triangles." << endl;
    myMesh->exportOriginalAsOBJ("OBJPARSER_DEFEXPORT.obj");
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
                myMesh->getVertex(i)->setPinned(true);
                cout<<"Fixed Point Found in OBJ"<<endl;
            }
        }
           // */
        //printDebug(3, "Parsed Vertex input to ("<<x<<","<<y<<","<<z<<") into vertice buffer.");


    }
    else if (operand.compare("vn") == 0) {

        double x, y, z;
        ss >>x >>y >>z;
        myMesh->createVertexNormal(x,y,z);
        parseDebug("Parsed Vertex Normal input to ("<<x<<","<<y<<","<<z<<") into vertice normals buffer index.");
        vertexNormalCount++;
    }
    else if (operand.compare("vt") == 0) {

        double u, v;
        ss >>u >>v;
        myMesh->createVertexTexture(u, v);
        parseDebug("Parsed Vertex Texture input to ("<<u<<","<<v<<") into vertice normals buffer index.")
        vertexTextureCount++;

    }
    else if (operand.compare("m") == 0) {

    }
    else if (operand.compare("f") == 0) {

    	    int v1, v2, v3;
            int n1 = -1, n2 = -1, n3 = -1;
            int t1 = -1, t2 = -1, t3 = -1;
            bool hasNormals = false;
            bool hasTextures = false;

            ss >>v1;
            if (ss.peek() == '/') {
				ss.get();
				if (ss.peek() != '/') {
					hasTextures = true;
					ss >> t1;
				}
				if (ss.peek() == '/') {
					ss.get();
					hasNormals = true;
					ss >> n1;
				}
            }

            ss >>v2;
            if (ss.peek() == '/') {
				ss.get();
				if (ss.peek() != '/') {
					ss >> t2;
				}
				if (ss.peek() == '/') {
					ss.get();
					ss >> n2;
				}
            }

            ss >>v3;
            if (ss.peek() == '/') {
				ss.get();
				if (ss.peek() != '/') {
					ss >> t3;
				}
				if (ss.peek() == '/') {
					ss.get();
					ss >> n3;
				}
            }


            if (v1 < 0)
                v1 = vertexCount + v1 + 1;
            if (v2 < 0)
                v2 = vertexCount + v2 + 1;
            if (v3 < 0)
                v3 = vertexCount + v3 + 1;

            int t = myMesh->createTriangle(v1-1,v2-1,v3-1);

            TriangleMeshTriangle* tr = myMesh->getTriangle(t);
            if (hasNormals)
            	tr->setVerticeNormals(myMesh, n1-1, n2-1, n3-1);
            if (hasTextures)
            	tr->setVerticeTexture(myMesh, t1-1, t2-1, t3-1);

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
