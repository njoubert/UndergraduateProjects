/*
 * World.h
 *
 *  Created on: Oct 23, 2008
 *      Author: njoubert
 */

#ifndef WORLD_H_
#define WORLD_H_

#include "global.h"
#include "Model.h"
#include "Mesh.h"
#include "Constraint.h"
#include "io/EllipseParser.h"

/**
 * The main class concerned with everything in our world.
 * Here we store all our models, and all the information about our world.
 */
class World {
public:
    World();
    virtual ~World();

    // Moves the animation of the world forward.
    void advance(double);

    // Draws the world using OpenGL
    void draw();

    // Loads up a new module from disk.
    bool loadStatModel(string);
    bool loadSimModel(TriangleMesh*, double, float);
    bool loadAniModel(string);
    bool loadEllipseModel(string, int);

    bool createVertexToAnimatedEllipseContraint();
    bool createFixedVertexContraints();
    bool createVertexToAnimatedEllipseCollisions();

    double getTime();

    vec3 getFocusPoint();

    void enableMouseForce(vec3);
    void disableMouseForce();
    bool isMouseEnabled();
    void updateMouseForce(vec3);

private:
    bool addModel(Model* model);

private:
    vector<StatModel*> _statmodels;
    vector<Model*> _models;
    double _time;
    bool _displayListsValid;
    int _displayListsIndex;
};

#endif /* WORLD_H_ */
