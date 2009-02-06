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
    bool loadSimModel(TriangleMesh*, double, float, string);
    bool loadAniModel(string);
    bool loadEllipseModel(string, int);

    bool initializeMeshSyncing(int, int);
    bool enableMeshSyncing(int, int);
    bool disableMeshSyncing(int);

    bool createVertexToAnimatedEllipseContraints(vector < vector < std::pair < int, int > > >);
    bool createVertexToAnimatedEllipseContraint();
    bool createFixedVertexContraints();
    bool createVertexToAnimatedEllipseCollisions();

    double getTime();

    vec3 getFocusPoint();

    void inializeExportSim(string directory, double inverseFPS);
    void exportSim(int simNum, double time, bool JustDoIt, double inverseFPS);

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

    //For Obj Output
    int _frameCount;
    double _lastTime;
    bool _doObjOutput;
    string _objOutDir;

    //For Syncing
    int _syncCounter;

    bool _displayListsValid;
    int _displayListsIndex;
};

#endif /* WORLD_H_ */
