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

void World::advance(double netTime) {
    for (int j = 0; j < _models.size(); j++)
        _models[j]->advance(netTime);
    _time +=  netTime;
}

void World::draw() {
    for (int j = 0; j < _models.size(); j++)
        _models[j]->draw();
}

bool World::loadStatModel(string filename) {
    OBJParser parser;
    TriangleMesh* mesh = parser.parseOBJ(filename);
}

bool World::loadSimModel(string filename) {
    OBJParser parser;
    TriangleMesh* mesh = parser.parseOBJ(filename);
    int verticeCount = mesh->countVertices();
    sys = new System(myMesh, verticeCount);
}

bool World::loadAniModel(string filename) {

}
