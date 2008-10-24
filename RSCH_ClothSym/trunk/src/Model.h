/*
 * Model.h
 *
 *  Created on: Oct 23, 2008
 *      Author: njoubert
 */

#include "System.h"
#include "Mesh.h"
#include "Material.h"

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
    virtual ~Model();
    virtual void takeStep(double)=0;
    virtual void draw()=0;

protected:
    double _timeStep;
    Material* _material;
};

/**
 * Represents a static object.
 */
class StaticModel : public Model {
public:
    StaticModel();
    ~StaticModel();
    void takeStep(double);
    void draw();

private:

};

/**
 * Represents an object that we need to simulate
 */
class SimulatedModel : public Model {
public:
    SimulatedModel();
    ~SimulatedModel();
    void takeStep(double);
    void draw();

private:
    Solver* _solver;
    System* _system;
    TriangleMesh* _mesh;
};

/**
 * Represents an object with frames given to us.
 */
class AnimatedModel : public Model {
public:
    AnimatedModel();
    ~AnimatedModel();
    void takeStep(double);
    void draw();

private:
};

#endif /* MODEL_H_ */
