/* Interceptor memory management system
 * Aaron Staley, cs61c-tg@imail.eecs.berkeley.edu
 
 *  Do NOT change anything in this file!
 *  We will be using our version of i_memory.h
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

/*memory*/
void * iMalloc(size_t nbytes, char * fname, int line);
void * iRealloc(void * ptr, size_t nbytes, char * fname, int line);
void * iCalloc(size_t n, size_t s, char * fname, int line);
void iFree(void * ptr, char * fname, int line);

//tester:
void doMemCheck();

#define majic 0xC0EDBABE

