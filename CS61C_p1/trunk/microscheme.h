/*
 *  Do NOT change anything in this file! 
 *  We will be using our version of microscheme.h
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

// Add boolean type to C
typedef int boolean;
#define TRUE 1
#define FALSE 0

/*********************************************************************
 *       Redefining malloc() and company for tracking purposes
 *********************************************************************/
#include "i_memory.h"

#define malloc(PTR) iMalloc(PTR,__FILE__,__LINE__)
#define free(PTR) iFree(PTR,__FILE__,__LINE__)
#define realloc(PTR,SZ) iRealloc(PTR,SZ,__FILE__,__LINE__)
#define calloc(SZ1,SZ2) iCalloc(SZ1,SZ2,__FILE__,__LINE__)

#define end_function doMemCheck()

/*********************************************************************
 *       Thing definition for microscheme
 *********************************************************************/

#include "thing.h"

// Types accepted by struct thing
#define SYMBOL 1
#define NUMBER 2
#define PAIR   3

// Shortcuts for accessing the various fields of struct thing
#define th_symbol th_union.thu_symbol
#define th_number th_union.thu_number
#define th_car th_union.thu_pair.thup_car
#define th_cdr th_union.thu_pair.thup_cdr

// Always work with pointers to things.
typedef struct thing* ThingPtr;

// Define NIL, Scheme's version of NULL
#define NIL (ThingPtr)0

// Define name of quote procedure using a string instead of '
extern char* quote;

/*********************************************************************
 *       Core functions for microscheme
 *********************************************************************/

// Constructors for various types
ThingPtr cons(ThingPtr a, ThingPtr b);
ThingPtr make_symbol(char *str);
ThingPtr quoted(ThingPtr th);

// Selectors
ThingPtr car(ThingPtr pair);
ThingPtr cdr(ThingPtr pair);
#define cadr(x) (car(cdr(x)))
#define caddr(x) (car(cdr(cdr(x))))

// Evaluation functions
void printtail(ThingPtr);
void printthing(ThingPtr th);
ThingPtr readlist(void);
ThingPtr readthing(void);

// Memory management functions
void free_thing(ThingPtr th, int pair_delete_level);
ThingPtr car_with_free(ThingPtr pair);
ThingPtr cdr_with_free(ThingPtr pair);

/*********************************************************************
 *       The functions you have to modify/implement
 *********************************************************************/

ThingPtr make_number(int num);
void free_number(ThingPtr th);
void print_number(ThingPtr th);
boolean isNumber(char *str);
ThingPtr plus(ThingPtr a, ThingPtr b);
ThingPtr handleArithmeticOp(char* op, ThingPtr argList);
ThingPtr eval(ThingPtr exp);
