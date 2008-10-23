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

private:
    System* _system;
    TriangleMesh* _mesh;
    Material* _material;
};

#endif /* MODEL_H_ */
