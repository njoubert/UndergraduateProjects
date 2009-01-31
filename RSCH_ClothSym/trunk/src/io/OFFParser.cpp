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
#include "OFFParser.h"

OFFParser::OFFParser() {
    vertexCount = 0;
}

/**
 * Attempts to parse an OBJ/OFF file. Returns false if we fail.
 */
TriangleMesh* OFFParser::parse(string filename) {
    //printInfo("Parsing Scene File " << filename);
    char line[1024];
    TriangleMesh* myMesh = new TriangleMesh();
    ifstream inFile(filename.c_str(), ifstream::in);
    if (!inFile) {
        std::cout << "Could not open given OFF file " << filename << endl;
        delete myMesh;
        inFile.close();
        return NULL;
    }
    inFile.getline(line, 1023);
    if (string(line) != "OFF") {
        std::cout << "File corrupted.." << endl;
        inFile.close();
        return NULL;
    }
    inFile.getline(line, 1023);
    stringstream ss(stringstream::in | stringstream::out);
    ss.str(string(line));
    int nvert, nface, nedge;
    ss >> nvert >> nface >> nedge;

    for (int i = 0; i < nvert; i++) {
        inFile.getline(line, 1023);
        stringstream s2(stringstream::in | stringstream::out);
        s2.str(string(line));

        double x, y, z;
        s2 >>x >>y >>z;
        int j = myMesh->createVertex(x,y,z);
        s2 >>ws;
        if (!s2.eof()) {
            string args;
            s2 >> args;
            //cout << "reading MORE! " << args;
            if (args.compare("fixed") == 0) {
                parseDebug("Constraining particle " << i);
                myMesh->getVertex(j)->setConstraint(true);
                cout<<"Fixed Point Found in OBJ"<<endl;
            }
        }

    }


    int a,b,c,d;
    cout << "Finished reading " <<nvert <<" lines, last which was:" << string(line) << endl;
    string temp;
    while (inFile.good()) {
        inFile.getline(line, 1023);
        temp = string(line);
        stringstream s(stringstream::in | stringstream::out);
        s.str(temp);
        if (line[0] == '#')
            continue;
        s >> a >> b >> c >> d;
        if (a != 3) {
            cout << a << " " << b << " " << c << " " << d << endl;
            cout << "Line: " << string(line) << endl;
            cout << "Wrong number of vertices per polygon. We only support triangles! We detected " << a << endl;
            inFile.close();
            return NULL;
        } else {
            if (b == 0 || c == 0 || d == 0)
                cout << "ADDED POINT 0" << endl;
            myMesh->createTriangle(b,c,d);
        }
    }
    cout << "Mesh created with " << myMesh->countTriangles() << " triangles, " << myMesh->countVertices() << " vertices." << endl;
    myMesh->setGlobalMassPerParticle(1.0 / nvert);
    inFile.close();
    parseDebug("Parser exiting...");
    cout << "Parsed an OFF mesh with " << myMesh->countVertices() << " vertices, " << myMesh->countTriangles() << " triangles." << endl;
    return myMesh;
}

#endif /*PARSER_C_*/
