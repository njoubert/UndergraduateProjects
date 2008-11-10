/*
 * Material.h
 *
 *  Created on: Oct 23, 2008
 *      Author: njoubert
 */

#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "global.h"

/***
 * Represents material properties.
 */
class Material {
public:
    Material();
    virtual ~Material();
    float getKe();
    float getKd();
    float getKBe();
    float getKBd();
private:
    float _ks;
    float _kd;
    float _kbe;
    float _kbd;
};

#endif /* MATERIAL_H_ */
