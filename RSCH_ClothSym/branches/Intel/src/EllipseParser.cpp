/*
 * EllipseParser.cpp
 *
 *  Created on: Oct 27, 2008
 *      Author: silversatin
 */

/**
 * The aim of this file is to parse input files of ellipsoid transformations
 *	- These transformations assume that the reference shape is a unit sphere
 *		with it's center located at (0,0,0)
 *	- Files must be named filname#.txt		ex. frame1.txt, frame2.txt, frame3.txt, ..., frame669.txt
 *
 * File structure:
 * [operative] [operands]\n
 *
 * operative        operands
 *
 *
 *
 *
 */
#include "ellipseParser.h"

//*
ellipseParser::ellipseParser() {

}

vector < vector <mat4> > ellipseParser::parseEllipsoids(string filename, int numFrames) {
    //printInfo("Parsing Scene File " << filename);
	char line[10024];
    char fullFileName[50];
    string rootFileName = filename.c_str();
    std::vector < std::vector <mat4> > myEllipsoidFrames;
    std::vector<mat4> myEllipsoidFrame;
    ifstream inFile;

    cout<<"Number of Frames: "<<numFrames<<endl<<endl;
    for(int i = 0; i < numFrames; i++) {

    	//cout<<"i= "<<i+1<<" out of "<<numFrames<<" frames."<<endl;
    	//cout<<"Root Filename: "<<rootFileName.c_str()<<endl;
    	sprintf (fullFileName, "%s%d.txt", rootFileName.c_str(), i+1);
    	//cout<<"Parsing: "<<fullFileName<<endl<<endl;

    	inFile.open(fullFileName, ifstream::in);
    	if (!inFile) {
    		std::cout << "Could not open given etrans file " << filename << endl;
    	}
    	while (inFile.good()) {
    		inFile.getline(line, 10023);
    		if (string(line).empty()) {

    		}
    		else {
    			myEllipsoidFrame.push_back(parseLine(string(line)));
    		}
    	}
    	inFile.close();
    	myEllipsoidFrames.push_back(myEllipsoidFrame);
    	/*
    	//for(int j = 0; j < 20; j++) {
    		cout<<myEllipsoidFrame[20]<<endl;
			cout<<myEllipsoidFrames[i][20]<<endl<<endl;
    	//}
    	//*/
		myEllipsoidFrame.erase(myEllipsoidFrame.begin(),myEllipsoidFrame.end());
    }
    parseDebug("Ellipsoid Parser exiting...");

    return myEllipsoidFrames;
}


mat4 ellipseParser::parseLine(string line) {
    string operand;
    mat4 transform;

    parseDebug("Parsing Line: " << line);
    if (line.empty()) {
        parseDebug("Done parsing!");
        cout<<"something is wrong, line should not be empty"<<endl;
        exit(1);
        //return true;
    }
    stringstream ss(stringstream::in | stringstream::out);
    ss.str(line);
    ss >> operand;
    //printDebug(4, " Operand: " << operand);
    if (operand[0] == '#') {
        cout<<"can't use # to skip line anymore"<<endl;
    } else if (operand.compare("mesh:") == 0) {
        string ellipseName, dump;
        ss >> ellipseName >> dump >> dump;

        ss >> transform[0][0] >> transform[0][1] >> transform[0][2] >> transform[0][3] >>
			  transform[1][0] >> transform[1][1] >> transform[1][2] >> transform[1][3] >>
			  transform[2][0] >> transform[2][1] >> transform[2][2] >> transform[2][3] >>
			  transform[3][0] >> transform[3][1] >> transform[3][2] >> transform[3][3];
    }
    else {
        cout << "Unknown operand in scene file, skipping line: " << operand << endl;
        exit(1);
    }

    if (ss.fail()) {
        std::cout << "The bad bit of the input file's line's stringstream is set! Couldn't parse:" << endl;
        std::cout << "  " << line << endl;
        exit(1);
    }
    //cout<<"transform: "<<transform<<endl<<endl;
    return transform;
}
//*/
