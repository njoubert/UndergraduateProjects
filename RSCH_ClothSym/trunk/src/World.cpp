/*
 * World.cpp
 *
 *  Created on: Oct 23, 2008
 *      Author: njoubert
 */

#include "World.h"

World::World() {
    _timeStep = 0.02;
    _time = 0;
}

World::~World() {

}

void World::animate(double netTime) {
    for (int j = 0; j < _models.size(); j++)
        _models[j]->takeStep(netTime);
    _time +=  netTime;
}

void World::draw() {
    for (int j = 0; j < _models.size(); j++)
        _models[j]->draw();
}

bool World::loadModel(string filename) {
    OBJParser parser;
    Mesh* mesh = parser.parseOBJ(filename);
    int verticeCount = myMesh->countVertices();
    sys = new System(myMesh, verticeCount);
}

