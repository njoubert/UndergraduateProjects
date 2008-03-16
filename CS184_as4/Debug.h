#ifndef DEBUG_C_
#define DEBUG_C_

#include <iostream>

using namespace std;

#define printDebug(A)	if (DEBUG_F) { cout << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": " << A << endl; }
#define printStartDebug(A)	if (DEBUG_F) { cout << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": " << A; }
#define printPartialDebug(A)	if (DEBUG_F) { cout << A; }

int DEBUG;

#endif /*DEBUG_C_*/
