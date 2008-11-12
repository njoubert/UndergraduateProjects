#include "unittest.h"
#include "vector.h"
#include <stdio.h>

int main() {

	printf("Vector Test Start\n");

	vector_t *v;

	v = vector_new();

	vector_delete(v);

	v = vector_new();

	assert_equal_int(0, vector_get(v, 0), "");
	assert_equal_int(0, vector_get(v, 8), "");
	assert_equal_int(0, vector_get(v, 23), "");
	assert_equal_int(0, vector_get(v, 1234), "");
	assert_equal_int(0, vector_get(v, 655), "");
	assert_equal_int(0, vector_get(v, 10000), "");
	assert_equal_int(0, vector_get(v, 4433330), "");
	assert_equal_int(0, vector_get(v, 1010101010), "");

	vector_set(v, 0, 98);
	vector_set(v, 11, 15);
	vector_set(v, 15, -23);
	vector_set(v, 24, 65);
	vector_set(v, 12, -123);
	vector_set(v, 15, 21);
	vector_set(v, 25, 43);
	assert_equal_int(26, v->size, "Is v the length of the given input values?");
	vector_append(v, 99);
	assert_equal_int(52, v->size, "Did we double when we added to the end?");
	assert_equal_int(26, v->last, "When we double, is the output fine?");
	for (int j = 27; j < v->size; j++) {
		assert_equal_int(0, vector_get(v, j), "must be zero");
	}

	assert_equal_int(98, vector_get(v, 0), "");
	assert_equal_int(15, vector_get(v, 11), "");
	assert_equal_int(65, vector_get(v, 24), "");
	assert_equal_int(-123, vector_get(v, 12), "");
	assert_equal_int(21, vector_get(v, 15), "");
	assert_equal_int(43, vector_get(v, 25), "");
	assert_equal_int(99, vector_get(v, 26), "Is our appended number there?");

	vector_t *a = vector_new();
	vector_append(a, 10);
	vector_append(a, 20);
	vector_append(a, 30);
	vector_append(a, 40);
	vector_append(a, 50);
	vector_append(a, 60);
	vector_append(a, 70);
	vector_append(a, 80);
	vector_append(a, 90);
	vector_append(a, 100);
	vector_append(a, 110);
	vector_append(a, 120);
	vector_append(a, 17);
	assert_equal_int(10, vector_get(a, 0), "");
	assert_equal_int(20, vector_get(a, 1), "");
	assert_equal_int(30, vector_get(a, 2), "");
	assert_equal_int(40, vector_get(a, 3), "");
	assert_equal_int(50, vector_get(a, 4), "");
	assert_equal_int(60, vector_get(a, 5), "");
	assert_equal_int(70, vector_get(a, 6), "");
	assert_equal_int(80, vector_get(a, 7), "");
	assert_equal_int(90, vector_get(a, 8), "");
	assert_equal_int(100, vector_get(a, 9), "");
	assert_equal_int(110, vector_get(a, 10), "");
	assert_equal_int(120, vector_get(a, 11), "");
	assert_equal_int(17, vector_get(a, 12), "");
	assert_equal_int(16, a->size, "Check whether we doubled the size every time.");

	vector_set(a, 6, 11);
	assert_equal_int(11, vector_get(a, 6), "Check whether insert screws up append");
	assert_equal_int(16, a->size, "");
	assert_equal_int(12, a->last, "");

	printf("Vector Test Done\n");
	return 0;
}
