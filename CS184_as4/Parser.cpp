#ifndef PARSER_C_
#define PARSER_C_

/**
 * The aim of this file is to parse input files
 * describing objects in a scene. Three main categories
 * are in a scene:
 * -- Camera (eye and viewport)
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
 *  eye             x y z
 *  viewport        llx lly llz lrx lry lrz urx ury urz ulx uly ulz
 *  boundingbox     llx lly llz lrx lry lrz urx ury urz ulx uly ulz
 *  sphere          center radius ksr ksg ksb kar kag kab kdr kdg kdb
 *  ellipse         [...]
 *  triangle        [vertex1] [vertex2] [vertex3] ksr ksg ksb kar kag kab kdr kdg kdb
 *  directionlight  x y z r g b
 *  pointlight      x y z r g b
 * 
 */
#include <string>
#include <iostream>
#include <fstream>
#include "Debug.cpp"
#include "Scene.cpp"

class Parser {
public:
    static Scene* parseScene(string filename) {
        printInfo("Parsing Scene File " << filename);
        char line[256];
        Scene* sc = new Scene();
        ifstream inFile(filename.c_str(), ifstream::in);
        if (!inFile) {
            printError("Could not open given scene file " << filename);
            exit(1);
        }
        while (inFile.good()) {
            inFile.getline(line, 255);
            if (!parseLine(string(line), sc))
                exit(1);
        }
        inFile.close();
        return sc;
    }
private:
    static bool parseLine(string line, Scene * scene) {
        printDebug(5, "Parsing Line: " << line);
        return true;
    }
};









#endif /*PARSER_C_*/
