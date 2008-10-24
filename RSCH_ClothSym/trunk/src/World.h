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
#include "io/OBJParser.h"

/**
 * The main class concerned with everything in our world.
 * Here we store all our models, and all the information about our world.
 */
class World {
public:
    World();
    virtual ~World();

    // Moves the animation of the world forward.
    void animate(double);

    // Draws the world using OpenGL
    void draw();

    // Loads up a new module from disk.
    bool loadModel(string);

private:
    vector<Model*> _models;
    double _time;
    double _timeStep;
};

#endif /* WORLD_H_ */
