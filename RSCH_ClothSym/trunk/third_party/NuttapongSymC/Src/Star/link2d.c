/********* 2D link/triangulation representation routines begin here  *********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  Compressed 2D links                                                      */
/*                                                                           */
/*  The link of a vertex in a 3D triangulation is a topologically two-       */
/*  dimensional triangulation, embedded in 3D space.  If the vertex is in    */
/*  the interior of the triangulation, then its link is a triangulation of a */
/*  topological sphere.  (This data structure can store the link of any      */
/*  vertex in a 3D triangulation, though.)                                   */
/*                                                                           */
/*  The triangles in a 2D link should be thought of as sharing a fixed       */
/*  "orientation".  I choose the orientation that has the vertices of each   */
/*  triangle in counterclockwise order, as viewed by the vertex whose link   */
/*  it is.  Here I'm assuming that there are no geometrically "inverted"     */
/*  tetrahedra, but this code is deliberately oblivious to geometry--it      */
/*  never looks at a coordinate.  You could use all-clockwise if you prefer. */
/*  However, the topological orientations of the triangles should all be     */
/*  mutually consistent.  You could not, for example, represent an           */
/*  unoriented manifold (e.g. a Mobius strip or Klein bottle).               */
/*                                                                           */
/*  2D links are stored in a compressed format adapted from Blandford,       */
/*  Blelloch, Cardoze, and Kadow.  See the header for a full citation.       */
/*                                                                           */
/*  Each link ring is stored in a linked list of list nodes of type          */
/*  `molecule'.  Molecules are described in the comments for "Compressed     */
/*  link rings."  2D links use essentially the same representation, and have */
/*  mostly the same invariants, as link rings.  The molecules for 2D links   */
/*  and link rings are allocated from the same proxipool.                    */
/*                                                                           */
/*  Here are the invariants of 2D links that differ from link rings:         */
/*                                                                           */
/*  - The compressed tags in the linked list alternate between vertex tags   */
/*    and link ring tags.  Each vertex in the list is followed by the tag    */
/*    for its link ring.                                                     */
/*                                                                           */
/*  - Vertices in a 2D link may appear in any order.  Their order is not     */
/*    relevant.                                                              */
/*                                                                           */
/*  - A 2D link can contain a GHOSTVERTEX, but only one.  In a 2D link, the  */
/*    link ring of a ghost vertex represents ghost triangles and ghost       */
/*    tetrahedra.                                                            */
/*                                                                           */
/*  - A 2D link cannot contain an empty link ring.  When a link ring empties */
/*    out, it must be removed from any 2D link that contains it.             */
/*                                                                           */
/*  - Whereas each vertex tag is compressed against a "ground tag" (just     */
/*    like in link rings), each link ring tag is compressed against the tag  */
/*    for the 2D link.                                                       */
/*                                                                           */
/*  Public interface:                                                        */
/*  GHOSTVERTEX   Tag representing a ghost vertex; borrowed from linkring.   */
/*  STOP   Tag representing an unsuccessful query; borrowed from linkring.   */
/*  link2dcache   Type for a cache used to speed up link2dfindring().        */
/*  struct link2dposition   Represents a position in a 2D link.              */
/*  tag link2dnew(pool, allocindex)   Allocate a new, empty 2D link.         */
/*  void link2ddelete(pool, link)   Free a 2D link to the pool.              */
/*  void link2disempty(pool, link)   Determine if a 2D link is empty.        */
/*  void link2dcacheinit(cache)   Reset a 2D link cache to empty.            */
/*  tag link2dfindring(pool, cache, link, groundtag, searchvertex)   Return  */
/*    the link ring associated with a vertex in a 2D link.                   */
/*  tag link2dfindinsert(pool, cache, linkhead, linktail, groundtag,         */
/*    searchvertex)   Return or create a vertex/link ring in a 2D link.      */
/*  void link2dinsertvertex(pool, cache, linkhead, linktail, groundtag,      */
/*    newvertex, linkring)   Insert a vertex/link ring in a 2D link.         */
/*  tag link2ddeletevertex(pool, cache, linkhead, linktail, groundtag,       */
/*    deletevertex)   Delete a vertex from a 2D link; return link ring.      */
/*  void link2diteratorinit(pool, link, groundtag, pos)   Initialize an      */
/*    iterator that traverses all the vertices in a link ring one by one.    */
/*  void link2diterate(pool, pos, vertexandlink[2])   Read two tags (for a   */
/*    vertex and its link ring) and advance the iterator.                    */
/*  void link2dprint(pool, link, grountag)   Print the contents of a link.   */
/*                                                                           */
/*  For internal use only:                                                   */
/*  tag link2dfindringnocache(pool, link, groundtag, searchvertex)   Return  */
/*    the link ring associated with a vertex in a 2D link.                   */
/*  void link2dinsertvertexnocache(pool, linkhead, linktail, groundtag,      */
/*    newvertex, linkring)   Insert a vertex/link ring in a 2D link.         */
/*  tag link2ddeletevertexnocache(pool, linkhead, linktail, groundtag,       */
/*    deletevertex)   Delete a vertex from a 2D link; return link ring.      */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/*  The 2D link cache                                                        */
/*                                                                           */
/*  A user of the link2d procedures may optionally use a cache of type       */
/*  "link2dcache" to speed up the link2dfindring() procedure.  In my         */
/*  Delaunay triangulation implementation, this improves link2dfindring()    */
/*  from taking about 30% of the running time to taking about 3.5% of the    */
/*  running time.                                                            */
/*                                                                           */
/*  A cache should be initialized with link2dcacheinit() before use.  Once   */
/*  used, the cache must be passed to all subsequent calls to                */
/*  link2dinsertvertex(), link2dfindinsert(), and link2ddeletevertex() on    */
/*  the same proxipool; or link2dfindring() may subsequently return out-of-  */
/*  date information.  The cache is passed to link2dfindring() to improve    */
/*  its speed.                                                               */
/*                                                                           */
/*  A cache cannot be used for more than one link proxipool simultaneously.  */
/*  The cache simply maps a 2D link tag and a vertex tag to a link ring tag, */
/*  with no knowledge of what pools the tags are drawn from.                 */
/*                                                                           */
/*  Any procedure that takes a cache as a parameter will accept a NULL       */
/*  pointer (if you don't want to bother with cacheing).                     */
/*                                                                           */
/*****************************************************************************/

/*  LINK2DCACHESIZE is the total number of entries in the cache that speeds  */
/*  up lookup of vertices in 2D links.  Because the cache is two-way         */
/*  associative, the number of buckets is half this number.  LINK2DCACHESIZE */
/*  _must_ be a power of two.                                                */

#define LINK2DCACHESIZE 16384

/*  LINK2DPRIME is a prime number used in a (not very good) hash function.   */

#define LINK2DPRIME 16908799u


/*  A link2dcache is an array that caches the link rings for 2D link/vertex  */
/*  pairs that have been recently looked up or updated.  A link cache        */
/*  greatly speeds up the link2dfindring() procedure.                        */

typedef struct {
  tag mylink2d;
  tag myvertex;
  tag mylinkring;
} aaa;
typedef aaa link2dcache[LINK2DCACHESIZE];

/*  A link2dposition represents an atom in a 2D link.  It is the data        */
/*  structure used as an iterator that walks through a 2D link.              */

struct link2dposition {
  struct linkposition innerpos;                  /* Position in the 2D link. */
  tag linktag;     /* Tag for the link's first molecule, used to decompress. */
  tag groundvertex;         /* Tag for the link's owner, used to decompress. */
};


/*****************************************************************************/
/*                                                                           */
/*  link2dnew()   Allocate a new, empty 2D link triangulation.               */
/*                                                                           */
/*  The parameters include an allocation index, used to determine where the  */
/*  new link will be stored in memory.  Links with the same allocation index */
/*  go into common areas in memory.  The idea is to create spatial           */
/*  coherence:  links that are geometrically close to each other are near    */
/*  each other in memory, too.                                               */
/*                                                                           */
/*  pool:  The proxipool to allocate the link from.                          */
/*  allocindex:  An allocation index associated with the link.               */
/*                                                                           */
/*  Returns the tag of the first (and only) molecule of the new link.        */
/*                                                                           */
/*****************************************************************************/

tag link2dnew(struct proxipool *pool,
              proxipoolulong allocindex)
{
  molecule cule;
  tag newlink;

  /* Allocate a molecule to hold the (empty) link triangulation. */
  newlink = proxipoolnew(pool, allocindex, (void **) &cule);
  /* There are no vertices in this link. */
  cule[0] = (char) STOP;
  /* There is no next molecule. */
  cule[MOLECULESIZE - 1] = (char) STOP;

  return newlink;
}

/*****************************************************************************/
/*                                                                           */
/*  link2ddelete()   Free all the molecules in a 2D link to the pool.        */
/*                                                                           */
/*  Does not free the link rings referenced by the link.                     */
/*                                                                           */
/*  WARNING:  If a "link2dcache" is in use, this procedure does not clear    */
/*  entries in this 2D link from the cache.  If `link' is not empty, then    */
/*  then you should clear the cache with link2dcacheinit().                  */
/*                                                                           */
/*  pool:  The proxipool that the molecules were allocated from.             */
/*  link:  Tag for the first molecule in the link.                           */
/*                                                                           */
/*****************************************************************************/

void link2ddelete(struct proxipool *pool,
                  tag link)
{
  molecule cule;
  tag nextmoleculetag;
  tag moleculetag;
  int atomindex;
  char nowatom;

  if (link >= STOP) {
    return;
  }

  nextmoleculetag = link;

  /* Loop through the molecules and free them. */
  while (nextmoleculetag != STOP) {
    /* Convert the tag to a molecule. */
    cule = (molecule) proxipooltag2object(pool, nextmoleculetag);
#ifdef SELF_CHECK
    if (cule == (molecule) NULL) {
      printf("Internal error in link2ddelete():\n");
      printf("  Molecule tag indexes a NULL pointer.\n");
      internalerror();
    }
#endif /* SELF_CHECK */
    moleculetag = nextmoleculetag;
    /* Read the "next molecule tag". */
    linkringreadtag(moleculetag, cule, atomindex, nextmoleculetag, nowatom,
                    "Internal error in link2ddelete():\n");
    /* Free the molecule. */
    proxipoolfree(pool, moleculetag);
  }
}

/*****************************************************************************/
/*                                                                           */
/*  link2disempty()   Determine if a 2D link is empty (or invalid).          */
/*                                                                           */
/*  pool:  The proxipool that the link was allocated from.                   */
/*  link:  Tag for the first molecule in the link.                           */
/*                                                                           */
/*  Returns 1 if the 2D link is empty (or the tag is invalid); 0 otherwise.  */
/*                                                                           */
/*****************************************************************************/

int link2disempty(struct proxipool *pool,
                  tag link)
{
  molecule cule;

  if (link >= STOP) {
    /* Not a valid link tag. */
    return 1;
  }

  /* Convert the tag to a molecule. */
  cule = (molecule) proxipooltag2object(pool, link);
  if ((cule == (molecule) NULL) || (cule[0] == (char) STOP)) {
    /* Missing or empty link. */
    return 1;
  } else {
    /* Not empty. */
    return 0;
  }
}

/*****************************************************************************/
/*                                                                           */
/*  link2diteratorinit()   Initialize an iterator that traverses all the     */
/*                         vertices in a link one by one.                    */
/*                                                                           */
/*  The iterator is a variable `pos', whose internals should not be          */
/*  examined or modified by the client.  The iterator's job is to keep       */
/*  track of where it is in the link.  This procedure sets the iterator to   */
/*  reference the first vertex in the link.                                  */
/*                                                                           */
/*  When a link is modified, any iterators on that link may be corrupted and */
/*  should not be used without being initialized (by this procedure) again.  */
/*                                                                           */
/*  pool:  The proxipool that the link was allocated from.                   */
/*  link:  Tag for the first molecule in the link to traverse.               */
/*  groundtag:  The ground tag relative to which the vertices in the link    */
/*    will be decompressed.                                                  */
/*  pos:  The iterator.                                                      */
/*                                                                           */
/*****************************************************************************/

void link2diteratorinit(struct proxipool *pool,
                        tag link,
                        tag groundtag,
                        struct link2dposition *pos)
{
  char nowatom;

  pos->linktag = link;
  pos->groundvertex = groundtag;
  if (link >= STOP) {
    /* Not a link. */
    pos->innerpos.cule = (molecule) NULL;
    pos->innerpos.moleculetag = STOP;
    pos->innerpos.nextmoleculetag = STOP;
    pos->innerpos.textindex = MOLECULESIZE;
    pos->innerpos.lasttextindex = -1;
  } else {
    /* Find the molecule identified by the tag `link'. */
    pos->innerpos.cule = (molecule) proxipooltag2object(pool, link);
#ifdef SELF_CHECK
    if (pos->innerpos.cule == (molecule) NULL) {
      printf("Internal error in link2diteratorinit():\n");
      printf("  Molecule tag indexes a NULL pointer.\n");
      internalerror();
    }
#endif /* SELF_CHECK */
    pos->innerpos.moleculetag = link;
    /* Read the molecule's "next molecule tag". */
    linkringreadtag(link, pos->innerpos.cule, pos->innerpos.lasttextindex,
                    pos->innerpos.nextmoleculetag, nowatom,
                    "Internal error in link2diteratorinit():\n");
    /* Start the iterations from the beginning of the molecule. */
    pos->innerpos.textindex = 0;
  }
}

/*****************************************************************************/
/*                                                                           */
/*  link2diterate()   Return the two tags (for a vertex and its link ring)   */
/*                    that a 2D link iterator references, and advance the    */
/*                    iterator so it will return the next vertex next time.  */
/*                                                                           */
/*  The iterator is a variable `pos', whose internals should not be          */
/*  examined or modified by the client.                                      */
/*                                                                           */
/*  After a link is modified, any iterators on that link created before the  */
/*  modification may be corrupted and should not be passed to this procedure */
/*  again until they are re-initialized.                                     */
/*                                                                           */
/*  pool:  The proxipool that the link was allocated from.                   */
/*  pos:  The iterator.                                                      */
/*  vertexandlink:  An array used to return the tags for a vertex (in        */
/*    vertexandlink[0]) and the link ring associated with it (in             */
/*    vertexandlink[1]).  If the iterator has reached the end of the list,   */
/*    both values will be STOP on return.  Does not need to be initialized   */
/*    before the call.                                                       */
/*                                                                           */
/*****************************************************************************/

void link2diterate(struct proxipool *pool,
                   struct link2dposition *pos,
                   tag vertexandlink[2])
{
  tag texttag;
  int textatoms;
  int vertextagflag;
  char nowatom;

  texttag = 0;
  textatoms = 0;
  /* Set to 1 while reading a vertex tag; 0 when reading a link ring tag. */
  vertextagflag = 1;

  /* Loop through atoms to build up two tags. */
  while (1) {
    /* Have we read the last text atom in this molecule? */
    if ((pos->innerpos.textindex > pos->innerpos.lasttextindex) ||
        ((pos->innerpos.cule[pos->innerpos.textindex] == (char) STOP) &&
         (textatoms == 0))) {
      /* Yes, we have.  Are there any more molecules after this one? */
      if (pos->innerpos.nextmoleculetag == STOP) {
        /* No, there aren't.  The iterator is finished. */
#ifdef SELF_CHECK
        if (textatoms > 0) {
          /* The end of the last tag is missing from the last molecule. */
          printf("Internal error in link2diterate():\n");
          printf("  Tag at end of 2D link not properly terminated.\n");
          internalerror();
        }
        if (!vertextagflag) {
          /* There is a vertex tag without a link ring tag following it. */
          printf("Internal error in link2diterate():\n");
          printf("  Vertex tag not followed by link ring tag.\n");
          internalerror();
        }
#endif /* SELF_CHECK */

        vertexandlink[0] = STOP;
        vertexandlink[1] = STOP;
        return;
      }

      /* Find the next molecule in the linked list. */
      pos->innerpos.cule =
        (molecule) proxipooltag2object(pool, pos->innerpos.nextmoleculetag);
#ifdef SELF_CHECK
      if (pos->innerpos.cule == (molecule) NULL) {
        printf("Internal error in link2diterate():\n");
        printf("  Molecule tag indexes a NULL pointer.\n");
        internalerror();
      }
#endif /* SELF_CHECK */
      pos->innerpos.moleculetag = pos->innerpos.nextmoleculetag;
      /* Find the next molecule's "next molecule tag". */
      linkringreadtag(pos->innerpos.moleculetag, pos->innerpos.cule,
                      pos->innerpos.lasttextindex,
                      pos->innerpos.nextmoleculetag, nowatom,
                      "Internal error in link2diterate():\n");
      /* Start from the beginning of this molecule. */
      pos->innerpos.textindex = 0;
    }

    /* Read the next atom. */
    nowatom = pos->innerpos.cule[pos->innerpos.textindex];
    /* Append it to the tag. */
    texttag = (texttag << 7) + (nowatom & (char) 127);
    textatoms++;
    pos->innerpos.textindex++;

    /* If this is the end of a tag, store it for the caller. */
    if (nowatom < (char) 0) {
      if (vertextagflag) {
        if ((nowatom == (char) GHOSTVERTEX) && (textatoms == 1)) {
          vertexandlink[0] = GHOSTVERTEX;
        } else {
          /* Get the high-order bits from the ground vertex's tag. */
          vertexandlink[0] = texttag +
            ((pos->groundvertex >> (7 * textatoms)) << (7 * textatoms));
        }
        /* Prepare to read another tag. */
        texttag = 0;
        textatoms = 0;
        vertextagflag = 0;
      } else {
        /* Get the high-order bits from the first molecule's tag. */
        vertexandlink[1] = texttag +
          ((pos->linktag >> (7 * textatoms)) << (7 * textatoms));
        return;
      }
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*  link2dprint()   Print the contents of a 2D link.                         */
/*                                                                           */
/*  pool:  The proxipool that the link was allocated from.                   */
/*  link:  Tag for the first molecule in the link.                           */
/*  groundtag:  The ground tag relative to which the vertices in the link    */
/*    will be decompressed.                                                  */
/*                                                                           */
/*****************************************************************************/

void link2dprint(struct proxipool *pool,
                 tag link,
                 tag groundtag)
{
  struct link2dposition pos;
  tag vertexandlink[2];

  printf("2D link %lu:\n", (unsigned long) link);
  link2diteratorinit(pool, link, groundtag, &pos);
  link2diterate(pool, &pos, vertexandlink);
  if (vertexandlink[0] == STOP) {
    printf("  EMPTY\n");
  } else do {
    if (vertexandlink[0] == GHOSTVERTEX) {
      printf("  Vertex tag GHOST, ");
    } else {
      printf("  Vertex tag %lu, ", (unsigned long) vertexandlink[0]);
    }
    linkringprint(pool, vertexandlink[1], groundtag);
    link2diterate(pool, &pos, vertexandlink);
  } while (vertexandlink[0] != STOP);
}

/*****************************************************************************/
/*                                                                           */
/*  link2dcacheinit()   Reset a 2D link cache to empty.                      */
/*                                                                           */
/*  A "link2dcache" should always be initialized before use.  It should also */
/*  be cleared by this procedure if you use link2ddelete() to free a 2D link */
/*  that is not empty.                                                       */
/*                                                                           */
/*  cache:  The cache to initialize.                                         */
/*                                                                           */
/*****************************************************************************/

void link2dcacheinit(link2dcache cache) {
  int i;

  for (i = 0; i < LINK2DCACHESIZE; i++) {
    cache[i].mylink2d = STOP;
  }
}

/*****************************************************************************/
/*                                                                           */
/*  link2dfindringnocache()   Return the link ring associated with a vertex  */
/*                            in a 2D link.                                  */
/*                                                                           */
/*  pool:  The proxipool that the link was allocated from.                   */
/*  link:  Tag for the first molecule in the link.                           */
/*  groundtag:  The ground tag relative to which the vertices are            */
/*    decompressed.                                                          */
/*  searchvertex:  The tag of the vertex to search for.                      */
/*                                                                           */
/*  Returns the tag of the link ring associated with `searchvertex' if       */
/*    `searchvertex' is in the link; STOP otherwise.                         */
/*                                                                           */
/*****************************************************************************/

tag link2dfindringnocache(struct proxipool *pool,
                          tag link,
                          tag groundtag,
                          tag searchvertex)
{
  molecule cule;
  tag nextmoleculetag;
  tag moleculetag;
  tag texttag;
  int textatoms;
  int atomindex;
  int lasttextindex;
  int vertextagflag;
  int matchflag;
  char nowatom;

  if (link >= STOP) {
    /* Not a link. */
    return STOP;
  }

  texttag = 0;
  textatoms = 0;
  /* Start at the first molecule. */
  nextmoleculetag = link;
  /* Set to 1 while reading a vertex tag; 0 when reading a link ring tag. */
  vertextagflag = 1;
  matchflag = 0;

  /* Loop through the linked list of molecules. */
  do {
    /* Convert the molecule's tag to a pointer. */
    cule = (molecule) proxipooltag2object(pool, nextmoleculetag);
#ifdef SELF_CHECK
    if (cule == (molecule) NULL) {
      printf("Internal error in link2dfindringnocache():\n");
      printf("  Molecule tag indexes a NULL pointer.\n");
      internalerror();
    }
#endif /* SELF_CHECK */
    moleculetag = nextmoleculetag;
    /* Read the "next molecule tag". */
    linkringreadtag(moleculetag, cule, lasttextindex, nextmoleculetag, nowatom,
                    "Internal error in link2dfindringnocache():\n");

    atomindex = 0;
    /* Loop through the atoms, stopping at the "next molecule tag". */
    while (atomindex <= lasttextindex) {
      nowatom = cule[atomindex];
      /* Append the atom to the text tag. */
      texttag = (texttag << 7) + (nowatom & (char) 127);
      textatoms++;
      atomindex++;

      /* Is this the last atom in the compressed tag? */
      if (nowatom < (char) 0) {
        /* Yes; we have an uncompressed tag now.  What tag is it? */
        if ((nowatom == (char) STOP) && (textatoms == 1)) {
          /* STOP tag.  Exit the inner loop early; go on to next molecule. */
          break;
        } else if (vertextagflag) {
          /* Is this the vertex we're searching for? */
          if ((nowatom == (char) GHOSTVERTEX) && (textatoms == 1)) {
            /* Ghost vertex. */
            matchflag = searchvertex == GHOSTVERTEX;
          } else {
            /* Use the ground tag to supply the high-order bits of the */
            /*   vertex tag.                                           */
            matchflag =
              searchvertex ==
              texttag + ((groundtag >> (7 * textatoms)) << (7 * textatoms));
          }
        } else if (matchflag) {
          /* Use the link tag to supply the high-order bits of the */
          /*   link ring.  Return the latter.                      */
          return texttag + ((link >> (7 * textatoms)) << (7 * textatoms));
        }

        /* Prepare to read another tag. */
        texttag = 0;
        textatoms = 0;
        /* Alternate between reading vertex tags and link ring tags. */
        vertextagflag = !vertextagflag;
      }
    }
  } while (nextmoleculetag != STOP);

#ifdef SELF_CHECK
  if (nowatom >= (char) 0) {
    /* Oops.  The end of the last tag is missing from the last molecule. */
    printf("Internal error in link2dfindringnocache():\n");
    printf("  Tag at end of 2D link not properly terminated.\n");
    internalerror();
  }
  if (!vertextagflag) {
    /* There is a vertex tag without a link ring tag following it. */
    printf("Internal error in link2dfindringnocache():\n");
    printf("  Vertex tag not followed by link ring tag.\n");
    internalerror();
  }
#endif /* SELF_CHECK */

  /* `searchvertex' is not in the link. */
  return STOP;
}

/*****************************************************************************/
/*                                                                           */
/*  link2dfindring()   Return the link ring associated with a vertex in a 2D */
/*                     link.                                                 */
/*                                                                           */
/*  Resorts to calling link2dfindringnocache() if the answer is not in the   */
/*  cache.                                                                   */
/*                                                                           */
/*  pool:  The proxipool that the link was allocated from.                   */
/*  cache:  An (optional) cache that may speed up the lookup.                */
/*  link:  Tag for the first molecule in the link.                           */
/*  groundtag:  The ground tag relative to which the vertices are            */
/*    decompressed.                                                          */
/*  searchvertex:  The tag of the vertex to search for.                      */
/*                                                                           */
/*  Returns the tag of the link ring associated with `searchvertex' if       */
/*    `searchvertex' is in the link; STOP otherwise.                         */
/*                                                                           */
/*****************************************************************************/

tag link2dfindring(struct proxipool *pool,
                   link2dcache cache,
                   tag link,
                   tag groundtag,
                   tag searchvertex)
{
  int hash;
  tag linkring;

  if (link >= STOP) {
    /* Not a link. */
    return STOP;
  } else if (cache == NULL) {
    return link2dfindringnocache(pool, link, groundtag, searchvertex);
  }

  hash = (LINK2DPRIME * link * link + 2u * searchvertex) &
         (LINK2DCACHESIZE - 2);
  if ((cache[hash].mylink2d == link) &&
      (cache[hash].myvertex == searchvertex)) {
    return cache[hash].mylinkring;
  }
  if ((cache[hash + 1].mylink2d == link) &&
      (cache[hash + 1].myvertex == searchvertex)) {
    linkring = cache[hash + 1].mylinkring;
  } else {
    linkring = link2dfindringnocache(pool, link, groundtag, searchvertex);
  }

  cache[hash + 1].mylink2d = cache[hash].mylink2d;
  cache[hash + 1].myvertex = cache[hash].myvertex;
  cache[hash + 1].mylinkring = cache[hash].mylinkring;
  cache[hash].mylink2d = link;
  cache[hash].myvertex = searchvertex;
  cache[hash].mylinkring = linkring;
  return linkring;
}

/*****************************************************************************/
/*                                                                           */
/*  link2dinsertvertexnocache()   Insert a vertex into a 2D link, associated */
/*                                with a link ring.                          */
/*                                                                           */
/*  This procedure does not check whether the vertex `newvertex' is already  */
/*  in the link.  It's the client's responsibility not to call this          */
/*  procedure if the vertex is already there.  If it might be there or might */
/*  not, call link2dfindring() first, or use link2dfindinsert() instead.     */
/*                                                                           */
/*  pool:  The proxipool that the link was allocated from, and that new      */
/*    molecules will be allocated from.                                      */
/*  linkhead:  Tag for the first molecule in the link.                       */
/*  linktail:  Tag for the last molecule in the link.  Passed by pointer,    */
/*    and may be modified on return.                                         */
/*  groundtag:  The ground tag relative to which the vertices are            */
/*    decompressed.                                                          */
/*  newvertex:  The new vertex to insert into the link.                      */
/*  linkring:  The link ring associated with the vertex `newvertex'.         */
/*                                                                           */
/*****************************************************************************/

void link2dinsertvertexnocache(struct proxipool *pool,
                               tag linkhead,
                               tag *linktail,
                               tag groundtag,
                               tag newvertex,
                               tag linkring)
{
  struct linkposition position;
  int numatoms;
  int starttagflag;
  char newtextatoms[2 * COMPRESSEDTAGLENGTH];

  if ((linkhead >= STOP) || (*linktail >= STOP) ||
      (newvertex == STOP) || (linkring >= STOP)) {
    /* Invalid parameters. */
    return;
  }

  /* Start at the last molecule. */
  position.moleculetag = *linktail;
  position.nextmoleculetag = STOP;
  /* Convert the molecule's tag to a pointer. */
  position.cule = (molecule) proxipooltag2object(pool, position.moleculetag);
#ifdef SELF_CHECK
  if (position.cule == (molecule) NULL) {
    printf("Internal error in link2dinsertvertexnocache():\n");
    printf("  Molecule tag indexes a NULL pointer.\n");
    internalerror();
  }
#endif /* SELF_CHECK */
  if (position.cule[MOLECULESIZE - 1] != (char) STOP) {
    /* Oops.  `linktail' isn't really a tail molecule. */
    printf("Internal error in link2dinsertvertexnocache():\n");
    printf("  Parameter `linktail' is not the tail of a list.\n");
    internalerror();
  }
  position.lasttextindex = MOLECULESIZE - 2;

  /* Search for the end of the atoms in the link.  If the head and tail   */
  /*   molecules are the same, the list might be empty, in which case the */
  /*   very first atom is STOP.  Otherwise, the tail molecule must begin  */
  /*   with at least one non-STOP atom (this is an invariant of the link  */
  /*   data structure), so we check the stop bit of the first atom to see */
  /*   whether the second atom might be a STOP atom.                      */
  if (linkhead == *linktail) {
    starttagflag = 1;
    position.textindex = 0;
  } else {
    starttagflag = position.cule[0] < (char) 0;
    position.textindex = 1;
  }
  /* Loop through the atoms, looking for a STOP tag. */
  while ((position.cule[position.textindex] != (char) STOP) ||
         !starttagflag) {
    if (position.textindex >= MOLECULESIZE - 1) {
      /* Oops.  The end of the last tag is missing from the last molecule. */
      printf("Internal error in link2dinsertvertexnocache():\n");
      printf("  Link ring not properly terminated.\n");
      internalerror();
    }
    starttagflag = position.cule[position.textindex] < (char) 0;
    position.textindex++;
  }

  numatoms = linkringtagcompress(linkhead, linkring, newtextatoms);
  numatoms += linkringtagcompress(groundtag, newvertex,
                                  &newtextatoms[numatoms]);
  *linktail = linkringinsertatoms(pool, &position, numatoms, newtextatoms,
                                  proxipooltag2allocindex(pool, linkhead));
}

/*****************************************************************************/
/*                                                                           */
/*  link2dinsertvertex()   Insert a vertex into a 2D link, associated with   */
/*                         a link ring.                                      */
/*                                                                           */
/*  This procedure does not check whether the vertex `newvertex' is already  */
/*  in the link.  It's the client's responsibility not to call this          */
/*  procedure if the vertex is already there.  If it might be there or might */
/*  not, call link2dfindring() first, or use link2dfindinsert() instead.     */
/*                                                                           */
/*  This procedure updates the cache to reflect the insertion, then calls    */
/*  link2dinsertvertexnocache() to perform the insertion.                    */
/*                                                                           */
/*  pool:  The proxipool that the link was allocated from, and that new      */
/*    molecules will be allocated from.                                      */
/*  cache:  An (optional) cache that will be updated by the insertion.       */
/*  linkhead:  Tag for the first molecule in the link.                       */
/*  linktail:  Tag for the last molecule in the link.  Passed by pointer,    */
/*    and may be modified on return.                                         */
/*  groundtag:  The ground tag relative to which the vertices are            */
/*    decompressed.                                                          */
/*  newvertex:  The new vertex to insert into the link.                      */
/*  linkring:  The link ring associated with the vertex `newvertex'.         */
/*                                                                           */
/*****************************************************************************/

void link2dinsertvertex(struct proxipool *pool,
                        link2dcache cache,
                        tag linkhead,
                        tag *linktail,
                        tag groundtag,
                        tag newvertex,
                        tag linkring)
{
  int hash;

  if ((linkhead >= STOP) || (*linktail >= STOP) ||
      (newvertex == STOP) || (linkring >= STOP)) {
    /* Invalid parameters. */
    return;
  }

  if (cache != NULL) {
    hash = (LINK2DPRIME * linkhead * linkhead + 2u * newvertex) &
           (LINK2DCACHESIZE - 2);
    if ((cache[hash].mylink2d != linkhead) ||
        (cache[hash].myvertex != newvertex)) {
      cache[hash + 1].mylink2d = cache[hash].mylink2d;
      cache[hash + 1].myvertex = cache[hash].myvertex;
      cache[hash + 1].mylinkring = cache[hash].mylinkring;
      cache[hash].mylink2d = linkhead;
      cache[hash].myvertex = newvertex;
    }
    cache[hash].mylinkring = linkring;
  }

  return link2dinsertvertexnocache(pool, linkhead, linktail, groundtag,
                                   newvertex, linkring);
}

/*****************************************************************************/
/*                                                                           */
/*  link2dfindinsert()   Return the link ring associated with a vertex in a  */
/*                       2D link.  If the vertex is not in the link, create  */
/*                       an empty link ring for it and add it to the link.   */
/*                                                                           */
/*  pool:  The proxipool that the link was allocated from, and that new      */
/*    molecules will be allocated from.                                      */
/*  cache:  An (optional) cache that may speed up the lookup, and will be    */
/*    updated if the vertex is inserted.                                     */
/*  linkhead:  Tag for the first molecule in the link.                       */
/*  linktail:  Tag for the last molecule in the link.  Passed by pointer,    */
/*    and may be modified on return.                                         */
/*  groundtag:  The ground tag relative to which the vertices are            */
/*    decompressed.                                                          */
/*  searchvertex:  The tag of the vertex to search for, and possibly insert. */
/*                                                                           */
/*  Returns the link ring associated with `searchvertex' (or STOP if         */
/*    `linkhead' isn't a legal tag--namely, if it's STOP or GHOSTVERTEX).    */
/*                                                                           */
/*****************************************************************************/

tag link2dfindinsert(struct proxipool *pool,
                     link2dcache cache,
                     tag linkhead,
                     tag *linktail,
                     tag groundtag,
                     tag searchvertex)
{
  tag linkring;

  if (linkhead >= STOP) {
    /* Not a link. */
    return STOP;
  }

  linkring = link2dfindring(pool, cache, linkhead, groundtag, searchvertex);
  if (linkring == STOP) {
    linkring = linkringnew(pool, proxipooltag2allocindex(pool, linkhead));
    link2dinsertvertex(pool, cache, linkhead, linktail, groundtag,
                       searchvertex, linkring);
  }

  return linkring;
}

/*****************************************************************************/
/*                                                                           */
/*  link2ddeletevertexnocache()   Delete a vertex from a 2D link.  Also      */
/*                                returns the link ring associated with the  */
/*                                vertex.                                    */
/*                                                                           */
/*  This procedure does not free the link ring's molecules.                  */
/*                                                                           */
/*  pool:  The proxipool that the link was allocated from.                   */
/*  linkhead:  Tag for the first molecule in the link.                       */
/*  linktail:  Tag for the last molecule in the link.  Passed by pointer,    */
/*    and may be modified on return.                                         */
/*  groundtag:  The ground tag relative to which the vertices are            */
/*    decompressed.                                                          */
/*  deletevertex:  The tag of the vertex to delete.                          */
/*                                                                           */
/*  Returns the tag of the link ring associated with `deletevertex' if       */
/*    `deletevertex' is in the link (in which case it is deleted from the    */
/*    link); STOP otherwise.                                                 */
/*                                                                           */
/*****************************************************************************/

tag link2ddeletevertexnocache(struct proxipool *pool,
                              tag linkhead,
                              tag *linktail,
                              tag groundtag,
                              tag deletevertex)
{
  struct linkposition position;
  molecule cule;
  tag nextmoleculetag;
  tag moleculetag;
  tag texttag;
  int textatoms;
  int vertexatoms;
  int atomindex;
  int lasttextindex;
  int vertextagflag;
  int firstflag;
  int matchflag;
  char nowatom;

  if (linkhead >= STOP) {
    /* Not a link. */
    return STOP;
  }

  texttag = 0;
  textatoms = 0;
  vertexatoms = 0;
  /* Start at the first molecule. */
  nextmoleculetag = linkhead;
  /* Set to 1 while reading a vertex tag; 0 when reading a link ring tag. */
  vertextagflag = 1;
  /* Set to 1 during the first iteration of the "do" loop. */
  firstflag = 1;
  matchflag = 0;

  /* Loop through the linked list of molecules. */
  do {
    /* Convert the molecule's tag to a pointer. */
    cule = (molecule) proxipooltag2object(pool, nextmoleculetag);
#ifdef SELF_CHECK
    if (cule == (molecule) NULL) {
      printf("Internal error in link2ddeletevertexnocache():\n");
      printf("  Molecule tag indexes a NULL pointer.\n");
      internalerror();
    }
#endif /* SELF_CHECK */
    moleculetag = nextmoleculetag;
    /* Read the "next molecule tag". */
    linkringreadtag(moleculetag, cule, lasttextindex, nextmoleculetag, nowatom,
                    "Internal error in link2ddeletevertexnocache():\n");
    if (firstflag) {
      /* Position to delete atoms from the beginning of the linked list. */
      position.cule = cule;
      position.moleculetag = moleculetag;
      position.textindex = 0;
      position.lasttextindex = lasttextindex;
      position.nextmoleculetag = nextmoleculetag;
      firstflag = 0;
    }

    atomindex = 0;
    /* Loop through the atoms, stopping at the "next molecule tag". */
    while (atomindex <= lasttextindex) {
      nowatom = cule[atomindex];
      /* Append the atom to the text tag. */
      texttag = (texttag << 7) + (nowatom & (char) 127);
      textatoms++;
      atomindex++;

      /* Is this the last atom in the compressed tag? */
      if (nowatom < (char) 0) {
        /* Yes; we have an uncompressed tag now.  What tag is it? */
        if ((nowatom == (char) STOP) && (textatoms == 1)) {
          /* STOP tag.  Exit the inner loop early; go on to next molecule. */
          break;
        } else if (vertextagflag) {
          /* Is this the vertex we're searching for? */
          if ((nowatom == (char) GHOSTVERTEX) && (textatoms == 1)) {
            /* Ghost vertex. */
            matchflag = deletevertex == GHOSTVERTEX;
          } else {
            /* Use the ground tag to supply the high-order bits of the */
            /*   vertex tag.                                           */
            matchflag =
              deletevertex ==
              texttag + ((groundtag >> (7 * textatoms)) << (7 * textatoms));
          }
          vertexatoms = textatoms;
        } else if (matchflag) {
          /* Delete the vertex and its link ring. */
          *linktail = linkringdeleteatoms(pool, &position,
                                          vertexatoms + textatoms);
          /* Use the link tag to supply the high-order bits of the */
          /*   link ring.  Return the latter.                      */
          return texttag + ((linkhead >> (7 * textatoms)) << (7 * textatoms));
        } else {
          /* Store the position of the beginning of the next compressed tag, */
          /*   in case the vertex starting here proves to be `deletevertex'. */
          position.cule = cule;
          position.moleculetag = moleculetag;
          position.textindex = atomindex;
          position.lasttextindex = lasttextindex;
          position.nextmoleculetag = nextmoleculetag;
        }

        /* Prepare to read another tag. */
        texttag = 0;
        textatoms = 0;
        /* Alternate between reading vertex tags and link ring tags. */
        vertextagflag = !vertextagflag;
      }
    }
  } while (nextmoleculetag != STOP);

  if (nowatom >= (char) 0) {
    /* Oops.  The end of the last tag is missing from the last molecule. */
    printf("Internal error in link2ddeletevertexnocache():\n");
    printf("  Tag at end of 2D link not properly terminated.\n");
    internalerror();
  }

  /* `deletevertex' is not in the link. */
  return STOP;
}

/*****************************************************************************/
/*                                                                           */
/*  link2ddeletevertex()   Delete a vertex from a 2D link.  Also returns the */
/*                         link ring associated with the vertex.             */
/*                                                                           */
/*  This procedure does not free the link ring's molecules.                  */
/*                                                                           */
/*  This procedure updates the cache to reflect the deletion, then calls     */
/*  link2ddeletevertexnocache() to perform the deletion (unless the first    */
/*  cache entry says that the vertex is not present).                        */
/*                                                                           */
/*  pool:  The proxipool that the link was allocated from.                   */
/*  cache:  An (optional) cache that will be updated by the deletion.        */
/*  linkhead:  Tag for the first molecule in the link.                       */
/*  linktail:  Tag for the last molecule in the link.  Passed by pointer,    */
/*    and may be modified on return.                                         */
/*  groundtag:  The ground tag relative to which the vertices are            */
/*    decompressed.                                                          */
/*  deletevertex:  The tag of the vertex to delete.                          */
/*                                                                           */
/*  Returns the tag of the link ring associated with `deletevertex' if       */
/*    `deletevertex' is in the link (in which case it is deleted from the    */
/*    link); STOP otherwise.                                                 */
/*                                                                           */
/*****************************************************************************/

tag link2ddeletevertex(struct proxipool *pool,
                       link2dcache cache,
                       tag linkhead,
                       tag *linktail,
                       tag groundtag,
                       tag deletevertex)
{
  int hash;

  if (linkhead >= STOP) {
    /* Not a link. */
    return STOP;
  }

  if (cache != NULL) {
    hash = (LINK2DPRIME * linkhead * linkhead + 2u * deletevertex) &
           (LINK2DCACHESIZE - 2);
    if ((cache[hash].mylink2d == linkhead) &&
        (cache[hash].myvertex == deletevertex)) {
      if (cache[hash].mylinkring == STOP) {
        return STOP;
      }
    } else {
      cache[hash + 1].mylink2d = cache[hash].mylink2d;
      cache[hash + 1].myvertex = cache[hash].myvertex;
      cache[hash + 1].mylinkring = cache[hash].mylinkring;
      cache[hash].mylink2d = linkhead;
      cache[hash].myvertex = deletevertex;
    }
    cache[hash].mylinkring = STOP;
  }

  return link2ddeletevertexnocache(pool, linkhead, linktail, groundtag,
                                   deletevertex);
}

/**                                                                         **/
/**                                                                         **/
/********* 2D link/triangulation representation routines end here    *********/


