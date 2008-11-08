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
}

SimModel::~SimModel() {
	delete _mesh;
	delete _system;
	delete _solver;
}

void SimModel::advance(double netTime) {
    //Load x, vx from mesh into system:
    _system->loadStateFromMesh();

    int stepsToTake = floor(netTime / _timeStep);
    for (int i = 0; i < stepsToTake; i++)
        _system->takeStep(_solver, &_constraints, _timeStep);
    double timeLeft = netTime - stepsToTake*_timeStep;
    if (timeLeft > 0)
        _system->takeStep(_solver, &_constraints, _timeStep);

}

double SimModel::getTimeStep() {
	return _timeStep;
}

void SimModel::draw() {
    vec3 a, b, c, na, nb, nc;

    //Make the simulation green
    GLfloat mat_diffuse[] = { 0.0, 1.0, 0.0, 1.0 };
    GLfloat mat_ambient[] = { 0.0, 0.1, 0.0, 1.0 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);

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

	_takeConst.push_back(vec3(0));
	_takeConst.push_back(vec3(0));

}

AniElliModel::~AniElliModel() {

}

void AniElliModel::advance(double netTime) {
    frametimers.switchToTimer("AniEliModel::advance");

	if(_count < _ellipsoids.size()-1){
			//cout<<"Drawing Frame: "<<_count+1<<endl;
			//cout<<_ellipsoids[_count].size()<<" ellipsoids"<<endl;
	    _count++;
		}
	else
		_count = 0;
	frametimers.switchToGlobal();
}

double AniElliModel::getTimeStep() {
	return _timeStep;
}

void AniElliModel::draw() {
//*
	for(unsigned int i = 0; i < _ellipsoids[_count].size(); i++) {

		glPushMatrix();

		GLdouble m[16];
		int count = 0;
		for(int j = 0; j < 4; j++)
			for(int k = 0; k < 4; k++) {
				m[count] = _ellipsoids[_count][i][j][k];
				count++;
				//cout<<m[j*(k+1)]<<endl;
			}
		//exit(1);
		glMultMatrixd(m);
		glutSolidSphere(1, 100, 100);

		glPopMatrix();
	}
//*/
}

mat4 AniElliModel::getEllipsoid(int Indx) { return _ellipsoids[_count][Indx]; }
