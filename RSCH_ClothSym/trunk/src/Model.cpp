/*
 * Model.cpp
 *
 *  Created on: Oct 23, 2008
 *      Author: njoubert
 */

#include "Model.h"

/*-------------------------------------------------
 *
 *   MODEL BASE
 *
 *------------------------------------------------*/

Model::Model() {
    printDebug("Initialized Model Base Class.");
    _material = new DEFAULT_MATERIAL();
}

Model::Model(Material* mat) {
	_material = mat;
}

Model::~Model() {
    delete _material;
}

/*-------------------------------------------------
 *
 *   STATIC MODEL
 *
 *------------------------------------------------*/

StatModel::StatModel(TriangleMesh* mesh) {
	_mesh = mesh;
	_timeStep = 0;
	_material->makeStatColor();
}

StatModel::~StatModel() {
	delete _mesh;
}


void StatModel::advance(double netTime, double globalTime, double futureTimeStep) {
	return;
}

double StatModel::getTimeStep() {
	return _timeStep;
}

void StatModel::draw() {

    //Make the Static Objects Blue
    _material->setGLcolors();

	//TODO: Lets do this with display lists for real.
    vec3 a, b, c, na, nb, nc;

    TriangleMeshVertex** vertices;
    std::vector< TriangleMeshTriangle* >::const_iterator it =
        _mesh->triangles.begin();
    while (it != _mesh->triangles.end()) {
        vertices = (*it)->getVertices();

        a = vertices[0]->getX();
        na = vertices[0]->getNormal();
        b = vertices[1]->getX();
        nb = vertices[1]->getNormal();
        c = vertices[2]->getX();
        nc = vertices[2]->getNormal();

        glBegin(GL_TRIANGLES);
            glNormal3f( na[0], na[1], na[2]);
            glVertex3f(a[0],a[1],a[2]);
            glNormal3f( nb[0], nb[1], nb[2]);
            glVertex3f(b[0],b[1],b[2]);
            glNormal3f( nc[0], nc[1], nc[2]);
            glVertex3f(c[0],c[1],c[2]);
        glEnd();

        it++;
    }
}

/*-------------------------------------------------
 *
 *   SIMULATED MODEL
 *
 *------------------------------------------------*/

SimModel::SimModel(TriangleMesh* mesh,
		System* system, Solver* solver, Material* mat, double timeStep) : Model(mat) {
	_mesh = mesh;
	_system = system;
	_solver = solver;
		if(timeStep == 0)
		_timeStep = DEFAULT_TIMESTEP;
	else
		_timeStep = timeStep;


    //Load x, vx from mesh into system:
    _system->loadStateFromMesh();

}

SimModel::~SimModel() {
	delete _mesh;
	delete _system;
	delete _solver;
}

void SimModel::advance(double netTime, double globalTime, double futureTimeStep) {
    //Loading state happens on object creation.

    int stepsToTake = floor(netTime / _timeStep);
    for (int i = 0; i < stepsToTake; i++) {
        cout<<"				Model "<<_timeStep<<" Took Step"<<endl;
        _system->takeStep(_solver, &_constraints, &_collisions, _timeStep);
    }
    double timeLeft = netTime - stepsToTake*_timeStep;
    if (timeLeft > 0) {
        cout<<"				Model "<<_timeStep<<" Took Roundoff Step"<<endl;
        _system->takeStep(_solver, &_constraints, &_collisions, _timeStep);
    }

}

double SimModel::getTimeStep() {
	return _timeStep;
}

void SimModel::enableMouseForce(vec3 mPos) { _system->enableMouseForce(mPos); }
void SimModel::updateMouseForce(vec3 new_mPos) { _system->updateMouseForce(new_mPos); }
void SimModel::disableMouseForce() { _system->disableMouseForce(); }
bool SimModel::isMouseEnabled() { return _system->isMouseEnabled(); }

void SimModel::draw() {
    _material->setGLcolors();

    vec3 a, b, c, na, nb, nc;
    TriangleMeshVertex** vertices;
    std::vector< TriangleMeshTriangle* >::const_iterator it =
        _mesh->triangles.begin();

    while (it != _mesh->triangles.end()) {
        vertices = (*it)->getVertices();

        a = vertices[0]->getX();
        na = vertices[0]->getNormal();
        b = vertices[1]->getX();
        nb = vertices[1]->getNormal();
        c = vertices[2]->getX();
        nc = vertices[2]->getNormal();

        glBegin(GL_TRIANGLES);
            glNormal3f( na[0], na[1], na[2]);
            glVertex3f(a[0],a[1],a[2]);
            glNormal3f( nb[0], nb[1], nb[2]);
            glVertex3f(b[0],b[1],b[2]);
            glNormal3f( nc[0], nc[1], nc[2]);
            glVertex3f(c[0],c[1],c[2]);
        glEnd();

        it++;
    }
//*
    if(_system->getErrorInd() > 0) {
    	vector<double> posError = _system->getPosError();
    	vector<double> velError = _system->getVelError();
    	int ErrorCount = _system->getErrorInd();
    	float x1, x2;
    	glBegin(GL_LINES);
			glVertex3f(-4, -1.5, 1);
			glVertex3f(4, -1.5, 1);
    	glEnd();
    	glBegin(GL_LINES);
			glVertex3f(-4, -1.5, 1);
    		glVertex3f(-4, 0, 1);
        glBegin(GL_LINES);
    		for(int i = 0; i < ErrorCount-1; i++){
    			x1 = float(i)/100 - 4;
    			x2 = float(i+1)/100 - 4;
    			//x1 = TIME - 4;
    			//x2 = TIME + _timeStep - 4;
    			glVertex3f(x1,-1.5+posError[i]*3,1);
    			glVertex3f(x2,-1.5+posError[i+1]*3,1);
    		}
        glEnd();
    	/*
    	glBegin(GL_LINES);
			glVertex3f(-2, 2, 0);
			glVertex3f(0, 2, 0);
    	glEnd();
    	glBegin(GL_LINES);
			glVertex3f(-2, 2, 0);
    		glVertex3f(-2, 3, 0);
    	glEnd();
    	glBegin(GL_LINES);
			glVertex3f(-2, 0.5, 0);
			glVertex3f(0, 0.5, 0);
    	glEnd();
    	glBegin(GL_LINES);
			glVertex3f(-2, 0.5, 0);
    		glVertex3f(-2, 1.5, 0);
    	glEnd();

    	glBegin(GL_LINES);
			for(int i = 0; i < ErrorCount-1; i++){
				x1 = float(i)/100 - 2;
				x2 = float(i+1)/100 - 2;
				//cout<<x<<endl;
				glVertex3f(x1,2+posError[i]*3,0);
				glVertex3f(x2,2+posError[i+1]*3,0);
			}
    	glEnd();

    	glBegin(GL_LINES);
			for(int i = 0; i < ErrorCount-1; i++){
				x1 = float(i)/100 - 2;
				x2 = float(i+1)/100 - 2;
				//cout<<x<<endl;
				glVertex3f(x1,0.5+velError[i]*.1,0);
				glVertex3f(x2,0.5+velError[i+1]*.1,0);
			}
    	glEnd();
    	//*/
    }
    //*/
}

TriangleMesh* SimModel::getMesh() const {
    return _mesh;
}

void SimModel::registerConstraint(Constraint* c) {
    _constraints.push_back(c);
}

void SimModel::applyInitialConstraints() {
	_system->applyConstraints(_solver, &_constraints);
}

void SimModel::registerCollision(VertexToEllipseCollision* c) {
    _collisions.push_back(c);
}

void SimModel::registerHighQmodel(TriangleMesh* cMesh) {
	//cout<<"Testing Correction Mesh..."<<endl;
		//cout<<"		"<<cMesh->getVertex(300)<<endl;

	_system->setHighQmesh(cMesh);
}

void SimModel::nullHighQmodel() {
	_system->setHighQmesh(NULL);
}

/*-------------------------------------------------
 *
 *   ANIMATED MODEL
 *
 *------------------------------------------------*/

AniModel::AniModel(string filename) {
	_filename = filename;
	_count = 0;
	_mesh = NULL;
	_timeStep = 0.04; //TODO: Read from file.

}

AniModel::~AniModel() {

}

void AniModel::advance(double netTime, double globalTime, double futureTimeStep) {
    if (_mesh != NULL)
	    delete _mesh;
    OBJParser parser;
	ostringstream filename;
	filename << _filename;
	filename << _count;
	filename << ".obj";
	printDebug("Loading OBJ " << filename)
	TriangleMesh* mesh = parser.parseOBJ(filename.str());
	if (mesh != NULL)
		_mesh = mesh;
	_count++;
}

double AniModel::getTimeStep() {
	return _timeStep;
}

void AniModel::draw() {

    _material->setGLcolors();

    vec3 a, b, c, na, nb, nc;

    TriangleMeshVertex** vertices;
    std::vector< TriangleMeshTriangle* >::const_iterator it =
        _mesh->triangles.begin();
    while (it != _mesh->triangles.end()) {
        vertices = (*it)->getVertices();

        a = vertices[0]->getX();
        na = vertices[0]->getNormal();
        b = vertices[1]->getX();
        nb = vertices[1]->getNormal();
        c = vertices[2]->getX();
        nc = vertices[2]->getNormal();

        glBegin(GL_TRIANGLES);
            glNormal3f( na[0], na[1], na[2]);
            glVertex3f(a[0],a[1],a[2]);
            glNormal3f( nb[0], nb[1], nb[2]);
            glVertex3f(b[0],b[1],b[2]);
            glNormal3f( nc[0], nc[1], nc[2]);
            glVertex3f(c[0],c[1],c[2]);
        glEnd();

        it++;
    }
}

/*-------------------------------------------------
 *
 *   ANIMATED EllIPSOID MODEL
 *
 *------------------------------------------------*/

AniElliModel::AniElliModel(std::pair<  vector < vector <mat4> > , vector < vector <vec3> > > ellipsoids) {
	_ellipsoids = ellipsoids.first;
	_elliRots = ellipsoids.second;
	cout<<"Loaded Rotation Data for Angular Velocity: "<<_elliRots.size()<<endl;
/*
	for(int i = 0; i < _elliRots.size(); i++) {
		cout<<"Frame: "<<i<<endl;
		for(int j = 0; j < _elliRots[i].size(); j++) {
		cout<<"Ellipsoid: "<<j<<endl;
		cout<<"Matrix Transformation: "<<_ellipsoids[i][j]<<endl;
		cout<<"Rotation Angles (Scalars): "<<_elliRots[i][j]<<endl;
		}
		cout<<endl;
	}
	cout<<endl;
//*/
	/* PARSER DEBUG
	for(unsigned int i = 0; i < ellipsoids.size(); i++) {
		cout<<endl<<endl<<"Frame: "<<i+1<<endl<<endl;
			for (unsigned int j = 0; j < ellipsoids[i].size(); j++) {
			cout<<_ellipsoids[i][j]<<endl<<endl;
			}
	}
	//*/
	_count = 0;
	_timeStep = 1.0/120.0;
	_loops = 0;

	_muS = .6;
	//_muD = 20.4;
	_muD = .8;
	_takeConst.push_back(vec3(0));
	_takeConst.push_back(vec3(0));

}

AniElliModel::~AniElliModel() {

}

void AniElliModel::advance(double netTime, double globalTime, double futureTimeStep) {
    profiler.frametimers.switchToTimer("AniEliModel::advance");

//*
    int totalFrames = _ellipsoids.size()-1;
    unsigned int frameNumber = floor( (globalTime/_timeStep) - _loops*totalFrames);
    int futureFrameNumber = floor( ((globalTime+futureTimeStep)/_timeStep) - _loops*totalFrames);

    if(frameNumber < _ellipsoids.size()-1){
    	_pastCount = _count;
    	_count = frameNumber;
    	if(futureFrameNumber < _ellipsoids.size()-1)
    		_futureCount = futureFrameNumber;
    	else
    		_futureCount = _ellipsoids.size()-1;
    }
    else {
    	_pastCount = 0;
    	_count = 0;
    	_loops++;
    	futureFrameNumber = floor( ((globalTime+futureTimeStep)/_timeStep) - _loops*totalFrames);
     	_futureCount = 0;
    }
    _elliTime[0] = (_pastCount + _loops*totalFrames)*_timeStep;
    _elliTime[1] = (_count + _loops*totalFrames)*_timeStep;
    _elliTime[2] = (_futureCount + _loops*totalFrames)*_timeStep;

        //cout<<"Ellipsoid Model is on FRAME: "<<_count<<" It's Future Frame is: "<<_futureCount<<" It's Past Frame was "<<_pastCount
       //<<". Global Time is: "<<globalTime<<". ElliTime is: "<<_elliTime[1]
        //<<". The FRAMERATE is: "<<(_count + _loops*totalFrames)/globalTime<<"FPS."<<endl;

        cout<<"				Ellipsoid Model is on FRAME: "<<_count<<". ElliTime is: "<<_elliTime[1]<<". The FRAMERATE is: "<<(_count + _loops*totalFrames)/globalTime<<"FPS."<<endl;

//*/

/*
    int stepsToTake = floor(netTime / _timeStep);
        for (int i = 0; i < stepsToTake; i++) {
        	cout<<"Ellipsoids Model Took 1 Step "<<_timeStep<<"s"<<endl;
        	if(_count < _ellipsoids.size()-1)
        	    _count++;
        	else
        		_count = 0;
        }
        double timeLeft = netTime - stepsToTake*_timeStep;
        if (timeLeft > 0) {
        	cout<<"Ellipsoids Model Took 1 Step (Roundoff) "<<_timeStep<<"s"<<endl;
        	if(_count < _ellipsoids.size()-1)
        	    _count++;
        	else
        		_count = 0;
        }
        cout<<"Ellipsoid Model is on FRAME: "<<_count<<". Global Time is: "<<globalTime<<". The FRAMERATE is: "<<(_count)/globalTime<<"FPS."<<endl;

//*/


	_normalPos.clear();
	_normalDir.clear();
	profiler.frametimers.switchToGlobal();
}

double AniElliModel::getTimeStep() {
	return _timeStep;
}



vec3 AniElliModel::getNormal(int j, vec4 X_elli_4) {
	//Get Position Were normal is
	mat4 elliTransform = getEllipsoid(j);

//	X_elli_4 = X_elli_4 / sqrt(X_elli_4[0] * X_elli_4[0] + X_elli_4[1]
//			* X_elli_4[1] + X_elli_4[2] * X_elli_4[2]);
//	X_elli_4[3] = 1;

	X_elli_4[3] = 0;
	vec3 Xc(X_elli_4 * elliTransform.inverse().transpose(), VW);
	Xc.normalize();

	return Xc;
}

vec3 AniElliModel::getNormal(int j, vec3 Xc_world) {
	//DOES NOT GIVE CORRECT NORMAL
		mat4 elliTransform = getEllipsoid(j);

		vec4 Xc_world_4;
		for(int i = 0; i < 3; i++)
			Xc_world_4[i]= Xc_world[i];
		Xc_world_4[3] = 1;
		vec4 X_elli_4(elliTransform.inverse() * Xc_world_4);

		X_elli_4 = X_elli_4 / sqrt(X_elli_4[0] * X_elli_4[0] + X_elli_4[1]
					* X_elli_4[1] + X_elli_4[2] * X_elli_4[2]);

		X_elli_4[3] = 0;
		vec3 Xc(elliTransform.inverse().transpose() * X_elli_4, VW);
		Xc.normalize();

		return Xc;
}

void AniElliModel::setNormal(vec3 pos, vec3 dir) { _normalPos.push_back(pos);
													_normalDir.push_back(dir); }

void AniElliModel::draw() {

    _material->setGLcolors();

	if (DRAWELLIPSOIDS) {
		//*
		for (unsigned int i = 0; i < _ellipsoids[_count].size(); i++) {
//*
			glPushMatrix();

			GLdouble m[16];
			int count = 0;
			for (int j = 0; j < 4; j++)
				for (int k = 0; k < 4; k++) {
					m[count] = _ellipsoids[_count][i][j][k];
					count++;
					//cout<<m[j*(k+1)]<<endl;
				}
			//exit(1);
			glMultMatrixd(m);
			glutSolidSphere(.75, 10, 10);
			glPopMatrix();
		}
	}
//*/
/*
			for (int i = 0; i < _normalPos.size(); i++) {
				vec3 pos1 = _normalPos[i];
				vec3 pos2 = _normalPos[i] + 1 * _normalDir[i];
				glBegin(GL_QUADS);
					glVertex3f(pos1[0] - 0.05, pos1[1], pos1[2]);
					glVertex3f(pos1[0] + 0.05, pos1[1], pos1[2]);
					glVertex3f(pos2[0] + 0.05, pos2[1], pos2[2]);
					glVertex3f(pos2[0] - 0.05, pos2[1], pos2[2]);
				glEnd();
			}
//*/

}

int AniElliModel::getFrameCount(){ return _count; }

double AniElliModel::getElliTimePast(){	return _elliTime[0];}
double AniElliModel::getElliTimeCurrent(){	return _elliTime[1];}
double AniElliModel::getElliTimeFuture(){	return _elliTime[2];}

mat4 AniElliModel::getEllipsoid(int Indx) { return _ellipsoids[_count][Indx]; }

int AniElliModel::getSize() { return _ellipsoids[_count].size(); }

//FOR COLLISIONS

vec3 AniElliModel::calcAngularVel(int i, vec3 Xc) {
	//	CurrentTime: Xc -> position of collision, 				Xo -> point of ellipsoid origin (should be center of mass)
	//	FutureTime: Xc_f -> position of collision in future, 	Xo_f -> point of future ellipsoid origin (should be center of mass)
	//	PastTime: Xc_p -> position of collision in past, 		Xo_f -> point of past ellipsoid origin (should be center of mass)
//	cout<<"1"<<endl;


	//Get Current Future and Past Ellipsoid Transformations
	mat4 ElliTransformCurrent = _ellipsoids[_count][i];
	mat4 ElliTransformFuture;
	mat4 ElliTransformPast;
	vec3 ElliRots = _elliRots[_count][i];
	vec3 ElliRotsFuture, ElliRotsPast;
	//cout<<"Size of Ellis: "<<_ellipsoids[_count].size()<<endl;
	//cout<<"Frame: "<<_count<<"elli: "<<i<<endl;
	//cout<<"elliRotsOrgin: " <<_elliRots[_count][i]<<endl;
	//cout<<"elliRots: "<<ElliRots<<endl;
	if (_count == _ellipsoids.size() - 1) {
		ElliTransformFuture = _ellipsoids[0][i];
		ElliRotsFuture = _elliRots[0][i];
	}
	else {
		ElliTransformFuture = _ellipsoids[_count + 1][i];
		ElliRotsFuture = _elliRots[_count + 1][i];
	}

	if (_count == 0) {
		ElliTransformPast = _ellipsoids[_ellipsoids.size()-1][i];
		ElliRotsPast = _elliRots[_elliRots.size()-1][i];
	}
	else {
		ElliTransformPast = _ellipsoids[_count - 1][i];
		ElliRotsPast = _elliRots[_count - 1][i];
	}

	//Determine Angular Velocity
	//cout<<"Future: "<<ElliRotsFuture<< " - "<<"Past: "<<ElliRotsPast<<endl;
	vec3 W = (ElliRotsFuture - ElliRotsPast)/(2*_timeStep)*(3.1415/180);
	//cout<<"W: "<<W<<endl;


	vec3 Xo = getOrigin(i);
	vec3 r = (Xc - Xo);
//	cout<<"Xo: "<<Xo<<" Xc: "<<Xc<<endl;
//	cout<<"r: "<<r<<endl;
	vec3 Wtan = W ^ r;
//	cout<<"tangental velocity: " << Wtan << endl;

	return Wtan;


}

mat4 AniElliModel::calculateRotationMatrix(int i) {
	//	CurrentTime: Xc -> position of collision, 				Xo -> point of ellipsoid origin (should be center of mass)
	//	FutureTime: Xc_f -> position of collision in future, 	Xo_f -> point of future ellipsoid origin (should be center of mass)
	//	PastTime: Xc_p -> position of collision in past, 		Xo_f -> point of past ellipsoid origin (should be center of mass)


	//Get Current Future and Past Ellipsoid Transformations
	mat4 ElliTransformCurrent = _ellipsoids[_count][i];
	mat4 ElliTransformFuture;
	if (_count == _ellipsoids.size() - 1)
		ElliTransformFuture = _ellipsoids[0][i];
	else
		ElliTransformFuture = _ellipsoids[_count + 1][i];
	mat4 ElliTransformPast;
	if (_count == 0)
		ElliTransformPast = _ellipsoids[_ellipsoids.size()-1][i];
	else
		ElliTransformPast = _ellipsoids[_count - 1][i];
/*
	cout<<"ElliTransformFuture: "<<ElliTransformFuture.transpose()<<endl;
	cout<<ElliTransformFuture[0][0]<<" "<<ElliTransformFuture[0][1]<<" "<<ElliTransformFuture[0][2]<<" "<<ElliTransformFuture[0][3]<<endl;
	cout<<ElliTransformFuture[1][0]<<" "<<ElliTransformFuture[1][1]<<" "<<ElliTransformFuture[1][2]<<" "<<ElliTransformFuture[1][3]<<endl;
	cout<<ElliTransformFuture[2][0]<<" "<<ElliTransformFuture[2][1]<<" "<<ElliTransformFuture[2][2]<<" "<<ElliTransformFuture[2][3]<<endl;
	cout<<ElliTransformFuture[3][0]<<" "<<ElliTransformFuture[3][1]<<" "<<ElliTransformFuture[3][2]<<" "<<ElliTransformFuture[3][3]<<endl;
	cout<<"ElliTransformPast: "<<ElliTransformFuture<<endl;
*/
	mat4 RotationMatrix = ElliTransformFuture - ElliTransformPast;


	return RotationMatrix;

}

vec3 AniElliModel::calculateAngularVelocity(int i, vec3 Xc_world) {
	//	CurrentTime: Xc -> position of collision, 				Xo -> point of ellipsoid origin (should be center of mass)
	//	FutureTime: Xc_f -> position of collision in future, 	Xo_f -> point of future ellipsoid origin (should be center of mass)
	//	PastTime: Xc_p -> position of collision in past, 		Xo_f -> point of past ellipsoid origin (should be center of mass)
//	cout<<"1"<<endl;
	vec4 Xc_world_4;
	for(int p = 0; p < 3; p++)
		Xc_world_4[p] = Xc_world[p];
	Xc_world_4[3] = 1;
//	cout<<"2"<<endl;

	//Get Current Future and Past Ellipsoid Transformations
	mat4 ElliTransformCurrent = _ellipsoids[_count][i];
	mat4 ElliTransformFuture;
	if (_count == _ellipsoids.size() - 1)
		ElliTransformFuture = _ellipsoids[0][i];
	else
		ElliTransformFuture = _ellipsoids[_count + 1][i];
	mat4 ElliTransformPast;
	if (_count == 0)
		ElliTransformPast = _ellipsoids[_ellipsoids.size()-1][i];
	else
		ElliTransformPast = _ellipsoids[_count - 1][i];
//	cout<<"3"<<endl;

	//Get into Space of Current, Future and Past Ellipsoids
	vec4 Xc_f_4 = Xc_world_4 * ElliTransformFuture.inverse();
	vec4 Xc_p_4 = Xc_world_4 * ElliTransformPast.inverse();
	vec4 Xc_4 = Xc_world_4 * ElliTransformCurrent.inverse();
//	cout<<"4"<<endl;

	//Drop Homogenious Coordinate
	vec3 Xo(0, 0, 0);
	double elliTimeStep = getTimeStep();
	//Xc_f_4 = Xc_f_4/Xc_f_4[3];
	//Xc_p_4 = Xc_f_4/Xc_f_4[3];
	//Xc_4 = Xc_4/Xc_4[3];
	vec3 Xc_f(Xc_f_4, VW);
	vec3 Xc_p(Xc_p_4, VW);
	vec3 Xc(Xc_4, VW);
//	cout<<"5"<<endl;

	//Determine Angular Velocity
	vec3 r = (Xc - Xo), r_p = (Xc_p - Xo), r_f = (Xc_f - Xo);
	vec3 Vr = (r_f - r_p) / (2 * elliTimeStep);
	vec3 W = (r ^ Vr) / (r.length() * r.length());
//	cout<<"W sphereSpace: "<<W<<endl;
	vec3 Wtan = W ^ r;
//	vec3 Wtan = W;
//	cout<<"6"<<endl;

	//Convert to world space
	vec4 Wtan_4;
	for (int p = 0; p < 3; p++)
		Wtan_4[p] = Wtan[p];
	Wtan_4[3] = 1;
	vec3 Wtan_world(ElliTransformCurrent.inverse().transpose() * Wtan_4, VW);
	//vec3 Wtan_world(ElliTransformCurrent.inverse() * Wtan_4, VW);
	//cout<<"7"<<endl;

	return Wtan_world;

}

vec3 AniElliModel::getOrigin(int i) {
	vec4 origin(0); origin[1] = 1;
	for(int j = 0; j < 4; j++)
		origin[j] = _ellipsoids[0][i][j][3];
	vec3 ElliCenter_3;
	vec4 ElliCenter_4;
	mat4 ElliTransform;

	//Get Ellipsoid Position From Ellipsoid Transformation
	ElliTransform = _ellipsoids[_count][i];
	ElliCenter_4 = origin * ElliTransform;
	//cout<<"Past Transform: "<<ElliTransform<<endl;
	//cout<<"Past Pos (4): "<<ElliCenter_4<<endl;
	for (int k = 0; k < 3; k++)
		ElliCenter_3[k] = ElliCenter_4[k];

	return ElliCenter_3;
}

vec3 AniElliModel::getFutureOrigin(int i) {
	vec4 origin(0); origin[1] = 1;
	for(int j = 0; j < 4; j++)
		origin[j] = _ellipsoids[0][i][j][3];
	vec3 ElliCenter_3;
	vec4 ElliCenter_4;
	mat4 ElliTransform;

	//Get Ellipsoid Position From Ellipsoid Transformation
	if (_count == _ellipsoids.size() - 1)
		ElliTransform = _ellipsoids[_count][i];
	else
		//ElliTransform = _ellipsoids[_futureCount][i];
		ElliTransform = _ellipsoids[_count + 1][i];
	ElliCenter_4 = origin * ElliTransform;
	//cout<<"Future Transform: "<<ElliTransform<<endl;
	//cout<<"Future Pos (4): "<<ElliCenter_4<<endl;
	for (int k = 0; k < 3; k++)
		ElliCenter_3[k] = ElliCenter_4[k];

	return ElliCenter_3;
}

vec3 AniElliModel::getPastOrigin(int i) {
	vec4 origin(0); origin[1] = 1;
	for(int j = 0; j < 4; j++)
		origin[j] = _ellipsoids[0][i][j][3];
	vec3 ElliCenter_3;
	vec4 ElliCenter_4;
	mat4 ElliTransform;

	//Get Ellipsoid Position From Ellipsoid Transformation
	if (_count == 0)
		ElliTransform = _ellipsoids[_count][i];
	else
		//ElliTransform = _ellipsoids[_pastCount][i];
		ElliTransform = _ellipsoids[_count - 1][i];
	ElliCenter_4 = origin * ElliTransform;
	//cout<<"Past Transform: "<<ElliTransform<<endl;
	//cout<<"Past Pos (4): "<<ElliCenter_4<<endl;
	for (int k = 0; k < 3; k++)
		ElliCenter_3[k] = ElliCenter_4[k];

	return ElliCenter_3;
}

bool AniElliModel::isPointInsideElli(int elliNr, vec4 X_elli_4) {

	double L = sqrt(X_elli_4[0]*X_elli_4[0] + X_elli_4[1]*X_elli_4[1] + X_elli_4[2]*X_elli_4[2]);
	//cout<<"L : "<<L<<endl<<endl;

	if(L < 1)
		return true;
	else
		return false;
}

vec4 AniElliModel::convertPoint2ElliSpace(int j, vec3 X_world_3) {
	mat4 elliTransform = getEllipsoid(j);
	vec4 X_world_4, X_elli_4;
	for (int k = 0; k < 3; k++)
		X_world_4[k] = X_world_3[k];
	X_world_4[3] = 1;
	//cout<<"v : "<<v<<endl;
	//cout<<"elliTransform: "<<endl<<elliTransform<<endl;
	X_elli_4 = X_world_4 * elliTransform.inverse();
	//cout<<"vT: "<<vT<<endl;

	return X_elli_4;
}

vec3 AniElliModel::getPointInsideElli2Surface(int j, vec4 X_elli_4) {
	//Get Position Were Collision Occurs
	mat4 elliTransform = getEllipsoid(j);
	vec3 X_elli_3;

	for (int k = 0; k < 3; k++)
		X_elli_3[k] = X_elli_4[k];
	X_elli_3 = X_elli_3 / sqrt(X_elli_3[0] * X_elli_3[0] + X_elli_3[1] * X_elli_3[1] + X_elli_3[2]* X_elli_3[2]);
	for (int k = 0; k < 3; k++)
		X_elli_4[k] = X_elli_3[k];

	X_elli_4[3] = 1;
	vec4 Xc_4 = X_elli_4 * elliTransform;
	vec3 Xc;
	for (int k = 0; k < 3; k++)
		Xc[k] = Xc_4[k];
/*
	X_elli_4 = X_elli_4 / sqrt(X_elli_4[0] * X_elli_4[0] + X_elli_4[1] * X_elli_4[1] + X_elli_4[2]* X_elli_4[2]);
	X_elli_4[3] = 1;

	vec4 Xc_4 = X_elli_4 * elliTransform;
	vec3 Xc(Xc_4, VW);
//*/

	return Xc;
}

vec3 AniElliModel::getPointInFuture(int i, vec3 x_WorldSpace_3) {
	vec3 x_Future_WorldSpace_3;
	vec4 x_Future_WorldSpace_4;
	mat4 ElliTransform;
	mat4 ElliTransformFuture;
	vec4 x_WorldSpace;
	vec4 x_ElliSpace;

	//Take Point From World Space Bring it into Ellipsoid Space
	for(int k = 0; k < 3; k++)
		x_WorldSpace[k] = x_WorldSpace_3[k];
	x_WorldSpace[3] = 1;
	ElliTransform = _ellipsoids[_count][i];
	x_ElliSpace = x_WorldSpace*ElliTransform.inverse();

	//Take Point in Ellipsoid Space and Bring it into world space one step into the future
	if (_count == _ellipsoids.size() - 1)
		ElliTransformFuture = _ellipsoids[0][i];
	else
		ElliTransformFuture = _ellipsoids[_count + 1][i];
	x_Future_WorldSpace_4 = x_ElliSpace * ElliTransformFuture;
	//cout<<"Future Transform: "<<ElliTransform<<endl;
	//cout<<"Future Pos (4): "<<ElliCenter_4<<endl;
	for (int k = 0; k < 3; k++)
		x_Future_WorldSpace_3[k] = x_Future_WorldSpace_4[k];

	return x_Future_WorldSpace_3;
}

vec3 AniElliModel::getPointInPast(int i, vec3 x_WorldSpace_3) {
	vec3 x_Past_WorldSpace_3;
	vec4 x_Past_WorldSpace_4;
	mat4 ElliTransform;
	mat4 ElliTransformPast;
	vec4 x_WorldSpace;
	vec4 x_ElliSpace;

	//Take Point From World Space Bring it into Ellipsoid Space
	for(int k = 0; k < 3; k++)
		x_WorldSpace[k] = x_WorldSpace_3[k];
	x_WorldSpace[3] = 1;
	ElliTransform = _ellipsoids[_count][i];
	x_ElliSpace = x_WorldSpace*ElliTransform.inverse();

	//Take Point in Ellipsoid Space and Bring it into world space one step into the past
	if (_count == 0)
		ElliTransformPast = _ellipsoids[_ellipsoids.size()-1][i];
	else
		ElliTransformPast = _ellipsoids[_count - 1][i];
	x_Past_WorldSpace_4 = x_ElliSpace * ElliTransformPast;
	//cout<<"Future Transform: "<<ElliTransform<<endl;
	//cout<<"Future Pos (4): "<<ElliCenter_4<<endl;
	for (int k = 0; k < 3; k++)
		x_Past_WorldSpace_3[k] = x_Past_WorldSpace_4[k];

	return x_Past_WorldSpace_3;
}

//FOR FRICTION

double AniElliModel::getMu_s() {
	return _muS;
}

double AniElliModel::getMu_d() {
	return _muD;
}
