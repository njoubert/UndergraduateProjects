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

	vector<int> LeadEllipsoids;
	LeadEllipsoids.push_back(3);
	LeadEllipsoids.push_back(4);

	vector<int>	FollowVertices;
	FollowVertices.push_back(30);
	FollowVertices.push_back(35);
	//FollowVertices.push_back(7);
	//FollowVertices.push_back(8);

	for (int i = 0; i < FollowVertices.size(); i++) {

		if (_models.size() > 1) {
			//Get the model I want to constrain
			SimModel* followModel = (SimModel*) _models[1];
			//Get the vertex on this model you want to constrain (the Follow):
			TriangleMeshVertex* v = followModel->getMesh()->getVertex(
					FollowVertices[i]);

			//Get the model I want to constrain against
			AniElliModel* leadModel = (AniElliModel*) _models[0];
			//Get the index of the Ellipsoid you want to constrain to (the Lead)
			int ellipsoidNr = LeadEllipsoids[i];

			//Set up the constraint
			VertexToAnimatedEllipseConstraint* constraint =
					new VertexToAnimatedEllipseConstraint();
			constraint->setLead(leadModel, ellipsoidNr);
			constraint->setFollow(v);

			followModel->registerConstraint(constraint);

		}

	}

    //COLISIONS (can be thought of as "Collision Constraints")
	if (_models.size() > 1) {
		SimModel* collisionModel = (SimModel*) _models[1];
		TriangleMesh* collisionMesh = collisionModel->getMesh();

		AniElliModel* collisionEllipsoids = (AniElliModel*) _models[0];

		VertexToAnimatedEllipseConstraint* collisionConstraint =
				new VertexToAnimatedEllipseConstraint();

		collisionConstraint->setCollisionMesh(collisionMesh);
		collisionConstraint->setCollisionEllipsoids(collisionEllipsoids);
		collisionModel->registerCollision(collisionConstraint);
	}
    return true;
}



double World::getTime() {
	return _time;
}

vec3 World::getFocusPoint() {
	//*
	AniElliModel* focusModel = (AniElliModel*) _models[0];
	int ellipsoidNr = 13;
	mat4 elliTransform = focusModel->getEllipsoid(ellipsoidNr);

	vec4 origin(0);
	origin[3] = 1;
	//cout<<"transform:"<<endl<<elliTransform<<endl<<endl;
	vec4 focusPt = origin*elliTransform;
	//cout<<focusPt<<endl;
	//for(int i = 0; i < 3; i++)
		//focusPt[i] = focusPt[i]/focusPt[3];
	//*/
	return focusPt;

	//SimModel* focusModel = (SimModel*) _models[1];

	//return focusModel->getConstraintPos(0);
}
