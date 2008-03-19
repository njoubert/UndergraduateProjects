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
 *  eye             x y z
 *  viewport        llx lly llz lrx lry lrz urx ury urz ulx uly ulz
 *  sphere          x y z radius ksr ksg ksb kar kag kab kdr kdg kdb rr rg rb
 *  ellipse         [...]
 *  triangle        x1 y1 z1 x2 y2 z2 x3 y3 z3 ksr ksg ksb kar kag kab kdr kdg kdb rr rg rb
 *  directionlight  x y z r g b
 *  pointlight      x y z r g b
 * 
 */
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include "Debug.cpp"
#include "Scene.cpp"

class Parser {
public:
    Parser() {
        parsedEye = false; parsedViewport = false;
    }
    
    bool isDone() {
        return (parsedEye && parsedViewport);
    }
    
    Scene* parseScene(string filename) {
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
        
        //Check that the scene contains:
        // - an eye
        // - a viewport
        
        inFile.close();
        return sc;
    }
private:
    bool parsedEye;
    bool parsedViewport;
    
    bool parseLine(string line, Scene * scene) {
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
        } else if (operand.compare("eye") == 0) {
            
                if (parsedEye) {
                    printError("Multiple definitions of an eye in input file!");
                    exit(1);
                }
                ss >> scene->eye.pos.x;
                ss >> scene->eye.pos.y;
                ss >> scene->eye.pos.z;
                printDebug(3, "Parsed Eye Input to (" << scene->eye.pos.x << "," << scene->eye.pos.y << "," << scene->eye.pos.x << ")");
                parsedEye = true;
                
        } else if (operand.compare("viewport") == 0) {
            
                if (parsedViewport) {
                    printError("Multiple definitions of an viewport in input file!");
                    exit(1);
                }
                ss >> scene->viewport.LL.x;
                ss >> scene->viewport.LL.y;
                ss >> scene->viewport.LL.z;
                ss >> scene->viewport.LR.x;
                ss >> scene->viewport.LR.y;
                ss >> scene->viewport.LR.z;
                ss >> scene->viewport.UR.x;
                ss >> scene->viewport.UR.y;
                ss >> scene->viewport.UR.z;
                ss >> scene->viewport.UL.x;
                ss >> scene->viewport.UL.y;
                ss >> scene->viewport.UL.z; 
                printDebug(3, "Parsed Viewport Input to LL=(" << scene->viewport.LL.x << ","
                        << scene->viewport.LL.y << ","
                        << scene->viewport.LL.z << ") LR=("
                        << scene->viewport.LR.x << ","
                        << scene->viewport.LR.y << ","
                        << scene->viewport.LR.z << ") UR=("
                        << scene->viewport.UR.x << ","
                        << scene->viewport.UR.y << ","
                        << scene->viewport.UR.z << ") UL=("
                        << scene->viewport.UL.x << ","
                        << scene->viewport.UL.y << ","
                        << scene->viewport.UL.z << ")");
                
                parsedViewport = true;
                
        } else if (operand.compare("sphere") == 0) {
            
            float x, y, z, r, ksr, ksg, ksb, kar, kag, kab, kdr, kdg, kdb, rr, rg, rb;
            ss >>x >>y >>z >>r >> ksr>> ksg>> ksb>> kar>> kag>> kab>> kdr>> kdg>> kdb>> rr>> rg>> rb;
            success = scene->addSphere(r,x,y,z,ksr,ksg,ksb,kar,kag,kab,kdr,kdg,kdb,rr,rg,rb);
            printDebug(3, "Parsed Sphere Input to (" << x << "," << y << "," << z << ") r=" << r);
            
        } else if (operand.compare("ellipse") == 0) {
            
            printDebug(3, "  ");
            
        } else if (operand.compare("triangle") == 0) {
            
            float x1, y1, z1, x2, y2, z2, x3, y3, z3, ksr, ksg, ksb, kar, kag, kab, kdr, kdg, kdb, rr, rg, rb;
            ss >>x1>> y1>> z1>> x2>> y2>> z2>> x3>> y3>> z3>> ksr>> ksg>> ksb>> kar>> kag>> kab>> kdr>> kdg>> kdb>> rr>> rg>> rb;
            success = scene->addTriangle(x1, y1, z1, x2, y2, z2, x3, y3, z3, ksr, ksg, ksb, kar, kag, kab, kdr, kdg, kdb,rr,rg,rb);
            printDebug(3, "Parsed Triangle Input to ("<<x1<<","<<y1<<","<<z1<<") ("<<x2<<","<<y2<<","<<z2<<") ("<<x3<<","<<y3<<","<<z3<<")");
            
        } else if (operand.compare("directionlight") == 0) {
            
            float x, y, z, r, g, b;
            ss >>x >>y >>z >>r >>g >>b;
            success = scene->addDirectionLight(x,y,z,r,g,b);
            printDebug(3, "Parsed DirectionLight input to ("<<x<<","<<y<<","<<z<<") color ("<<r<<","<<g<<","<<b<<")");
            
        } else if (operand.compare("pointlight") == 0) {
            
            printDebug(3, "  ");
            
        } else {
            printError("Unknown operand in scene file, skipping line: " << operand);
        }
        
        if (ss.fail()) {
            printError("The bad bit of the input file's line's stringstream is set! Couldn't parse!")
            exit(1);
        }     
        return success;
    }
};









#endif /*PARSER_C_*/
