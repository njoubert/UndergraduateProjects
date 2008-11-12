#include <stdio.h>
#include <stdlib.h>


#include "unittest.h"
#include "bignum.h"

void test_add() {
	BigNumPtr a = BigNum_new();
	BigNumPtr b = BigNum_new();
	BigNumPtr result;
	BigNumPtr expected = BigNum_new();

	BigNum_readchar(a, "12126.577");
	BigNum_readchar(b, "12195.62");
	BigNum_readchar(expected, "24322.197");
	result = BigNum_add(a, b);
	assert_equal_bignum(expected, result, "Bignum Addition doesnt work right for small nr!");

	BigNum_delete(a);
	BigNum_delete(b);
	BigNum_delete(result);
	BigNum_delete(expected);
	a = BigNum_new();
	b = BigNum_new();
	result = BigNum_new();
	expected = BigNum_new();

	BigNum_readchar(a, "999.999999999999999999999");
	BigNum_readchar(b, "0.000000000000000000001");
	//BigNum_readchar(
	result = BigNum_add(a, b);
	BigNum_readchar(expected, "1000");
	assert_equal_bignum(expected, result, "addition working for you?");

	BigNum_delete(a);
	BigNum_delete(b);
	BigNum_delete(result);
	BigNum_delete(expected);
	a = BigNum_new();
	b = BigNum_new();
	result = BigNum_new();
	expected = BigNum_new();

	BigNum_readchar(a, "-10.5");
	BigNum_readchar(b, "-6.55");
	result = BigNum_add(a, b);
	BigNum_readchar(expected, "-17.05");
	assert_equal_bignum(expected, result, "adding two negative numbers doesnt work.");

	BigNum_delete(a);
	BigNum_delete(b);
	BigNum_delete(result);
	BigNum_delete(expected);
	a = BigNum_new();
	b = BigNum_new();
	result = BigNum_new();
	expected = BigNum_new();

	BigNum_readchar(a, "999.90145432114123459");
	BigNum_readchar(b, "999.90145432114123459");
	result = BigNum_add(a, b);
	BigNum_print(result);
	vector_print(result->whole);
	printf("\n");
	vector_pprint(result->fraction);
	printf("\n");
	BigNum_readchar(expected, "1999.80290864228246918");
	BigNum_print(expected);
	vector_pprint(expected->fraction);
	vector_pprint(expected->whole);
	assert_equal_bignum(expected, result, "adding two negative numbers doesnt work.");
};

void test_subtract() {

};

void test_multiply() {

};

void test_exponent() {

};

int main(int argc, char **argv) {

	printf("BigNum Test Start\n");
	BigNumPtr a = BigNum_new();
	BigNumPtr b = BigNum_new();
	assert_equal_int(1, BigNum_readchar(a, "-1545003333444.5678995656994949494949494949"), "Reading Chars Doesnt Works");
	assert_equal_int(12, a->whole->last, "Doesnt read in whole chars correctly.");
	assert_equal_int(27, a->fraction->last, "Doesnt read in fraction correctly.");
	assert_equal_int(1, BigNum_readchar(b, "0.9999999999999999999999999999999999999999999"), "Reading Chars Doesnt Work");
	assert_equal_int(0, b->whole->last, "Doesnt read in whole chars correctly.");
	assert_equal_int(42, b->fraction->last, "Doesnt read in fraction correctly.");

	BigNum_delete(a);
	BigNum_delete(b);
	a = BigNum_new();
	b = BigNum_new();

	assert_equal_bignum(a, b, "");

	test_add();
	test_subtract();
	test_multiply();
	test_exponent();

	BigNumPtr c = BigNum_new();
	assert_equal_int(0, BigNum_readchar(c, "-1.5aba556"), "Read in ONLY numbers!");
	printf("BigNum Test Done\n");

	return 0;
}

