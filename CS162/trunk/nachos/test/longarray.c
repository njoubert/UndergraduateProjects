/* matmult.c 
 *    Test program to do matrix multiplication on large arrays.
 *
 *    Intended to stress virtual memory system. Should return 7220 if Dim==20
 */

#include "syscall.h"
#include "stdio.h"
#include "stdlib.h"

char longarray[20480];
int i;

int main() {
  for (i = 0; i < 20480; i++) {
    longarray[i] = 'a';
  }
  for (i = 0; i < 20480; i++) {
    longarray[i] = 'b';
  }
  printf("first element in array (expect b): %c", longarray[0]);
  printf("last element in array (expect b): %c", longarray[20479]);
  return 0;
}
