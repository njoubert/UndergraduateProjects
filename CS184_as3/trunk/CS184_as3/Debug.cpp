#ifndef DEBUG_C_
#define DEBUG_C_

#include <iostream>

using namespace std;

#define printDebug(A)	if (DEBUG) { cout << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": " << A << endl; }
#define printStartDebug(A)	if (DEBUG) { cout << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": " << A; }
#define printPartialDebug(A)	if (DEBUG) { cout << A; }

int DEBUG = 0;

#endif /*DEBUG_C_*/
