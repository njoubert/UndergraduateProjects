/*
 * testing.h
 *
 *  Created on: Nov 3, 2008
 *      Author: njoubert
 */

#ifndef TESTING_H_
#define TESTING_H_

const char TCOL_RESET[] = "\x1b[0m";
const char TRED[]     = "\x1b[31m";
const char TGREEN[]   = "\x1b[32m";
const char TYELLOW[]  = "\x1b[33m";
const char TBLUE[]    = "\x1b[34m";
const char TMAGENTA[] = "\x1b[35m";
const char TCYAN[]    = "\x1b[36m";

#define printCategory(A) { cout << endl << TBLUE << "=========================================" << endl << A << endl << "=========================================" << endl << endl << TCOL_RESET; }
#define printHeading(A)  { cout << TBLUE << "--------------------" << endl << A << endl << "--------------------" << endl << TCOL_RESET; }

#define printResult(A) { cout << TRED << A << endl << TCOL_RESET; }


#endif /* TESTING_H_ */
