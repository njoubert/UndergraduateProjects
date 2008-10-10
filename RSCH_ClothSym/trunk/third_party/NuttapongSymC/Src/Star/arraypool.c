/********* Tiered array memory pool routines begin here              *********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  Tiered arrays, which double as memory pools (struct arraypool)           */
/*                                                                           */
/*  Resizable arrays are a lovely data structure, because they allow you to  */
/*  access objects quickly by number, without limiting how many objects you  */
/*  can allocate.  Unfortunately, the usual technique for resizing arrays is */
/*  to copy everything from the old array to the new, then discard the old.  */
/*  It is not usually possible to reuse the old array's space for the new    */
/*  array.  For an application that uses the computer's entire main memory,  */
/*  this is not acceptable.                                                  */
/*                                                                           */
/*  Hence, tiered arrays, which seem to be part of the oral tradition of     */
/*  computer science, but are rarely covered in data structures textbooks.   */
/*  The simplest kind of tiered array, used here, has two tiers.  The upper  */
/*  tier is a single long array of pointers to second-tier blocks.  The      */
/*  second tier is composed of blocks, each containing the same fixed number */
/*  of objects.                                                              */
/*                                                                           */
/*  Each (nonnegative) array index addresses a particular object in a        */
/*  particular second-tier block.  The most significant bits of the index    */
/*  choose a second-tier block by addressing a pointer in the top tier       */
/*  array.  The remaining, less significant bits address an object within    */
/*  the block.  A tiered array is slightly slower than a normal array,       */
/*  because it requires an extra indiretion; but the top tier is small       */
/*  enough to cache well, so this is rarely a big handicap.                  */
/*                                                                           */
/*  Second-tier blocks are allocated one at a time, as necessary.  The       */
/*  top-tier array is resized in the usual manner.  However, the top tier    */
/*  comprises such a small percentage of the tiered array's total memory,    */
/*  it doesn't matter much if the old, discarded upper tier's memory is not  */
/*  reused.                                                                  */
/*                                                                           */
/*  The tiered array implemented here can be used in two different ways:  as */
/*  a normal array, or as a memory allocator (for objects of a single size)  */
/*  that can allocate and free objects, managing the index space itself.     */
/*  It's not usually wise to mix these two modes, though it can be done.     */
/*                                                                           */
/*  For a normal array, use the procedure arraypoolforcelookup(), which      */
/*  makes sure that an object with the given index exists, then returns a    */
/*  pointer to it.  For this behavior, initialize the arraypool by passing   */
/*  an `alloctype' of 0 to arraypoolinit().  Once an object has been         */
/*  initialized, it can be accessed quickly with the arraypoolfastlookup()   */
/*  macro, which is much faster than arraypoolforcelookup().                 */
/*                                                                           */
/*  For a memory allocator, either use arraypoolnewindex() and               */
/*  arraypoolfreeindex(), which allocate and free array indices, or          */
/*  arraypoolnewptr() and arraypoolfreeptr(), which allocate and free        */
/*  pointers.  For the former behavior, pass an `alloctype' of 1 to          */
/*  arraypoolinit().  For the latter behavior, pass an `alloctype' of 2.     */
/*                                                                           */
/*  Public interface:                                                        */
/*  typedef arraypoolulong   Unsigned array index.                           */
/*  struct arraypool   Tiered array (and memory pool) object.                */
/*  void arraypoolinit(pool, objectbytes, log2objectsperblock, alloctype)    */
/*    Initialize an arraypool for allocation of objects.                     */
/*  void arraypoolrestart(pool)   Deallocate all objects (but not to OS).    */
/*  void arraypooldeinit(pool)   Free an arraypool's memory to the OS.       */
/*  void *arraypoolforcelookup(pool, index)   Dereference; create if absent. */
/*  void *arraypoollookup(pool, index)   Dereference; return NULL if absent. */
/*  void *arraypoolfastlookup(pool, index)   Unsafe dereference; fast macro. */
/*  arraypoolulong arraypoolnewindex(pool, newptr)   Allocate object,        */
/*    returning an array index.                                              */
/*  void *arraypoolnewptr(pool)   Allocate object, returning a pointer.      */
/*  arraypoolulong arraypoolallocated(pool)   Return maximum number of       */
/*    objects allocated.                                                     */
/*  arraypoolulong arraypoolbytes(pool)   Return bytes of dynamic memory     */
/*    used by the tiered array.                                              */
/*  With `alloctype' 1 only:                                                 */
/*  void arraypoolfreeindex(pool, dyingindex)   Deallocate index for reuse.  */
/*  With `alloctype' 2 only:                                                 */
/*  void arraypoolfreeptr(pool, dyingobject)   Deallocate pointer for reuse. */
/*                                                                           */
/*  For internal use only:                                                   */
/*  char *arraypoolsecondtier(pool, index)                                   */
/*                                                                           */
/*****************************************************************************/

/*  `arraypoolulong' is the type of (unsigned) int used by the tiered array  */
/*  memory pool.  It determines the number of objects that can be allocated  */
/*  and the size of the array indices that may be used, so a long choice     */
/*  (e.g. size_t, defined in stddef.h) is recommended.  However, choosing a  */
/*  shorter type may make the tiered array occupy less memory in the case    */
/*  where arraypoolinit() is called with alloctype == 1, which forces each   */
/*  object to occupy at least as much space as an `arraypoolulong'.          */

typedef starulong arraypoolulong;


/*  TOPARRAYSTARTSIZE is the initial size of the top tier of the tiered      */
/*  array, namely the array `toparray'.  Its value is not critical, as the   */
/*  `toparray' is enlarged whenever necessary.                               */

#define TOPARRAYSTARTSIZE 128

/*  When the `toparray' runs out of room, it is enlarged, its length         */
/*  multiplied by the following factor.  If you think that the freed space   */
/*  occupied by the old array will not be reused (e.g. it's too small to be  */
/*  re-malloc'd for any other purpose), then a factor of 3 minimizes the     */
/*  average-case memory wasted.  If you think the freed space will be        */
/*  reused, then a factor of (1 + epsilon) minimizes the average-case memory */
/*  wasted, but you should choose at least 2 for speed's sake.               */

#define TOPRESIZEFACTOR 3

/*  NOTAPOOLINDEX is an index denoting no freed object in the arraypool.     */

#define NOTAPOOLINDEX ((arraypoolulong) ~0)


/*  An arraypool is a two-tiered array from which object can be allocated,   */
/*  either by index or by pointer.  `toparray' is a pointer to the upper     */
/*  tier, which is an array of pointers to the blocks that make up the lower */
/*  tier; and `toparraylen' is the upper tier's length.                      */
/*                                                                           */
/*  There are two mutually exclusive ways to store the stack of freed        */
/*  objects that are waiting to be reallocated.  `deadindexstack' and        */
/*  `deadobjectstack' store the indices or pointers, respectively, of the    */
/*  freed objects.  Indices are more flexible, as they can be converted to   */
/*  pointers, whereas pointers cannot be converted to indices.               */
/*                                                                           */
/*  Other fields are explained by comments below.  `objectsperblock' must be */
/*  2 raised to an integral power.  `objects' is a count of objects that the */
/*  arraypool has allocated for the client, NOT a count of objects malloc'd  */
/*  from the operating system.  `objects' does not include objects that have */
/*  been freed and are waiting on the dead stack to be reallocated.          */
/*  `objects' does not include objects that a client has accessed directly   */
/*  by a call to arraypoolforcelookup() without first being allocated by the */
/*  arraypool.                                                               */

struct arraypool {
  arraypoolulong objectbytes;       /* Size of one object in the lower tier. */
  arraypoolulong objectsperblock;           /* Objects per lower tier block. */
  arraypoolulong log2objectsperblock;      /* Base-2 logarithm of the above. */
  arraypoolulong totalmemory;     /* Total bytes used by whole tiered array. */
  arraypoolulong objects;          /* Number of currently allocated objects. */

  char **toparray;                             /* Pointer to the upper tier. */
  arraypoolulong toparraylen;      /* Length of the upper tier, in pointers. */
  arraypoolulong firstvirgin;          /* First never-allocated array index. */
  arraypoolulong deadindexstack;        /* Stack of freed objects, by index. */
  void *deadobjectstack;              /* Stack of freed objects, by pointer. */
};


/*****************************************************************************/
/*                                                                           */
/*  arraypoolrestart()   Deallocate all objects in an arraypool.             */
/*                                                                           */
/*  The arraypool returns to a fresh state, like after it was initialized    */
/*  by poolinit(), except that no memory is freed to the operating system.   */
/*  Rather, the previously allocated blocks are ready to be reused.          */
/*                                                                           */
/*  pool:  The arraypool to restart.                                         */
/*                                                                           */
/*****************************************************************************/

void arraypoolrestart(struct arraypool *pool)
{
  /* No objects have been freed. */
  pool->deadobjectstack = (void *) NULL;
  pool->deadindexstack = NOTAPOOLINDEX;
  /* Every object is ready to allocate. */
  pool->firstvirgin = 0;
  pool->objects = 0;
}

/*****************************************************************************/
/*                                                                           */
/*  arraypoolinit()   Initialize an arraypool for allocation of objects.     */
/*                                                                           */
/*  Before an arraypool may be used, it must be initialized by this          */
/*  procedure.  After initialization, memory can be allocated and freed in   */
/*  an arraypool, and array indices can be "dereferenced", by use of the     */
/*  other procedures.  arraypoolinit() does not allocate any memory from the */
/*  operating system itself.                                                 */
/*                                                                           */
/*  Don't call this procedure on an arraypool that has already been          */
/*  initialized (by this procedure), unless you call arraypooldeinit() on    */
/*  it first.  You will leak memory.  Also see arraypoolrestart().           */
/*                                                                           */
/*  pool:  The arraypool to initialize.                                      */
/*  objectbytes:  The number of bytes in each object.                        */
/*  log2objectsperblock:  The base-2 logarithm of the number of objects in   */
/*    each block of the second tier.  (The latter number must be 2 raised to */
/*    an integral power, and this interface ensures that.)                   */
/*  alloctype:  Sets the method used to maintain freed objects, as follows.  */
/*    0:  The arraypool does not manage freeing of objects at all.  Objects  */
/*        can be allocated, using arraypoolnewindex() or arraypoolnewptr(),  */
/*        or accessed (and allocated) directly by supplying an index to      */
/*        arraypoolforcelookup() (in which case the client must manage its   */
/*        own index space, and `objects' is not a correct count).  The       */
/*        advantage of disallowing freeing is that objects can be as short   */
/*        as one byte.                                                       */
/*    1:  The arraypool manages indices for the client via                   */
/*        arraypoolnewindex() and arraypoolfreeindex().  However, each       */
/*        object is made long enough to accommodate an arraypoolulong, if it */
/*        isn't already.  (To accommodate the stack of dead objects.)        */
/*    2:  The arraypool manages pointers for the client via                  */
/*        arraypoolnewptr() and arraypoolfreeptr().  If indices are not      */
/*        needed, this is convenient because it skips the index-to-pointer   */
/*        "dereferencing" step.  Each object is made long enough to          */
/*        accommodate a pointer.                                             */
/*                                                                           */
/*****************************************************************************/

void arraypoolinit(struct arraypool *pool,
                   arraypoolulong objectbytes,
                   arraypoolulong log2objectsperblock,
                   int alloctype)
{
  if (alloctype == 0) {
    /* Each object must be at least one byte long. */
    pool->objectbytes = objectbytes > 1 ? objectbytes : 1;
  } else if (alloctype == 1) {
    /* Each object must be at least sizeof(arraypoolulong) bytes long. */
    pool->objectbytes = objectbytes > sizeof(arraypoolulong) ?
                        objectbytes : sizeof(arraypoolulong);
  } else {
    /* Each object must be at least as long as a pointer. */
    pool->objectbytes = objectbytes > sizeof(void *) ?
                        objectbytes : sizeof(void *);
  }

  pool->log2objectsperblock = log2objectsperblock;
  /* Compute the number of objects in each block of the second tier. */
  pool->objectsperblock = ((arraypoolulong) 1) << pool->log2objectsperblock;
  /* No memory has been allocated. */
  pool->totalmemory = 0;
  /* The upper tier array has not been allocated yet. */
  pool->toparray = (char **) NULL;
  pool->toparraylen = 0;

  /* Ready all indices to be allocated. */
  arraypoolrestart(pool);
}

/*****************************************************************************/
/*                                                                           */
/*  arraypooldeinit()   Free to the operating system all memory taken by an  */
/*                      arraypool.                                           */
/*                                                                           */
/*  pool:  The arraypool to free.                                            */
/*                                                                           */
/*****************************************************************************/

void arraypooldeinit(struct arraypool *pool)
{
  arraypoolulong i;

  /* Has anything been allocated at all? */
  if (pool->toparray != (char **) NULL) {
    /* Walk through the top tier array. */
    for (i = 0; i < pool->toparraylen; i++) {
      /* Check every top-tier pointer; NULLs may be scattered randomly. */
      if (pool->toparray[i] != (char *) NULL) {
        /* Free a second-tier block. */
        starfree((void *) pool->toparray[i]);
      }
    }
    /* Free the top-tier array. */
    starfree((void *) pool->toparray);
  }

  /* The upper tier array is no longer allocated. */
  pool->toparray = (char **) NULL;
  pool->toparraylen = 0;
  pool->objects = 0;
  pool->totalmemory = 0;
}

/*****************************************************************************/
/*                                                                           */
/*  arraypoolsecondtier()   Return (and perhaps create) the second-tier      */
/*                          block containing the object with a given index.  */
/*                                                                           */
/*  This procedure takes care of allocating or resizing the top-tier array   */
/*  if necessary, and of allocating the second-tier block if it hasn't yet   */
/*  been allocated.                                                          */
/*                                                                           */
/*  pool:  The arraypool containing the object.                              */
/*  index:  The index of the object sought.                                  */
/*                                                                           */
/*  Returns a pointer to the beginning of the second-tier block.  (Not to    */
/*    the object with the specified index; just the block containing it.)    */
/*                                                                           */
/*****************************************************************************/

char *arraypoolsecondtier(struct arraypool *pool,
                          arraypoolulong index)
{
  char **newarray;
  char *secondtier;
  arraypoolulong newsize;
  arraypoolulong topindex;
  arraypoolulong i;

  /* Compute the index in the top-tier array (upper bits). */
  topindex = index >> pool->log2objectsperblock;
  /* Does the top-tier array need to be allocated or resized? */
  if (pool->toparray == (char **) NULL) {
    /* Allocate a top-tier array big enough to hold `topindex' (plus */
    /*   some slack), and NULL out its contents.                     */
    newsize = topindex + TOPARRAYSTARTSIZE;
    pool->toparray = (char **) starmalloc((size_t) (newsize * sizeof(char *)));
    pool->toparraylen = newsize;
    for (i = 0; i < newsize; i++) {
      pool->toparray[i] = (char *) NULL;
    }

    /* Account for the memory. */
    pool->totalmemory = newsize * (arraypoolulong) sizeof(char *);
  } else if (topindex >= pool->toparraylen) {
    /* Resize the top-tier array, making sure it holds `topindex'. */
    newsize = TOPRESIZEFACTOR * pool->toparraylen;
    if (topindex >= newsize) {
      newsize = topindex + TOPARRAYSTARTSIZE;
    }

    /* Allocate the new array, copy the contents, NULL out the rest, and */
    /*   free the old array.                                             */
    newarray = (char **) starmalloc((size_t) (newsize * sizeof(char *)));
    for (i = 0; i < pool->toparraylen; i++) {
      newarray[i] = pool->toparray[i];
    }
    for (i = pool->toparraylen; i < newsize; i++) {
      newarray[i] = (char *) NULL;
    }
    starfree(pool->toparray);

    /* Account for the memory. */
    pool->totalmemory += (newsize - pool->toparraylen) * sizeof(char *);

    pool->toparray = newarray;
    pool->toparraylen = newsize;
  }

  /* Find the second-tier block, or learn that it hasn't been allocated yet. */
  secondtier = pool->toparray[topindex];
  if (secondtier == (char *) NULL) {
    /* Allocate a second-tier block at this index. */
    secondtier = (char *) starmalloc((size_t) (pool->objectsperblock *
                                               pool->objectbytes));
    pool->toparray[topindex] = secondtier;
    /* Account for the memory. */
    pool->totalmemory += pool->objectsperblock * pool->objectbytes;
  }

  /* Return a pointer to the second-tier block. */
  return secondtier;
}

/*****************************************************************************/
/*                                                                           */
/*  arraypoolforcelookup()   A "dereferencing" operation:  return the        */
/*                           pointer to the object with a given index,       */
/*                           creating the object if it doesn't yet exist.    */
/*                                                                           */
/*  If memory does not yet exist for the object with the given index, this   */
/*  procedure allocates a second-tier block with room for the object.  (The  */
/*  new object's memory is not initialized in any way.)  Subsequent calls    */
/*  with the same index will always return the same object.                  */
/*                                                                           */
/*  The object with the given index is NOT allocated to the client by the    */
/*  arraypool, and the object may be subsequently returned by a call to      */
/*  arraypoolnewindex() or arraypoolnewptr(), perhaps causing an object to   */
/*  have two conflicting uses.  Generally, those procedures do not mix       */
/*  easily with this one, but they can be mixed with enough care.            */
/*                                                                           */
/*  pool:  The arraypool containing the object.                              */
/*  index:  The index of the object sought.                                  */
/*                                                                           */
/*  Returns a pointer to the object.                                         */
/*                                                                           */
/*****************************************************************************/

void *arraypoolforcelookup(struct arraypool *pool,
                           arraypoolulong index)
{
  /* Find the second-tier block and compute a pointer to the object with the */
  /*   given index.  Note that `objectsperblock' is a power of two, so the   */
  /*   & operation is a bit mask that preserves the lower bits.              */
  return (void *) (arraypoolsecondtier(pool, index) +
                   (index & (pool->objectsperblock - 1)) * pool->objectbytes);
}

/*****************************************************************************/
/*                                                                           */
/*  arraypoollookup()   A "dereferencing" operation:  return the pointer to  */
/*                      the object with a given index, or NULL if the        */
/*                      object's second-tier block doesn't yet exist.        */
/*                                                                           */
/*  Note that this procedure cannot tell whether an object with the given    */
/*  index has been allocated by the arraypool or initialized by the client   */
/*  yet; it can only tell whether memory has been allocated for the second-  */
/*  tier block that contains the object.                                     */
/*                                                                           */
/*  pool:  The arraypool containing the object.                              */
/*  index:  The index of the object sought.                                  */
/*                                                                           */
/*  Returns a pointer to the object if its block exists; NULL otherwise.     */
/*                                                                           */
/*****************************************************************************/

void *arraypoollookup(struct arraypool *pool,
                      arraypoolulong index)
{
  char *secondtier;
  arraypoolulong topindex;

  /* Has the top-tier array been allocated yet? */
  if (pool->toparray == (char **) NULL) {
    return (void *) NULL;
  }

  /* Compute the index in the top-tier array (upper bits). */
  topindex = index >> pool->log2objectsperblock;
  /* Does the top-tier index fit in the top-tier array? */
  if (topindex >= pool->toparraylen) {
    return (void *) NULL;
  }

  /* Find the second-tier block, or learn that it hasn't been allocated yet. */
  secondtier = pool->toparray[topindex];
  if (secondtier == (char *) NULL) {
    return (void *) NULL;
  }

  /* Compute a pointer to the object with the given index.  Note that        */
  /*   `objectsperblock' is a power of two, so the & operation is a bit mask */
  /*   that preserves the lower bits.                                        */
  return (void *) (secondtier +
                   (index & (pool->objectsperblock - 1)) * pool->objectbytes);
}

/*****************************************************************************/
/*                                                                           */
/*  arraypoolfastlookup()   A fast, unsafe "dereferencing" operation:        */
/*                          return the pointer to the object with a given    */
/*                          index.  The object's second-tier block must have */
/*                          been allocated.                                  */
/*                                                                           */
/*  Use this procedure only for indices that have previously been allocated  */
/*  with arraypoolnewindex() or "dereferenced" with arraypoolforcelookup().  */
/*  WARNING:  fails catastrophically, probably with a segmentation fault, if */
/*  the index's second-tier block doesn't yet exist.                         */
/*                                                                           */
/*  Implemented as a macro to meet your need for speed.                      */
/*                                                                           */
/*  pool:  The arraypool containing the object.                              */
/*         Type (struct arraypool *).                                        */
/*  index:  The index of the object sought.                                  */
/*          Type (arraypoolulong).                                           */
/*                                                                           */
/*  Returns a pointer to the object if its block exists; might crash         */
/*    otherwise.                                                             */
/*    Type (void *).                                                         */
/*                                                                           */
/*****************************************************************************/

/* Dereference the top tier `toparray' with the upper bits to find the  */
/*   second-tier bock, then add an offset computed from the lower bits. */

#define arraypoolfastlookup(pool, index)  \
  (void *) ((pool)->toparray[(index) >> (pool)->log2objectsperblock] +  \
            ((index) & ((pool)->objectsperblock - 1)) * (pool)->objectbytes)

/*****************************************************************************/
/*                                                                           */
/*  arraypoolnewptr()   Allocate space for a fresh object from an arraypool. */
/*                                                                           */
/*  Returns an object that was not already allocated, or has been freed      */
/*  since it was last allocated.  Be forewarned that this procedure is not   */
/*  aware of objects that have been accessed by arraypoolforcelookup(), and  */
/*  may allocate them, possibly resulting in conflicting uses of an object.  */
/*                                                                           */
/*  pool:  The arraypool to allocate an object from.                         */
/*                                                                           */
/*  Returns a pointer to a fresh object.                                     */
/*                                                                           */
/*****************************************************************************/

void *arraypoolnewptr(struct arraypool *pool)
{
  void *newobject;

  /* Check if any freed objects have not yet been reallocated. */
  if (pool->deadobjectstack != (void *) NULL) {
    /* Reallocate an object from the stack of dead objects. */
    newobject = pool->deadobjectstack;
    /* Remove the object from the stack. */
    pool->deadobjectstack = * (void **) newobject;
  } else {
    /* Allocate an object at index `firstvirgin'. */
    newobject = (void *) (arraypoolsecondtier(pool, pool->firstvirgin) +
                          (pool->firstvirgin & (pool->objectsperblock - 1)) *
                          pool->objectbytes);
    pool->firstvirgin++;
  }
  pool->objects++;

  return newobject;
}

/*****************************************************************************/
/*                                                                           */
/*  arraypoolfreeptr()   Deallocate an object, freeing its space for reuse.  */
/*                                                                           */
/*  The object's memory is not freed to the operating system; rather, the    */
/*  object is stored in a stack so it can be reallocated later by            */
/*  arraypoolnewptr().  (It cannot be reallocated by arraypoolnewindex(),    */
/*  though.)                                                                 */
/*                                                                           */
/*  WARNING:  Use this procedure only if the arraypool was initialized with  */
/*  alloctype == 2!  Not compatible with an alloctype of 0 or 1.             */
/*                                                                           */
/*  pool:  The arraypool the object was allocated from.                      */
/*  dyingobject:  A pointer to the object to deallocate.  Must be in `pool'! */
/*                                                                           */
/*****************************************************************************/

void arraypoolfreeptr(struct arraypool *pool,
                      void *dyingobject)
{
  /* Store the object on the stack of dead objects for later reuse. */
  * (void **) dyingobject = pool->deadobjectstack;
  pool->deadobjectstack = dyingobject;
  pool->objects--;
}

/*****************************************************************************/
/*                                                                           */
/*  arraypoolnewindex()   Allocate space for a fresh object from an          */
/*                        arraypool.                                         */
/*                                                                           */
/*  Returns an object that was not already allocated, or has been freed      */
/*  since it was last allocated.  Be forewarned that this procedure is not   */
/*  aware of objects that have been accessed by arraypoolforcelookup(), and  */
/*  may allocate them, possibly resulting in conflicting uses of an object.  */
/*                                                                           */
/*  From a fresh pool, indices are allocated starting from zero.             */
/*                                                                           */
/*  pool:  The arraypool to allocate an object from.                         */
/*  newptr:  If this pointer is not NULL, a pointer to the new object is     */
/*    written at the location that `newptr' points to.                       */
/*                                                                           */
/*  Returns the index of a fresh object.                                     */
/*                                                                           */
/*****************************************************************************/

arraypoolulong arraypoolnewindex(struct arraypool *pool,
                                 void **newptr)
{
  void *newobject;
  arraypoolulong newindex;

  /* Check if any freed objects have not yet been reallocated. */
  if (pool->deadindexstack != NOTAPOOLINDEX) {
    /* Reallocate an object (by index) from the stack of dead objects. */
    newindex = pool->deadindexstack;
    /* Find a pointer to the object. */
    newobject = arraypoollookup(pool, newindex);
    /* Memory should already exist for this index. */
    if (newobject == (void *) NULL) {
      printf("Internal error in arraypoolnewindex():\n");
      printf("  Illegal index on stack of deallocated objects.\n");
      internalerror();
    }
    /* Remove the object from the stack. */
    pool->deadindexstack = * (arraypoolulong *) newobject;
  } else {
    /* Allocate an object at index `firstvirgin'. */
    newindex = pool->firstvirgin;
    newobject = (void *) (arraypoolsecondtier(pool, pool->firstvirgin) +
                          (pool->firstvirgin & (pool->objectsperblock - 1)) *
                          pool->objectbytes);
    pool->firstvirgin++;
  }
  pool->objects++;

  /* If `newptr' is not NULL, use it to return a pointer to the object. */
  if (newptr != (void **) NULL) {
    *newptr = newobject;
  }
  return newindex;
}

/*****************************************************************************/
/*                                                                           */
/*  arraypoolfreeindex()   Deallocate an object, freeing its index and space */
/*                         for reuse.                                        */
/*                                                                           */
/*  The object's memory is not freed to the operating system; rather, the    */
/*  object is stored in a stack so it can be reallocated later by            */
/*  arraypoolnewindex().  (It cannot be reallocated by arraypoolnewptr(),    */
/*  though.)                                                                 */
/*                                                                           */
/*  WARNING:  Use this procedure only if the arraypool was initialized with  */
/*  alloctype == 1!  Not compatible with an alloctype of 0 or 2.             */
/*                                                                           */
/*  pool:  The arraypool the object was allocated from.                      */
/*  dyingindex:  The index of the object to deallocate.                      */
/*                                                                           */
/*****************************************************************************/

void arraypoolfreeindex(struct arraypool *pool,
                        arraypoolulong dyingindex)
{
  void *dyingptr;

  /* Find a pointer to the object. */
  dyingptr = arraypoollookup(pool, dyingindex);
  if (dyingptr == (void *) NULL) {
    printf("Internal error in arraypoolfreeindex():\n");
    printf("  Attempt to free an unallocated index.\n");
    internalerror();
  }

  /* Store the index on the stack of dead objects for later reuse. */
  * (arraypoolulong *) dyingptr = pool->deadindexstack;
  pool->deadindexstack = dyingindex;
  pool->objects--;
}

/*****************************************************************************/
/*                                                                           */
/*  arraypoolallocated()   Return the maximum number of allocated objects.   */
/*                                                                           */
/*  Returns the greatest number of objects that have been allocated from the */
/*  arraypool at once.  This number is one greater than the greatest index   */
/*  of any object ever allocated from the arraypool.  Therefore, it can be   */
/*  used to loop through all the objects in the pool--if no object has been  */
/*  freed, or you are clever enough to be able to distinguish between freed  */
/*  items and allocated ones.                                                */
/*                                                                           */
/*  pool:  The arraypool in question.                                        */
/*                                                                           */
/*  Returns the maximum number of objects currently allocated in `pool'.     */
/*                                                                           */
/*****************************************************************************/

arraypoolulong arraypoolallocated(struct arraypool *pool)
{
  return pool->firstvirgin;
}

/*****************************************************************************/
/*                                                                           */
/*  arraypoolbytes()   Returns the number of bytes of dynamic memory used by */
/*                     the tiered array memory pool.                         */
/*                                                                           */
/*  The result sums all the bytes dynamically allocated on the heap for the  */
/*  allocpool, including the top tier array and the second-tier blocks.      */
/*  Does not include the size of the `struct arraypool', which is presumably */
/*  part of some other object and accounted for there.  Note that arraypools */
/*  only release memory to the operating system when arraypooldeinit() is    */
/*  called, so the result is the _maximum_ number of bytes allocated since   */
/*  the initiating call to arraypoolinit().                                  */
/*                                                                           */
/*  pool:  The arraypool in question.                                        */
/*                                                                           */
/*  Returns the number of dynamically allocated bytes in `pool'.             */
/*                                                                           */
/*****************************************************************************/

arraypoolulong arraypoolbytes(struct arraypool *pool)
{
  return pool->totalmemory;
}

/**                                                                         **/
/**                                                                         **/
/********* Tiered array memory pool routines end here                *********/


