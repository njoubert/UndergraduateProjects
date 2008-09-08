/*
 * System.cpp
 *
 *  Created on: Sep 7, 2008
 *      Author: njoubert
 */

#include "System.h"

System::System() {

};

System::~System() {

};

std::vector<int> System::getDim() {

};

double System::getT() {

};

void System::setT(double t) {

};

std::vector< std::vector< Point > > * System::getX() {

};

void System::setX(std::vector< std::vector< Point > > * newX, std::vector<int> dim) {
    //delete our current x
    x.clear();

    //create a new x of same dim as given
    for (int i = 0; i < dim[0]; i++)
        x.push_back(std::vector< Point >(dim[1]));

    //copy!
    for (int i = 0; i < dim[0]; i++) {
        for (int j = 0; j < dim[1]; j++) {
            x[i][j] = (*newX)[i][j];
        }
    }
};

std::vector<Point> * System::evalDeriv() {

};

void System::draw() {

    glBegin(GL_POINTS);
    for (int i = 0; i < x.size(); i++) {
        for (int j = 0; j < x[i].size(); j++) {
            glColor3f(0.0f, 0.0f, 0.0f);
            glVertex2f(x[i][j].x, x[i][j].y);
        }
    }
    glEnd();

}
