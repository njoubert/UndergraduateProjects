/*
 * World.cpp
 *
 *  Created on: Oct 23, 2008
 *      Author: njoubert
 */

#include "World.h"

World::World() {
    _time = 0;
    _syncCounter = 0;
    _displayListsValid = false;
}

World::~World() {

}

void World::advance(double netTime) {
	cout<<"Advance has Been Called"<<endl;
    //Find the duration of a single step for each model
//*
    //Sort the list of updates from longest duration to shortest duration
	Model* tempModel;
	vector <int> modelIndex;
	vector<double> modelTsteps;
	double tempTstep;
	for(int i = 0; i < _models.size(); i++)
		modelIndex.push_back(i);
	for(int k = 0; k < _models.size(); k++)
		modelTsteps.push_back(_models[k]->getTimeStep());

	//for(int j = 0 ; j < _models.size(); j++)
		//cout<<modelTsteps[j]<<" ";
	//cout<<endl;

	for (int i = 0; i < modelTsteps.size()-1; i++) {
	  for (int j = 0; j < modelTsteps.size()-1-i; j++)
	    if (modelTsteps[j+1] > modelTsteps[j]) {
	    	tempTstep = modelTsteps[j];
	    	modelTsteps[j] = modelTsteps[j+1];
	    	modelTsteps[j+1] = tempTstep;
	  }
	}
	//for(int j = 0 ; j < _models.size(); j++)
		//cout<<modelTsteps[j]<<" ";
	//cout<<endl;

	for(int i = 0; i < _models.size(); i++)
		for(int j = 0; j < _models.size(); j++)
			if(modelTsteps[i] == _models[j]->getTimeStep())
				modelIndex[i] = j;


		//cout<<"Greatest to Smallest "<<_models[modelIndex[0]]->getTimeStep()<<" "<<_models[modelIndex[1]]->getTimeStep()<<" "<<_models[modelIndex[2]]->getTimeStep()<<endl;

	//
//*/
	//_models[modelIndex[0]]->getTimeStep() = model with biggest time step
	double biggestTimeStep = _models[modelIndex[0]]->getTimeStep();
	if(PLAYALLFRAMES)
		biggestTimeStep = BIGGESTTIMESTEP;
	int numSteps = floor(netTime / biggestTimeStep);
	double StepOfTimeStep = netTime/numSteps;
	double durationSoFar = 0;
	double timeStepDuration = 0;
	if(DEBUG)
		cout<<"A step of: "<<netTime<<" will be broken up into "<<netTime / biggestTimeStep<<" steps based on that "<<biggestTimeStep<<" is the largest timestep"<<endl;
	for (int i = 0; i < numSteps; i++) {
		if(DEBUG)
			cout<<"		Time is now: "<<_time<<" netTime is: "<<netTime<<" duration so far is : "<<durationSoFar<<" numSteps: "<<i+1<<" out of "<<numSteps<<endl;

		timeStepDuration = biggestTimeStep;
		for (unsigned int j = 0; j < _models.size(); j++) {
			if(DEBUG)
				cout<<"			Model: "<<j<<" w/ timeStep: "<<timeStepDuration<<" must take "<<ceil(timeStepDuration/_models[j]->getTimeStep())<<" steps to fufill "<<timeStepDuration<<"s"<<endl;
			/*
			if(STEPSBEFORESYNC != -1 && j == LOWQINDEX){
				if(_syncCounter%STEPSBEFORESYNC == 0) {
					cout<<"SYNCED AT TIME: "<<_time<<endl;
					enableMeshCorrection(LOWQINDEX, HIGHQINDEX);
				}
				else
					disableMeshCorrection(LOWQINDEX);
			}
			//*/
			_models[j]->advance(timeStepDuration, _time, StepOfTimeStep);

		}


		/* THIS SECTION TAKES CARE OF ROUNDOFF TIMESTEPS.
		 * INVARIANT: End of outer loop simulates forward in time for _exactly_ the netTime amount. */


		durationSoFar += timeStepDuration;
		double netTimeLeft = netTime - durationSoFar;
		if (netTimeLeft +0.000000000000001 < StepOfTimeStep) {
			if (netTimeLeft == 0) {
				if (DEBUG)
					cout << "    -- Did not need to take an extra roundoff step  netTimeLeft: "<<netTimeLeft<<" StepOfTimeStep "<<StepOfTimeStep << endl;
				//break;
			}
			//Change the next timeStep to take a step of the correct duration
			//to bring us to the end of the global timestep.
			else {
				timeStepDuration = netTimeLeft;
				if(DEBUG)
					cout << "    -- Taking extra roundoff step of duration " << timeStepDuration << "s, where current stepCount is " << i << " of " << numSteps << endl;
				for (unsigned int j = 0; j < _models.size(); j++)
					_models[j]->advance(timeStepDuration, _time, StepOfTimeStep);
			}
		}

		_time += StepOfTimeStep;
		TIME = _time;
		_syncCounter++;
		if(DEBUG)
			cout<<endl;
	}


    //_time += netTime;
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
        if(DRAWMODELS[j])
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

bool World::loadSimModel(TriangleMesh* mesh, double timeStep, float mass, string solver) {
    mesh->setGlobalMassPerParticle(mass/mesh->countVertices());
    MESHSIZE = mesh->countVertices();
    if (mesh == NULL)
        return false;
    Material* mat = new DEFAULT_MATERIAL();
    mat->makeSimColor();
    Model* model;
    if(solver == "newmark")
    	model = new SimModel(mesh, new DEFAULT_SYSTEM(mesh, mat),
            new NewmarkSolver(mesh, mesh->countVertices()), mat, timeStep);
    else if(solver == "explicit")
    	model = new SimModel(mesh, new DEFAULT_SYSTEM(mesh, mat),
            new ExplicitSolver(mesh, mesh->countVertices()), mat, timeStep);
    else
    	model = new SimModel(mesh, new DEFAULT_SYSTEM(mesh, mat),
            new DEFAULT_SOLVER(mesh, mesh->countVertices()), mat, timeStep);
    _models.push_back(model);
    cout<<"Finished Loading Sim Model"<<endl;
    return true;
}

bool World::loadAniModel(string filename) {
    Model* model = new AniModel(filename);
    _models.push_back(model);
    return true;
}

bool World::loadEllipseModel(string filename, int numFrames, int startFrame) {
    cout << "Parsing and Loading EllipseModel..." << endl;

    ellipseParser parser;
    std::pair<  vector < vector <mat4> > , vector < vector < std::pair < int, int > > > >
		ellipsoids = parser.parseEllipsoids(filename, numFrames, startFrame);
    /*	DEBUG FOR PARSER
     for(int i = 0; i < 21; i++) {
     cout<<endl<<endl<<"Frame: "<<i+1<<endl;
     for(int j = 0; j < 20; j++) {
     cout<<ellipsoids[i][j]<<endl<<endl;
     }
     }
     //*/

	Model* model = new AniElliModel(ellipsoids.first);
    _models.push_back(model);
    if(!OVERRIDE_DYNAMIC_CONSTRAINTS)
    	createVertexToAnimatedEllipseContraints(ellipsoids.second);
    cout << "Done Parsing and loading EllipseModel." << endl;
    return true;
}

bool World::initializeMeshSyncing(int lowQmodelIndex, int highQmodelIndex) {
	SimModel* lowQmodel = (SimModel*) _models[lowQmodelIndex];
	SimModel* highQmodel = (SimModel*) _models[highQmodelIndex];
	//cout<<"Testing Correction Mesh..."<<endl;
		//cout<<"		"<<highQmodel->getMesh()->getVertex(300)<<endl;
	lowQmodel->registerHighQmodel(highQmodel->getMesh());
	lowQmodel->initializeSyncTimes();

	return true;
}

bool World::enableMeshSyncing(int lowQmodelIndex, int highQmodelIndex) {
	SimModel* lowQmodel = (SimModel*) _models[lowQmodelIndex];
	SimModel* highQmodel = (SimModel*) _models[highQmodelIndex];
	//cout<<"Testing Correction Mesh..."<<endl;
		//cout<<"		"<<highQmodel->getMesh()->getVertex(300)<<endl;
	lowQmodel->registerHighQmodel(highQmodel->getMesh());

	return true;
}

bool World::disableMeshSyncing(int lowQmodelIndex) {
	SimModel* lowQmodel = (SimModel*) _models[lowQmodelIndex];
	lowQmodel->nullHighQmodel();
	return true;
}

/**
 * Magic happens here to load in constraints somehow...
 * TODO: Parse this from a file or something..
 */
bool World::createVertexToAnimatedEllipseContraints(vector < vector < std::pair < int, int > > > constraints) {
	vector<int> LeadEllipsoids;
	vector<int> FollowVertices;
	for(unsigned int i = 0; i < constraints[0].size(); i++) {		//Modify constraints[0] with another for loop if you want time varying constraints
		LeadEllipsoids.push_back(constraints[0][i].second);
		FollowVertices.push_back(constraints[0][i].first);
	}

	for (unsigned int i = 0; i < FollowVertices.size(); i++) {
				for(int j = 0; j < _models.size()-1; j++) {	//Ellipsoid Model Should be Last so j = 1; j <_models.size() -1; j++
				//Get the model I want to constrain
				SimModel* followModel = (SimModel*) _models[j];
				//Get the vertex on this model you want to constrain (the Follow):
				TriangleMeshVertex* v = followModel->getMesh()->getVertex(FollowVertices[i]);

				//Get the model I want to constrain against
				AniElliModel* leadModel = (AniElliModel*) _models[_models.size()-1];
				//Get the index of the Ellipsoid you want to constrain to (the Lead)
				int ellipsoidNr = LeadEllipsoids[i];

				//Set up the constraint
				VertexToAnimatedEllipseConstraint* constraint = new VertexToAnimatedEllipseConstraint();
				constraint->setLead(leadModel, ellipsoidNr);
				constraint->setFollow(v);

				v->setConstraint(true);

				followModel->registerConstraint(constraint);

				cout << "Created Dynamic Constraint: Mesh Vertex: "
						<< FollowVertices[i] << " is connected to Ellipsoid "
						<< LeadEllipsoids[i] << endl;
				//followModel->applyInitialConstraints();
				//cout<<"Dynamic Constraint Initialized."<<endl;
				}
			STATIC_CONSTRAINTS = false;
			DYNAMIC_CONSTRAINTS = true;
		}
	return true;
}

bool World::createVertexToAnimatedEllipseContraint() {
	vector<int> LeadEllipsoids;
	if (LEAD1 != -1)
		LeadEllipsoids.push_back(LEAD1);
	if (LEAD2 != -1)
		LeadEllipsoids.push_back(LEAD2);
	if (LEAD3 != -1)
		LeadEllipsoids.push_back(LEAD3);
	if (LEAD4 != -1)
		LeadEllipsoids.push_back(LEAD4);

	vector<int> hierarchyEllipsoids;
	if (HIERARCHY1 != -1)
		hierarchyEllipsoids.push_back(HIERARCHY1);
	if (HIERARCHY2 != -1)
		hierarchyEllipsoids.push_back(HIERARCHY2);
	if (HIERARCHY3 != -1)
		hierarchyEllipsoids.push_back(HIERARCHY3);
	if (HIERARCHY4 != -1)
		hierarchyEllipsoids.push_back(HIERARCHY4);

	vector<int> FollowVertices;
	if (FOLLOW1 != -1)
		FollowVertices.push_back(FOLLOW1);
	if (FOLLOW2 != -1)
		FollowVertices.push_back(FOLLOW2);
	if (FOLLOW3 != -1)
		FollowVertices.push_back(FOLLOW3);
	if (FOLLOW4 != -1)
		FollowVertices.push_back(FOLLOW4);

	for (unsigned int i = 0; i < FollowVertices.size(); i++) {

		if (_models.size() > 1) {
			for(int j = 1; j < _models.size(); j++) {
				cout<<"Creating Dynamic Constraints Manually (by use of command line)"<<endl;
			//Get the model I want to constrain
			SimModel* followModel = (SimModel*) _models[j];
			//Get the vertex on this model you want to constrain (the Follow):
			TriangleMeshVertex* v = followModel->getMesh()->getVertex(
					FollowVertices[i]);

			//Get the model I want to constrain against
			AniElliModel* leadModel = (AniElliModel*) _models[_models.size()-1];
			//Get the index of the Ellipsoid you want to constrain to (the Lead)
			int ellipsoidNr = LeadEllipsoids[i];

			//Set up the constraint
			VertexToAnimatedEllipseConstraint* constraint =
					new VertexToAnimatedEllipseConstraint();
			constraint->setLead(leadModel, ellipsoidNr);
			constraint->setFollow(v);
			constraint->setHierarchy(hierarchyEllipsoids[i]);

			followModel->registerConstraint(constraint);

			cout << "Created Dynamic Constraint: Mesh Vertex: "
					<< FollowVertices[i] << " is connected to Ellipsoid "
					<< LeadEllipsoids[i] << endl;
			//followModel->applyInitialConstraints();
			//cout<<"Dynamic Constraint Initialized."<<endl;
			}
		}

	}

	cout << FollowVertices.size() << " Dynamic Constraints Manually Created." << endl;
	STATIC_CONSTRAINTS = false;
	DYNAMIC_CONSTRAINTS = true;
	return true;
}

bool World::createVertexToAnimatedEllipseCollisions() {
	//COLISIONS (can be thought of as "Collision Constraints")
	if (_models.size() > 1) {
		for (int i = 0; i < _models.size()-1; i++) {
			SimModel* collisionModel = (SimModel*) _models[i];
			TriangleMesh* collisionMesh = collisionModel->getMesh();

			AniElliModel* collisionEllipsoids = (AniElliModel*) _models[_models.size()-1];

			VertexToEllipseCollision* collisionConstraint =
					new VertexToEllipseCollision();

			collisionConstraint->setCollisionMesh(collisionMesh);
			collisionConstraint->setCollisionEllipsoids(collisionEllipsoids);
			collisionModel->registerCollision(collisionConstraint);
		}
	}
	return true;
}

bool World::createFixedVertexContraints() {
	if (STATIC_CONSTRAINTS) {
		cout << "Checking for Fixed Constraints" << endl;
		SimModel* simModel = (SimModel*) _models[0];
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

void World::inializeExportSim(string directory, double inverseFPS) {
        cout << "Saving .objs to directory " << directory << endl;
        _objOutDir = directory;
        if (_objOutDir[_objOutDir.size()-1] != '/')
        	_objOutDir.append("/");
        _doObjOutput = true;
        _lastTime = -1 - inverseFPS;

        _ellipsoidsSyncFile.open ("ellipsoidsSyncData.txt");

}
void World::exportSim(int simNum, double time, bool JustDoIt, double inverseFPS) {
	if (_models.size() > 1) {
		if (!_doObjOutput)
		            return;
		        if (time >= _lastTime + inverseFPS || JustDoIt)
		            _lastTime = time;
		        else
		            return;

		        _frameCount++;
		                stringstream filename(stringstream::in | stringstream::out);
		                filename << _objOutDir << "sym";
		                filename << std::setfill('0') << setw(6) << _frameCount << ".obj";
		                cout << "Save obj " << _frameCount << " at "<< setprecision(3) << time <<"s ..." << endl;

				//Export Model
				SimModel* simModel = (SimModel*) _models[simNum];
				simModel->getMesh()->exportAsOBJ(filename.str());
				AniElliModel* elliModel = (AniElliModel*) _models[_models.size()-1];
				_ellipsoidsSyncFile <<elliModel->getFrameCount()<<endl;
			}
}

void World::apocalypse_EndOfTheClothAge() {
	_ellipsoidsSyncFile.close();
}

//MOUSE FORCES ARE CURRENTLY DEPENDENT ON WHICH MODEL IS FIRST SO I DISABLED THEM
void World::enableMouseForce(vec3 mPos) {
	SimModel* simModel = (SimModel*) _models[0];
	simModel->enableMouseForce(mPos);
}
void World::disableMouseForce() {
	SimModel* simModel = (SimModel*) _models[0];
	simModel->disableMouseForce();
}
void World::updateMouseForce(vec3 new_mPos) {
	SimModel* simModel = (SimModel*) _models[0];
	simModel->updateMouseForce(new_mPos);
}
bool World::isMouseEnabled() {
	SimModel* simModel = (SimModel*) _models[0];
	return simModel->isMouseEnabled();
}
