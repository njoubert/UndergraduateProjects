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

   mat_diffuse[0] = 245.0f / 255.0f;
   mat_diffuse[1] = 245.0f / 255.0f;
   mat_diffuse[2] = 220.0f / 255.0f;
   mat_diffuse[3] = 1.0f;
   mat_ambient[0] = 245.0f / 255.0f;
   mat_ambient[1] = 245.0f / 255.0f;
   mat_ambient[2] = 220.0f / 255.0f;
   mat_ambient[3] = 1.0f;
}

Material::~Material() {
    // empty
}

float Material::getKe() { return _ks; }
float Material::getKd() { return _kd; }
float Material::getKBe() { return _kbe; }
float Material::getKBd() { return _kbd; }
GLfloat* Material::getMatDiffuse() { return mat_diffuse; }
GLfloat* Material::getMatAmbient() { return mat_ambient; }
