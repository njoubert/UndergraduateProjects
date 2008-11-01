/*
 * World.cpp
 *
 *  Created on: Oct 23, 2008
 *      Author: njoubert
 */

#include "World.h"

World::World() {
    _time = 0;
}

World::~World() {

}

void World::advance(double netTime) {
    for (unsigned int j = 0; j < _models.size(); j++)
        _models[j]->advance(netTime);

    _time +=  netTime;
}

void World::draw() {
    for (unsigned int j = 0; j < _models.size(); j++)
        _models[j]->draw();
}

bool World::loadStatModel(string filename) {
    OBJParser parser;
    TriangleMesh* mesh = parser.parseOBJ(filename);
    if (mesh == NULL)
    	return false;
    Model* model = new StatModel(mesh);
    _models.push_back(model);
    return true;
}

bool World::loadSimModel(string filename) {
    OBJParser parser;
    TriangleMesh* mesh = parser.parseOBJ(filename);
    if (mesh == NULL){
    	return false;
    	cout<<"No mesh Parsed"<<endl;
    }
    Material* mat = new DEFAULT_MATERIAL();
    	Model* model = new SimModel(
    	    		mesh,
    	    		new DEFAULT_SYSTEM(mesh,mesh->countVertices()),
    	    		new DEFAULT_SOLVER(mesh, mesh->countVertices()),
    	    		mat);
    _models.push_back(model);
    return true;
}

bool World::loadAniModel(string filename) {
	Model* model = new AniModel(filename);
    _models.push_back(model);
    return true;
}

bool World::loadEllipseModel(string filename, int numFrames) {
	cout<<"Parsing and Loading EllipseModel..."<<endl;

	ellipseParser parser;
	vector < vector <mat4> > ellipsoids = parser.parseEllipsoids(filename, numFrames);
	/*	DEBUG FOR PARSER
	for(int i = 0; i < 21; i++) {
		cout<<endl<<endl<<"Frame: "<<i+1<<endl;
		for(int j = 0; j < 20; j++) {
			cout<<ellipsoids[i][j]<<endl<<endl;
		}
	}
	//*/
	Model* model = new AniElliModel(ellipsoids);
	_models.push_back(model);
	cout<<"Done Parsing and loading EllipseModel."<<endl;
	return true;
}

/**
 * Magic happens here to load in constraints somehow...
 */
bool World::createVertexToAnimatedEllipseContraint() {

    //Get the model I want to constrain
    SimModel* followModel = (SimModel*) _models[1];
    //Get the vertex on this model you want to constrain:
    TriangleMeshVertex* v = followModel->getMesh()->getVertex(0);

    //Get the model I want to constrain against
    AniElliModel* leadModel = (AniElliModel*) _models[0];
    int ellipsoidNr = 0;

    //Set up the constraint
    VertexToAnimatedEllipseConstraint* constraint =
        new VertexToAnimatedEllipseConstraint();
    constraint->setLead(leadModel, ellipsoidNr);
    constraint->setFollow(v);

    followModel->registerConstraint(constraint);

    return true;
}

double World::getTime() {
	return _time;
}


