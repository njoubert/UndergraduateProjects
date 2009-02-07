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
#include "EllipseParser.h"

//*
ellipseParser::ellipseParser() {

}

std::pair<  vector < vector <mat4> > , vector < vector < std::pair < int, int > > > >
	ellipseParser::parseEllipsoids(string filename, int numFrames, int startFrame) {

    //printInfo("Parsing Scene File " << filename);
	char line[10024];
    char fullFileName[50];
    string rootFileName = filename.c_str();
    std::pair<  vector < vector <mat4> > , vector < vector < std::pair < int, int > > > > elliData;
    std::pair< std::pair< bool, mat4 >, std::pair< bool, int > > elliDataTemp;

    std::vector < std::vector <mat4> > myEllipsoidFrames;
    std::vector<mat4> myEllipsoidFrame;

    std::vector < std::vector < std::pair <int, int > > > myEllipsoidConsts;
    std::vector < std::pair <int, int > > myEllipsoidConst;

    //std::vector < std::vector <vec3> > myEllipsoidRots;
    //std::vector<vec3> myEllipsoidRot;
    ifstream inFile;

    cout<<"Number of Frames: "<<numFrames<<endl<<endl;
    for(int i = startFrame; i < numFrames; i++) {

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
    			elliDataTemp = parseLine(string(line));
    			myEllipsoidFrame.push_back(elliDataTemp.first.second);
    			if(elliDataTemp.second.first) {
    				std::pair <int, int > temp;
    				temp.first = elliDataTemp.second.second;
    				temp.second = myEllipsoidFrame.size()-1;
    				myEllipsoidConst.push_back(temp);
    			}
    			//else
    				//myEllipsoidRot.push_back(elliDataTemp.second.second);
    		}
    	}
    	inFile.close();
    	myEllipsoidFrames.push_back(myEllipsoidFrame);
    	myEllipsoidConsts.push_back(myEllipsoidConst);
    	/*
    	//for(int j = 0; j < 20; j++) {
    		cout<<myEllipsoidFrame[20]<<endl;
			cout<<myEllipsoidFrames[i][20]<<endl<<endl;
    	//}
    	//*/
		myEllipsoidFrame.erase(myEllipsoidFrame.begin(),myEllipsoidFrame.end());
		myEllipsoidConst.erase(myEllipsoidConst.begin(),myEllipsoidConst.end());
		//myEllipsoidRot.erase(myEllipsoidRot.begin(),myEllipsoidRot.end());
    }
    parseDebug("Ellipsoid Parser exiting...");
//*
    for(int i = 0; i < 1; i++) {
    		cout<<"Frame: "<<i<<endl;
    		for(int j = 0; j < myEllipsoidFrames[i].size(); j++) {
    			//cout<<"Ellipsoid: "<<j<<endl;
    			//cout<<"Matrix Transformation: "<<myEllipsoidFrames[i][j]<<endl;
    		}
    		//cout<<endl;
    		for(int k = 0; k < myEllipsoidConsts[i].size(); k++) {
    		    //cout<<"Constraint: "<<k<<endl;
    		    //cout<<"Dynamic Constraint Vetex: "<<myEllipsoidConsts[i][k].first<<" To Ellipsoid: "<<myEllipsoidConsts[i][k].second<<endl;
    		}
    		//cout<<endl;
    	}
    	//cout<<endl;
//*/

    elliData.first = myEllipsoidFrames;
    elliData.second = myEllipsoidConsts;
    return elliData;
}


std::pair< std::pair< bool, mat4 >, std::pair< bool, int > > ellipseParser::parseLine(string line) {
    string operand;
    std::pair< std::pair< bool, mat4 >, std::pair< bool, int > > lineData;
    mat4 transform;
    vec3 rotAngles;
    int dynConstInd;
    lineData.first.first = false;
    lineData.second.first = false;

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
        string ellipseName, transformIdentifier, constraintIdentifier;
        ss >> ellipseName >> transformIdentifier >> constraintIdentifier;
        //if(constraint) convert to char, register a constraint with given index
        //const char *constraintIdentifier;
        //constraintIdentifier=transName.c_str();
        if(constraintIdentifier[0] == 'v') {
        	char dynConstInd_char[constraintIdentifier.size()-1];
        	for(int i = 1; i < constraintIdentifier.size(); i++)
        		dynConstInd_char[i-1] = constraintIdentifier.c_str()[i];
        	//cout<<"Constraint Identifier: "<<constraintIdentifier<<endl;
        	//cout<<"Converted to Char: "<<dynConstInd_char<<endl;
        	dynConstInd = atoi(dynConstInd_char);
        	lineData.second.first = true;
        	//cout<<"Dynamic Constraint Found for Vertex: "<<dynConstInd<<endl;//<<" and Ellipsoid: "<<<<endl;
        }
        else
        	lineData.second.first = false;

        ss >> transform[0][0] >> transform[0][1] >> transform[0][2] >> transform[0][3] >>
			  transform[1][0] >> transform[1][1] >> transform[1][2] >> transform[1][3] >>
			  transform[2][0] >> transform[2][1] >> transform[2][2] >> transform[2][3] >>
			  transform[3][0] >> transform[3][1] >> transform[3][2] >> transform[3][3];
        lineData.first.first = true;
    } //else if (operand.compare("rotAnglesXYZ:") == 0) {

        //ss >> rotAngles[0] >> rotAngles[1] >> rotAngles[2];
        //lineData.second.first = true;
   // }
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


    lineData.first.second = transform;
    //lineData.second.second = rotAngles;
    lineData.second.second = dynConstInd;
    return lineData;
}
//*/
