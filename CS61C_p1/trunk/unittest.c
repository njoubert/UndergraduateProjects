
#include "unittest.h"

int returnsomething() {
	return 1;
}

void printMessage(char* message) {
	fprintf(stderr, message);
	fprintf(stderr, "\n");
};

int assert_equal_int(int expected, int given, char* message) {
	if (expected != given) {
		fprintf(stderr, "Test Failed! Expected %d, Recieved %d\n", expected, given);
		printMessage(message);
		return 1;
	} else {
		return 0;
	}
};

int assert_equal_bignum(BigNumPtr expected, BigNumPtr given, char* message) {
	int i;
	if (expected->whole->last != given->whole->last) {
		fprintf(stderr, "Test Failed! Expected BigNum Whole Length %d, Received Length %d\n", expected->whole->last+1, given->whole->last+1);
		printMessage(message);
		return 1;
	}
	if (expected->fraction->last != given->fraction->last) {
		fprintf(stderr, "Test Failed! Expected BigNum Fraction Length %d, Received Length %d\n", expected->fraction->last+1, given->fraction->last+1);
		printMessage(message);
		return 1;
	}
	i = 0;
	while (i <= expected->whole->last) {
		if (expected->whole->data[i] != given->whole->data[i]) {
			fprintf(stderr, "Test Failed! Expected BigNum Digit %d, Received Digit %d\n", expected->whole->data[i], given->whole->data[i]);
			printMessage(message);
			return 1;
		}
		i++;
	}
	i = 0;
	while (i <= expected->fraction->last) {
		if (expected->fraction->data[i] != given->fraction->data[i]) {
			fprintf(stderr, "Test Failed! Expected BigNum Digit %d, Received Digit %d\n", expected->whole->data[i], given->whole->data[i]);
			printMessage(message);
			return 1;
		}
		i++;
	}
	return 0;
};
