/*
 * Model.h
 *
 *  Created on: Oct 23, 2008
 *      Author: njoubert
 */

#ifndef MODEL_H_
#define MODEL_H_

class Model;
class StatModel;
class SimModel;
class AniModel;
class AniElliModel;

#include "global.h"
#include "System.h"
#include "Mesh.h"
#include "Material.h"
#include "Constraint.h"
#include "io/OBJParser.h"

/**
 * Defines a model in our scene.
 * This class encapsulates everything about a certain model.
 * This includes:
 *  - Its Mesh (possibly animated)
 *  - Its Material Properties
 *  - Its System of internal force calculations
 *
 *  And it has the ability to change over time.
 */
class Model {
public:
    Model();
    Model(Material*);
    virtual ~Model();
    virtual void advance(double)=0;
    virtual void draw()=0;
	virtual double getTimeStep()=0;
protected:
    Material* _material;
    double _timeStep;
};

/**
 * Represents a static object.
 */
class StatModel : public Model {
public:
    StatModel(TriangleMesh*);
    ~StatModel();
    void advance(double);
    double getTimeStep();
    void draw();

private:
	TriangleMesh* _mesh;
};

/**
 * Represents an object that we need to simulate
 */
class SimModel : public Model {
public:
    SimModel(TriangleMesh*, System*, Solver*, Material*, double);
    ~SimModel();
    void advance(double);
    double getTimeStep();
    void draw();
    TriangleMesh* getMesh() const;
    void registerConstraint(Constraint*);

private:
    Solver* _solver;
    System* _system;
    TriangleMesh* _mesh;
    double _timeStep;
    vector<Constraint*> _constraints;
};

/**
 * Represents an object with frames given to us.
 */
class AniModel : public Model {
public:
    AniModel(string);
    ~AniModel();
    void advance(double);
     double getTimeStep();
    void draw();

private:
	TriangleMesh* getCurrentMesh();
	void stepToNextMesh();
	void loadMeshAtNextBufferPos(string filename);

private:
	string _filename; //the template filename from which to iterate.
	int _count;
//	int _currenti; //current position in buffer
//	int _lasti; //last valid position in buffer
	TriangleMesh* _mesh;
	double _timeStep;
//	TriangleMesh* _buffer[DEFAULT_MESHBUFFERSIZE]; //This will be a circular buffer
};

/**
 * Represents an object made of ellipsoids with frames given to us.
 */
class AniElliModel : public Model {
public:
	AniElliModel(vector < vector <mat4> >);
    ~AniElliModel();
    void advance(double);
    double getTimeStep();
    void draw();

    mat4 getEllipsoid(int Indx);
    int getSize();

private:
	//TriangleMesh* getCurrentMesh();
	//void stepToNextMesh();
	//void loadMeshAtNextBufferPos(string filename);

private:
	string _filename; //the template filename from which to iterate.
	unsigned int _count;
	vector < vector <mat4> > _ellipsoids;
	vector<vec3> _takeConst;
	double _timeStep;
//	int _currenti; //current position in buffer
//	int _lasti; //last valid position in buffer
	//TriangleMesh* _mesh;
//	TriangleMesh* _buffer[DEFAULT_MESHBUFFERSIZE]; //This will be a circular buffer
};


#endif /* MODEL_H_ */
