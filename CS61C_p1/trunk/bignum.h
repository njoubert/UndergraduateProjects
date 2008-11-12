/*
 * This is the header file for the data structure for working with big numbers
 * */

#ifndef BIGNUM_H_
#define BIGNUM_H_

#include <stdio.h>
#include <stdlib.h>

#include "vector.h"

#define NEG_SIGN 45
#define POS_SIGN 43

typedef struct big_num {
	char sign;
	VectorPtr whole;
	VectorPtr fraction;
} BigNum;

typedef BigNum* BigNumPtr;

/* Create a new blank BigNum */
BigNumPtr BigNum_new();

/* Delete a BigNum */
void BigNum_delete();

int BigNum_readchar(BigNumPtr bignum, char* string);

int BigNum_print(BigNumPtr bignum);

BigNumPtr BigNum_add(BigNumPtr a, BigNumPtr b);

BigNumPtr BigNum_subtract(BigNumPtr a, BigNumPtr b);

BigNumPtr BigNum_multiply(BigNumPtr a, BigNumPtr b);

BigNumPtr BigNum_exponent(BigNumPtr a, BigNumPtr b);

#endif /*BIGNUM_H_*/
