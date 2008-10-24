/*
 * Model.h
 *
 *  Created on: Oct 23, 2008
 *      Author: njoubert
 */

#include "global.h"
#include "System.h"
#include "Mesh.h"
#include "Material.h"
#include "io/OBJParser.h"

#ifndef MODEL_H_
#define MODEL_H_

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

protected:
    Material* _material;
};

/**
 * Represents a static object.
 */
class StatModel : public Model {
public:
    StatModel(TriangleMesh*);
    ~StatModel();
    void advance(double);
    void draw();

private:
	TriangleMesh* _mesh;
};

/**
 * Represents an object that we need to simulate
 */
class SimModel : public Model {
public:
    SimModel(TriangleMesh*, System*, Solver*, Material*);
    ~SimModel();
    void advance(double);
    void draw();

private:
    Solver* _solver;
    System* _system;
    TriangleMesh* _mesh;
    double _timeStep;
};

/**
 * Represents an object with frames given to us.
 */
class AniModel : public Model {
public:
    AniModel(string);
    ~AniModel();
    void advance(double);
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
//	TriangleMesh* _buffer[DEFAULT_MESHBUFFERSIZE]; //This will be a circular buffer
};

#endif /* MODEL_H_ */
