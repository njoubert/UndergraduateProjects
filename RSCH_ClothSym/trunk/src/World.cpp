/*
 * World.cpp
 *
 *  Created on: Oct 23, 2008
 *      Author: njoubert
 */

#include "World.h"

World::World() {
    _time = 0;
    _displayListsValid = false;
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
    if (!_displayListsValid) {
        _displayListsIndex = glGenLists(_statmodels.size());
        for (unsigned int j = 0; j < _statmodels.size(); j++) {
            glNewList(_displayListsIndex + j, GL_COMPILE);
            _statmodels[j]->draw();
            glEndList();
        }
        _displayListsValid = true;
    }
    for (unsigned int j = 0; j < _statmodels.size(); j++) {
        glCallList(_displayListsIndex + j);
    }

    for (unsigned int j = 0; j < _models.size(); j++)
        _models[j]->draw();
}

bool World::loadStatModel(string filename) {
    OBJParser parser;
    TriangleMesh* mesh = parser.parseOBJ(filename);
    if (mesh == NULL)
        return false;
    StatModel* model = new StatModel(mesh);
    _statmodels.push_back(model);
    return true;
}

bool World::loadSimModel(TriangleMesh* mesh, double timeStep, float mass) {
    mesh->setGlobalMassPerParticle(mass/mesh->countVertices());
    MESHSIZE = mesh->countVertices();
    if (mesh == NULL)
        return false;
    Material* mat = new DEFAULT_MATERIAL();
    mat->makeSimColor();
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
	if(LEAD1 != -1)
		LeadEllipsoids.push_back(LEAD1);
	if(LEAD2 != -1)
			LeadEllipsoids.push_back(LEAD2);
	if(LEAD3 != -1)
			LeadEllipsoids.push_back(LEAD3);
	if(LEAD4 != -1)
			LeadEllipsoids.push_back(LEAD4);

	//LeadEllipsoids.push_back(3);
	//LeadEllipsoids.push_back(4);

	vector<int>	FollowVertices;
	//FollowVertices.push_back(110);
	//FollowVertices.push_back(120);
	//FollowVertices.push_back(30);
	//FollowVertices.push_back(35);
	if(FOLLOW1 != -1)
		FollowVertices.push_back(FOLLOW1);
	if(FOLLOW2 != -1)
			FollowVertices.push_back(FOLLOW2);
	if(FOLLOW3 != -1)
			FollowVertices.push_back(FOLLOW3);
	if(FOLLOW4 != -1)
			FollowVertices.push_back(FOLLOW4);


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
			//followModel->applyInitialConstraints();
			//cout<<"Dynamic Constraint Initialized."<<endl;
		}

	}

	cout<<FollowVertices.size()<< " Dynamic Constraints Created."<<endl;
	STATIC_CONSTRAINTS = false;
	DYNAMIC_CONSTRAINTS = true;
    return true;
}

bool World::createVertexToAnimatedEllipseCollisions() {
	//COLISIONS (can be thought of as "Collision Constraints")
		if (_models.size() > 1) {
			SimModel* collisionModel = (SimModel*) _models[1];
			TriangleMesh* collisionMesh = collisionModel->getMesh();

			AniElliModel* collisionEllipsoids = (AniElliModel*) _models[0];

			VertexToEllipseCollision* collisionConstraint =
					new VertexToEllipseCollision();

			collisionConstraint->setCollisionMesh(collisionMesh);
			collisionConstraint->setCollisionEllipsoids(collisionEllipsoids);
			collisionModel->registerCollision(collisionConstraint);
		}
		return true;
}

bool World::createFixedVertexContraints() {
	if (STATIC_CONSTRAINTS) {
		cout << "Checking for Fixed Constraints" << endl;
		SimModel* simModel = (SimModel*) _models[1];
		TriangleMesh* mesh = simModel->getMesh();
		for (int i = 0; i < mesh->countVertices(); i++) {
			TriangleMeshVertex* v = mesh->getVertex(i);
			if (v->getConstaint()) {
				FixedConstraint* constraint = new FixedConstraint();
				constraint->setFollow(v);
				simModel->registerConstraint(constraint);
				STATIC_CONSTRAINTS = true;
				DYNAMIC_CONSTRAINTS = false;
				cout << "Created Fixed Constraint" << endl;
			}
		}
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

void World::enableMouseForce(vec3 mPos) {
	SimModel* simModel = (SimModel*) _models[1];
	simModel->enableMouseForce(mPos);
}

void World::disableMouseForce() {
	SimModel* simModel = (SimModel*) _models[1];
	simModel->disableMouseForce();
}

void World::updateMouseForce(vec3 new_mPos) {
	SimModel* simModel = (SimModel*) _models[1];
	simModel->updateMouseForce(new_mPos);
}

bool World::isMouseEnabled() {
	SimModel* simModel = (SimModel*) _models[1];
	return simModel->isMouseEnabled();
}
