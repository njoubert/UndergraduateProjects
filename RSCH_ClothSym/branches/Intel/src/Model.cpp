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
    //printDebug("Initialized Model Base Class.");
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

SimModel::SimModel(TriangleMesh* mesh,
		System* system, Solver* solver, Material* mat) : Model(mat) {
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
        _system->takeStep(_solver, &_constraints, _timeStep);
    double timeLeft = netTime - stepsToTake*_timeStep;
    if (timeLeft > 0)
        _system->takeStep(_solver, &_constraints, _timeStep);
}

void SimModel::registerConstraint(Constraint* c) {
    _constraints.push_back(c);
}

void SimModel::applyConstraints(double time, SparseMatrix* m_A, Physics_LargeVector* m_b) {
	for(int i = 0; i < _constraints.size(); i++) {
		_constraints[i]->applyConstraintToPoints();
		_constraints[i]->applyConstraintToSolverMatrices(time, m_A, m_b);
	}
}

TriangleMesh* SimModel::getMesh() const {
    return _mesh;
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
	//printDebug("Loading OBJ " << filename)
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

	_origin[0] = 0;
	_origin[1] = 0;
	_origin[2] = 0;
	_origin[3] = 1;
	_takeConst.push_back(vec3(0));
	_takeConst.push_back(vec3(0));
}

AniElliModel::~AniElliModel() {

}

void AniElliModel::advance(double netTime) {
	if(_count < _ellipsoids.size()-1){
			//cout<<"Drawing Frame: "<<_count+1<<endl;
			//cout<<_ellipsoids[_count].size()<<" ellipsoids"<<endl;
	    _count++;
		}
	else
		_count = 0;
/*
	//Get Position of Lead Point
	int markEllipsoid[2];
	markEllipsoid[0] = 5;
	markEllipsoid[1] = 7;
	vec4 temp;
	for(int i = 0; i < 2; i++){
		cout<<"got here: "<<i<<endl;
		cout<<markEllipsoid[i]<<endl;
		cout<< _origin  * _ellipsoids[_count][markEllipsoid[i]]<<endl;
		temp = _origin * _ellipsoids[_count][markEllipsoid[i]];
		cout<<temp<<endl;
		for(int j = 0; j < 3; j++)
			_takeConst[i][j] = temp[j];
	}
//*/



}

void AniElliModel::draw() {
	for(unsigned int i = 0; i < _ellipsoids[_count].size(); i++) {

		glPushMatrix();

		GLdouble m[_ellipsoids[_count].size()];
		int count = 0;
		for(int j = 0; j < 4; j++)
			for(int k = 0; k < 4; k++) {
				m[count] = _ellipsoids[_count][i][j][k];
				count++;
				//cout<<m[j*(k+1)]<<endl;
			}
		//exit(1);
		glMultMatrixd(m);
		glutSolidSphere(1, 10, 10);

		glPopMatrix();
	}
}

mat4 AniElliModel::getEllipsoid(int Indx) { return _ellipsoids[_count][Indx]; }
