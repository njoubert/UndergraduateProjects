/******** Proximity-based memory pool management routines begin here  ********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  Proximity-based memory pools (struct proxipool)                          */
/*                                                                           */
/*  A proximity-based memory pool is a memory allocator (for objects of a    */
/*  single size) which divides memory into separate pools, each with a       */
/*  different "allocation index."  The idea is that objects that are         */
/*  geometrically close to each other are likely to have the same allocation */
/*  index, and therefore be close to each other--both close in memory, and   */
/*  close in the space of "tags."  Each object is assigned a distance tag,   */
/*  which can be used like an array index to address the object.  The reason */
/*  I want proximate objects to have proximate tags is so that tags can be   */
/*  compressed well in the triangulation data structures.                    */
/*                                                                           */
/*  A proxipool is a tiered array.  (You should read the documentation for   */
/*  tiered arrays, "struct arraypool", before reading on.)  There are some   */
/*  important differences, of course.  Each second-tier block in a proxipool */
/*  is called a "minipool," and is assigned to a single allocation index.    */
/*  All the objects in a second-tier block have the same allocation index;   */
/*  this is the mechanism for maintaining memory proximity and tag           */
/*  proximity.  Each allocation index has a set of minipools associated with */
/*  it; this set is called a "poolpool" because it is a pool of minipools.   */
/*                                                                           */
/*  To further improve tag proximity, tags are allocated to allocation       */
/*  indices in "groups" of minipools.  For example, when the very first      */
/*  object is allocated, receiving tag 0, the proxipool will allocate a      */
/*  minipool for that object's allocation index.  Moreover, the next seven   */
/*  or so minipools' worth of tags will also be reserved for the allocation  */
/*  index--though the seven minipools are not allocated until (and unless)   */
/*  they are needed.                                                         */
/*                                                                           */
/*  Objects in a proxipool can be freed for reuse.  Each minipool maintains  */
/*  its own linked list of freed objects in the minipool (as opposed to      */
/*  having one global linked list for each allocation index).  This helps to */
/*  keep memory accesses more local.                                         */
/*                                                                           */
/*  When an object that resides in a full minipool is freed, its memory is   */
/*  available again for new objects with the same allocation index.  For     */
/*  each allocation index, a poolpool record maintains a linked list of all  */
/*  the minipools that belong to that allocation index and have free space   */
/*  available to allocate.  When an object is freed in a full minipool, the  */
/*  minipool adds itself to the end of the poolpool's linked list.           */
/*                                                                           */
/*  The poolpool records are stored in an arraypool (tiered array).          */
/*                                                                           */
/*  A new object is allocated as follows.  Find the poolpool associated with */
/*  the object's allocation index.  If the poolpool's linked list of         */
/*  minipools is not empty, allocate the object in the first minipool on the */
/*  list.  If the minipool is now full, remove it from the linked list.      */
/*  On the other hand, if the linked list was empty, allocate a new          */
/*  minipool--from the same group as the last allocated minipool if that     */
/*  group is not exhausted; otherwise, from a newly allocated group.         */
/*                                                                           */
/*  A proxipool is also built on an arraypool, in the sense that the objects */
/*  allocated from a proxipool live in the memory of an arraypool.  However, */
/*  the proxipool manages each second-tier block (minipool) itself, so it    */
/*  creates an arraypool having only one "object" per second-tier block,     */
/*  that "object" being a minipool.  Each minipool has a header at the       */
/*  beginning of the second-tier block, followed by a sequence of objects,   */
/*  which are managed by the minipool.                                       */
/*                                                                           */
/*  Some invariants of proxipools:                                           */
/*                                                                           */
/*  - Each object is allocated from a minipool that has the same allocation  */
/*    index as the object.                                                   */
/*                                                                           */
/*  - Every minipool with an unallocated slot appear in its poolpool's       */
/*    linked list.                                                           */
/*                                                                           */
/*  - No full minipool appears in a poolpool's linked list.                  */
/*                                                                           */
/*  - A new minipool is allocated only when a new object's allocation index  */
/*    indexes a poolpool whose linked list of minipools is empty.            */
/*                                                                           */
/*  - Minipools whose tags fall into the same "group" have the same          */
/*    allocation index.                                                      */
/*                                                                           */
/*  - A poolpool reserves only one group at a time, and does not reserve     */
/*    another group until it has filled all the minipools in the first       */
/*    group.                                                                 */
/*                                                                           */
/*  Public interface:                                                        */
/*  typedef proxipoolulong   Unsigned allocation index.                      */
/*  typedef tag   Unsigned tag for indexing objects in the memory pool.      */
/*  struct proxipool   Proximity-based memory pool object.                   */
/*  void proxipoolinit(pool, objectbytes1, objectbytes2, verbose)            */
/*    Initialize a proxipool for allocation of objects.                      */
/*  void proxipoolrestart(pool)   Deallocate all the objects (not to OS).    */
/*  void proxipooldeinit(pool)   Free a proxipool's memory to the OS.        */
/*  void *proxipooltag2object(pool, searchtag)   Unsafe dereference; fast    */
/*    macro.                                                                 */
/*  void *proxipooltag2object2(pool, searchtag)   Unsafe dereference to      */
/*    supplementary object; fast macro.                                      */
/*  proxipoolulong proxipooltag2allocindex(pool, searchtag)   Unsafe lookup  */
/*    of the allocation index that a tag is associated with; fast macro.     */
/*  tag proxipooliterate(pool, thistag)   Iterate over the allocated tags.   */
/*  tag proxipoolnew(pool, allocindex, outobject)   Allocate an object.      */
/*  void proxipoolfree(pool, killtag)   Deallocate tag for reuse.            */
/*  arraypoolulong proxipoolobjects(pool)   Return # of objects in pool.     */
/*  arraypoolulong proxipoolbytes(pool)   Return bytes of dynamic memory     */
/*    used by the proxipool.                                                 */
/*                                                                           */
/*  For internal use only:                                                   */
/*  typedef miniindex                                                        */
/*  struct minipoolheader                                                    */
/*  struct poolpool                                                          */
/*  void proxipoolrestartmini(pool, mini)                                    */
/*  struct minipoolheader *proxipooltag2mini(pool, searchtag)                */
/*  void proxipoolinitpoolpools(pool, endindex)                              */
/*                                                                           */
/*****************************************************************************/


/*  `proxipoolulong' is the type of (unsigned) integer used for allocation   */
/*  indices, here and in the allocation map (allocmap).  Its length can be   */
/*  changed.  A shorter choice consumes less memory; a longer choice         */
/*  increases the number of allocation indices that can exist.               */
/*                                                                           */
/*  In many cases, it might be possible to use a 32-bit proxipoolulong, even */
/*  if 64 bits are needed to index the vertices and other objects.  However, */
/*  proxipoolulongs take up such a tiny proportion of memory, it's probably  */
/*  not worth the risk.  On the other hand, it doesn't make sense to make    */
/*  proxipoolulongs longer than arraypoolulongs, because the allocation      */
/*  indices index objects in an arraypool.                                   */
/*                                                                           */
/*  The `tag' type is used specifically to denote tags, which are internal   */
/*  indices chosen to compress well.  A `miniindex' type indexes an object   */
/*  within a single minipool, and is kept as short as possible to minimize   */
/*  space in minipoolheaders; but a `miniindex' must be at least             */
/*  LOG2OBJECTSPERMINI + 1 bits long.                                        */

typedef arraypoolulong proxipoolulong;
typedef arraypoolulong tag;
typedef unsigned short miniindex;


/*  NOTATAG is a tag denoting no object in a proxipool.                      */

#define NOTATAG ((tag) ~0)

/*  NOTAMINIINDEX is an index denoting no freed object within a minipool.    */

#define NOTAMINIINDEX ((miniindex) ~0)


/*  The number of objects in a minipool.  (A minipool is the smallest unit   */
/*  of objects allocated at one time.)  Must be a power of 2.                */

#define OBJECTSPERMINI 1024

/*  The following MUST be the base-2 logarithm of the above.  If you change  */
/*  one, change the other to match.  Also, make sure that a `miniindex' can  */
/*  hold this many bits plus one.                                            */

#define LOG2OBJECTSPERMINI 10

/*  The number of minipools that are clustered together in a group, all      */
/*  associated with the same allocation index.  Must be a power of 2.        */

#define MINISPERGROUP 8

/*  LOG2POOLPOOLSPERBLOCK is the base-2 logarithm of the number of poolpools */
/*  allocated at a time.                                                     */

#define LOG2POOLPOOLSPERBLOCK 8


/*  A minipool is a block of OBJECTSPERMINI objects.  It consists of one     */
/*  minipoolheader, then OBJECTSPERMINI contiguous objects, then optionally  */
/*  OBJECTSPERMINI contiguous "supplementary" objects that are paired with   */
/*  the first OBJECTSPERMINI objects.  Typically, the supplementary objects  */
/*  contain information that is rarely accessed, and so ought to go into     */
/*  separate pages of virtual memory.                                        */
/*                                                                           */
/*  The minipoolheader contains information about the minipool.  To keep     */
/*  track of objects that are free to be allocated, `firstvirgin' indicates  */
/*  the index of the first object in the minipool that has never been        */
/*  allocated (implying that the objects that follow it are virgins too),    */
/*  and `freestack' is the head of a linked stack of objects in the minipool */
/*  that have been freed.  `object1block' and `object2block' indicate where  */
/*  the two blocks of OBJECTSPERMINI objects start.                          */
/*                                                                           */
/*  One goal is that objects that are spatially close should be close in     */
/*  memory too.  Therefore, each minipool has an allocation index,           */
/*  `allocindex'.  Typically, geometric objects like vertices are assigned   */
/*  allocation indices based on their positions.  For each alloction index,  */
/*  there is a structure of type `struc poolpool' (stored in an array of     */
/*  poolpools) that maintains a linked list of all the minipools associated  */
/*  with that allocation index.  `nextminifree' is the next minipool in the  */
/*  linked list of minipools that have free space.  (There is one such       */
/*  linked list for each allocation index.)                                  */

struct minipoolheader {
  char *object1block;    /* Pointer to the block of objects in the minipool. */
  char *object2block;          /* Optional pointer to supplementary objects. */
  proxipoolulong allocindex;                       /* Index of the poolpool. */
  tag nextminifree;     /* Next in linked list of minipools with free space. */
  miniindex firstvirgin;        /* First never-allocated object in minipool. */
  miniindex freestack; /* Head of linked stack of freed objects in minipool. */
};

/*  A poolpool is a pool of minipools associated with one allocation index.  */
/*  It references a list of minipools that have free space, through          */
/*  `freelisthead' and `freelisttail'.  It also references a group in which  */
/*  further minipools can be allocated.  `mygroup' specifies a group by      */
/*  tagging the next minipool that will be allocated in the group.           */

struct poolpool {
  tag freelisthead;            /* Head of list of minipools with free space. */
  tag freelisttail;            /* Tail of list of minipools with free space. */
  tag mygroup;                 /* Next minipool to be allocated in my group. */
};

/*  A proxipool is a pool from which objects can be allocated.  With the     */
/*  help of an allocation tree, it allocates objects so that they are close  */
/*  in memory to objects they are spatially close to.  `objectbytes1' is the */
/*  length of each object in bytes, and `objectbytes2' is the length of the  */
/*  supplementary information (if any) associated with each object.          */
/*  `block1offset' is the offset (in bytes) at which the first object        */
/*  appears after the beginning of each minipoolheader, and `block2offset'   */
/*  is the offset at which the first supplementary object appears.           */
/*  `minipoolsize' is the size (in bytes) of each minipool, including one    */
/*  minipoolheader, OBJECTSPERMINI objects, and perhaps OBJECTSPERMINI       */
/*  supplementary objects.                                                   */
/*                                                                           */
/*  `minipoolarray' is a tiered array of minipools.  `objects' is the number */
/*  of objects currently allocated from the proxipool, and `maxobjects' is   */
/*  the largest number that have been allocated at any one time.             */
/*  `nextgroup' is the first tag of the first minipool of the group of       */
/*  minipools that will be allocated next.                                   */
/*                                                                           */
/*  For each allocation index, each proxipool has a poolpool (pool of        */
/*  minipools) from which to allocate objects.  `poolpools' is an arraypool  */
/*  used to map allocation indices to poolpools.  `nextinitindex' is the     */
/*  index of the first poolpool that has not yet been initialized.           */
/*                                                                           */
/*  The number `verbosity' indicates how much debugging information to       */
/*  print, from none (0) to lots (4+).                                       */

struct proxipool {
  size_t objectbytes1;                    /* Size of one object in the pool. */
  size_t objectbytes2;                      /* Size of supplementary object. */
  size_t block1offset;                /* Offset of first object from header. */
  size_t block2offset;              /* Offset of first supplementary object. */
  size_t minipoolsize;            /* Size of one minipool, including header. */

  struct arraypool minipoolarray;              /* Tiered array of minipools. */
  arraypoolulong objects;          /* Number of currently allocated objects. */
  arraypoolulong maxobjects;              /* Maximum allocated objects ever. */
  tag nextgroup;                 /* Next group of minipools to be allocated. */

  struct arraypool poolpools;                  /* Tiered array of poolpools. */
  proxipoolulong nextinitindex;   /* First uninitialized index in the array. */
  int verbosity;                /* Amount of debugging information to print. */
};


/*****************************************************************************/
/*                                                                           */
/*  proxipoolrestartmini()   Reset a minipool to a pristine state.           */
/*                                                                           */
/*  The minipool is set so that it contains no objects, and all its space    */
/*  is available for allocating.                                             */
/*                                                                           */
/*  pool:  The proxipool containing the minipool.                            */
/*  mini:  The minipool to restart.                                          */
/*                                                                           */
/*****************************************************************************/

void proxipoolrestartmini(struct proxipool *pool,
                          struct minipoolheader *mini)
{
  /* Give the minipool pointers to its objects. */
  mini->object1block = ((char *) mini) + pool->block1offset;
  mini->object2block = pool->objectbytes2 > 0 ?
    ((char *) mini) + pool->block2offset : (char *) NULL;

  /* The minipool is not on any poolpool's list. */
  mini->nextminifree = NOTATAG;
  /* The minipool is associated with the default poolpool (index zero). */
  mini->allocindex = 0;
  /* The pool contains no objects. */
  mini->firstvirgin = 0;
  mini->freestack = NOTAMINIINDEX;
}

/*****************************************************************************/
/*                                                                           */
/*  proxipoolrestart()   Deallocate all objects in a proxipool.              */
/*                                                                           */
/*  The pool is returned to its starting state, except that no memory is     */
/*  freed to the operating system.  Rather, the previously allocated         */
/*  minipools are ready to be reused.                                        */
/*                                                                           */
/*  pool:  The proxipool to restart.                                         */
/*                                                                           */
/*****************************************************************************/

void proxipoolrestart(struct proxipool *pool)
{
  struct minipoolheader *mini;
  arraypoolulong maxindex;
  arraypoolulong i;

  /* Restart all the allocated minipools. */
  maxindex = (arraypoolulong) (pool->nextgroup >> LOG2OBJECTSPERMINI);
  for (i = 0; i < maxindex; i++) {
    mini = (struct minipoolheader *) arraypoollookup(&pool->minipoolarray, i);
    if (mini != (struct minipoolheader *) NULL) {
      proxipoolrestartmini(pool, mini);
    }
  }

  /* Free all the poolpools for reuse. */
  arraypoolrestart(&pool->poolpools);

  /* No objects have been allocated. */
  pool->objects = 0;
  /* The next group of minipools that will be allocated starts with tag */
  /*   zero.                                                            */
  pool->nextgroup = 0;
  /* No poolpool has been initialized. */
  pool->nextinitindex = 0;
}

/*****************************************************************************/
/*                                                                           */
/*  proxipoolinit()   Initialize a pool of memory for allocation of objects. */
/*                                                                           */
/*  Before a proxipool may be used, it must be initialized by this           */
/*  procedure.  After initialization, tags (with associated memory) can be   */
/*  allocated and freed in a proxipool.  proxipoolinit() does not allocate   */
/*  any memory from the operating system itself.                             */
/*                                                                           */
/*  The `pool' allocates space for objects of size `objectbytes1'.  Each     */
/*  object may also have a supplementary object with size `objectbytes2'.    */
/*  The supplementary objects are used to store information that is used     */
/*  infrequently and thus is best stored in different memory pages than the  */
/*  primary objects.                                                         */
/*                                                                           */
/*  Don't call this procedure on a proxipool that has already been           */
/*  initialized (by this procedure), unless you call proxipooldeinit() on    */
/*  it first.  You will leak memory.  Also see proxipoolrestart().           */
/*                                                                           */
/*  pool:  The proxipool to initialize.                                      */
/*  objectbytes1:  The size, in bytes, of the objects.                       */
/*  objectbytes2:  Size of the supplementary objects.  If zero, no space is  */
/*    allocated for these.                                                   */
/*  verbose:  How much debugging information proxipool procedures should     */
/*    print, from none (0) to lots (4+).                                     */
/*                                                                           */
/*****************************************************************************/

void proxipoolinit(struct proxipool *pool,
                   size_t objectbytes1,
                   size_t objectbytes2,
                   int verbose)
{
  size_t maxword;

  /* Find the size of the largest data object:  pointer, real, or long. */
  maxword = sizeof(arraypoolulong) > sizeof(void *) ?
            sizeof(arraypoolulong) : sizeof(void *);
  maxword = sizeof(starreal) > maxword ? sizeof(starreal) : maxword;

  /* The number of bytes occupied by an object. */
  pool->objectbytes1 = objectbytes1 > sizeof(miniindex) ?
                       objectbytes1 : sizeof(miniindex);
  /* The number of bytes occupied by a subsidiary object. */
  pool->objectbytes2 = objectbytes2 > 0 ? objectbytes2 : 0;

  /* The objects in a minipool begin `block1offset' bytes after the    */
  /*   minipoolheader.  It's rounded up to be a multiple of `maxword'. */
  pool->block1offset = ((sizeof(struct minipoolheader) - 1) / maxword + 1) *
                       maxword;
  /* The subsidiary objects in a minipool begin `block2offset' bytes after */
  /*   the minipoolheader.  It's rounded up to be a multiple of `maxword'. */
  pool->block2offset = ((pool->block1offset +
                         OBJECTSPERMINI * pool->objectbytes1 - 1) / maxword +
                        1) * maxword;
  /* `minipoolsize' is the total number of bytes in one minipool. */
  pool->minipoolsize = ((pool->block2offset +
                         OBJECTSPERMINI * pool->objectbytes2 - 1) / maxword +
                        1) * maxword;

  pool->verbosity = verbose;
  /* No objects in the pool yet. */
  pool->maxobjects = 0;

  /* Create a teired array of minipools.  Note that there is only one        */
  /*   minipool per block of the tiered array.  This is because I don't want */
  /*   to allocate memory for minipools that aren't being used, and because  */
  /*   the proxipool manages multiple objects within each minipool           */
  /*   differently than the arraypool manages objects within a block.        */
  arraypoolinit(&pool->minipoolarray, (arraypoolulong) pool->minipoolsize,
                (arraypoolulong) 0, 0);

  /* Create an array of poolpools. */
  arraypoolinit(&pool->poolpools, (arraypoolulong) sizeof(struct poolpool),
                (arraypoolulong) LOG2POOLPOOLSPERBLOCK, 0);

  proxipoolrestart(pool);
}

/*****************************************************************************/
/*                                                                           */
/*  proxipooldeinit()   Free to the operating system all memory taken by a   */
/*                      proxipool.                                           */
/*                                                                           */
/*  pool:  The proxipool to free.                                            */
/*                                                                           */
/*****************************************************************************/

void proxipooldeinit(struct proxipool *pool)
{
  /* Free the array of minipools. */
  arraypooldeinit(&pool->minipoolarray);
  /* Free the array of poolpools. */
  arraypooldeinit(&pool->poolpools);
}

/*****************************************************************************/
/*                                                                           */
/*  proxipooltag2mini()   Map a tag to the minipool it indexes.              */
/*                                                                           */
/*  WARNING:  fails catastrophically, probably with a segmentation fault, if */
/*  the tag's minipool doesn't yet exist.                                    */
/*                                                                           */
/*  Implemented as a macro to meet your need for speed.                      */
/*                                                                           */
/*  pool:  The proxipool in which to find a minipool.                        */
/*         Type (struct proxipool *).                                        */
/*  searchtag:  The tag whose minipool is sought.                            */
/*              Type (tag).                                                  */
/*                                                                           */
/*  Returns the minipool that contains the object with the given tag.  Might */
/*    crash if no such minipool has been allocated yet.                      */
/*    Type (struct minipoolheader *).                                        */
/*                                                                           */
/*****************************************************************************/

#define proxipooltag2mini(pool, searchtag)  \
  ((struct minipoolheader *)  \
   arraypoolfastlookup(&(pool)->minipoolarray,  \
                       (searchtag) >> LOG2OBJECTSPERMINI))

/*****************************************************************************/
/*                                                                           */
/*  proxipooltag2object()   Map a tag to the object it indexes.              */
/*                                                                           */
/*  WARNING:  fails catastrophically, probably with a segmentation fault, if */
/*  the tag's minipool doesn't yet exist.                                    */
/*                                                                           */
/*  Implemented as a macro to meet your need for speed.                      */
/*                                                                           */
/*  pool:  The proxipool in which to find an object.                         */
/*         Type (struct proxipool *).                                        */
/*  searchtag:  The tag whose object is sought.                              */
/*              Type (tag).                                                  */
/*                                                                           */
/*  Returns a pointer to the object.  Might crash if the tag has not been    */
/*    allocated yet.                                                         */
/*    Type (void *).                                                         */
/*                                                                           */
/*****************************************************************************/

#define proxipooltag2object(pool, searchtag)  \
  ((void *) (proxipooltag2mini(pool, searchtag)->object1block +  \
             (searchtag & (OBJECTSPERMINI - 1)) * (pool)->objectbytes1))

/*****************************************************************************/
/*                                                                           */
/*  proxipooltag2object2()   Map a tag to the supplementary object it        */
/*                           indexes.                                        */
/*                                                                           */
/*  WARNING:  fails catastrophically, probably with a segmentation fault, if */
/*  the tag's minipool doesn't yet exist.                                    */
/*                                                                           */
/*  Implemented as a macro to meet your need for speed.                      */
/*                                                                           */
/*  pool:  The proxipool in which to find a supplementary object.            */
/*         Type (struct proxipool *).                                        */
/*  searchtag:  The tag whose supplementary object is sought.                */
/*              Type (tag).                                                  */
/*                                                                           */
/*  Returns a pointer to the supplementary object.  Might crash if the tag   */
/*    has not been allocated yet.                                            */
/*    Type (void *).                                                         */
/*                                                                           */
/*****************************************************************************/

#define proxipooltag2object2(pool, searchtag)  \
  ((void *) (proxipooltag2mini(pool, searchtag)->object2block +  \
             (searchtag & (tag) (OBJECTSPERMINI - 1)) * (pool)->objectbytes2))

/*****************************************************************************/
/*                                                                           */
/*  proxipooltag2allocindex()   Map a tag to its allocation index.           */
/*                                                                           */
/*  WARNING:  fails catastrophically, probably with a segmentation fault, if */
/*  the tag's minipool doesn't yet exist.                                    */
/*                                                                           */
/*  Implemented as a macro to meet your need for speed.                      */
/*                                                                           */
/*  pool:  The proxipool in which an object is allocated.                    */
/*         Type (struct proxipool *).                                        */
/*  searchtag:  The tag of the object whose allocation index is sought.      */
/*              Type (tag).                                                  */
/*                                                                           */
/*  Returns the allocation index of the object.  Might crash if the tag has  */
/*    not been allocated yet.                                                */
/*    Type (proxipoolulong).                                                 */
/*                                                                           */
/*****************************************************************************/

#define proxipooltag2allocindex(pool, searchtag)  \
  (proxipooltag2mini(pool, searchtag)->allocindex)

/*****************************************************************************/
/*                                                                           */
/*  proxipooliterate()   Return the next allocated tag after a specified     */
/*                       tag.                                                */
/*                                                                           */
/*  This procedure is used to iterate over all the tags in a proxipool, from */
/*  smallest to largest, that have ever been allocated.  The iterator visits */
/*  tags that have been freed, as well as those that are still in use.       */
/*  It's up to the caller to distinguish objects that are not in use.        */
/*                                                                           */
/*  To get the iterator started (i.e. to find the first tag), call this      */
/*  procedure with `thistag' set to NOTATAG.                                 */
/*                                                                           */
/*  When there are no more tags, this procedure returns NOTATAG.             */
/*                                                                           */
/*  Note that when an object is freed, its first word or so gets             */
/*  overwritten by internal data, so it's a little bit dangerous to use this */
/*  procedure.  The caller needs to have a way to distinguish objects that   */
/*  it has previously freed, that does not rely on the first word of the     */
/*  object.  Yes, this is kludgy; but it would be slow for this procedure to */
/*  figure out whether an object has been freed or not, so speed was chosen  */
/*  over elegance.                                                           */
/*                                                                           */
/*  pool:  The proxipool whose tags you wish to iterate over.                */
/*  thistag:  The tag whose successor you seek, or NOTATAG to get started.   */
/*                                                                           */
/*  Returns the smallest allocated tag greater than `thistag', or NOTATAG if */
/*    there is no greater allocated tag.                                     */
/*                                                                           */
/*****************************************************************************/

tag proxipooliterate(struct proxipool *pool,
                     tag thistag)
{
  struct minipoolheader *mini;

  if (thistag == NOTATAG) {
    thistag = 0;
  } else {
    thistag++;
  }

  while (1) {
    if (thistag >= pool->nextgroup) {
      return NOTATAG;
    }
    mini = (struct minipoolheader *)
      arraypoollookup(&pool->minipoolarray, thistag >> LOG2OBJECTSPERMINI);
    /* Does a minipool containing tag `thistag' exist? */
    if (mini == (struct minipoolheader *) NULL) {
      /* No.  Jump to the start of the next group. */
      thistag = (thistag & (tag) ~(OBJECTSPERMINI * MINISPERGROUP - 1)) +
                OBJECTSPERMINI * MINISPERGROUP;
    } else if ((thistag & (OBJECTSPERMINI - 1)) >= mini->firstvirgin) {
      /* `thistag' has never been allocated.  Jump to the start of the */
      /*   next minipool.                                              */
      thistag = (thistag & (tag) ~(OBJECTSPERMINI - 1)) + OBJECTSPERMINI;
    } else {
      return thistag;
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*  proxipoolinitpoolpools()   Initializes uninitialized poolpools.          */
/*                                                                           */
/*  The allocation map is an array of poolpools.  This procedure's job is to */
/*  initialize intervals of poolpools just before they are used.  This       */
/*  includes making sure the arraypool has allocated space for them.         */
/*                                                                           */
/*  pool:  The proxipool whose allocation map might need initializing.       */
/*  endindex:  Make sure all poolpools up to and including the one whose     */
/*    index is `endindex' are allocated and initialized.                     */
/*                                                                           */
/*****************************************************************************/

void proxipoolinitpoolpools(struct proxipool *pool,
                            proxipoolulong endindex)
{
  struct poolpool *poollist;

  /* Walk through the poolpools that have not been initialized yet, up to */
  /*   `endindex'.                                                        */
  while (pool->nextinitindex <= endindex) {
    /* Make sure memory has been allocated for this poolpool, and get */
    /*   a pointer to it.                                             */
    poollist = (struct poolpool *)
               arraypoolforcelookup(&pool->poolpools,
                                    (arraypoolulong) pool->nextinitindex);
    /* The linked lists of minipools and the groups are empty. */
    poollist->freelisthead = NOTATAG;
    poollist->freelisttail = NOTATAG;
    poollist->mygroup = NOTATAG;

    pool->nextinitindex++;
  }
}

/*****************************************************************************/
/*                                                                           */
/*  proxipoolnew()   Allocate space for an object in a proxipool.            */
/*                                                                           */
/*  An allocation index `allocindex' is used to place the new object close   */
/*  in memory to other objects with the same allocation index.               */
/*                                                                           */
/*  pool:  The proxipool to allocate an object from.                         */
/*  allocindex:  An allocation index associated with the object to ensure    */
/*    it is allocated in memory near other objects that are geometrically    */
/*    nearby.                                                                */
/*  outobject:  If `outobject' is not NULL, a pointer to the new object is   */
/*    returned at the location `outobject' points to.                        */
/*                                                                           */
/*  Returns the tag of the new object.                                       */
/*                                                                           */
/*****************************************************************************/

tag proxipoolnew(struct proxipool *pool,
                 proxipoolulong allocindex,
                 void **outobject)
{
  struct minipoolheader *allocmini;
  struct poolpool *poollist;
  void *objectptr;
  tag alloctag, nexttag;
  miniindex objectindex;

  /* If the poolpool at index `allocindex' has not been initialized yet, */
  /*   initialize all the unitialized poolpools up to `allocindex'.      */
  if (allocindex >= pool->nextinitindex) {
    proxipoolinitpoolpools(pool, allocindex);
  }

  /* Find the assigned poolpool. */
  poollist = (struct poolpool *) arraypoolfastlookup(&pool->poolpools,
                                                     allocindex);
  /* Find the first minipool in the poolpool's list. */
  alloctag = poollist->freelisthead;
  if (alloctag != NOTATAG) {
    /* Get a pointer to the minipool. */
    allocmini = proxipooltag2mini(pool, alloctag);
  } else {
    /* The poolpool's list of minipools is empty.  Check for an unfinished */
    /*   group to allocate a new minipool from.                            */
    alloctag = poollist->mygroup;
    if (alloctag == NOTATAG) {
      /* The poolpool has no group.  Assign a new minipool (and a new group) */
      /*   to this poolpool.                                                 */
      alloctag = pool->nextgroup;
      /* Allocate a different group next time. */
      pool->nextgroup += OBJECTSPERMINI * MINISPERGROUP;
    }

    /* Allocate a new minipool that starts with the tag `alloctag'. */
    allocmini = (struct minipoolheader *)
      arraypoolforcelookup(&pool->minipoolarray,
                           (arraypoolulong) (alloctag >> LOG2OBJECTSPERMINI));

    /* Reset the new minipool to a pristine state. */
    proxipoolrestartmini(pool, allocmini);
    /* Put the new minipool on the poolpool's list of minipools. */
    poollist->freelisthead = alloctag;
    poollist->freelisttail = alloctag;

    /* Inform the minipool of the index of its poolpool. */
    allocmini->allocindex = allocindex;

    /* Is there another minipool in the same group, and if so, is the next */
    /*   minipool unallocated?                                             */
    if ((((alloctag >> LOG2OBJECTSPERMINI) & (MINISPERGROUP - 1)) <
         MINISPERGROUP - 1) &&
        (arraypoollookup(&pool->minipoolarray,
                         (arraypoolulong)
                         (alloctag >> LOG2OBJECTSPERMINI) + 1) ==
         (void *) NULL)) {
      /* Remember this group and the next minipool for use in the future. */
      poollist->mygroup = alloctag + OBJECTSPERMINI;
    } else {
      poollist->mygroup = NOTATAG;
    }
  }

  /* Check if the linked list of dead objects is empty. */
  objectindex = allocmini->freestack;
  if (objectindex != NOTAMINIINDEX) {
    /* Allocate an object from the linked list, rather than a fresh one. */
    objectptr = (void *) 
                &allocmini->object1block[pool->objectbytes1 * objectindex];
    allocmini->freestack = * (miniindex *) objectptr;
  } else {
    /* Allocate a previously unused object. */
    objectindex = allocmini->firstvirgin;
    allocmini->firstvirgin++;
    objectptr = (void *)
                &allocmini->object1block[pool->objectbytes1 * objectindex];
  }

  /* The tag of the new object is its minipool prefix plus its index. */
  alloctag += objectindex;
  /* Is the minipool full? */
  if ((allocmini->freestack == NOTAMINIINDEX) &&
      (allocmini->firstvirgin >= (miniindex) OBJECTSPERMINI)) {
    /* The minipool is full.  Remove it from the linked list. */
    nexttag = allocmini->nextminifree;
    poollist->freelisthead = nexttag;
    if (nexttag == NOTATAG) {
      poollist->freelisttail = NOTATAG;
    }
  }

  pool->objects++;
  if (pool->objects > pool->maxobjects) {
    pool->maxobjects = pool->objects;
  }

  /* Return the new object's memory address (if desired) and tag. */
  if (outobject != (void **) NULL) {
    *outobject = objectptr;
  }
  return alloctag;
}

/*****************************************************************************/
/*                                                                           */
/*  proxipoolfree()   Deallocate an object, freeing its space for reuse.     */
/*                                                                           */
/*  WARNING:  Calling this procedure with a tag that is already free will    */
/*  probably corrupt the proxipool and cause the freed tag to be allocated   */
/*  more than once.                                                          */
/*                                                                           */
/*  pool:  The proxipool the object was allocated from.                      */
/*  killtag:  The tag of the object that is no longer in use.                */
/*                                                                           */
/*****************************************************************************/

void proxipoolfree(struct proxipool *pool,
                   tag killtag)
{
  struct minipoolheader *deallocmini;
  struct minipoolheader *tailmini;
  struct poolpool *poollist;
  miniindex objectindex;

  /* Get a pointer to the minipool. */
  deallocmini = proxipooltag2mini(pool, killtag);
  objectindex = (miniindex) (killtag & (tag) (OBJECTSPERMINI - 1));

  /* Is the minipool full? */
  if ((deallocmini->freestack == NOTAMINIINDEX) &&
      (deallocmini->firstvirgin >= (miniindex) OBJECTSPERMINI)) {
    /* The minipool will no longer be full, so put it back in its poolpool's */
    /*   linked list of minipools.                                           */
    poollist = (struct poolpool *)
               arraypoolfastlookup(&pool->poolpools, deallocmini->allocindex);
    killtag = killtag & ~ (tag) (OBJECTSPERMINI - 1);
    if (poollist->freelisthead == NOTATAG) {
      /* The list is empty; insert the minipool at the head of the list. */
      poollist->freelisthead = killtag;
    } else {
      /* Insert the minipool at the tail of the list. */
      tailmini = proxipooltag2mini(pool, poollist->freelisttail);
      tailmini->nextminifree = killtag;
    }
    poollist->freelisttail = killtag;
    /* There is no next minipool in the list. */
    deallocmini->nextminifree = NOTATAG;
  }

  /* Insert the object into the linked list of dead objects. */
  * (miniindex *)
    &deallocmini->object1block[pool->objectbytes1 * objectindex] =
      deallocmini->freestack;
  deallocmini->freestack = objectindex;

  pool->objects--;
}

/*****************************************************************************/
/*                                                                           */
/*  proxipoolobjects()   Returns the number of objects allocated in the      */
/*                       proximity-based memory pool.                        */
/*                                                                           */
/*  pool:  The proxipool in question.                                        */
/*                                                                           */
/*  Returns the number of objects currently allocated in `pool'.             */
/*                                                                           */
/*****************************************************************************/

arraypoolulong proxipoolobjects(struct proxipool *pool)
{
  return pool->objects;
}

/*****************************************************************************/
/*                                                                           */
/*  proxipoolbytes()   Returns the number of bytes of dynamic memory used by */
/*                     the proximity-based memory pool.                      */
/*                                                                           */
/*  Does not include the memory for the `struct proxipool' record itself.    */
/*                                                                           */
/*  pool:  The proxipool in question.                                        */
/*                                                                           */
/*  Returns the number of dynamically allocated bytes in `pool'.             */
/*                                                                           */
/*****************************************************************************/

arraypoolulong proxipoolbytes(struct proxipool *pool)
{
  return arraypoolbytes(&pool->minipoolarray) +
         arraypoolbytes(&pool->poolpools);
}

/**                                                                         **/
/**                                                                         **/
/********* Proximity-based memory pool management routines end here  *********/


