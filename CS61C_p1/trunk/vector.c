/* Include the system headers we need */
#include <stdlib.h>
#include <stdio.h>

/* Include our header */
#include "vector.h"

/* Create a new vector */
vector_t *vector_new() {
  vector_t *retval;

  /* First, we need to allocate the memory for the struct */
  retval = (vector_t *)malloc(1 * sizeof(vector_t));

  /* Check our return value to make sure we got memory */
  if(retval == NULL)
    return NULL;

  /* Why does the above statement cast the malloc's return value to 'vector_t *'
   * instead of 'struct _vector_t *'?  Does it matter?
   */

  /* Now we need to initialize our data */
  retval->size = 4;
  retval->last = -1;
  retval->data = (VectorMember*) malloc(retval->size * sizeof(VectorMember));

  /* Check our return value to make sure we got memory */
  if(retval->data == NULL) {
    free(retval);
    return NULL;
  }

  for (int i = 0; i<retval->size; i++)
  	retval->data[i] = 0;

  return retval;
}

VectorPtr vector_new_sized(int size) {
  VectorPtr retval;

  /* First, we need to allocate the memory for the struct */
  retval = (vector_t *)malloc(1 * sizeof(vector_t));

  /* Check our return value to make sure we got memory */
  if(retval == NULL)
    return NULL;

  /* Why does the above statement cast the malloc's return value to 'vector_t *'
   * instead of 'struct _vector_t *'?  Does it matter?
   */

  /* Now we need to initialize our data */
  retval->size = size;
  retval->last = -1;
  retval->data = (VectorMember*) malloc(retval->size * sizeof(VectorMember));

  /* Check our return value to make sure we got memory */
  if(retval->data == NULL) {
    free(retval);
    return NULL;
  }

  //for (int count = 0; count <= size; count++)
  //	retval->data[count] = 0;

  return retval;

}

/* Free up the memory allocated for the passed vector */
void vector_delete(vector_t *v) {
    free(v->data);
    free(v);
}

/* Return the value in the vector */
VectorMember vector_get(vector_t *v, int loc) {

  /* If we are passed a NULL pointer for our vector, complain about it and
   * return 0.
   */
  if(v == NULL) {
    fprintf(stderr, "vector_get: passed a NULL vector.  Returning 0.\n");
    return 0;
  }
  if(loc < 0)
  	return 0;

  if(loc <= v->last) {
    return v->data[loc];
  } else {
    return 0;
  }
}

/* Set a value in the vector */
int vector_set(vector_t *v, int loc, VectorMember value) {

    int i;

    if (v == NULL) {
	fprintf(stderr, "vector_set: passed a NULL vector. Returning 0.\n");
	return 0;
    }

    if(loc < v->size) {

		v->data[loc] = value;
		if (v->last < loc)
			v->last = loc;

    } else {

		VectorMember* data = (VectorMember*) malloc((loc+1)*sizeof(VectorMember));

		if (data == NULL) {
		    fprintf(stderr, "vector_set: could not get memory!");
		    return 0;
		}
		for (i = 0; i <= v->last; i++) {
		    data[i] = v->data[i];
		}
		for (i = v->size; i<=loc; i++) {
		    data[i] = 0;
		}
		data[loc] = value;
		v->size = loc + 1;
		v->last = loc;
		free(v->data);
		v->data = data;

    }
    return 1;

}

int vector_append(vector_t *v, VectorMember value) {
	VectorMember* data;
	int i;

	if (v==NULL) {
		fprintf(stderr, "vector_append: passed a NULL vector. Returning 0.\n");
		return 0;
	}

	if (v->last < v->size-1) {
		v->data[++v->last] = value;
		return 1;
	}

	data = (VectorMember*) malloc((v->size)*2*sizeof(VectorMember));

	if (data==NULL) {
		fprintf(stderr, "vector_append: could not get memory!");
		return 0;
	}

	for (i = 0; i <= v->last; i++)
	    data[i] = v->data[i];

	v->last = i;
	v->size = v->size*2;
	data[i] = value;
	i++;
	for (; i < v->size; i++)
		data[i] = 0;

	free(v->data);
	v->data = data;
	return 1;

}

void vector_print(vector_t *v) {
	int i = 0;
	//printf("Printing Vector -- Size: %d, Last Value At: %d\n", v->size, v->last);
	while (i <= v->last) {
		printf("%d", v->data[i++]);
	}
}

void vector_pprint(vector_t *v) {
	printf("Printing vector. Size: %d, Last: %d\n", v->size, v->last);
	vector_print(v);
	printf("\n");
}
