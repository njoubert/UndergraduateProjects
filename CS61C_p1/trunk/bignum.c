#include "bignum.h"

/* Create a new, blank BigNum of value 0.0 */
BigNumPtr BigNum_new() {
	BigNumPtr retval = (BigNumPtr) malloc(sizeof(BigNum));
	if (retval == NULL)
		return NULL;
	retval->sign = POS_SIGN;
	if (!(retval->whole = vector_new()))
		return NULL;
	if (!(retval->fraction = vector_new()))
		return NULL;
	return retval;
};

/* Create a new, blank BigNum of value 0.0 */
BigNumPtr BigNum_new_sized(int whole_size, int frac_size) {
	BigNumPtr retval = (BigNumPtr) malloc(sizeof(BigNum));
	if (retval == NULL)
		return NULL;
	retval->sign = POS_SIGN;
	if (!(retval->whole = vector_new_sized(whole_size)))
		return NULL;
	if (!(retval->fraction = vector_new_sized(frac_size)))
		return NULL;;
	return retval;
};

/* Remove a BigNum from memory. */
void BigNum_delete(BigNumPtr bignum) {
	vector_delete(bignum->whole);
	vector_delete(bignum->fraction);
	free(bignum);
};

/* Read in a character string representing a number and output a BigNum.
 * Return 0 if it fails. */
int BigNum_readchar(BigNumPtr bignum, char* input) {
	int i = 0;
	if (input[i] == '-') {
		bignum->sign = NEG_SIGN;
		i++;
	};
	while (input[i] != '.' && input[i] != '\0') {
		if (input[i] < '0' || input[i] > '9') return 0;
		if (!vector_append(bignum->whole, input[i]-'0')) return 0;
		i++;
	}
	if (input[i] == '.') {
		i++;
		while (input[i] != '\0') {
			if (input[i] < '0' || input[i] > '9') return 0;
			if (!vector_append(bignum->fraction, input[i]-'0')) return 0;
			i++;
		}
	}
	return 1;
};

int BigNum_print(BigNumPtr bignum) {
	if (bignum->sign == NEG_SIGN)
		printf("%c", bignum->sign);
	vector_print(bignum->whole);
	if (bignum->fraction->last > -1) {	//IS THIS RIGHT?!?@!?!
		printf(".");
		vector_print(bignum->fraction);
	};
	printf("\n");
	return 1;
};

BigNumPtr BigNum_add(BigNumPtr a, BigNumPtr b) {

	int carry = 0;
	int i;
	int temp = 0;
	BigNumPtr retval = BigNum_new();

	if (a->sign == NEG_SIGN && b->sign == NEG_SIGN) {
		retval->sign = NEG_SIGN;
	};

	if (a->fraction->last > b->fraction->last) {
		i = a->fraction->last;
	} else {
		i = b->fraction->last;
	};

	//Add Fractional Part
	for (; i >= 0; i--) {
		temp = vector_get(a->fraction, i) + vector_get(b->fraction, i) + carry;
		if (temp%10 != 0) {
			if (!vector_set(retval->fraction, i, temp%10))
				return NULL;
		}
		carry = temp/10;
	}

	int max;
	if (a->whole->last > b->whole->last) {
		max = a->whole->last;
	} else {
		max = b->whole->last;
	};

	//Add whole parts
	for (i=0; i <= max; i++) {
		temp = vector_get(a->whole, a->whole->last - i) + vector_get(b->whole, b->whole->last - i) + carry;
		if (!vector_set(retval->whole, max-i, temp%10))
			return NULL;
		carry = temp/10;
	}

	//Copy over if there's more
	if (carry > 0) {
		BigNumPtr bigger = BigNum_new_sized(retval->whole->size + 2, retval->fraction->size);
		bigger->sign = retval->sign;
		vector_set(bigger->whole, 0, carry);
		for (i = 0; i <= retval->whole->last; i++) {
			if (!vector_set(bigger->whole, i+1, vector_get(retval->whole, i)))
				return NULL;
		};
		for (i = 0; i <= retval->fraction->last; i++) {
			if (!vector_set(bigger->fraction, i, vector_get(retval->fraction, i)))
				return NULL;
		};
		BigNum_delete(retval);
		return bigger;
	} else {
		return retval;
	};

};

BigNumPtr BigNum_subtract(BigNumPtr a, BigNumPtr b) {
	BigNumPtr retval = BigNum_new();

	return retval;
};

BigNumPtr BigNum_multiply(BigNumPtr a, BigNumPtr b) {
	BigNumPtr retval = BigNum_new();

	return retval;
};

BigNumPtr BigNum_exponent(BigNumPtr a, BigNumPtr b) {
	BigNumPtr retval = BigNum_new();

	return retval;
};



