/*  Interceptor memory management system
 *  Aaron Staley, cs61c-tg@imail.eecs.berkeley.edu
 * 
 *  Do NOT change anything in this file!
 *  We will be using our version of i_memory.c
 *
 *  To the student: This is a fine example of when pointer casts are actually useful
 *  To the student: breaking at doMemCheck() will allow you to inspect the memory region
 */

#include "i_memory.h"

static int bytesUsed = 0;
static int numAlloc = 0;

typedef struct mem_info{
  int line_num;
  uint32_t size;
  char * fname;
  struct mem_info * next;
  struct mem_info * prev;
} mem_info;

mem_info * head = NULL; //top of list

void add_block(mem_info * block){
  block->next = head;
  if (head)    //arguably not needed
    head->prev = block;
  head = block;
}

void remove_block(mem_info * block){
  if (block == head){
    head = block->next;
    if (head)
      head->prev = NULL;
  }
  else{
    if (block->next)
      block->next->prev = block->prev;
    block->prev->next = block->next;
  }
}

void * iMalloc(size_t nbytes, char * fname, int line){
  size_t allocReq = nbytes + sizeof(mem_info);
  mem_info * block = malloc(allocReq);
  if (!block){
    fprintf(stderr,"MEM TESTER HAS RUN OUT OF MEMORY. YOU PROBABLY HAS A MEMORY LEAK!\n");
    return 0;
  }
  block->line_num = line;
  block->size = nbytes;
  block->fname = fname;
  add_block(block);

  bytesUsed += nbytes;
  numAlloc++;
  
  return (void*)(block + 1);
}


/*dispatch malloc on this thing*/
void * iRealloc(void * ptr, size_t nbytes, char * fname, int line){
  mem_info * block;
  int oldSize;
  void * nPtr;

  if (!ptr)
    return iMalloc(nbytes,fname,line);
  if (nbytes == 0){
    iFree(ptr,fname,line);
    return 0;
  }

  block=((mem_info*)ptr) - 1;
  oldSize = block->size;

  nPtr = iMalloc(nbytes, fname,line);
  memcpy(nPtr,ptr,oldSize);
  iFree(ptr,fname,line); //free old ptr
  return nPtr;
}

void * iCalloc(size_t n, size_t s, char * fname, int line){ /*drive w/ malloc*/
  void * data = iMalloc(s*n,fname,line);
  memset(data,0,n*s);
  return data;
}

void iFree(void * ptr, char * fname, int line){
  mem_info * block;
  if (!ptr)
    return;
  block=((mem_info*)ptr) - 1;
  bytesUsed -= block->size;
  remove_block(block);
  numAlloc--;
  free(block);
}

//memory checker
void doMemCheck(){
  mem_info * block;
  int i = 0;
  if (!numAlloc){
    fprintf(stdout,"Microscheme terminated. No memory leaks found\n");
    return;
  }
  //error condition

  fprintf(stderr,"Memory Leak detected on microscheme termination.  %d outstanding allocations of %d bytes Total:\n",
	  numAlloc,bytesUsed);

  //show all outstanding leaks
  for (block = head; block != NULL; block = block->next, i++){
    fprintf(stderr,"%d: %u bytes allocated at %s:%d\n",i,(unsigned int)(block->size),block->fname,block->line_num);
  }
}

/*end malloc*/

