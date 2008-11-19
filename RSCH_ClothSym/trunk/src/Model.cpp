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
}

StatModel::~StatModel() {
	delete _mesh;
}


void StatModel::advance(double netTime) {
	return;
}

double StatModel::getTimeStep() {
	return _timeStep;
}

void StatModel::draw() {

    //Make the Static Objects Blue
    GLfloat mat_diffuse[] = { 0.0, 0.0, 1.0, 1.0 };
    GLfloat mat_ambient[] = { 0.0, 0.1, 0.0, 1.0 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);

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

void SimModel::advance(double netTime) {
    //Loading state happens on object creation.

    int stepsToTake = floor(netTime / _timeStep);
    for (int i = 0; i < stepsToTake; i++)
        _system->takeStep(_solver, &_constraints, &_collisions, _timeStep);
    double timeLeft = netTime - stepsToTake*_timeStep;
    if (timeLeft > 0)
        _system->takeStep(_solver, &_constraints, &_collisions, _timeStep);

}

double SimModel::getTimeStep() {
	return _timeStep;
}

void SimModel::enableMouseForce(vec3 mPos) { _system->enableMouseForce(mPos); }
void SimModel::updateMouseForce(vec3 new_mPos) { _system->updateMouseForce(new_mPos); }
void SimModel::disableMouseForce() { _system->disableMouseForce(); }
bool SimModel::isMouseEnabled() { return _system->isMouseEnabled(); }

void SimModel::draw() {
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, _material->getMatDiffuse());
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, _material->getMatAmbient());

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
	advance(0);

}

AniModel::~AniModel() {

}

void AniModel::advance(double netTime) {
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

AniElliModel::AniElliModel(vector < vector <mat4> > ellipsoids) {
	_ellipsoids = ellipsoids;

	/* PARSER DEBUG
	for(unsigned int i = 0; i < ellipsoids.size(); i++) {
		cout<<endl<<endl<<"Frame: "<<i+1<<endl<<endl;
			for (unsigned int j = 0; j < ellipsoids[i].size(); j++) {
			cout<<_ellipsoids[i][j]<<endl<<endl;
			}
	}
	//*/
	_count = -1;
	advance(0);
	_timeStep = .01;

	_muS = .6;
	//_muD = 20.4;
	_muD = .8;
	_takeConst.push_back(vec3(0));
	_takeConst.push_back(vec3(0));

}

AniElliModel::~AniElliModel() {

}

void AniElliModel::advance(double netTime) {
    profiler.frametimers.switchToTimer("AniEliModel::advance");

	if(_count < _ellipsoids.size()-1){
			//cout<<"Drawing Frame: "<<_count+1<<endl;
			//cout<<_ellipsoids[_count].size()<<" ellipsoids"<<endl;
	    _count++;
		}
	else
		_count = 0;
	profiler.frametimers.switchToGlobal();
}

double AniElliModel::getTimeStep() {
	return _timeStep;
}

vec3 AniElliModel::getNormal(int j, vec4 X_elli_4) {
	//Get Position Were Collision Occurs
		mat4 elliTransform = getEllipsoid(j);

		X_elli_4[3] = 0;
		vec3 Xc(elliTransform.inverse().transpose() * X_elli_4, VW);
		Xc.normalize();

		return Xc;
}

void AniElliModel::draw() {
	if (DRAWELLIPSOIDS) {
		//*
		for (unsigned int i = 0; i < _ellipsoids[_count].size(); i++) {

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
			glutSolidSphere(1, 100, 100);

			glPopMatrix();
		}
	}
//*/
}

mat4 AniElliModel::getEllipsoid(int Indx) { return _ellipsoids[_count][Indx]; }

int AniElliModel::getSize() { return _ellipsoids[_count].size(); }

//FOR COLLISIONS

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
		ElliTransform = _ellipsoids[0][i];
	else
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
		ElliTransform = _ellipsoids[_ellipsoids.size()-1][i];
	else
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

	X_elli_4 = X_elli_4 / sqrt(X_elli_4[0] * X_elli_4[0] + X_elli_4[1] * X_elli_4[1] + X_elli_4[2]* X_elli_4[2]);
	X_elli_4[3] = 1;

	vec4 Xc_4 = X_elli_4 * elliTransform;
	vec3 Xc(Xc_4, VW);

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
