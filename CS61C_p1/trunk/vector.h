#ifndef _61C_VECTOR_H_
#define _61C_VECTOR_H_
/* vector.h written by Jeremy Huddleston <jeremyhu@eecs.berkeley.edu> Sp2004 */

#include <sys/types.h>

typedef struct _vector_t vector_t;

typedef vector_t* VectorPtr;

typedef char VectorMember;

/* Define what our struct is */
struct _vector_t {
  int last;
  int size;
  VectorMember* data;
};

/* Create a new vector */
VectorPtr vector_new();

/* Create a new sized vector */
VectorPtr vector_new_sized(int size);

/* Free up the memory allocated for the passed vector */
void vector_delete(VectorPtr v);

/* Return the value in the vector */
VectorMember vector_get(VectorPtr v, int loc);

/* Set a value in the vector */
int vector_set(VectorPtr v, int loc, VectorMember value);

/* Append a value to a vector */
int vector_append(VectorPtr v, VectorMember value);

void vector_print(VectorPtr v);

void vector_pprint(VectorPtr v);

#endif
