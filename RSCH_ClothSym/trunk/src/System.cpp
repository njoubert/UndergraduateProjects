/*
 * System.cpp
 *
 *  Created on: Sep 7, 2008
 *      Author: njoubert
 */

#include "System.h"

Force::~Force() {

};

GravityForce::GravityForce() {
    type = GRAVITY;
}
void GravityForce::apply(std::vector< std::vector < Particle > > * x) {
    (*x)[u1][v1].fy += -1*(*x)[u1][v1].m*((double)0.0000667428);
    //std::cout << "mass of particle = " << (*x)[u1][v1].m << ", force = " << (*x)[u1][v1].fy << std::endl;

};

DragForce::DragForce() {
    type = DRAG;
}
void DragForce::apply(std::vector< std::vector < Particle > > * x) {

};

SpringForce::SpringForce() {
    type = SPRING;
}
void SpringForce::apply(std::vector< std::vector < Particle > > * x) {
    double lx = (*x)[u1][v1].x - (*x)[u2][v2].x;
    double ly = (*x)[u1][v1].y - (*x)[u2][v2].y;
    double ll = sqrt(lx*lx + ly*ly);
    double vlx = (*x)[u1][v1].vx - (*x)[u2][v2].vx;
    double vly = (*x)[u1][v1].vy - (*x)[u2][v2].vy;
    //double vll = sqrt(vlx*vlx + vly*vly);

    double fk = -1*(ks*(ll - r) + kd*(lx*vlx + ly*vly)/ll);
    double fx = fk*(lx/ll);
    double fy = fk*(ly/ll);

    //std::cout << "spring force fx=" << fx << " fy=" << fy << std::endl;

    (*x)[u1][v1].fx += fx;
    (*x)[u1][v1].fy += fy;
    (*x)[u2][v2].fx += -1*fx;
    (*x)[u2][v2].fy += -1*fy;
};

InputForce::InputForce() {
    type = INPUT;
    enabled = false;
    ks = 0.01;
    kd = 0.82;
    r = 2;
}

void InputForce::apply(std::vector< std::vector < Particle > > * x) {
    if (!enabled)
        return;
    double lx = (*x)[u1][v1].x - xi;
    double ly = (*x)[u1][v1].y - yi;
    double ll = sqrt(lx*lx + ly*ly);
    double vlx = (*x)[u1][v1].vx;
    double vly = (*x)[u1][v1].vy;
    //double vll = sqrt(vlx*vlx + vly*vly);

    double fk = -1*(ks*(ll - r) + kd*(lx*vlx + ly*vly)/ll);
    double fx = fk*(lx/ll);
    double fy = fk*(ly/ll);

    //std::cout << "spring force fx=" << fx << " fy=" << fy << std::endl;

    (*x)[u1][v1].fx += fx;
    (*x)[u1][v1].fy += fy;
};

System::System() {
    t = 0;

};

System::~System() {

};

void System::setDim(int u, int v) {
    x.clear();
    _dim.clear();
    _dim.push_back(u);
    _dim.push_back(v);
}

std::vector<int> System::getDim() {
    return _dim;
};

double System::getT() {
    return t;
};

void System::setT(double t) {
    this->t = t;
};

std::vector< std::vector< Particle > > * System::getX() {
    std::vector< std::vector< Particle > > * r = new std::vector< std::vector< Particle > >();

    //create a new x of same dim as given
    for (int i = 0; i < _dim[0]; i++)
        r->push_back(std::vector< Particle >(_dim[1]));

    //copy!
    for (int i = 0; i < _dim[0]; i++) {
        for (int j = 0; j < _dim[1]; j++) {
            (*r)[i][j] = x[i][j];
        }
    }

    return r;

};

void System::setX(std::vector< std::vector< Particle > > * newX) {
    //delete our current x
    x.clear();

    //create a new x of same dim as given
    for (int i = 0; i < _dim[0]; i++)
        x.push_back(std::vector< Particle >(_dim[1]));

    //copy!
    for (int i = 0; i < _dim[0]; i++) {
        for (int j = 0; j < _dim[1]; j++) {
            x[i][j] = (*newX)[i][j];
        }
    }
};

std::vector< std::vector< Particle > > * System::evalDeriv() {
    std::vector< std::vector< Particle > > * r = new std::vector< std::vector< Particle > >();

    for (int i = 0; i < _dim[0]; i++)
        r->push_back(std::vector< Particle >(_dim[1]));

    //slidesc p17
    for (int i = 0; i < _dim[0]; i++) {
        for (int j = 0; j < _dim[1]; j++) {
            (*r)[i][j] = x[i][j];
            (*r)[i][j].fx = (*r)[i][j].fy = (*r)[i][j].fz = 0;
        }
    }

    for (unsigned int i = 0; i < _forces.size(); i++) {
        _forces[i]->apply(r);
    }

    return r;
};

void System::draw() {

    glColor3f(1.0f,0.0f,0.0f);

        //**
    SpringForce * f;
    for (unsigned int i = 0; i < _forces.size(); i++) {
        if (_forces[i]->type == SPRING) {
            glBegin(GL_LINES);
                glVertex2f(x[_forces[i]->u1][_forces[i]->v1].x, x[_forces[i]->u1][_forces[i]->v1].y);
                glVertex2f(x[((SpringForce*)_forces[i])->u2][((SpringForce*)_forces[i])->v2].x,
                        x[((SpringForce*)_forces[i])->u2][((SpringForce*)_forces[i])->v2].y);
            glEnd();

        }
    }
        // */
        //**
        for (unsigned int i = 0; i < x.size(); i++) {
            for (unsigned int j = 0; j < x[i].size(); j++) {
                glColor3f(0.0f, 0.0f, 0.0f);
                double px = x[i][j].x;
                double py = x[i][j].y;
                glBegin(GL_POLYGON);

                glVertex2f(px+1, py+1);
                glVertex2f(px-1, py+1);
                glVertex2f(px-1, py-1);
                glVertex2f(px+1, py-1);
                glEnd();

            }
        }

        // */
}


void System::addForce(Force* f) {
    _forces.push_back(f);
    //printDebug("Added force to particle (" << f->u1 << "," << f->v1 << ")");
}


Particle* System::getClosestParticle(double xp, double yp, int* u1, int* v1) {
    Particle* closest;
    double dist = 999999;
    for (unsigned int i = 0; i < x.size(); i++) {
        for (unsigned int j = 0; j < x[i].size(); j++) {
            double yd = (yp - x[i][j].y);
            double xd = (xp - x[i][j].x);
            double ndist = sqrt(xd*xd + yd*yd);
            if (ndist < dist) {
                *(u1) = i;
                *(v1) = j;
                dist = ndist;
                closest = &(x[i][j]);
            }

        }
    }
    //std::cout << xp << "," << yp << " selected dist = " << dist << std::endl;
    return closest;
}

void System::copyX(std::vector< std::vector< Particle > > * original,
        std::vector< std::vector< Particle > > * copy) {



}

