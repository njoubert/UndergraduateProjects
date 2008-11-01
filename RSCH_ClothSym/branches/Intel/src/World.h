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
#include "OBJParser.h"
#include "EllipseParser.h"
#include "Solver.h"

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
    bool loadSimModel(string);
    bool loadAniModel(string);
    bool loadEllipseModel(string, int);
    bool createVertexToAnimatedEllipseContraint();

    double getTime();

private:
    bool addModel(Model* model);

private:
    vector<Model*> _models;
    double _time;
};

#endif /* WORLD_H_ */
