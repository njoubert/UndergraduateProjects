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
}

StatModel::~StatModel() {
	delete _mesh;
}


void StatModel::advance(double netTime) {
	return;
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

SimModel::SimModel(TriangleMesh* mesh, System* system, Solver* solver) {
	_mesh = mesh;
	_system = system;
	_solver = solver;
	_timeStep = DEFAULT_TIMESTEP;
}

SimModel::~SimModel() {
	delete _mesh;
	delete _system;
	delete _solver;
}

void SimModel::advance(double netTime) {
    int stepsToTake = floor(netTime / _timeStep);
    for (int i = 0; i < stepsToTake; i++)
        _system->takeStep(_solver, _timeStep);
    double timeLeft = netTime - stepsToTake*_timeStep;
    if (timeLeft > 0)
        _system->takeStep(_solver, _timeStep);
}

void SimModel::draw() {
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
 *   ANIMATED MODEL
 *
 *------------------------------------------------*/

AniModel::AniModel(string filename) {
	_filename = filename;
	_count = 0;
	_mesh = NULL;
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
