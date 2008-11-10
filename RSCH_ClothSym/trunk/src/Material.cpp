/*
 * Material.cpp
 *
 *  Created on: Oct 23, 2008
 *      Author: njoubert
 */

#include "Material.h"

Material::Material() {
   _ks = Ke;
   _kd = Kd;
   _kbe = KBe;
   _kbd = KBd;
}

Material::~Material() {
    // empty
}

float Material::getKe() { return _ks; }
float Material::getKd() { return _kd; }
float Material::getKBe() { return _kbe; }
float Material::getKBd() { return _kbd; }
