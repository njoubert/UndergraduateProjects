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
    //Find the duration of a single step for each model
/*
    //Sort the list of updates from longest duration to shortest duration
	Model* tempModel;
	for (int i = 0; i < _models.size()-1; i++) {
	  for (int j = 0; j < _models.size()-1-i; j++)
	    if (_models[j+1]->getTimeStep() > _models[j]->getTimeStep()) {
	    	tempModel = _models[j];
	      _models[j] = _models[j+1];
	      _models[j+1] = tempModel;
	  }
	}
	//
//*/
	//The Simple Version, Assuming only one dependency (cloth on ellipsoids)
	int stepSize = int(netTime / _models[0]->getTimeStep());
	for (int i = 0; i < stepSize; i++) {
		for (unsigned int j = 0; j < _models.size(); j++)
			_models[j]->advance(_models[0]->getTimeStep());
	}



    _time += netTime;
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

bool World::loadSimModel(string filename, double timeStep) {
    OBJParser parser;
    TriangleMesh* mesh = parser.parseOBJ(filename);
    if (mesh == NULL)
        return false;
    Material* mat = new DEFAULT_MATERIAL();
    Model* model = new SimModel(mesh, new DEFAULT_SYSTEM(mesh, mat),
            new DEFAULT_SOLVER(mesh, mesh->countVertices()), mat, timeStep);
    _models.push_back(model);
    return true;
}

bool World::loadAniModel(string filename) {
    Model* model = new AniModel(filename);
    _models.push_back(model);
    return true;
}

bool World::loadEllipseModel(string filename, int numFrames) {
    cout << "Parsing and Loading EllipseModel..." << endl;

    ellipseParser parser;
    vector<vector<mat4> > ellipsoids = parser.parseEllipsoids(filename,
            numFrames);
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
    cout << "Done Parsing and loading EllipseModel." << endl;
    return true;
}

/**
 * Magic happens here to load in constraints somehow...
 * TODO: Parse this from a file or something..
 */
bool World::createVertexToAnimatedEllipseContraint() {
	vector<int> LeadEllipsoids;
	LeadEllipsoids.push_back(3);
	LeadEllipsoids.push_back(4);

	vector<int>	FollowVertices;
	//FollowVertices.push_back(21);
	//FollowVertices.push_back(121);
	FollowVertices.push_back(30);
	FollowVertices.push_back(35);
	//FollowVertices.push_back(7);
	//FollowVertices.push_back(8);

	for (unsigned int i = 0; i < FollowVertices.size(); i++) {

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
			cout<<"Created Dynamic Constraint: Mesh Vertex: "<<FollowVertices[i]
				<<" is connected to Ellipsoid "<<LeadEllipsoids[i]<<endl;
		}

	}

	cout<<FollowVertices.size()<< " Dynamic Constraints Created."<<endl;

    return true;
}

bool World::createFixedVertexContraints() {
	cout<<"about to create constarint"<<endl;
	SimModel* simModel = (SimModel*) _models[1];
	TriangleMesh* mesh = simModel->getMesh();
	for(int i = 0; i < mesh->countVertices(); i++){
	TriangleMeshVertex*	v = mesh->getVertex(i);
		if(v->getConstaint()){
			FixedConstraint* constraint = new FixedConstraint();
			constraint->setFollow(v);
			simModel->registerConstraint(constraint);
			cout<<"Created Fixed Constraint"<<endl;
		}
	}
	return true;
}

double World::getTime() {
    return _time;
}

