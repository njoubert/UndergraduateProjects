#ifndef DEBUG_C_
#define DEBUG_C_

#include <iostream>
#include <string>
#include <time.h>
#include <assert.h>

using namespace std;

#define printDebug(A, B)	if (DEBUG >= A) { cout << "  "; cout << clock()*1000/CLOCKS_PER_SEC; cout <<"ms: " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": " << B << endl; }
#define printStartDebug(A)	if (DEBUG) { cout << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": " << A; }
#define printPartialDebug(A)	if (DEBUG) { cout << A; }

#define printError(A) 	cerr << "!!!  " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": " << A << endl;

#define printInfo(A)	if (INFO) {cout << clock()*1000/CLOCKS_PER_SEC; cout << "ms: " << A << endl; }

#define printInfoChar(a)    if (INFO) { cout << a; }

int DEBUG = 0;

int INFO = 1;

#endif /*DEBUG_C_*/
