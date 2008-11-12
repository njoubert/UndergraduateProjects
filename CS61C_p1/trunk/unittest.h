#ifndef UNITTEST_H_
#define UNITTEST_H_

#include <stdio.h>
#include "bignum.h"

int returnsomething();

int assert_equal_int(int expected, int given, char* message);

int assert_equal_bignum(BigNumPtr expected, BigNumPtr given, char* message);

#endif /*UNITTEST_H_*/
