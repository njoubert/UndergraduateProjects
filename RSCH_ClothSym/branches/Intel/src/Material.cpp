/*
 * Material.cpp
 *
 *  Created on: Oct 23, 2008
 *      Author: njoubert
 */

#include "Material.h"

Material::Material() {
   _ks = DEFAULT_KS;
   _kd = DEFAULT_KD;
   _kbe = DEFAULT_KBE;
   _kbd = DEFAULT_KBD;
}

Material::~Material() {
    // empty
}
