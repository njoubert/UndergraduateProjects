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

    void makeDefaultColor();
    void makeSimColor();
    void makeStatColor();
    void setGLcolors();

    float getKe();
    float getKd();
    float getKBe();
    float getKBd();
    GLfloat* getMatDiffuse();
    GLfloat* getMatAmbient();
    GLfloat* getMatSpecular();
    GLfloat* getMatShininess();
private:
    float _ks;
    float _kd;
    float _kbe;
    float _kbd;
    GLfloat mat_ambient[4];
    GLfloat mat_diffuse[4];
    GLfloat mat_specular[4];
    GLfloat mat_shininess[1];

};

#endif /* MATERIAL_H_ */
