#include <stdio.h>
#include "bignum.h"

int assert_equal_bignum(BigNumPtr expected, BigNumPtr given, char* message) {
	int i;
	if (expected->whole->last != given->whole->last) {
		fprintf(stderr, "Test Failed! Expected BigNum Whole	Length %d, Received Length %d", expected->whole->last+1, given->whole->last+1);
		//printMessage(message);
		return 1;
	}
	if (expected->fraction->last != given->fraction->last) {
		fprintf(stderr, "Test Failed! Expected BigNum Fraction Length %d, Received Length %d", expected->fraction->last+1, given->fraction->last+1);
		//printMessage(message);
		return 1;
	}
	i = 0;
	printf("Starting Whole Test\n");
	while (i <= expected->whole->last) {
		if (expected->whole->data[i] != given->whole->data[i]) {
			fprintf(stderr, "Test Failed! Expected BigNum Digit %d, Received Digit %d", expected->whole->data[i], given->whole->data[i]);
			//printMessage(message);
			return 1;
		}
		i++;
	}
	i = 0;
	printf("Starting Fractional Test\n");
	while (i <= expected->fraction->last) {
		if (expected->fraction->data[i] != given->fraction->data[i]) {
			fprintf(stderr, "Test Failed! Expected BigNum Digit %d, Received Digit %d", expected->whole->data[i], given->whole->data[i]);
			//printMessage(message);
			return 1;
		}
		i++;
	}
	return 0;
};

int main(int argc, char** argv) {
	BigNumPtr a = BigNum_new();
	BigNumPtr b = BigNum_new();
	char* input = (char*) malloc (20000000*sizeof(char));
	char c = 0;
	if (input == NULL) {
		printf("Couldn't get memory...");
	}
	int i = 0;
	while ((c = getchar()) != EOF && i < 20000000) {
		input[i] = c;
		i++;
	}
	BigNum_readchar(a, input);
	BigNum_readchar(b, input);

	BigNumPtr result = BigNum_add(a, b);

	printf("Numbers completely subtracted");
};
