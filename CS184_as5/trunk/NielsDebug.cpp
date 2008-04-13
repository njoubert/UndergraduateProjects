#ifndef NIELSDEBUG_C_
#define NIELSDEBUG_C_

#include <iostream>
#include <string>
#include <time.h>
#include <assert.h>

#define NIELSDEBUG 2
#define NIELSINFO 1

using namespace std;

#define printDebug(A, B)	if (NIELSDEBUG >= A) { cout << "  "; cout << clock()*1000/CLOCKS_PER_SEC; cout <<"ms: " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": " << B << endl; }
#define printStartDebug(A)	if (NIELSDEBUG) { cout << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": " << A; }
#define printPartialDebug(A)	if (NIELSDEBUG) { cout << A; }

#define printError(A) 	cerr << "!!!  " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": " << A << endl;

#define printInfo(A)	if (NIELSINFO) {cout << ((long)clock())*((double)1000/CLOCKS_PER_SEC); cout << "ms: " << A << endl; }

#define printInfoChar(a)    if (NIELSINFO) { cout << a; }

#endif /*NIELSDEBUG_C_*/
