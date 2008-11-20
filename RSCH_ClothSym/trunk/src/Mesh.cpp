/*
 * Mesh.cpp
 *
 *  Created on: Sep 17, 2008
 *      Author: njoubert
 */

#include "Mesh.h"

/****************************************************************
*                                                               *
*               TriangleMeshVertex                              *
*                                                               *
****************************************************************/

TriangleMeshVertex::TriangleMeshVertex(double x, double y, double z, int i):
		_X(x,y,z),
		_Xdot(0.0,0.0,0.0),
		_Xdotdot(0.0,0.0,0.0),
		_m(1),
		_U(x,y,z),
		_hasvn(false),
		_hasvt(false),
		_index(i),
		_pinned(false),
		_edges() {
	//Intentionally left blank.
}

vec3 & TriangleMeshVertex::getX() { return _X; }
vec3 & TriangleMeshVertex::getXdot() { return _Xdot; }
vec3 & TriangleMeshVertex::getXdotdot() { return _Xdotdot; }
vec3 & TriangleMeshVertex::getU() { return _U; }
double TriangleMeshVertex::getm() { return _m; }
void TriangleMeshVertex::setm(double m) { _m = m; }

int TriangleMeshVertex::getIndex() { return _index; }

vec3 TriangleMeshVertex::getNormal() {
    std::vector<TriangleMeshEdge*>::const_iterator it =
        getEdgesBeginIterator();
    vec3 n(0,0,0);
    while (it != getEdgesEndIterator()) {
        n += (*it)->getParentTriangle(0)->getNormal();
        if ((*it)->isPartOfTwoTriangles())
            n += (*it)->getParentTriangle(1)->getNormal();
        it++;
    }
    return n.normalize();
}

bool TriangleMeshVertex::isPinned() { return _pinned; }
void TriangleMeshVertex::setPinned(bool fixed) { _pinned = fixed; }
int TriangleMeshVertex::countEdges() { return _edges.size(); }
void TriangleMeshVertex::addToEdge(TriangleMeshEdge* edge) { _edges.push_back(edge); }

std::vector<TriangleMeshEdge*>::const_iterator TriangleMeshVertex::getEdgesBeginIterator() {
    return _edges.begin();
}
std::vector<TriangleMeshEdge*>::const_iterator TriangleMeshVertex::getEdgesEndIterator() {
    return _edges.end();
}

/****************************************************************
*                                                               *
*               TriangleMeshVertexTexture                       *
*                                                               *
****************************************************************/

TriangleMeshVertexTexture::TriangleMeshVertexTexture(double u, double v, int i):
		_vt(u, v), _index(i) {
	//Empty
}
vec2 & TriangleMeshVertexTexture::getvt() {
	return _vt;
}

int TriangleMeshVertexTexture::getIndex() {
	return _index;
}
/****************************************************************
*                                                               *
*               TriangleMeshVertexNormal                        *
*                                                               *
****************************************************************/

TriangleMeshVertexNormal::TriangleMeshVertexNormal(double x, double y, double z, int i):
		_vn(x, y, z), _index(i) {
	//Empty
}
vec3 & TriangleMeshVertexNormal::getvn() {
	return _vn;
}

int TriangleMeshVertexNormal::getIndex() {
	return _index;
}


/****************************************************************
*                                                               *
*               TriangleMeshEdge                                *
*                                                               *
****************************************************************/

TriangleMeshEdge::TriangleMeshEdge(TriangleMesh* callingMesh, int v1,int v2) {
    TriangleMeshVertex *vt1, *vt2;
    vt1 = callingMesh->getVertex(v1);
    vt2 = callingMesh->getVertex(v2);
    vt1->addToEdge(this);
    vt2->addToEdge(this);
    _vertices[0] = vt1;
    _vertices[1] = vt2;
    _triangles[0] = _triangles[1] = NULL;
    _rl = (_vertices[0]->getU() - _vertices[1]->getU()).length();
}

double TriangleMeshEdge::getRestLength() { return _rl; }
double TriangleMeshEdge::getRestAngle() { return _theta0; }
void TriangleMeshEdge::setRestAngle(double theta) { this->_theta0 = theta; }

bool TriangleMeshEdge::isValid() {
    return ((_triangles[0] != NULL) || (_triangles[1] != NULL)) &&
        ((_vertices[0] != NULL) && (_vertices[1] != NULL));
}

int TriangleMeshEdge::addParentTriangle(TriangleMeshTriangle* parent) {
    if (_triangles[0] == NULL) {
        _triangles[0] = parent;
        return 0;
    } else if (_triangles[1] == NULL) {
        _triangles[1] = parent;
        return 1;
    } else
        return -1;
}

bool TriangleMeshEdge::isPartOfTwoTriangles() {
    meshDebug("checking for triangles...")
    return (_triangles[0] != NULL && _triangles[1] != NULL);
}

TriangleMeshTriangle* TriangleMeshEdge::getParentTriangle(int i) {
    if (i < 2 && i >= 0)
        return _triangles[i];
    return NULL;
}

TriangleMeshTriangle* TriangleMeshEdge::getOtherTriangle(TriangleMeshTriangle* one) {
    if (_triangles[0] == one)
        return _triangles[1];
    return _triangles[0];
}

TriangleMeshVertex* TriangleMeshEdge::getVertex(int i) {
    if (i < 2 && i >= 0)
        return _vertices[i];
    return NULL;
}

TriangleMeshVertex* TriangleMeshEdge::getOtherVertex(TriangleMeshVertex* one) {
    if (_vertices[0] == one)
        return _vertices[1];
    return _vertices[0];
}

bool TriangleMeshEdge::setParentTriangle(int i,
        TriangleMeshTriangle* parent) {
    if (i < 2 && i >= 0) {
        _triangles[i] = parent;
        return true;
    }
    return false;
}


/****************************************************************
*                                                               *
*               TriangleMeshTriangle                            *
*                                                               *
****************************************************************/

TriangleMeshTriangle::TriangleMeshTriangle(TriangleMesh* callingMesh,
        int v1,
        int v2,
        int v3) {
    _myParent = callingMesh;

    _hasvt = false;
    _hasvn = false;

    assert(_myParent != NULL);

    _vertices[0] = callingMesh->getVertex(v1);
    _vertices[1] = callingMesh->getVertex(v2);
    _vertices[2] = callingMesh->getVertex(v3);

    assert(_vertices[0] != NULL);
    assert(_vertices[1] != NULL);
    assert(_vertices[2] != NULL);

    _edges[0] = callingMesh->getEdgeBetweenVertices(v1,v2);
    _edges[1] = callingMesh->getEdgeBetweenVertices(v1,v3);
    _edges[2] = callingMesh->getEdgeBetweenVertices(v2,v3);

    assert(_edges[0] != NULL);
    assert(_edges[1] != NULL);
    assert(_edges[2] != NULL);

    meshDebug("Found edge 0: " << _edges[0]);
    meshDebug("Found edge 1: " << _edges[1]);
    meshDebug("Found edge 2: " << _edges[2]);

    if ((_edges[0]->addParentTriangle(this) < 0) ||
        (_edges[1]->addParentTriangle(this) < 0) ||
        (_edges[2]->addParentTriangle(this) < 0))
        std::cout << __FILE__ <<":"<<__LINE__<< "COULDN'T ADD TRIANGLE TO EDGE!" << std::endl;

}

bool TriangleMeshTriangle::isValid() {
    return (_edges[0] != NULL && _edges[0]->isValid()) &&
    (_edges[1] != NULL && _edges[1]->isValid()) &&
    (_edges[2] != NULL && _edges[2]->isValid());
}

vec3 TriangleMeshTriangle::getNormal() {
    vec3 ab = _vertices[1]->getX() - _vertices[0]->getX();
    vec3 ac = _vertices[2]->getX() - _vertices[0]->getX();
    vec3 cross = (ab^ac).normalize();
    return cross;
}

TriangleMeshEdge** TriangleMeshTriangle::getEdges() { return _edges; }
TriangleMeshVertex** TriangleMeshTriangle::getVertices() { return _vertices; }
TriangleMeshVertexTexture** TriangleMeshTriangle::getVerticeTextures() { return _verticetextures; }
TriangleMeshVertexNormal** TriangleMeshTriangle::getVerticeNormals() { return _verticenormals; }

void TriangleMeshTriangle::setVerticeTexture(TriangleMesh* callingMesh, int vt1, int vt2, int vt3) {
	_verticetextures[0] = callingMesh->getVertexTexture(vt1);
	_verticetextures[1] = callingMesh->getVertexTexture(vt2);
	_verticetextures[2] = callingMesh->getVertexTexture(vt3);
    assert(_verticetextures[0] != NULL);
    assert(_verticetextures[1] != NULL);
    assert(_verticetextures[2] != NULL);
	_hasvt = true;
}
void TriangleMeshTriangle::setVerticeNormals(TriangleMesh* callingMesh, int vn1, int vn2, int vn3) {
	_verticenormals[0] = callingMesh->getVertexNormal(vn1);
	_verticenormals[1] = callingMesh->getVertexNormal(vn2);
	_verticenormals[2] = callingMesh->getVertexNormal(vn3);
    assert(_verticenormals[0] != NULL);
    assert(_verticenormals[1] != NULL);
    assert(_verticenormals[2] != NULL);
	_hasvn = true;
}

bool TriangleMeshTriangle::hasVerticeTexture() {
	return _hasvt;
}
bool TriangleMeshTriangle::hasVerticeNormals() {
	return _hasvn;
}

ostream& operator <<(ostream& s, const TriangleMeshVertex* v) {
    s << "(";
    s << v->_U[0];
    s << ",";
    s << v->_U[1];
    s << ",";
    s << v->_U[2];
    s << ")";
    return s;
}

ostream& operator <<(ostream& s, const TriangleMeshEdge* e) {
    s << "[";
    s << e->_vertices[0] << "," << e->_vertices[1];
    s << "]";
    return s;
}

ostream& operator <<(ostream& s, const TriangleMeshTriangle* t) {
    s << "{";
    s << " E=" << t->_edges[0] << t->_edges[1] << t->_edges[2];
    s << " V=" << t->_vertices[0] << t->_vertices[1] << t->_vertices[2];
    s << "}";
    return s;
}

/*
bool operator <(TriangleEdgeKey& first, TriangleEdgeKey& second) {
    cout << "WEEEEEEEEEEEEEEEEEEEEEEEE" << endl;
    return false;
}
*/
/****************************************************************
*                                                               *
*               TriangleMesh                                    *
*                                                               *
****************************************************************/


TriangleMesh::TriangleMesh() {
    // TODO Auto-generated constructor stub
}

TriangleMesh::~TriangleMesh() {
	for (unsigned int i = 0; i < _triangles.size(); i++)
		delete _triangles[i];
	for (unsigned int i = 0; i < _vertices.size(); i++) {
		delete _vertices[i].first;
		for (unsigned int j = 0; j < _vertices[i].second->size(); j++)
			delete (*_vertices[i].second)[j].second;
	}
}

int TriangleMesh::createVertex(double x, double y, double z) {
    TriangleMeshVertex* newV = new TriangleMeshVertex(x,y,z, _vertices.size());
    _vertices.push_back(make_pair(newV,
            new vector<std::pair< int, TriangleMeshEdge*> >()));
    return _vertices.size() - 1;
}
int TriangleMesh::createVertexTexture(double u, double v){
    TriangleMeshVertexTexture* newV = new TriangleMeshVertexTexture(u,v,_verticetextures.size());
    _verticetextures.push_back(newV);
    return _verticetextures.size() - 1;
}
int TriangleMesh::createVertexNormal(double x, double y, double z){
    TriangleMeshVertexNormal* newV = new TriangleMeshVertexNormal(x, y, z,_verticenormals.size());
    _verticenormals.push_back(newV);
    return _verticenormals.size() - 1;
}

/**
 * This function creates a new triangle given vertice
 * indices. This is here to help parse .OBJ files.
 *
 * Input:
 *      A
 *     / \
 *    e1  e2
 *   /      \
 *  B---e3---C
 *
 * This runs the following algorithm:
 * //Get the given vertices
 *       //If they don't exist, error out
 *   //Get the 3 edges between these vectors
 *       //If any does not exist
 *           //Create them and add them
 *       //If any of them are already full - error out
 *   //Create the triangle
 *       //(This will take in given vertices and get edges.)
 * @return index of new triangle in triangle vector.
 */
int TriangleMesh::createTriangle(int A, int B, int C) {
    if (A > (int) _vertices.size() ||
            B > (int) _vertices.size() ||
            C > (int) _vertices.size())
        return -1;

    TriangleMeshVertex *vp1, *vp2, *vp3;
    TriangleMeshEdge *ep1, *ep2, *ep3;
    vp1 = _vertices[A].first;
    vp2 = _vertices[B].first;
    vp3 = _vertices[C].first;

    meshDebug("Vertices are okay. Starting Edge 1.");

    if (NULL == (ep1 = getEdgeBetweenVertices(A,B))) {
        ep1 = new TriangleMeshEdge(this, A,B);
        assert(insertEdgeForVertices(A,B,ep1));
    }

    meshDebug("Edge 1 is okay: "<< (int) ep1<<". Starting Edge 2.");

    if (NULL == (ep2 = getEdgeBetweenVertices(A,C))) {
        ep2 = new TriangleMeshEdge(this, A,C);
        assert(insertEdgeForVertices(A,C,ep2));
    }

    meshDebug("Edge 2 is okay. Starting Edge 3.");

    if (NULL == (ep3 = getEdgeBetweenVertices(B,C))) {
        ep3 = new TriangleMeshEdge(this, B,C);
        assert(insertEdgeForVertices(B,C,ep3));
    }

    meshDebug("Edge 3 is okay. Checking edge triangles...");

    if (ep1->isPartOfTwoTriangles() ||
            ep2->isPartOfTwoTriangles() ||
            ep3->isPartOfTwoTriangles())
        return -1;  //Cannot add a triangle if an edge already has 2 triangles.

    meshDebug("Edges are good!");

    TriangleMeshTriangle *t = new TriangleMeshTriangle(this,
            A, B, C);
    _triangles.push_back(t);
    return _triangles.size() - 1;
}

TriangleMeshTriangle* TriangleMesh::getTriangle(int i) {
    if (i < 0 || i > (int) _triangles.size()-1) {
#ifdef CATCHERRORS
    cout << "WTF you accessed a NULL triangle" << endl;
#endif
    	return NULL;
    }
    return _triangles[i];
}

TriangleMeshVertex* TriangleMesh::getVertex(int i) {
    if (i < 0 || i > (int) _vertices.size()-1)
        return NULL;
#ifdef CATCHERRORS
    assert(i == _vertices[i].first->getIndex());
#endif
    return _vertices[i].first;
}

TriangleMeshVertexTexture* TriangleMesh::getVertexTexture(int i) {
    if (i < 0 || i > (int) _verticetextures.size()-1)
        return NULL;
#ifdef CATCHERRORS
    assert(i == _verticetextures[i]->getIndex());
#endif
    return _verticetextures[i];
}

TriangleMeshVertexNormal* TriangleMesh::getVertexNormal(int i) {
    if (i < 0 || i > (int) _verticenormals.size()-1) {
#ifdef CATCHERRORS
    	cout << "getVertexNormal INCORRECT INDEX!" << endl;
#endif
    	return NULL;

    }
#ifdef CATCHERRORS
    assert(i == _verticenormals[i]->getIndex());
#endif

    return _verticenormals[i];
}

unsigned int TriangleMesh::countVertices() {
    return _vertices.size();
}

unsigned int TriangleMesh::countTextureVertices() { return _verticetextures.size(); }
unsigned int TriangleMesh::countNormalVertices() { return _verticenormals.size(); }

unsigned int TriangleMesh::countTriangles() {
    return _triangles.size();
}

void TriangleMesh::setGlobalMassPerParticle(double m) {
    cout << "Setting mass per particle = " << m << " over " << _vertices.size() << " vertices." << endl;
    TriangleMeshVertex *v;
    for (unsigned int i = 0; i < _vertices.size(); i++) {
        v = getVertex(i);
        v->setm(m);
    }
}

TriangleMeshEdge* TriangleMesh::getEdgeBetweenVertices(
        int v1,
        int v2) {
    assert(v1 >= 0 && v1 < (int)_vertices.size());
    assert(v2 >= 0 && v2 < (int)_vertices.size());
    TriangleMeshVertex *vt1, *vt2;
    vt1 = _vertices[v1].first;
    vt2 = _vertices[v2].first;
    applyNaturalOrdering(&vt1,&vt2,&v1,&v2);

    TriangleMeshEdge *ret = NULL;

    std::vector< std::pair <int, TriangleMeshEdge* > >::const_iterator it =
        _vertices[v1].second->begin();
    while (it != _vertices[v1].second->end()) {
        if ((*it).first == v2) {
            ret = (*it).second;
            meshDebug("Found edge!");
            break;
        }
        it++;
    }
    return ret;
}

bool TriangleMesh::insertEdgeForVertices(int v1,
        int v2,
        TriangleMeshEdge* e) {
    assert(v1 >= 0 && v1 < (int)_vertices.size());
    assert(v2 >= 0 && v2 < (int)_vertices.size());
    TriangleMeshVertex* vt1 = _vertices[v1].first;
    TriangleMeshVertex* vt2 = _vertices[v2].first;
    applyNaturalOrdering(&vt1,&vt2, &v1,&v2);

    meshDebug("Natural ordering applied...");

    //DEBUG PURPOSES:
    std::vector< std::pair <int, TriangleMeshEdge* > >::const_iterator it =
        _vertices[v1].second->begin();
    while (it != _vertices[v1].second->end()) {
        if ((*it).first == v2) {
            meshDebug("Edge already defined!!! WTF!!!");
            assert((*it).first != v2);
            return false;
        }
        it++;
    }
    //END DEBUG

    _vertices[v1].second->push_back(make_pair(v2, e));
    meshDebug("Inserted edge into vertices-edges datastructure");
    //if insert fails
    return true;
}

EdgesIterator TriangleMesh::getEdgesIterator() {
    return EdgesIterator(this);
}

std::vector<TriangleMeshTriangle*>::const_iterator TriangleMesh::getTrianglesBeginIterator() {
    return _triangles.begin();
}

/**
 * Dumps either the original file's data or the new position and normal to an ofstream.
 * Has the form v/vt/vn, v/vt or v//vn
 */
void TriangleMesh::dumpVertexToFile(ofstream & outfile, TriangleMeshTriangle* t, int v, bool exportOriginalMesh) {
	outfile << t->getVertices()[v]->getIndex() + 1;
	if (t->hasVerticeTexture())
		outfile << "/" << t->getVerticeTextures()[v]->getIndex() + 1;

	if (exportOriginalMesh) {
		if (t->hasVerticeNormals()) {
			if (!t->hasVerticeTexture())
				outfile << "/";
			outfile << "/" << t->getVerticeNormals()[v]->getIndex() + 1;
		}
	} else {
		if (!t->hasVerticeTexture())
				outfile << "/";
		outfile << "/" << t->getVertices()[v]->getIndex() + 1;
	}
}

void TriangleMesh::exportAsOBJ(string filename, bool exportOriginalMesh) {

	ofstream outfile(filename.c_str());

	outfile << "# OBJ file created by MESH" << endl;
	outfile << "# VERTICES FOLLOW. " << countVertices() << " vertices." << endl;
	outfile << "# -----------------------------------------" << endl;

	TriangleMeshVertex* v;
	for (unsigned int i = 0; i < countVertices(); i++) {
		v = getVertex(i);
		if (exportOriginalMesh) {
			outfile << "v " << v->getU()[0] << " " << v->getU()[1] << " "
					<< v->getU()[2] << endl;
		} else {
			outfile << "v " << v->getX()[0] << " " << v->getX()[1] << " "
					<< v->getX()[2] << endl;
		}
	}

	outfile << "# VERTEX NORMALS FOLLOW. " << _verticenormals.size() << " vertice normals."<< endl;
	outfile << "# -----------------------------------------" << endl;

	if (exportOriginalMesh) {
		for (unsigned int i = 0; i < _verticenormals.size(); i++) {
			outfile << "vn " << _verticenormals[i]->getvn()[0] << " " << _verticenormals[i]->getvn()[1] << " "
					<< _verticenormals[i]->getvn()[2] << endl;
		}
	} else {
		for (unsigned int i = 0; i < countVertices(); i++) {
			v = getVertex(i);
			outfile << "vn " << v->getNormal()[0] << " " << v->getNormal()[1] << " "
					<< v->getNormal()[2] << endl;
		}
	}


	outfile << "# VERTEX TEXTURE COORDINATES FOLLOW. " << _verticetextures.size() << " vertices."<< endl;
	outfile << "# -----------------------------------------" << endl;

		for (unsigned int i = 0; i < _verticetextures.size(); i++) {
			outfile << "vt " << _verticetextures[i]->getvt()[0] << " " << _verticetextures[i]->getvt()[1] << endl;
		}

	//OUTPUT FACES
	outfile << "# FACES FOLLOW" << endl;
	outfile << "# -----------------------------------------" << endl;

	//Iterate over faces (triangles).
	TriangleMeshTriangle* t;
	for (unsigned int i = 0; i < countTriangles(); i++) {
		t = getTriangle(i);
		outfile << "f ";

		dumpVertexToFile(outfile, t, 0, exportOriginalMesh);
		outfile << " ";
		dumpVertexToFile(outfile, t, 0, exportOriginalMesh);
		outfile << " ";
		dumpVertexToFile(outfile, t, 0, exportOriginalMesh);

	}

	outfile.close();

}

std::vector<TriangleMeshTriangle*>::const_iterator TriangleMesh::getTrianglesEndIterator() {
    return _triangles.end();
}

/**
 * Enforces a natural ordering:
 * x[0] < y[0]
 *  or x[0] == y[0] and x[1] < y[1]
 *  or x[0] == y[0] and x[1] == y[1] and x[2] < y[2]
 * @param v1
 * @param v2
 */
void TriangleMesh::applyNaturalOrdering(TriangleMeshVertex** vt1,
        TriangleMeshVertex** vt2, int* v1, int* v2) {
    bool swap = false;

    //First Coordinate:
    if ((*vt1)->getU()[0] < (*vt2)->getU()[0]) {
        return;
    } else {
        if ((*vt1)->getU()[0] == (*vt2)->getU()[0]) {

            //Second Coordinate:
            if ((*vt1)->getU()[1] < (*vt2)->getU()[1]) {
                return;
            } else {
                if ((*vt1)->getU()[1] == (*vt2)->getU()[1]) {

                    //Third Coordinate:
                    if ((*vt1)->getU()[2] < (*vt2)->getU()[2]) {
                        return;
                    } else {

                        if ((*vt1)->getU()[2] == (*vt2)->getU()[2]) {
                            return; //Two of the same vectors...
                        } else {
                            swap = true;
                        }
                    }

                } else {
                    swap = true;
                }
            }

        } else {
            swap = true;
        }
    }
    if (swap) {
        TriangleMeshVertex* tempP;
        tempP = *vt1;
        *vt1 = *vt2;
        *vt2 = tempP;

        int tempI;
        tempI = *v1;
        *v1 = *v2;
        *v2 = tempI;
    }
}

void printVertex(TriangleMesh * m, int i) {
    TriangleMeshVertex* t = m->getVertex(i);
    std::cout << "Point " << i;
    cout << " at " << t->getX();
    cout << " with " << t->countEdges() << " edges locally and ";
/*        << m->vertices[i].second->size() << " edges globally.";
       std::vector< std::pair <int, TriangleMeshEdge* > >::const_iterator it =
                    m->vertices[i].second->begin();
        while (it != m->vertices[i].second->end()) {
            cout << (*it).second;
            it++;
        }
        */
    cout << endl;

}

