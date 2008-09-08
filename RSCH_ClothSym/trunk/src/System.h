/*
 * System.h
 *
 *  Created on: Sep 7, 2008
 *      Author: njoubert
 */

#include "global.h"

#ifndef SYSTEM_H_
#define SYSTEM_H_


class Point {
public:
    double x, y, vx, vy;

};

/// \brief Represents the cloth mesh system
///
/// Contains all the data needed to store a cloth mesh.
class System {
public:
	System();
	virtual ~System();

	/// \brief Returns the dimensions of the system.
	std::vector<int> getDim();

	/// \brief Gets or Sets the current point in time.
	double getT();
	void setT(double t);

	/// \brief Returns the position vector of the system,
	/// with dimensions as reported by getDim().
	std::vector< std::vector< Point > > * getX();

	/// \brief Sets the position vector of the system.
	void setX(std::vector< std::vector< Point > > *, std::vector<int> dim);

	/// \brief Evaluates the derivative at the current time.
	std::vector<Point> * evalDeriv();

	/// \brief Draws the system using OpenGL. Assumes a window exists.
	void draw();

private:
    std::vector< std::vector<Point> > x;

};

#endif /* SYSTEM_H_ */
