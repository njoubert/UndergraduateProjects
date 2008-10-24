/*
 * Debug.h
 *
 *  Created on: Oct 23, 2008
 *      Author: njoubert
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include <iostream>

using namespace std;

#define printDebug(A)	if (Debug::DEBUG) { cout << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": " << A << endl; }
#define printStartDebug(A)	if (Debug::DEBUG) { cout << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": " << A; }
#define printPartialDebug(A)	if (Debug::DEBUG) { cout << A; }

class Debug {
public:
	static int DEBUG;
};

#endif /* DEBUG_H_ */
