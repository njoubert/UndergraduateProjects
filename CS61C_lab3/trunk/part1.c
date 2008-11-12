#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

int main() {
	int num;
	int *ptr;
	int **handle;

	/* Describe (either as comments in this file or a separate document) the
	   effect of each of the following statements.  What areas of memory (heap or
	   stack) are affected bt the statement?  Does any memory get allocated or
	   freed?  If so, where is this memory?  Does the statement result in a
	   memory leak? 
	   
	   Feel free to use gdb or printf statements to help in your investigation.
	
	   Additionally, both of the malloc statements work, but one of them is
	   in bad form.  Which is "bad," why is it "bad," and how would you change it?
	 */
	 
	num = 14;       //Sets the current stack space assigned to num to the value 14.
	ptr = (int *)malloc(2 * sizeof(int)); // Creates a space on the heap for 2 integers, assigned the stack variable ptr to a pointer to it.
	handle = &ptr; //Sets the stack variable handle to the address of ptr
	*ptr = num; //Dereferences the ptr and sets the heap space assiged to the first integer that ptr point to, to the same value as num
	ptr = &num; //Sets the pointer ptr on the stack to point to the address of num, also on the stack.
	handle = (int **)malloc(1 * sizeof(int *)); //Creates space on the heap for one int**, and sets the stack var handle to point to this.
}
