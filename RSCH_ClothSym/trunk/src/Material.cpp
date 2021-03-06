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

    makeDefaultColor();
}

Material::~Material() {
    // empty
}

void Material::makeDefaultColor() {
    mat_diffuse[0] = 0.1f;
    mat_diffuse[1] = 0.1f;
    mat_diffuse[2] = 0.9f;
    mat_diffuse[3] = 1.0f;
    mat_ambient[0] = 0.1f;
    mat_ambient[1] = 0.1f;
    mat_ambient[2] = 0.1f;
    mat_ambient[3] = 1.0f;
    mat_shininess[0] = 50;
    mat_specular[0] = 0.6f;
    mat_specular[1] = 0.6f;
    mat_specular[2] = 0.6f;
    mat_specular[3] = 1.0f;

}

void Material::makeSimColor() {
    mat_diffuse[0] = 20.0f / 255.0f;
    mat_diffuse[1] = 225.0f / 255.0f;
    mat_diffuse[2] = 20.0f / 255.0f;
    mat_diffuse[3] = 1.0f;
    mat_ambient[0] = 24.5f / 255.0f;
    mat_ambient[1] = 22.5f / 255.0f;
    mat_ambient[2] = 24.0f / 255.0f;
    mat_ambient[3] = 1.0f;
}
/*
void Material::makeEllipsoidColor() {
    mat_diffuse[0] = 0.1f;
    mat_diffuse[1] = 0.1f;
    mat_diffuse[2] = 0.9f;
    mat_diffuse[3] = 1.0f;
    mat_ambient[0] = 0.1f;
    mat_ambient[1] = 0.1f;
    mat_ambient[2] = 0.1f;
    mat_ambient[3] = 1.0f;
    mat_shininess[0] = 50;
    mat_specular[0] = 0.6f;
    mat_specular[1] = 0.6f;
    mat_specular[2] = 0.6f;
    mat_specular[3] = 1.0f;
}


void Material::makeSimWireframeColor() {
    mat_diffuse[0] = 20.0f / 255.0f;
    mat_diffuse[1] = 225.0f / 255.0f;
    mat_diffuse[2] = 20.0f / 255.0f;
    mat_diffuse[3] = 1.0f;
    mat_ambient[0] = 24.5f / 255.0f;
    mat_ambient[1] = 22.5f / 255.0f;
    mat_ambient[2] = 24.0f / 255.0f;
    mat_ambient[3] = 1.0f;
}

void Material::makeEllipsoidWireframeColor() {
    mat_diffuse[0] = 0.1f;
    mat_diffuse[1] = 0.1f;
    mat_diffuse[2] = 0.9f;
    mat_diffuse[3] = 1.0f;
    mat_ambient[0] = 0.1f;
    mat_ambient[1] = 0.1f;
    mat_ambient[2] = 0.1f;
    mat_ambient[3] = 1.0f;
    mat_shininess[0] = 50;
    mat_specular[0] = 0.6f;
    mat_specular[1] = 0.6f;
    mat_specular[2] = 0.6f;
    mat_specular[3] = 1.0f;
}

void Material::makeStatWireframeColor() {
    mat_diffuse[0] = 100.0f / 255.0f;
    mat_diffuse[1] = 100.0f / 255.0f;
    mat_diffuse[2] = 100.0f / 255.0f;
    mat_diffuse[3] = 1.0f;
    mat_ambient[0] = 8.5f / 255.0f;
    mat_ambient[1] = 8.5f / 255.0f;
    mat_ambient[2] = 10.2f / 255.0f;
    mat_ambient[3] = 1.0f;
    mat_shininess[0] = 20;
    mat_specular[0] = 5.8f;
    mat_specular[1] = 5.8f;
    mat_specular[2] = 5.8f;
    mat_specular[3] = 1.0f;
}
*/

void Material::makeCollisionColor() {
    mat_diffuse[0] = 255.0f / 255.0f;
    mat_diffuse[1] = 20.0f / 255.0f;
    mat_diffuse[2] = 20.0f / 255.0f;
    mat_diffuse[3] = 1.0f;
    mat_ambient[0] = 255.0f / 255.0f;
    mat_ambient[1] = 10.0f / 255.0f;
    mat_ambient[2] = 10.0f / 255.0f;
    mat_ambient[3] = 1.0f;
}

void Material::makeNormalsColor() {
    mat_diffuse[0] = 0.0f / 255.0f;
    mat_diffuse[1] = 0.0f / 255.0f;
    mat_diffuse[2] = 0.0f / 255.0f;
    mat_diffuse[3] = 1.0f;
    mat_ambient[0] = 255.0f / 255.0f;
    mat_ambient[1] = 10.0f / 255.0f;
    mat_ambient[2] = 255.0f / 255.0f;
    mat_ambient[3] = 1.0f;
}

void Material::makeMeshDiffColor() {
    mat_diffuse[0] = 0.0f / 255.0f;
    mat_diffuse[1] = 0.0f / 255.0f;
    mat_diffuse[2] = 0.0f / 255.0f;
    mat_diffuse[3] = 1.0f;
    mat_ambient[0] = 255.0f / 255.0f;
    mat_ambient[1] = 255.0f / 255.0f;
    mat_ambient[2] = 255.0f / 255.0f;
    mat_ambient[3] = 1.0f;
}

void Material::makeStatColor() {
    mat_diffuse[0] = 100.0f / 255.0f;
    mat_diffuse[1] = 100.0f / 255.0f;
    mat_diffuse[2] = 100.0f / 255.0f;
    mat_diffuse[3] = 1.0f;
    mat_ambient[0] = 8.5f / 255.0f;
    mat_ambient[1] = 8.5f / 255.0f;
    mat_ambient[2] = 10.2f / 255.0f;
    mat_ambient[3] = 1.0f;
    mat_shininess[0] = 20;
    mat_specular[0] = 5.8f;
    mat_specular[1] = 5.8f;
    mat_specular[2] = 5.8f;
    mat_specular[3] = 1.0f;
    /*    mat_diffuse[0] = 85.0f / 255.0f;
    mat_diffuse[1] = 85.0f / 255.0f;
    mat_diffuse[2] = 102.0f / 255.0f;
    mat_diffuse[3] = 1.0f;
    mat_ambient[0] = 8.5f / 255.0f;
    mat_ambient[1] = 8.5f / 255.0f;
    mat_ambient[2] = 10.2f / 255.0f;
    mat_ambient[3] = 1.0f;
*/
}



void Material::setGLcolors() {
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}

float Material::getKe() {
    return _ks;
}
float Material::getKd() {
    return _kd;
}
float Material::getKBe() {
    return _kbe;
}
float Material::getKBd() {
    return _kbd;
}
GLfloat* Material::getMatDiffuse() {
    return mat_diffuse;
}
GLfloat* Material::getMatAmbient() {
    return mat_ambient;
}
GLfloat* Material::getMatSpecular() {
    return mat_diffuse;
}
GLfloat* Material::getMatShininess() {
    return mat_ambient;
}
