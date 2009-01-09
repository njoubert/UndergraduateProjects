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
class VertexToEllipseCollision;

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
    void applyInitialConstraints();
    void registerCollision(VertexToEllipseCollision*);

    void enableMouseForce(vec3);
    void updateMouseForce(vec3);
    void disableMouseForce();
    bool isMouseEnabled();

private:
    Solver* _solver;
    System* _system;
    TriangleMesh* _mesh;
    double _timeStep;
    vector<Constraint*> _constraints;
    vector<VertexToEllipseCollision*> _collisions;
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
	AniElliModel(std::pair<  vector < vector <mat4> > , vector < vector <vec3> > >);
    ~AniElliModel();
    void advance(double);
    double getTimeStep();
    void draw();

    //FOR CONSTRAINTS
    mat4 getEllipsoid(int Indx);
    int getSize();

    //FOR COLLISIONS
    vec3 calcAngularVel(int i, vec3 Xc_world);
    vec3 calculateAngularVelocity(int, vec3);
    mat4 calculateRotationMatrix(int i);
    vec3 getOrigin(int i);
    vec3 getPastOrigin(int i);
    vec3 getFutureOrigin(int i);
    vec4 convertPoint2ElliSpace(int, vec3);
    vec3 getPointInsideElli2Surface(int, vec4);
    bool isPointInsideElli(int, vec4);
    vec3 getPointInFuture(int i, vec3 x_WorldSpace_3);
    vec3 getPointInPast(int i, vec3 x_WorldSpace_3);

    //FOR FRICTION
    vec3 getNormal(int j, vec4 );
    vec3 getNormal(int j, vec3 );
    void setNormal(vec3, vec3);
    double getMu_s();
    double getMu_d();

private:
	//TriangleMesh* getCurrentMesh();
	//void stepToNextMesh();
	//void loadMeshAtNextBufferPos(string filename);

private:
	string _filename; //the template filename from which to iterate.
	unsigned int _count;
	vector < vector <mat4> > _ellipsoids;
	vector < vector <vec3> > _elliRots;
	vector<vec3> _takeConst;
	double _timeStep;
	vector < vec3 > _normalPos;
	vector < vec3 > _normalDir;

	//Collision Variables
	vec4 _origin;
	double _linearVelocity;
	double _angularVelocity;

	//Friction Variables
	double _muS;
	double _muD;
};


#endif /* MODEL_H_ */
