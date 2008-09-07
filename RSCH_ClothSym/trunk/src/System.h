/*
 * System.h
 *
 *  Created on: Sep 7, 2008
 *      Author: njoubert
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

/// \brief Represents the cloth mesh system
///
/// Contains all the data needed to store a cloth mesh.
class System {
public:
	System();
	virtual ~System();

	/// \brief Returns the dimensions of the system.
	int getDim();

	/// \brief Gets or Sets the current point in time.
	double getT();
	void setT(double t);

	/// \brief Returns the position vector of the system,
	/// with dimensions as reported by getDim().
	double* getX();

	/// \brief Sets the position vector of the system.
	/// x is assumed to have the same dimensions as reported by getDim()
	void setX(double*);

	/// \brief Evaluates the derivative at the current time.
	double* evalDeriv();

};

#endif /* SYSTEM_H_ */
