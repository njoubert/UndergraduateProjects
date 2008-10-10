/********* Link ring routines begin here                             *********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  Compressed link rings                                                    */
/*                                                                           */
/*  The link of a vertex in a 2D triangulation, and the link of an edge in   */
/*  a 3D triangulation, is a set of vertices and edges ordered circularly    */
/*  around the vertex/edge that defines the triangles/tetrahedra that        */
/*  include the vertex/edge.  I call these one-dimensional links "link       */
/*  rings", distinguishing them from higher-dimensional links like the link  */
/*  of a vertex in a 3D triangulation (which is a 2D triangulation).         */
/*                                                                           */
/*  Of course, a one-dimensional link is only truly a ring for a vertex/edge */
/*  in the interior of a triangulation.  A boundary vertex/edge has one or   */
/*  more gaps in its ring, so its link is comprised of one or more chains.   */
/*  I fill in the gaps with a "ghost vertex" whose sole job is to represent  */
/*  such gaps, so the link is always represented as a ring.                  */
/*                                                                           */
/*  Link rings should all be thought of as sharing a fixed "orientation".    */
/*  I use counterclockwise, but this code would never know if you reflected  */
/*  it.  In fact, this code is deliberately oblivious to geometry--it never  */
/*  looks at a coordinate.                                                   */
/*                                                                           */
/*  Link rings are stored in a compressed format adapted from Blandford,     */
/*  Blelloch, Cardoze, and Kadow.  See the header for a full citation.       */
/*                                                                           */
/*  Each link ring is stored in a linked list of list nodes of type          */
/*  `molecule'.  Molecules are allocated dynamically and constitute most of  */
/*  the memory occupied by a mesh.                                           */
/*                                                                           */
/*  Some invariants of link rings:                                           */
/*                                                                           */
/*  - Both vertices and link rings are represented by tags, which are        */
/*    defined as a part of the proxipool definitions.  (But vertices and     */
/*    links are allocated from different proxipools, so a vertex tag of 31   */
/*    and a link tag of 31 are unrelated to each other.)                     */
/*                                                                           */
/*  - A molecule is a short piece of memory.  Every molecule is the same     */
/*    length.  Every molecule has a tag (and is allocated from a proxipool). */
/*    Every link ring tag indexes the first molecule in the link ring.       */
/*                                                                           */
/*  - Molecules store compressed tags.  Each compressed tag is partitioned   */
/*    into 7-bit chunks called "atoms", ordered from most significant to     */
/*    least significant.  The high-order eighth bit of each atom is a "stop  */
/*    bit", set to one to signify the final atom of a compressed tag.        */
/*                                                                           */
/*  - Compressed tags vary in length.  The compression mechanism is simply   */
/*    to leave out the high-order atoms of the tag.  Some number of low-     */
/*    order atoms remain.  The number of atoms that remain depends on which  */
/*    atoms of the tag differ from the atoms of a "ground tag" used as the   */
/*    basis for compression.  Compressed vertex tags use a different ground  */
/*    tag than compressed molecule tags.                                     */
/*                                                                           */
/*  - The high-order bits of a compressed vertex are supplied by the ground  */
/*    tag, whose identity is fixed for any single link ring.  Typically, the */
/*    ground tag is the tag of the topmost vertex in the hierarchy.  For     */
/*    instance, if a vertex x has a 2D link, in which a vertex y has a link  */
/*    ring (representing the link of the edge xy), x is generally the ground */
/*    tag.  Hopefully, most of the vertices in x's link share a long prefix  */
/*    with x.                                                                */
/*                                                                           */
/* - At the end (high-index atoms) of every molecule, there is a compressed  */
/*    tag indexing the next molecule in the linked list of molecules.  This  */
/*    compressed tag is written BACKWARD, starting at index MOLECULESIZE - 1,*/
/*    so the least significant atom (whose stop bit is set) has the lowest   */
/*    index of the tag's atoms.  If a molecule is the last one in the linked */
/*    list, its "next molecule" tag is the STOP atom, which occupies just    */
/*    one byte.                                                              */
/*                                                                           */
/*  - The high-order bits of a compressed "next molecule" tag are supplied   */
/*    by the previous tag in the linked list.  (There is not a single        */
/*    "ground tag" as there is for vertex tags.)  This means that "next      */
/*    tags" are often just one byte long, because successive molecules often */
/*    have successive tags.  It also suggests that appending tags to the end */
/*    of a list is more pleasant than splicing them into the middle, which   */
/*    can reduce space efficiency by lengthening the "next molecule" tags.   */
/*                                                                           */
/*  - There are two special atoms, STOP and GHOSTVERTEX (both with their     */
/*    stop bits on).  To prevent confusion, no ordinary tag can compress to  */
/*    either of these.  A tag whose last atom matches STOP or GHOSTVERTEX is */
/*    distinguished by having at least two atoms in its compressed           */
/*    representation (even if it would otherwise compress to one atom).      */
/*                                                                           */
/*  - All bytes of a molecule not occupied by the "next molecule" tag are    */
/*    available for compressed vertices, which are written FORWARD from      */
/*    index zero.  In the last molecule of a linked list, some bytes might   */
/*    be left unused, in which case the last vertex byte must be followed by */
/*    a STOP atom to indicate that not all the bytes are used.  This is only */
/*    permitted in a molecule whose "next molecule" tag is STOP.  If there   */
/*    are no unused bytes in the last molecule of a linked list, then the    */
/*    STOP atom at position MOLECULESIZE - 1 of that molecule does double    */
/*    duty as both the "next molecule" tag and the vertex list terminator.   */
/*                                                                           */
/*  - The compressed tag pointing to the next molecule may occupy at most    */
/*    MOLECULESIZE - 1 bytes (atoms), leaving at least one byte for part of  */
/*    a compressed vertex tag.  (The default molecule size of 16 ought to    */
/*    leave a lot more space for compressed vertices than one byte, but      */
/*    perhaps in some huuuuuge mesh on a 128-bit machine of the future...)   */
/*                                                                           */
/*  - The boundaries between molecules are not related to the boundaries     */
/*    between compressed vertices.  A molecule normally holds several        */
/*    compressed vertices, and a compressed vertex might span two molecules  */
/*    (or more, if the tags are big enough and the molecules small enough).  */
/*                                                                           */
/*  - A link ring should not have any vertex appear twice except the         */
/*    GHOSTVERTEX.  This "invariant" isn't really an invariant, because some */
/*    of the procedures allow you to violate it, and the Bowyer-Watson       */
/*    implementation relies on being able to temporarily have multiple       */
/*    copies of a vertex in a link ring.  (They eventually get merged back   */
/*    into one by linkringdelete2vertices().)  However, this invariant       */
/*    ought to be restored by any algorithm upon completion.                 */
/*                                                                           */
/*  - A link ring cannot have two GHOSTVERTEXs consecutively.                */
/*                                                                           */
/*  - A link ring cannot have exactly one vertex (counting ghost vertices).  */
/*                                                                           */
/*  - A molecule cannot start (index 0) with the STOP vertex, unless the     */
/*    link ring is empty (and this is the sole molecule in the linked list). */
/*    Except in this circumstance, every molecule stores at least one atom   */
/*    from a compressed vertex.  However, the STOP _atom_ can appear at      */
/*    index 0, so long as it is the continuation of a compressed tag from    */
/*    the previous molecule (and therefore it doesn't mean STOP).            */
/*                                                                           */
/*  Here's what a molecule looks like.  Each column is an atom.              */
/*                                                                           */
/*  ------------------------H--------                                        */
/*  |0|0|1|1|0|1|1|0|0|0|1|0H1|0|0|0| <-- Stop bits.  The "next molecule"    */
/*  |1|1|0|0|1|0|1|1|1|0|0|1H0|1|1|0|     tag is written backward and        */
/*  |0|1|1|1|0|0|1|0|1|1|1|1H0|1|1|0|     terminated with a stop bit.  The   */
/*  |1|1|1|0|1|0|1|1|0|1|0|0H0|0|0|1|     vertex tags are written forward    */
/*  |1|0|0|1|1|0|1|1|1|0|1|1H0|0|0|0|     and terminated with a stop bit.    */
/*  |1|1|0|0|1|0|0|0|0|0|0|0H0|1|1|0|     In this example, the last vertex   */
/*  |0|1|1|1|1|0|1|1|1|1|1|1H0|1|0|1|     tag is continued in the next       */
/*  |1|1|0|0|1|1|1|1|1|0|0|1H0|0|1|0|     molecule.                          */
/*  ------------------------H--------                                        */
/*   \_____________________/ \_____/                                         */
/*     compressed vertices    next molecule tag (varies in width)            */
/*                                                                           */
/*  It is permissible to have a link ring with just two vertices (and no     */
/*  ghost vertex).  A link ring with vertices 1 and 2 represents two edges,  */
/*  1-2 and 2-1, which are distinguished by the vertex order (underscoring   */
/*  the importance of a link ring's orientation).                            */
/*                                                                           */
/*  It's also permissible to have a link ring with one "real" vertex and one */
/*  ghost vertex.  This link has no edges at all.  A link can have any       */
/*  number of dangling vertices that are not connected to edges, although    */
/*  I haven't included much support for creating these.  (You can do it by   */
/*  creating an edge, then deleteing one of its vertices.)                   */
/*                                                                           */
/*  The "correct" way to use the linkring interface to build link rings is   */
/*  to use linkringinsertedge() and linkringdeleteedge().  These procedures  */
/*  won't allow you to build invariant-defying link rings.  The procedures   */
/*  that insert and delete individual vertices can accomplish some things    */
/*  faster--inserting a vertex performs a 1 -> 2 bistellar flip (replacing   */
/*  one edge with two), and deleting a vertex performs a 2 -> 1 flip.  But   */
/*  they also allow you to violate invariants; for instance, you could       */
/*  create a link ring with just one vertex by deleting the others.  The     */
/*  Bowyer-Watson implementation exploits the fact that you can insert       */
/*  multiple copies of a vertex into a link ring, but it's careful to clean  */
/*  up after itself in the end.                                              */
/*                                                                           */
/*  Public interface:                                                        */
/*  GHOSTVERTEX   Tag representing a ghost vertex.                           */
/*  STOP   Tag representing an unsuccessful query.                           */
/*  struct linkposition   Represents a position in a link ring or 2D link.   */
/*  tag linkringnew(pool, allocindex)   Allocate a new, empty link ring.     */
/*  tag linkringnewfill(pool, groundtag, tagarray, tagarraylen, allocindex)  */
/*    Allocate a new link ring containing the tags specified in the input.   */
/*  void linkringrestart(pool, linkring)   Reset a link ring to empty.       */
/*  void linkringdelete(pool, linkring)   Free a link ring to the pool.      */
/*  int linkringadjacencies(pool, linkring, groundtag, searchvertex,         */
/*    adjacencies[2])   Read the two vertices neighboring a vertex in link.  */
/*  void linkringiteratorinit(pool, linkring, pos)   Initialize an iterator  */
/*    that traverses all the vertices in a link ring one by one.             */
/*  tag linkringiterate(pool, groundtag, pos)   Read and advance iterator.   */
/*  void linkringprint(pool, linkring, groundtag)   Print a link ring.       */
/*  int linkringinsertedge(pool, linkring, groundtag, endpoint1, endpoint2)  */
/*    Insert an edge into a link ring.                                       */
/*  int linkringdeleteedge(pool, linkring, groundtag, endpoint1, endpoint2)  */
/*    Delete an edge from a link ring.                                       */
/*  int linkringinsertvertex(pool, linkring, groundtag, searchvertex,        */
/*    newvertex)   Insert a vertex in link, following a specified vertex.    */
/*  int linkringdeletevertex(pool, linkring, groundtag, deletevertex)        */
/*    Delete a vertex from a link ring.                                      */
/*  int linkringdelete2vertices(pool, linkring, groundtag, deletevertex)     */
/*    Delete a vertex and the vertex that follows it from a link ring.       */
/*                                                                           */
/*  For internal use only:                                                   */
/*  int linkringtagcompress(groundtag, newtag, newtagatoms)                  */
/*  void linkringreadtag(moleculetag, cule, atomindex, nextmoleculetag,      */
/*    nowatom, errstring)                                                    */
/*  int linkringadjacencies2(pool, linkring, groundtag, searchvertex,        */
/*    adjacencies[2])                                                        */
/*  tag linkringinsertatoms(pool, insertposition, numatoms, newatombuffer,   */
/*    allocindex)                                                            */
/*  tag linkringinsertatoms2(pool, insertposition, numatoms, newatombuffer,  */
/*    allocindex)                                                            */
/*  tag linkringdeleteatoms(pool, deleteposition, numatoms)                  */
/*  tag linkringdeleteatoms2(pool, deleteposition, numatoms)                 */
/*  void linkringrotateatoms(pool, linkring, pos1, pos2)                     */
/*                                                                           */
/*****************************************************************************/


/*  `molecule' is a short piece of memory, with room for MOLECULESIZE atoms  */
/*  (characters).  Link rings are made up of molecules chained together in   */
/*  linked lists.  Each molecule has a compressed tag at the end which       */
/*  points to the next molecule in the list.  The rest of its space is       */
/*  devoted to compressed tags indexing vertices in the link ring.           */
/*                                                                           */
/*  2D links are also made of chains of molecules, with the compressed tags  */
/*  indexing both vertices and link rings.                                   */
/*                                                                           */
/*  The declaration "typedef char molecule[MOLECULESIZE]" would have been    */
/*  more appropriate, if C interpreted that as a reassignable pointer to an  */
/*  array of size MOLECULESIZE.  All molecules are the same size.            */

typedef char *molecule;


/*  GHOSTVERTEX is a tag that represents a "ghost vertex," which represents  */
/*  a gap in a link ring or a simplicial complex.  If a ghost vertex lies    */
/*  between vertices v and w in a link ring, it means that vw is not an edge */
/*  of the link, even though v and w are successive vertices in the link.    */
/*  To give another example, if a tetrahedron has a face f not shared by     */
/*  another tetrahedron in the simplicial complex, we represent it as        */
/*  sharing a face with a "ghost tetrahedron" whose fourth vertex is the     */
/*  ghost vertex.  Cast to a character, GHOSTVERTEX is also an atom that     */
/*  serves as the compressed form of the GHOSTVERTEX tag.                    */
/*                                                                           */
/*  STOP is a tag used to terminate a sequence of atoms in a molecule,       */
/*  implying either that the atoms continue in the next molecule, or there   */
/*  are no more atoms in the link ring.  STOP is also used as a "next tag"   */
/*  index at the end of a molecule, to indicate that it is the last molecule */
/*  in the linked list.  The STOP tag is not part of the interface of any of */
/*  the linkring procedures, but it is returned by some of the link2d        */
/*  procedures.  Cast to a character, STOP is the one-atom compressed form   */
/*  of the STOP tag.                                                         */

#define GHOSTVERTEX (~ (tag) 0)
#define STOP (~ (tag) 1)

/*  MOLECULESIZE is the size of one molecule in characters (and atoms).      */
/*  The choice makes a trade-off between wasted space (most compressed link  */
/*  rings do not use the entirely of the last molecule in their linked       */
/*  lists) and space occupied by the compressed "next molecule tag" index    */
/*  (which is space that can't be used for compressed vertices).  To obtain  */
/*  high speed, 20 seems to be a good choice.  (Higher values don't buy much */
/*  more speed, but cost a lot of memory.)  To obtain maximum compactness,   */
/*  8 is a good choice.                                                      */

#define MOLECULESIZE 20

/*  MOLECULEQUEUESIZE is the number of molecules that linkringdeleteatoms()  */
/*  can remember at once.  linkringdeleteatoms() should not be asked to      */
/*  delete more than this many molecules' atoms at once.  Fortunately, the   */
/*  maximum number of atoms that any procedure wants to delete at once is no */
/*  more than three compressed vertices' worth.                              */

#define MOLECULEQUEUESIZE 40

/*  COMPRESSEDTAGLENGTH is an upper bound on the maximum number of bytes a   */
/*  tag occupies after it is compressed.  Used to allocate buffers for       */
/*  atoms in transit.                                                        */

#define COMPRESSEDTAGLENGTH (8 * sizeof(tag) / 7 + 2)


/*  A linkposition represents an atom in a link ring or 2D link.  Usually,   */
/*  the atom is the first atom in some compressed vertex, or the STOP tag at */
/*  the end of the linked list.  Among other things, this struct is useful   */
/*  as an iterator that walks through a link ring or 2D link.                */
/*                                                                           */
/*  The atom in question appears at index `textindex' in molecule `cule',    */
/*  usually.  However, if textindex > lasttextindex, the atom in question is */
/*  really the first atom of the next molecule in the linked list.  This may */
/*  seem oblique, but it's sometimes useful.  For instance, when deleting    */
/*  atoms at a given position in a link ring, this representation increases  */
/*  the likelihood of being able to free an additional molecule at the end   */
/*  of the linked list for reuse elsewhere.                                  */
/*                                                                           */
/*  `moleculetag' is the tag of the current molecule (which is needed for    */
/*  use as a base for compressing the next tag when a new molecule gets      */
/*  appended to the linked list).  `lasttextindex' is the index of the last  */
/*  index in this tag used for compressed vertices (as opposed to the "next  */
/*  molecule tag").  `nextmoleculetag' is the tag of the next molecule in    */
/*  the linked list (possibly STOP).  These two fields are stored so that    */
/*  they only need to be computed once, when `cule' is first encountered.    */

struct linkposition {
  molecule cule;      /* The molecule containing the atom (or right before). */
  int textindex;                                   /* The index of the atom. */
  int lasttextindex;   /* The last atom not part of the "next molecule" tag. */
  tag moleculetag;                       /* The tag for the molecule `cule'. */
  tag nextmoleculetag;  /* The next tag following `moleculetag' in the list. */
};


/*****************************************************************************/
/*                                                                           */
/*  linkringtagcompress()   Compresses a tag, relative to another tag.       */
/*                                                                           */
/*  The compressed tag contains enough lower-order bits of the original      */
/*  tag to distinguish it from another tag, `groundtag'.  Decompression can  */
/*  be done later simply by using `groundtag' as the source of the missing   */
/*  high-order bits.                                                         */
/*                                                                           */
/*  The low-order bits are partitioned into atoms (7-bit chunks), and the    */
/*  high-order "stop bit" of the lowest-order atom is set.                   */
/*                                                                           */
/*  The compressed tag is always at least one atom long, even if `newtag' == */
/*  `groundtag'.  If `newtag' is STOP or GHOSTVERTEX, the compressed tag is  */
/*  just one atom long.  Otherwise, if the last atom of `newtag' is STOP or  */
/*  GHOSTVERTEX, then the compressed tag is at least two atoms long, so that */
/*  it cannot be mistaken for STOP or GHOSTVERTEX.                           */
/*                                                                           */
/*  IMPORTANT:  The atoms are written into the array `newtagatoms' in        */
/*  REVERSE order, with the least significant atom (with its stop bit set)   */
/*  at index zero.  (This is the right order for "next molecule" tags, but   */
/*  compressed vertices need to be reversed back.)                           */
/*                                                                           */
/*  groundtag:  The base tag against which `newtag' is compressed.           */
/*  newtag:  The tag to compress.                                            */
/*  newtagatoms:  The array into which the compressed tag's atoms are        */
/*    written BACKWARD.                                                      */
/*                                                                           */
/*  Returns the number of atoms in the compressed tag.                       */
/*                                                                           */
/*****************************************************************************/

int linkringtagcompress(tag groundtag,
                        tag newtag,
                        char *newtagatoms)
{
  int numatoms;

  /* Extract the least significant atom; set its stop bit. */
  newtagatoms[0] = (char) ((newtag & (tag) 127) | (tag) ~127);
  if ((newtag == GHOSTVERTEX) || (newtag == STOP)) {
    /* GHOSTVERTEX and STOP always compress to one atom. */
    return 1;
  }

  /* Cut the least significant atoms from the tag and the ground tag. */
  groundtag = groundtag >> 7;
  newtag = newtag >> 7;
  numatoms = 1;
  /* Keep cutting atoms until the two tags' remaining bits agree. */
  while (groundtag != newtag) {
    /* Extract the next atom. */
    newtagatoms[numatoms] = (char) (newtag & (tag) 127);
    /* Cut it off in anticipation of the next comparison. */
    groundtag = groundtag >> 7;
    newtag = newtag >> 7;
    numatoms++;
  }

  if ((numatoms == 1) &&
      ((newtagatoms[0] == (char) GHOSTVERTEX) ||
       (newtagatoms[0] == (char) STOP))) {
    /* To avoid confusion, any tag whose least significant atom matches the */
    /*   ghost vertex or STOP atom has to compress to at least two atoms.   */
    newtagatoms[1] = (char) (newtag & (tag) 127);
    numatoms++;
  }

  return numatoms;
}

/*****************************************************************************/
/*                                                                           */
/*  linkringnew()   Allocate a new, empty link ring.                         */
/*                                                                           */
/*  The parameters include an allocation index, used to determine where the  */
/*  new link ring will be stored in memory.  Link rings with the same        */
/*  allocation index go into common areas in memory.  The idea is to create  */
/*  spatial coherence:  links that are geometrically close to each other are */
/*  near each other in memory, too.                                          */
/*                                                                           */
/*  The parameters include the coordinates of a point associated with the    */
/*  link ring, so that the link ring can be allocated from memory near other */
/*  molecules that are spatially nearby.                                     */
/*                                                                           */
/*  pool:  The proxipool to allocate the link ring from.                     */
/*  allocindex:  An allocation index associated with the link ring.          */
/*                                                                           */
/*  Returns the tag of the first (and only) molecule of the new link ring.   */
/*                                                                           */
/*****************************************************************************/

tag linkringnew(struct proxipool *pool,
                proxipoolulong allocindex)
{
  molecule cule;
  tag newlinkring;

  /* Allocate a molecule to hold the (empty) link ring. */
  newlinkring = proxipoolnew(pool, allocindex, (void **) &cule);
  /* There are no vertices in this link ring. */
  cule[0] = (char) STOP;
  /* There is no next molecule. */
  cule[MOLECULESIZE - 1] = (char) STOP;
  return newlinkring;
}

/*****************************************************************************/
/*                                                                           */
/*  linkringnewfill()   Allocate a new link ring containing the tags         */
/*                      (vertices) specified in an input array.              */
/*                                                                           */
/*  The tags may include GHOSTVERTEX.  Be sure to obey the link ring         */
/*  invariants:  no vertex except GHOSTVERTEX may occur twice in a link      */
/*  ring, and GHOSTVERTEX may not occur twice consecutively.  (Remember      */
/*  that the link ring is a ring, so GHOSTVERTEX may not occur both first    */
/*  and last.)                                                               */
/*                                                                           */
/*  pool:  The proxipool to allocate the link ring from.                     */
/*  groundtag:  The ground tag that serves as a base for the compression of  */
/*    the other tags.                                                        */
/*  tagarray:  An array listing the tags to put in the link ring.            */
/*  tagarraylen:  The number of tags to put in the link ring.                */
/*  allocindex:  An allocation index associated with the link ring.          */
/*                                                                           */
/*  Returns the tag of the first molecule of the link ring.                  */
/*                                                                           */
/*****************************************************************************/

tag linkringnewfill(struct proxipool *pool,
                    tag groundtag,
                    tag *tagarray,
                    proxipoolulong tagarraylen,
                    proxipoolulong allocindex)
{
  molecule cule;
  molecule nextcule;
  tag newlinkring;
  tag moleculetag;
  tag nextculetag;
  proxipoolulong tagarrayindex;
  int vertexatomcount;
  int vertexwriteindex;
  int vertexcopyindex;
  int vertexleftindex;
  int tagsize;
  int nextculeindex;
  int i;
  char atombuffer[MOLECULESIZE + 3 * COMPRESSEDTAGLENGTH];
  unsigned int aftervertexindex[MOLECULESIZE + 3 * COMPRESSEDTAGLENGTH];

  /* Allocate the first molecule of the new link ring. */
  newlinkring = proxipoolnew(pool, allocindex, (void **) &cule);
  moleculetag = newlinkring;

  /* Compress the first tag (if there is any) into `atombuffer'. */
  vertexatomcount = tagarraylen <= 0 ? 0 :
                    linkringtagcompress(groundtag, tagarray[0], atombuffer);
  /* Compressed tags are written backward by linkringtagcompress(), so the */
  /*   compressed tag will be copied from index `vertexcopyindex' back to  */
  /*   index `vertexleftindex' (presently zero).                           */
  vertexleftindex = 0;
  vertexcopyindex = vertexatomcount - 1;
  /* Keep track of where this compressed vertex ends, and the second vertex */
  /*   will begin.                                                          */
  aftervertexindex[0] = vertexatomcount;
  vertexwriteindex = vertexatomcount;
  /* The second vertex has index one. */
  tagarrayindex = 1;
  nextcule = (molecule) NULL;
  nextculetag = 0;

  /* The outer loop fills one molecule per iteration. */
  do {
    /* The next loop compresses enough tags to yield MOLECULESIZE atoms */
    /*   (unless we run out of tags).                                   */
    while ((tagarrayindex < tagarraylen) && (vertexatomcount < MOLECULESIZE)) {
      /* Compress the next tag into the buffer. */
      tagsize = linkringtagcompress(groundtag, tagarray[tagarrayindex],
                                    &atombuffer[vertexwriteindex]);
      tagarrayindex++;
      /* Keep track of the number of atoms waiting to be put into link ring. */
      vertexatomcount += tagsize;
      /* Keep track of where this compressed vertex ends. */
      aftervertexindex[vertexwriteindex] = vertexwriteindex + tagsize;
      vertexwriteindex += tagsize;
      /* If too close to the end of the buffer, return to the beginning */
      /*   for the next vertex.                                         */
      if (vertexwriteindex > MOLECULESIZE + 2 * COMPRESSEDTAGLENGTH) {
        vertexwriteindex = 0;
      }
    }

    /* Check if this is the last molecule in the link ring. */
    if (vertexatomcount >= MOLECULESIZE) {
      /* Too many atoms left to fit in one molecule.  Allocate another one. */
      nextculetag = proxipoolnew(pool, allocindex, (void **) &nextcule);
      /* Compress the "next molecule" tag and figure where to put it. */
      nextculeindex = MOLECULESIZE -
                      linkringtagcompress(moleculetag, nextculetag,
                                          (char *) cule);
      /* Shift the compressed tag from the left to the right side of `cule'. */
      for (i = nextculeindex; i < MOLECULESIZE; i++) {
        cule[i] = cule[i - nextculeindex];
      }
    } else {
      /* Write a STOP atom after the last vertex. */
      cule[vertexatomcount] = (char) STOP;
      /* Write a STOP atom for the "next molecule" tag. */
      cule[MOLECULESIZE - 1] = (char) STOP;
      /* Set up `nextculeindex' for the upcoming loop. */
      nextculeindex = vertexatomcount;
    }

    /* Fill the rest of the molecule with atoms. */
    for (i = 0; i < nextculeindex; i++) {
      cule[i] = atombuffer[vertexcopyindex];
      /* Tags are compressed backward, so we reverse them back to forward. */
      vertexcopyindex--;
      /* Have we just written the last atom of a vertex? */
      if (vertexcopyindex < vertexleftindex) {
        /* Yes.  Jump to the next vertex. */
        vertexleftindex = aftervertexindex[vertexleftindex];
        /* If too close to the end of the buffer, return to the beginning */
        /*   (following the process that wrote the buffer).               */
        if (vertexleftindex > MOLECULESIZE + 2 * COMPRESSEDTAGLENGTH) {
          vertexleftindex = 0;
        }
        /* Find the rightmost atom of this compressed vertex. */
        vertexcopyindex = aftervertexindex[vertexleftindex] - 1;
      }
    }
    /* How many atoms in the buffer still await copying? */
    vertexatomcount -= nextculeindex;

    /* Go on to the next molecule in the next iteration. */
    cule = nextcule;
    moleculetag = nextculetag;
  } while (vertexatomcount > 0);

  return newlinkring;
}

/*****************************************************************************/
/*                                                                           */
/*  linkringreadtag()   Read the "next molecule" tag from a molecule.        */
/*                                                                           */
/*  This macro is a sequence of operations repeated in a bunch of the        */
/*  linkring procedures.  It's repeated often enough that it's worth         */
/*  inlining; hence, I've made it a macro.                                   */
/*                                                                           */
/*  moleculetag:  Used as a ground tag for decompressing `nextmoleculetag'   */
/*    (input, not changed).                                                  */
/*  cule:  The molecule to read (input, not changed).                        */
/*  atomindex:  Output only, whereupon it is the last index devoted to       */
/*    compressed vertices (i.e. one before the first index devoted to the    */
/*    compressed "next molecule" tag) in `cule'.                             */
/*  nextmoleculetag:  Output only, whereupon it is the tag for the next      */
/*    molecule after `cule' in the linked list.                              */
/*  nowatom:  A work variable; neither input nor output.                     */
/*  errstring:  A string to print if there's an unrecoverable error.         */
/*                                                                           */
/*****************************************************************************/

#define linkringreadtag(moleculetag, cule, atomindex, nextmoleculetag,  \
                        nowatom, errstring)  \
  nowatom = cule[MOLECULESIZE - 1];  \
  atomindex = MOLECULESIZE - 2;  \
  if (nowatom == (char) STOP) {  \
    nextmoleculetag = STOP;  \
  } else {  \
    nextmoleculetag = nowatom & (char) 127;  \
    while (nowatom >= (char) 0) {  \
      if (atomindex < 0) {  \
        printf(errstring);  \
        printf("  Tag for next molecule not properly terminated.\n");  \
        internalerror();  \
      }  \
      nowatom = cule[atomindex];  \
      nextmoleculetag = (nextmoleculetag << 7) + (nowatom & (char) 127);  \
      atomindex--;  \
    }  \
    nextmoleculetag += (moleculetag >>  \
                        (7 * (MOLECULESIZE - 1 - atomindex))) <<  \
                       (7 * (MOLECULESIZE - 1 - atomindex));  \
  }

/*****************************************************************************/
/*                                                                           */
/*  linkringrestart()   Reset a link ring to empty.                          */
/*                                                                           */
/*  pool:  The proxipool that the link ring was allocated from.              */
/*  linkring:  Tag for the first molecule in the link ring.                  */
/*                                                                           */
/*****************************************************************************/

void linkringrestart(struct proxipool *pool,
                     tag linkring)
{
  molecule cule;
  tag nextmoleculetag;
  tag moleculetag;
  int atomindex;
  char nowatom;

  if (linkring >= STOP) {
    return;
  }

  moleculetag = linkring;

  /* Loop through the molecules and free all but the first. */
  do {
    /* Convert the tag to a molecule. */
    cule = (molecule) proxipooltag2object(pool, moleculetag);
    /* Read the "next molecule" tag. */
    linkringreadtag(moleculetag, cule, atomindex, nextmoleculetag, nowatom,
                    "Internal error in linkringrestart():\n");
    if (moleculetag == linkring) {
      /* There are no vertices in the first link ring. */
      cule[0] = (char) STOP;
      /* There is no next molecule. */
      cule[MOLECULESIZE - 1] = (char) STOP;
    } else {
      /* Free the molecule. */
      proxipoolfree(pool, moleculetag);
    }
    moleculetag = nextmoleculetag;
  } while (nextmoleculetag != STOP);
}

/*****************************************************************************/
/*                                                                           */
/*  linkringdelete()   Free all the molecules in a link ring to the pool.    */
/*                                                                           */
/*  pool:  The proxipool that the molecules were allocated from.             */
/*  linkring:  Tag for the first molecule in the link ring.                  */
/*                                                                           */
/*****************************************************************************/

void linkringdelete(struct proxipool *pool,
                    tag linkring)
{
  molecule cule;
  tag nextmoleculetag;
  tag moleculetag;
  int atomindex;
  char nowatom;

  if (linkring >= STOP) {
    return;
  }

  moleculetag = linkring;

  /* Loop through the molecules and free them. */
  do {
    /* Convert the tag to a molecule. */
    cule = (molecule) proxipooltag2object(pool, moleculetag);
    /* Read the "next molecule" tag. */
    linkringreadtag(moleculetag, cule, atomindex, nextmoleculetag, nowatom,
                    "Internal error in linkringdelete():\n");
    /* Free the molecule. */
    proxipoolfree(pool, moleculetag);
    moleculetag = nextmoleculetag;
  } while (nextmoleculetag != STOP);
}

/*****************************************************************************/
/*                                                                           */
/*  linkringadjacencies()    Return the two neighboring vertices adjoining a */
/*  linkringadjacencies2()   vertex in a link ring.                          */
/*                                                                           */
/*  These procedures are interchangeable.  linkringadjacencies2() is a       */
/*  complete implementation.  linkringadjacencies() is an optimized wrapper  */
/*  that handles the special case where the entire link ring fits in a       */
/*  single molecule faster, and calls linkringadjacencies2() otherwise.      */
/*  Profiling suggests that when MOLECULESIZE is 20, linkringadjacencies()   */
/*  resorts to calling linkringadjacencies2() only 2.5% of the time.         */
/*                                                                           */
/*  pool:  The proxipool that the link ring was allocated from.              */
/*  linkring:  Tag for the first molecule in the link ring.                  */
/*  groundtag:  The ground tag relative to which the vertices are            */
/*    decompressed.                                                          */
/*  searchvertex:  The tag of the vertex to search for.                      */
/*  adjacencies[2]:  Array in which the procedure returns the tags of the    */
/*    vertices adjacent to `searchvertex'.  adjacencies[0] is just before    */
/*    (clockwise from) `searchvertex', and adjacencies[1] is just after      */
/*    (counterclockwise from) `searchvertex'.  One or both of these may be   */
/*    GHOSTVERTEX, if `searchvertex' does not participate in two edges of    */
/*    the link ring.  The contents of this array do not need to be           */
/*    initialized prior to calling this procedure.                           */
/*                                                                           */
/*  Returns 1 if `searchvertex' is in the link ring; 0 otherwise.            */
/*                                                                           */
/*****************************************************************************/

int linkringadjacencies2(struct proxipool *pool,
                         tag linkring,
                         tag groundtag,
                         tag searchvertex,
                         tag adjacencies[2])
{
  molecule cule;
  tag vertextag;
  tag firstvertextag;
  tag prevvertextag;
  tag nextmoleculetag;
  tag moleculetag;
  long vertexcount;
  int vertexatoms;
  int atomindex1, atomindex2;
  char nowatom;

  if (linkring >= STOP) {
    /* Not a link ring. */
    adjacencies[0] = GHOSTVERTEX;
    adjacencies[1] = GHOSTVERTEX;
    return 0;
  }

  /* There is no "previous" vertex we've visited yet. */
  firstvertextag = STOP;
  prevvertextag = STOP;
  vertexcount = 0;
  vertextag = 0;
  vertexatoms = 0;
  /* Start at the first molecule. */
  nextmoleculetag = linkring;

  /* Loop through the linked list of molecules. */
  do {
    /* Convert the molecule's tag to a pointer. */
    cule = (molecule) proxipooltag2object(pool, nextmoleculetag);
    moleculetag = nextmoleculetag;
    /* Read the "next molecule" tag. */
    linkringreadtag(moleculetag, cule, atomindex2, nextmoleculetag, nowatom,
                    "Internal error in linkringadjacencies():\n");

    atomindex1 = 0;
    /* Loop through the atoms, stopping at the "next molecule" tag. */
    while (atomindex1 <= atomindex2) {
      nowatom = cule[atomindex1];
      /* Append the atom to the vertex tag. */
      vertextag = (vertextag << 7) + (nowatom & (char) 127);
      vertexatoms++;
      atomindex1++;

      /* Is this the last atom in the compressed tag? */
      if (nowatom < (char) 0) {
        /* Yes; we have an uncompressed tag now.  What tag is it? */
        if ((nowatom == (char) STOP) && (vertexatoms == 1)) {
          /* STOP tag.  Exit the inner loop early; go on to next molecule. */
          break;
        } else if ((nowatom == (char) GHOSTVERTEX) && (vertexatoms == 1)) {
          /* Ghost vertex. */
          vertextag = GHOSTVERTEX;
        } else {
          /* Use the ground tag to supply the high-order bits. */
          vertextag += (groundtag >> (7 * vertexatoms)) << (7 * vertexatoms);
        }

        vertexcount++;
        /* Is this the vertex we're searching for? */
        if (vertextag == searchvertex) {
          /* Yes.  Output the previous vertex.  (If we're at the start of  */
          /*   the list, `prevvertextag' is STOP, but we'll fix it later.) */
          adjacencies[0] = prevvertextag;
        }

        if (vertexcount == 1) {
          /* This is the first vertex.  Remember it. */
          firstvertextag = vertextag;
        } else if (prevvertextag == searchvertex) {
          /* The previous vertex was `searchvertex', so output this vertex. */
          adjacencies[1] = vertextag;
          /* Return early...unless `searchvertex' was at the start of the  */
          /*   list, in which case one neighbor is at the end of the list. */
          if (vertexcount > 2) {
            return 1;
          }
        }

        /* Remember this tag during the next iteration. */
        prevvertextag = vertextag;
        /* Prepare to read another tag. */
        vertextag = 0;
        vertexatoms = 0;
      }
    }
  } while (nextmoleculetag != STOP);

  if (nowatom >= (char) 0) {
    /* Oops.  The end of the last vertex is missing from the last molecule. */
    printf("Internal error in linkringadjacencies():\n");
    printf("  Vertex at end of link ring not properly terminated.\n");
    internalerror();
  }

  /* The link ring is circular, so check the first and last vertices. */
  if (prevvertextag == searchvertex) {
    /* The last vertex is `searchvertex', so output the first vertex. */
    adjacencies[1] = firstvertextag;
    if (vertexcount == 1) {
      /* There is only one vertex in the ring. */
      adjacencies[0] = firstvertextag;
    }
    return 1;
  } else if (firstvertextag == searchvertex) {
    /* The first vertex is `searchvertex', so output the last vertex. */
    adjacencies[0] = prevvertextag;
    return 1;
  } else {
    /* Execution can only get this far if `searchvertex' is not in the ring. */
    adjacencies[0] = GHOSTVERTEX;
    adjacencies[1] = GHOSTVERTEX;
    return 0;
  }
}

/*****************************************************************************/
/*                                                                           */
/*  linkringadjacencies()   Return the two neighboring vertices adjoining a  */
/*                          vertex in a link ring.                           */
/*                                                                           */
/*  See header above linkringadjacencies2().                                 */
/*                                                                           */
/*****************************************************************************/

int linkringadjacencies(struct proxipool *pool,
                        tag linkring,
                        tag groundtag,
                        tag searchvertex,
                        tag adjacencies[2])
{
  molecule cule;
  tag vertextag;
  tag firstvertextag;
  tag prevvertextag;
  long vertexcount;
  int vertexatoms;
  int atomindex;
  char nowatom;

  if (linkring >= STOP) {
    /* Not a link ring. */
    adjacencies[0] = GHOSTVERTEX;
    adjacencies[1] = GHOSTVERTEX;
    return 0;
  }

  /* Convert the molecule's tag to a pointer. */
  cule = (molecule) proxipooltag2object(pool, linkring);
  /* Is the list just one molecule? */
  if (cule[MOLECULESIZE - 1] == (char) STOP) {
    /* Yes.  Run a faster lookup. */

    /* There is no "previous" vertex we've visited yet. */
    firstvertextag = STOP;
    prevvertextag = STOP;
    vertexcount = 0;
    vertextag = 0;
    vertexatoms = 0;

    atomindex = 0;
    /* Loop through the atoms, stopping at the "next molecule" tag. */
    while (1) {
      nowatom = cule[atomindex];
      /* Append the atom to the vertex tag. */
      vertextag = (vertextag << 7) + (nowatom & (char) 127);
      vertexatoms++;
      atomindex++;

      /* Is this the last atom in the compressed tag? */
      if (nowatom < (char) 0) {
        /* Yes; we have an uncompressed tag now.  What tag is it? */
        if (((nowatom == (char) STOP) && (vertexatoms == 1)) ||
            (atomindex >= MOLECULESIZE)) {
          /* STOP tag.  End of the list.  The link ring is circular, so */
          /*   check the first and last vertices.                       */
          if (prevvertextag == searchvertex) {
            /* The last vertex is `searchvertex', so output the first */
            /*   vertex.                                              */
            adjacencies[1] = firstvertextag;
            if (vertexcount == 1) {
              /* There is only one vertex in the ring. */
              adjacencies[0] = firstvertextag;
            }
            return 1;
          } else if (firstvertextag == searchvertex) {
            /* The first vertex is `searchvertex', so output the last */
            /*   vertex.                                              */
            adjacencies[0] = prevvertextag;
            return 1;
          }
          /* `searchvertex' is not in the ring. */
          adjacencies[0] = GHOSTVERTEX;
          adjacencies[1] = GHOSTVERTEX;
          return 0;
        } else if ((nowatom == (char) GHOSTVERTEX) && (vertexatoms == 1)) {
          /* Ghost vertex. */
          vertextag = GHOSTVERTEX;
        } else {
          /* Use the ground tag to supply the high-order bits. */
          vertextag += (groundtag >> (7 * vertexatoms)) << (7 * vertexatoms);
        }

        vertexcount++;
        /* Is this the vertex we're searching for? */
        if (vertextag == searchvertex) {
          /* Yes.  Output the previous vertex.  (If we're at the start of  */
          /*   the list, `prevvertextag' is STOP, but we'll fix it later.) */
          adjacencies[0] = prevvertextag;
        }

        if (vertexcount == 1) {
          /* This is the first vertex.  Remember it. */
          firstvertextag = vertextag;
        } else if (prevvertextag == searchvertex) {
          /* The previous vertex was `searchvertex', so output this vertex. */
          adjacencies[1] = vertextag;
          /* Return early...unless `searchvertex' was at the start of the  */
          /*   list, in which case one neighbor is at the end of the list. */
          if (vertexcount > 2) {
            return 1;
          }
        }

        /* Remember this tag during the next iteration. */
        prevvertextag = vertextag;
        /* Prepare to read another tag. */
        vertextag = 0;
        vertexatoms = 0;
      }
    }
  } else {
    /* The list does not fit in one molecule.  Do it the general (slow) way. */
    return linkringadjacencies2(pool, linkring, groundtag, searchvertex,
                                adjacencies);
  }
}

/*****************************************************************************/
/*                                                                           */
/*  linkringiteratorinit()   Initialize an iterator that traverses all the   */
/*                           vertices in a link ring (including ghost        */
/*                           vertices) one by one.                           */
/*                                                                           */
/*  The iterator is a variable `pos', whose internals should not be          */
/*  examined or modified by the client.  The iterator's job is to keep       */
/*  track of where it is in the link ring.  This procedure sets the iterator */
/*  to reference the first vertex in the link ring.                          */
/*                                                                           */
/*  After a link ring is modified, any iterators on that link ring may be    */
/*  corrupted and should not be used without being initialized (by this      */
/*  procedure) again.                                                        */
/*                                                                           */
/*  pool:  The proxipool that the link ring was allocated from.              */
/*  linkring:  The link ring to traverse.                                    */
/*  pos:  The iterator.                                                      */
/*                                                                           */
/*****************************************************************************/

void linkringiteratorinit(struct proxipool *pool,
                          tag linkring,
                          struct linkposition *pos)
{
  char nowatom;

  if (linkring >= STOP) {
    pos->cule = (molecule) NULL;
    pos->moleculetag = STOP;
    pos->nextmoleculetag = STOP;
    pos->textindex = MOLECULESIZE;
    pos->lasttextindex = -1;
    return;
  }

  pos->moleculetag = linkring;
  /* Find the molecule identified by the tag `linkring'. */
  pos->cule = (molecule) proxipooltag2object(pool, linkring);
  /* Read the molecule's "next molecule" tag. */
  linkringreadtag(linkring, pos->cule, pos->lasttextindex,
                  pos->nextmoleculetag, nowatom,
                  "Internal error in linkringiteratorinit():\n");
  /* Start the iterations from the beginning of the molecule. */
  pos->textindex = 0;
}

/*****************************************************************************/
/*                                                                           */
/*  linkringiterate()   Return the tag that a link ring iterator references, */
/*                      and advance the iterator so it will return the next  */
/*                      vertex next time.                                    */
/*                                                                           */
/*  The iterator is a variable `pos', whose internals should not be          */
/*  examined or modified by the client.                                      */
/*                                                                           */
/*  After a link ring is modified, any iterators on that link ring created   */
/*  before the modification may be corrupted and should not be passed to     */
/*  this procedure again until they are initialized again.                   */
/*                                                                           */
/*  pool:  The proxipool that the link ring was allocated from.              */
/*  pos:  The iterator.                                                      */
/*  groundtag:  The ground tag relative to which the vertices are            */
/*    decompressed.                                                          */
/*                                                                           */
/*  Returns the tag of a vertex in the link ring (possibly GHOSTVERTEX), or  */
/*    STOP if the iterator is at the end of the link ring.                   */
/*                                                                           */
/*****************************************************************************/

tag linkringiterate(struct proxipool *pool,
                    tag groundtag,
                    struct linkposition *pos)
{
  tag vertextag;
  int vertexatoms;
  char nowatom;

  vertextag = 0;
  vertexatoms = 0;

  /* Loop through atoms to build up one vertex tag. */
  do {
    /* Have we read the last vertex atom in this molecule? */
    while ((pos->textindex > pos->lasttextindex) ||
           ((pos->cule[pos->textindex] == (char) STOP) &&
            (vertexatoms == 0))) {
      /* Yes, we have.  Are there any more molecules after this one? */
      if (pos->nextmoleculetag == STOP) {
        /* No, there aren't.  The iterator is finished. */
        return STOP;
      }

      /* Find the next molecule in the linked list. */
      pos->cule = (molecule) proxipooltag2object(pool, pos->nextmoleculetag);
      pos->moleculetag = pos->nextmoleculetag;
      /* Read the next molecule's "next molecule" tag. */
      linkringreadtag(pos->moleculetag, pos->cule, pos->lasttextindex,
                      pos->nextmoleculetag, nowatom,
                      "Internal error in linkringiterate():\n");
      /* Start from the beginning of this molecule. */
      pos->textindex = 0;
    }

    /* Read the next atom. */
    nowatom = pos->cule[pos->textindex];
    /* Append it to the tag. */
    vertextag = (vertextag << 7) + (nowatom & (char) 127);
    vertexatoms++;
    pos->textindex++;
    /* End the loop when `nowatom' has a stop bit (i.e. is negative). */
  } while (nowatom >= (char) 0);

  if ((nowatom == (char) GHOSTVERTEX) && (vertexatoms == 1)) {
    return GHOSTVERTEX;
  } else {
    /* Get the high-order bits from the ground tag. */
    vertextag += (groundtag >> (7 * vertexatoms)) << (7 * vertexatoms);
    return vertextag;
  }
}

/*****************************************************************************/
/*                                                                           */
/*  linkringprint()   Print the contents of a link ring.                     */
/*                                                                           */
/*  pool:  The proxipool that the link ring was allocated from.              */
/*  linkring:  Tag for the first molecule in the link ring.                  */
/*  groundtag:  The ground tag relative to which the vertices are            */
/*    decompressed.                                                          */
/*                                                                           */
/*****************************************************************************/

void linkringprint(struct proxipool *pool,
                   tag linkring,
                   tag groundtag)
{
  struct linkposition pos;
  tag nexttag;

  printf("Ring %lu: ", (unsigned long) linkring);
  linkringiteratorinit(pool, linkring, &pos);
  nexttag = linkringiterate(pool, groundtag, &pos);
  if (nexttag == STOP) {
    printf("EMPTY");
  } else do {
    if (nexttag == GHOSTVERTEX) {
      printf("GHOST ");
    } else {
      printf("%lu ", (unsigned long) nexttag);
    }
    nexttag = linkringiterate(pool, groundtag, &pos);
  } while (nexttag != STOP);
  printf("\n");
}

/*****************************************************************************/
/*                                                                           */
/*  linkringinsertatoms()    Insert atoms at a specified position in a link  */
/*  linkringinsertatoms2()   ring (moving the other atoms back to make       */
/*                           room).                                          */
/*                                                                           */
/*  These procedures are interchangeable.  linkringinsertatoms2() is a       */
/*  complete implementation.  linkringinsertatoms() is an optimized wrapper  */
/*  that handles the special case where the entire link ring fits in a       */
/*  single molecule faster, and calls linkringinsertatoms2() otherwise.      */
/*  Profiling suggests that when MOLECULESIZE is 20, linkringinsertatoms()   */
/*  resorts to calling linkringinsertatoms2() only 5.5% of the time.         */
/*                                                                           */
/*  These procedures insert atoms in the REVERSE order they occur in the     */
/*  buffer (to compensate for the fact that linkringtagcompress() writes the */
/*  atoms in reverse order).                                                 */
/*                                                                           */
/*  WARNING:  These procedures uses the input `newatombuffer' not only as a  */
/*  source of atoms to insert, but also as a buffer to shift the following   */
/*  atoms back.  Therefore, the contents of the buffer are trashed.          */
/*                                                                           */
/*  These procedures are meant for internal use by other linkring            */
/*  procedures.                                                              */
/*                                                                           */
/*  pool:  The proxipool that the link ring was allocated from, and that new */
/*    molecules may be allocated from.                                       */
/*  insertposition:  References the position in the link ring to insert the  */
/*    atoms.                                                                 */
/*  insertposition->cule:  Pointer to the molecule.                          */
/*  insertposition->textindex:  The index within the molecule.               */
/*  insertposition->lasttextindex:  Index of the last atom in the molecule   */
/*    thatrepresents a vertex (rather than the "next molecule" tag).         */
/*  insertposition->moleculetag:  Tag for the molecule `position->cule'.     */
/*  insertposition->nextmoleculetag:  Tag for the next molecule in the       */
/*    linked list.  (This field and position->lasttextindex are included so  */
/*    they won't need to be computed a second time.)                         */
/*  numatoms:  The number of atoms to insert.  Must be greater than zero.    */
/*  newatombuffer:  Array of atoms to insert, in reverse order.  This        */
/*    procedure trashes the contents of this buffer.                         */
/*  allocindex:  The allocation index associated with the link ring.         */
/*                                                                           */
/*  Returns the tag of the last molecule in the modified linked list.        */
/*                                                                           */
/*****************************************************************************/

tag linkringinsertatoms2(struct proxipool *pool,
                         struct linkposition *insertposition,
                         int numatoms,
                         char *newatombuffer,
                         proxipoolulong allocindex)
{
  molecule cule;
  molecule nextmolecule;
  tag moleculetag;
  tag nextmoleculetag;
  int atomindex;
  int newatomindex;
  int lasttextindex;
  int nextculeindex;
  int bufferatoms;
  int bufferindex;
  int i;
  char nextculeatoms[COMPRESSEDTAGLENGTH];
  char swapatom;
  char nowatom;

  cule = insertposition->cule;
  atomindex = insertposition->textindex;
  lasttextindex = insertposition->lasttextindex;
  moleculetag = insertposition->moleculetag;
  nextmoleculetag = insertposition->nextmoleculetag;
  /* Start at the end of `newatombuffer' and work backward. */
  bufferindex = numatoms - 1;
  /* In the link ring, we are at the start of a compressed tag. */
  swapatom = (char) STOP;

  /* Loop through the linked list of molecules. */
  while (1) {
    /* Loop through the vertex atoms in this molecule. */
    while ((atomindex <= lasttextindex) &&
           ((cule[atomindex] != (char) STOP) || (swapatom >= (char) 0))) {
      /* Swap an atom in the link ring with one in the buffer. */
      swapatom = cule[atomindex];
      cule[atomindex] = newatombuffer[bufferindex];
      newatombuffer[bufferindex] = swapatom;
      atomindex++;
      /* Advance backward through the circular buffer.  This reverses the  */
      /*   order of the input atoms.  The circularity makes it possible to */
      /*   shift the link ring atoms forward through a linked list.        */
      bufferindex = ((bufferindex == 0) ? numatoms : bufferindex) - 1;
    }

    if (nextmoleculetag == STOP) {
      /* We've swapped the last vertex atom in the list into the buffer. */
      break;
    } else {
      /* Find the next molecule. */
      cule = (molecule) proxipooltag2object(pool, nextmoleculetag);
      moleculetag = nextmoleculetag;
      /* Read the tag for the next molecule after that. */
      linkringreadtag(moleculetag, cule, lasttextindex, nextmoleculetag,
                      nowatom, "Internal error in linkringinsertatoms():\n");
      atomindex = 0;
    }
  }

  if (swapatom >= (char) 0) {
    /* The end of the last vertex is missing from the last molecule. */
    printf("Internal error in linkringinsertatoms():\n");
    printf("  Vertex at end of link ring not properly terminated.\n");
    internalerror();
  }

  /* Initialize counter of atoms remaining to append. */
  bufferatoms = numatoms;
  /* While there are too many atoms to fit in one molecule, make molecules. */
  while (atomindex + bufferatoms >= MOLECULESIZE) {
    /* Allocate a new molecule. */
    nextmoleculetag = proxipoolnew(pool, allocindex, (void **) &nextmolecule);
    /* Compress its tag and figure the index to store it at. */
    nextculeindex = MOLECULESIZE -
                    linkringtagcompress(moleculetag, nextmoleculetag,
                                        nextculeatoms);

    /* Is there room for the "next molecule" tag in `cule'? */
    if (atomindex > nextculeindex) {
      /* No; move atoms to the next molecule to make room for the tag. */
      for (newatomindex = 0; newatomindex < atomindex - nextculeindex;
           newatomindex++) {
        nextmolecule[newatomindex] = cule[nextculeindex + newatomindex];
      }
      /* We'll continue writing atoms at this index on the next iteration. */
      atomindex = newatomindex;
    } else {
      /* Fill in the remaining space with atoms from the buffer. */
      while (atomindex < nextculeindex) {
        cule[atomindex] = newatombuffer[bufferindex];
        bufferindex = ((bufferindex == 0) ? numatoms : bufferindex) - 1;
        atomindex++;
        bufferatoms--;
      }
      atomindex = 0;
    }

    /* Copy the "next molecule" tag into the end of `cule'. */
    for (i = nextculeindex; i < MOLECULESIZE; i++) {
      cule[i] = nextculeatoms[i - nextculeindex];
    }
    /* Iterate on the next molecule. */
    moleculetag = nextmoleculetag;
    cule = nextmolecule;
  }

  /* Copy the remaining atoms into the last molecule. */
  while (bufferatoms > 0) {
    cule[atomindex] = newatombuffer[bufferindex];
    bufferindex = ((bufferindex == 0) ? numatoms : bufferindex) - 1;
    bufferatoms--;
    atomindex++;
  }

  /* Write a STOP atom after the last vertex. */
  cule[atomindex] = (char) STOP;
  /* Write a STOP atom for the "next molecule" tag. */
  cule[MOLECULESIZE - 1] = (char) STOP;

  /* Return the tail molecule of the link ring. */
  return moleculetag;
}

/*****************************************************************************/
/*                                                                           */
/*  linkringinsertatoms()   Insert atoms at a specified position in a link   */
/*                          ring (moving the other atoms back to make room). */
/*                                                                           */
/*  See header above linkringinsertatoms2().                                 */
/*                                                                           */
/*****************************************************************************/

tag linkringinsertatoms(struct proxipool *pool,
                        struct linkposition *insertposition,
                        int numatoms,
                        char *newatombuffer,
                        proxipoolulong allocindex)
{
  int atomindex;
  char checkatom;

  /* Is the list just one molecule? */
  if (insertposition->nextmoleculetag == STOP) {
    /* Yes.  Find the end of the vertex tags. */
    /* In the link ring, we are at the start of a compressed tag. */
    checkatom = (char) STOP;
    atomindex = insertposition->textindex;
    while ((atomindex <= MOLECULESIZE - 2) &&
           ((insertposition->cule[atomindex] != (char) STOP) ||
            (checkatom >= (char) 0))) {
      checkatom = insertposition->cule[atomindex];
      atomindex++;
    }
    /* Can we fit the new atoms in this molecule? */
    if (atomindex + numatoms < MOLECULESIZE) {
      /* Yes.  Shift atoms to the right to make room for new ones.  Note     */
      /*   that this loop might overwrite the molecule's final atom with the */
      /*   STOP tag, but that's okay, because it's already a STOP tag.       */
      while (atomindex >= insertposition->textindex) {
        insertposition->cule[atomindex + numatoms] =
          insertposition->cule[atomindex];
        atomindex--;
      }
      /* Fill in the new atoms. */
      while (numatoms > 0) {
        atomindex++;
        numatoms--;
        insertposition->cule[atomindex] = newatombuffer[numatoms];
      }
      /* Return the lone molecule's tag. */
      return insertposition->moleculetag;
    }
  }

  /* The updated list does not fit in one molecule.  Do it the general */
  /*   (slow) way.                                                     */
  return linkringinsertatoms2(pool, insertposition, numatoms, newatombuffer,
                              allocindex);
}

/*****************************************************************************/
/*                                                                           */
/*  linkringdeleteatoms()    Delete atoms at a specified position in a link  */
/*  linkringdeleteatoms2()   ring (shifting the following atoms forward).    */
/*                                                                           */
/*  These procedures are interchangeable.  linkringdeleteatoms2() is a       */
/*  complete implementation.  linkringdeleteatoms() is an optimized wrapper  */
/*  that handles the special case where the entire link ring fits in a       */
/*  single molecule faster, and calls linkringdeleteatoms2() otherwise.      */
/*  Profiling suggests that when MOLECULESIZE is 20, linkringdeleteatoms()   */
/*  resorts to calling linkringdeleteatoms2() only 5.0% of the time.         */
/*                                                                           */
/*  Usually, the parameter `position' signifies the first atom to delete.    */
/*  However, if the first atom to delete is at the beginning of a molecule,  */
/*  and that molecule is not the first in the link ring, it's better for     */
/*  `position' to be at the _end_ of the previous molecule, with `textindex' */
/*  past the last vertex atoms in the molecule.  Why?  Because               */
/*  linkringdeleteatoms2() might be able to free the molecule containing the */
/*  first deleted atom--either because all the remaining atoms in the list   */
/*  are deleted, or because the remaining atoms can fit in the previous      */
/*  molecule, overwriting the "next molecule" tag.                           */
/*                                                                           */
/*  linkringdeleteatoms2() deletes atoms from a link ring by having two      */
/*  references traverse through the list, one at a distance behind the       */
/*  other, and by copying atoms from the lead reference to the one following */
/*  it.  To avoid having to decode the "next molecule" tags twice, the lead  */
/*  reference maintains a queue of molecules for use by the following        */
/*  reference.  The queue is an array whose members are reused in circular   */
/*  order, so the link ring can be arbitrarily long.  The size of the queue  */
/*  is fixed, but these procedures are never used to delete more than a      */
/*  small number of atoms, so the lead reference doesn't get too far ahead   */
/*  of the follower.                                                         */
/*                                                                           */
/*  These procedures are meant for internal use by other linkring            */
/*  procedures.  Any external caller needs to be aware that there is a limit */
/*  on the number of atoms that this procedure can delete at once (which can */
/*  be increased by increasing MOLECULEQUEUESIZE).                           */
/*                                                                           */
/*  pool:  The proxipool that the link ring was allocated from.              */
/*  deleteposition:  References the position of the deleted atoms in the     */
/*    link ring.                                                             */
/*  deleteposition->cule:  Pointer to the molecule.                          */
/*  deleteposition->moleculetag:  Tag for the molecule `cule'.  Only needs   */
/*    to be initialized if you are using the return result.  If this is not  */
/*    initialized, the return result might be wrong.                         */
/*  deleteposition->textindex:  The index within the molecule to start       */
/*    deleting atoms from.                                                   */
/*  deleteposition->lasttextindex:  Index of the last atom in the molecule   */
/*    `cule' that represents a vertex (rather than the "next molecule" tag). */
/*  deleteposition->nextmoleculetag:  Tag for the next molecule in the       */
/*    linked list.  (This field and position->lasttextindex are included so  */
/*    they won't need to be computed a second time.)                         */
/*  numatoms:  The number of atoms to delete.                                */
/*                                                                           */
/*  Returns the tag of the last molecule in the modified linked list.        */
/*                                                                           */
/*****************************************************************************/

tag linkringdeleteatoms2(struct proxipool *pool,
                         struct linkposition *deleteposition,
                         int numatoms)
{
  struct {
    molecule cule;
    tag culetag;
    int lasttextindex;
  } molequeue[MOLECULEQUEUESIZE];
  molecule leadmolecule;
  molecule followmolecule;
  molecule prevmolecule;
  tag leadnexttag;
  tag leadtag;
  tag returntag;
  int leadindex;
  int followindex;
  int leadlastindex;
  int followlastindex;
  int previndex;
  int leadnextqindex;
  int followcurrentqindex;
  int followprevqindex;
  int followadvanceflag;
  int vertexstartflag;
  int i;
  char nowatom;

  /* Remember the starting position. */
  molequeue[0].cule = deleteposition->cule;
  molequeue[0].culetag = deleteposition->moleculetag;
  molequeue[0].lasttextindex = deleteposition->lasttextindex;
  leadnexttag = deleteposition->nextmoleculetag;

  /* Put the leading and following references at the starting position. */
  leadmolecule = deleteposition->cule;
  followmolecule = deleteposition->cule;
  leadindex = deleteposition->textindex;
  followindex = deleteposition->textindex;
  leadlastindex = deleteposition->lasttextindex;
  followlastindex = deleteposition->lasttextindex;
  /* Keep track of the queue index the lead reference will use next, and the */
  /*   index the follower is using now ('cause that's what's convenient).    */
  leadnextqindex = 1;
  followcurrentqindex = 0;
  /* A flag that indicates whether followcurrentqindex has ever advanced. */
  followadvanceflag = 0;
  /* A flag that indicates we're reading the first atom of a tag. */
  vertexstartflag = 1;

  /* Are we past the last atom in the linked list? */
  if ((leadnexttag == STOP) && (leadindex > leadlastindex)) {
    /* There's nothing here to delete. */
    return deleteposition->moleculetag;
  }

  /* The front reference loops through the molecules of the linked list. */
  do {
    /* Skip this block on the first iteration...unless `deleteposition' is */
    /*   already past all the vertex atoms in its molecule.  On subsequent */
    /*   iterations, this block is always executed.                        */
    if (leadindex > leadlastindex) {
      /* Look up the next molecule (for the lead reference). */
      leadmolecule = (molecule) proxipooltag2object(pool, leadnexttag);
      /* Save it for the follower. */
      molequeue[leadnextqindex].culetag = leadnexttag;
      molequeue[leadnextqindex].cule = leadmolecule;
      leadtag = leadnexttag;
      /* Read the "next molecule" tag. */
      linkringreadtag(leadtag, leadmolecule, leadlastindex, leadnexttag,
                      nowatom, "Internal error in linkringdeleteatoms():\n");
      /* Save the index where the vertex atoms end, for the follower. */
      molequeue[leadnextqindex].lasttextindex = leadlastindex;

      leadindex = 0;
      /* Advance (circularly) the queue index for the next iteration. */
      leadnextqindex = (leadnextqindex + 1 == MOLECULEQUEUESIZE) ?
                       0 : leadnextqindex + 1;
      if (leadnextqindex == followcurrentqindex) {
        /* The leader has just stepped on the follower's data.  The queue */
        /*   isn't large enough to delete this many atoms.                */
        printf("Internal error in linkringdeleteatoms():\n");
        printf("  Asked to delete too many atoms; queue too small.\n");
        printf("  (Increase MOLECULEQUEUESIZE and recompile.)\n");
        internalerror();
      }
    }

    /* Skip up to `numatoms' atoms in this molecule. */
    while ((numatoms > 0) && (leadindex <= leadlastindex)) {
      nowatom = leadmolecule[leadindex];
      if (vertexstartflag && (nowatom == (char) STOP)) {
        /* We've reached the end of the link ring. */
        leadindex = MOLECULESIZE;
#ifdef SELF_CHECK
        if (leadnexttag != STOP) {
          /* Oops.  The link ring ends before the last molecule. */
          printf("Internal error in linkringdeleteatoms():\n");
          printf("  Link ring terminator encountered before last molecule.\n");
          internalerror();
        }
#endif /* SELF_CHECK */
      } else {
        vertexstartflag = nowatom < (char) 0;
        leadindex++;
        numatoms--;
      }
    }

    /* Loop through the atoms in this molecule and copy them from the lead */
    /*   to the follower.  If numatoms > 0, this loop is skipped.          */
    while (leadindex <= leadlastindex) {
      nowatom = leadmolecule[leadindex];
      if (vertexstartflag && (nowatom == (char) STOP)) {
        /* We've reached the end of the link ring. */
        leadindex = MOLECULESIZE;
#ifdef SELF_CHECK
        if (leadnexttag != STOP) {
          /* Oops.  The link ring ends before the last molecule. */
          printf("Internal error in linkringdeleteatoms():\n");
          printf("  Link ring terminator encountered before last molecule.\n");
          internalerror();
        }
#endif /* SELF_CHECK */
      } else {
        /* Is the follower at the end of its molecule? */
        if (followindex > followlastindex) {
          /* Yes.  Advance (circularly) the queue index and molecule. */
          followcurrentqindex =
            (followcurrentqindex + 1 == MOLECULEQUEUESIZE) ?
            0 : followcurrentqindex + 1;
          followmolecule = molequeue[followcurrentqindex].cule;
          followindex = 0;
          followlastindex = molequeue[followcurrentqindex].lasttextindex;
          followadvanceflag = 1;
        }

        /* Copy an atom from the leader. */
        followmolecule[followindex] = nowatom;
        followindex++;

        /* Check the atom for a stop bit. */
        vertexstartflag = nowatom < (char) 0;
        leadindex++;
      }
    }
  } while (leadnexttag != STOP);

  if (nowatom >= (char) 0) {
    /* Oops.  The end of the last vertex is missing from the last molecule. */
    printf("Internal error in linkringdeleteatoms():\n");
    printf("  Vertex at end of link ring not properly terminated.\n");
    internalerror();
  }

  /* The leader has reached the end of the linked list.  The follower might */
  /*   be able to get rid of the molecule it's currently in by moving atoms */
  /*   into the previous molecule, writing over atoms taken by the "next    */
  /*   molecule tag".  Check whether it's possible.  It's only possible if  */
  /*   there is a previous molecule, and that molecule has enough space     */
  /*   currently taken by the "next molecule" tag.                          */
  followprevqindex = (followcurrentqindex == 0) ?
                     MOLECULEQUEUESIZE - 1 : followcurrentqindex - 1;
  if (followadvanceflag &&
      (molequeue[followprevqindex].cule != (molecule) NULL) &&
      (followindex <=
       MOLECULESIZE - 2 - molequeue[followprevqindex].lasttextindex)) {
    prevmolecule = molequeue[followprevqindex].cule;
    previndex = molequeue[followprevqindex].lasttextindex + 1;
    /* Prepare to return the tail molecule. */
    returntag = molequeue[followprevqindex].culetag;
    /* Copy atoms from the follower's molecule to its previous molecule. */
    for (i = 0; i < followindex; i++) {
      prevmolecule[previndex] = followmolecule[i];
      previndex++;
    }
    /* Write a STOP atom after the last vertex. */
    prevmolecule[previndex] = (char) STOP;
    /* Write a STOP atom for the "next molecule" tag. */
    prevmolecule[MOLECULESIZE - 1] = (char) STOP;
  } else {
    /* Prepare to return the tail molecule. */
    returntag = molequeue[followcurrentqindex].culetag;
    /* Write a STOP atom after the last vertex. */
    followmolecule[followindex] = (char) STOP;
    /* Write a STOP atom for the "next molecule" tag. */
    followmolecule[MOLECULESIZE - 1] = (char) STOP;
    /* Start freeing molecules with the next one. */
    followcurrentqindex = (followcurrentqindex + 1 == MOLECULEQUEUESIZE) ?
                          0 : followcurrentqindex + 1;
  }

  /* Loop through the remaining molecules and free them. */
  while (followcurrentqindex != leadnextqindex) {
    proxipoolfree(pool, molequeue[followcurrentqindex].culetag);
    followcurrentqindex = (followcurrentqindex + 1 == MOLECULEQUEUESIZE) ?
                          0 : followcurrentqindex + 1;
  }

  /* Return the tail molecule of the link ring. */
  return returntag;
}

/*****************************************************************************/
/*                                                                           */
/*  linkringdeleteatoms()   Delete atoms at a specified position in a link   */
/*                          ring (shifting the following atoms forward).     */
/*                                                                           */
/*  See header above linkringdeleteatoms2().                                 */
/*                                                                           */
/*****************************************************************************/

tag linkringdeleteatoms(struct proxipool *pool,
                        struct linkposition *deleteposition,
                        int numatoms)
{
  int i;

  /* Is the list just one molecule? */
  if (deleteposition->nextmoleculetag == STOP) {
    /* Yes.  Shift atoms to the left to delete `numatoms' atoms. */
    for (i = deleteposition->textindex + numatoms; i < MOLECULESIZE - 1; i++) {
      deleteposition->cule[i - numatoms] = deleteposition->cule[i];
    }
    deleteposition->cule[i - numatoms] = (char) STOP;
    /* Return the lone molecule's tag. */
    return deleteposition->moleculetag;
  } else {
    /* The list does not fit in one molecule.  Do it the general (slow) way. */
    return linkringdeleteatoms2(pool, deleteposition, numatoms);
  }
}

/*****************************************************************************/
/*                                                                           */
/*  linkringrotateatoms()   Swap two contiguous sets of atoms.               */
/*                                                                           */
/*  This is a special-purpose procedure used solely to handle one special    */
/*  case in linkringinsertedge().  If a link ring consists of three or more  */
/*  distinct chains of edges (with three or more ghost vertices filling the  */
/*  gaps), a newly inserted edge might glue together two of those chains--   */
/*  and the chains might not be listed in the right order.  So some of the   */
/*  chains need to be swapped.                                               */
/*                                                                           */
/*  Imagine dividing a link ring into three segments of vertices (including  */
/*  ghost vertices).  One segment begins at position `pos1' and ends with    */
/*  the atom right before position `pos2'.  Another segment begins at `pos2' */
/*  and ends with the atom right before `pos3'.  A third segment begins at   */
/*  `pos3' and (circularly) ends right before `pos1'.  This procedure swaps  */
/*  the first two segments (beginning at `pos1' and `pos2').                 */
/*                                                                           */
/*  WARNING:  It's dangerous to swap two arbitrary segments, because the     */
/*  beginning of the linked list must coincide with the beginning of a       */
/*  compressed vertex tag.  Therefore, the positions `pos1', `pos2', and     */
/*  `pos3' should occur in that order in the linked list, or one of those    */
/*  three should be the very start of the linked list.  Calling this         */
/*  procedure in other circumstances might misalign a compressed tag at the  */
/*  start of the list (though it will swap the atoms as advertised).         */
/*                                                                           */
/*  On return, `pos2' references the first atom of the _moved_ copy of the   */
/*  segment that was originally (but no longer) at `pos1'.  The contents of  */
/*  `pos1' may be trashed, so copy it before calling if you need it.  `pos3' */
/*  is unchanged.                                                            */
/*                                                                           */
/*  It's nearly impossible to do this efficiently without extra buffer       */
/*  space (since the link ring is only singly-linked), and this procedure    */
/*  might temporarily allocate as large an array as it needs.                */
/*                                                                           */
/*  pool:  The proxipool that the link ring was allocated from.              */
/*  linkring:  Tag for the first molecule in the link ring.                  */
/*  pos1:  References the first atom in the first segment to swap.  Contents */
/*    are trashed on return.                                                 */
/*  pos2:  References the first atom in the second segment to swap.  Also    */
/*    demarcates the end of the first segment.  On return, this position     */
/*    references the first atom in the _moved_ segment that was originally   */
/*    the first segment (but now comes second).                              */
/*  pos3:  References the first atom in a third segment, which doesn't move. */
/*    Therefore, `pos3' demarcates the end of the second segment.            */
/*                                                                           */
/*****************************************************************************/

void linkringrotateatoms(struct proxipool *pool,
                         tag linkring,
                         struct linkposition *pos1,
                         struct linkposition *pos2,
                         struct linkposition *pos3)
{
#define STARTBUFFERSIZE 1024

  starlong bufferindex;
  starlong buffersize;
  starlong i;
  int atomindex1, atomindex2, atomindex3;
  int starttag1flag, starttag2flag;
  char firstbuffer[STARTBUFFERSIZE];
  char *buffer;
  char *newbuffer;
  char nowatom;

  /* Start with a buffer in static memory.  We'll allocate a larger one */
  /*   from dynamic memory only if necessary.                           */
  buffer = firstbuffer;
  buffersize = STARTBUFFERSIZE;
  bufferindex = 0;
  /* Move the indices into local variables (hopefully registers). */
  atomindex1 = pos1->textindex;
  atomindex2 = pos2->textindex;
  atomindex3 = pos3->textindex;
  /* Indicate that each position, 1 and 2, is at the start of a compressed */
  /*   vertex tag.                                                         */
  starttag1flag = 1;
  starttag2flag = 1;

  /* Each iteration of this loop advances both positions by one atom. */
  do {
    /* Have we read the last vertex atom in the molecule at position 1? */
    if ((atomindex1 > pos1->lasttextindex) ||
        (starttag1flag && (pos1->cule[atomindex1] == (char) STOP))) {
      /* Yes, we have.  Are there any more molecules after this one? */
      if (pos1->nextmoleculetag == STOP) {
        /* No, there aren't.  Loop back to the start of the linked list. */
        pos1->nextmoleculetag = linkring;
      }
      /* Find the next molecule in the linked list. */
      pos1->cule = (molecule) proxipooltag2object(pool, pos1->nextmoleculetag);
      pos1->moleculetag = pos1->nextmoleculetag;
      /* Read the next molecule's "next molecule" tag. */
      linkringreadtag(pos1->moleculetag, pos1->cule, pos1->lasttextindex,
                      pos1->nextmoleculetag, nowatom,
                      "Internal error in linkringrotateatoms():\n");
      /* Start from the beginning of this molecule. */
      atomindex1 = 0;
    }

    /* Have we read the last vertex atom in the molecule at position 2? */
    if ((atomindex2 > pos2->lasttextindex) ||
        (starttag2flag && (pos2->cule[atomindex2] == (char) STOP))) {
      /* Yes, we have.  Are there any more molecules after this one? */
      if (pos2->nextmoleculetag == STOP) {
        /* No, there aren't.  Loop back to the start of the linked list. */
        pos2->nextmoleculetag = linkring;
      }
      /* Find the next molecule in the linked list. */
      pos2->cule = (molecule) proxipooltag2object(pool, pos2->nextmoleculetag);
      pos2->moleculetag = pos2->nextmoleculetag;
      /* Read the next molecule's "next molecule" tag. */
      linkringreadtag(pos2->moleculetag, pos2->cule, pos2->lasttextindex,
                      pos2->nextmoleculetag, nowatom,
                      "Internal error in linkringrotateatoms():\n");
      /* Start from the beginning of this molecule. */
      atomindex2 = 0;

      /* Exit the loop when position 2 reaches position 3. */
      if ((pos2->cule == pos3->cule) && (atomindex2 == atomindex3)) {
        break;
      }
    }

    /* There's no telling at the start how large a buffer we need, so we */
    /*   may need to resize it.                                          */
    if (bufferindex >= buffersize) {
      newbuffer = (char *) starmalloc((size_t) (buffersize * 3));
      for (i = 0; i < buffersize; i++) {
        newbuffer[i] = buffer[i];
      }
      /* The first buffer is in static memory; only free subsequent buffers. */
      if (buffer != firstbuffer) {
        starfree(buffer);
      }
      buffer = newbuffer;
      buffersize *= 3;
    }

    /* Copy atoms from position 1 to the buffer. */
    nowatom = pos1->cule[atomindex1];
    buffer[bufferindex] = nowatom;
    /* Will we be starting a new tag at position 1?  Check for a stop bit. */
    starttag1flag = nowatom < (char) 0;
    /* Copy atoms from position 2 to position 1. */
    nowatom = pos2->cule[atomindex2];
    pos1->cule[atomindex1] = nowatom;
    /* Will we be starting a new tag at position 2? */
    starttag2flag = nowatom < (char) 0;
    /* Advance all the positions. */
    bufferindex++;
    atomindex1++;
    atomindex2++;

    /* Exit the loop when position 2 reaches position 3. */
  } while ((pos2->cule != pos3->cule) || (atomindex2 != atomindex3));

  /* Now we know how many atoms are in the second segment.  Set up the */
  /*   buffer to be a circular buffer of that length.                  */
  buffersize = bufferindex;
  bufferindex = 0;

  /* Store the position of the (now) second of the two swapped segments, to */
  /*   return to the caller.                                                */
  *pos2 = *pos1;
  pos2->textindex = atomindex1;

  /* Position 1 continues to loop through atoms, swapping them with atoms   */
  /*   in the circular buffer, until it reaches position 3.  The circular   */
  /*   buffer makes it possible to move all the atoms in the first segment  */
  /*   back by a number of atoms equal to the length of the second segment, */
  /*   without knowing how long the first segment is.                       */
  do {
    /* Have we read the last vertex atom in the molecule at position 1? */
    if ((atomindex1 > pos1->lasttextindex) ||
        (starttag1flag && (pos1->cule[atomindex1] == (char) STOP))) {
      /* Yes, we have.  Are there any more molecules after this one? */
      if (pos1->nextmoleculetag == STOP) {
        /* No, there aren't.  Loop back to the start of the linked list. */
        pos1->nextmoleculetag = linkring;
      }
      /* Find the next molecule in the linked list. */
      pos1->cule = (molecule) proxipooltag2object(pool, pos1->nextmoleculetag);
      pos1->moleculetag = pos1->nextmoleculetag;
      /* Read the next molecule's "next molecule" tag. */
      linkringreadtag(pos1->moleculetag, pos1->cule, pos1->lasttextindex,
                      pos1->nextmoleculetag, nowatom,
                      "Internal error in linkringrotateatoms():\n");
      /* Start from the beginning of this molecule. */
      atomindex1 = 0;

      /* Exit the loop when position 1 reaches position 3. */
      if ((pos1->cule == pos3->cule) && (atomindex1 == atomindex3)) {
        break;
      }
    }

    /* Swap atoms between position 1 and the buffer. */
    nowatom = pos1->cule[atomindex1];
    pos1->cule[atomindex1] = buffer[bufferindex];
    buffer[bufferindex] = nowatom;
    /* Will we be starting a new tag?  Check for a stop bit. */
    starttag1flag = nowatom < (char) 0;
    /* Advance the positions. */
    bufferindex = (bufferindex + 1 >= buffersize) ? 0 : bufferindex + 1;
    atomindex1++;
    /* Exit the loop when position 1 reaches position 3. */
  } while ((pos1->cule != pos3->cule) || (atomindex1 != atomindex3));

  /* If the buffer was allocated dynamically, free it. */
  if (buffer != firstbuffer) {
    starfree(buffer);
  }
}

/*****************************************************************************/
/*                                                                           */
/*  linkringinsertedge()   Insert an edge into a link ring.                  */
/*                                                                           */
/*  If this link ring is the link of an edge, you are actually inserting a   */
/*  tetrahedron (from the edge's point of view--you may need to inform the   */
/*  tetrahedron's other edges as well.)  If this link ring is the link of a  */
/*  vertex, you are actually inserting a triangle (from the vertex's point   */
/*  of view).                                                                */
/*                                                                           */
/*  The new edge is `endpoint1'-`endpoint2', with `endpoint2' immediately    */
/*  following (counterclockwise from--unless you're using the reverse        */
/*  orientation everywhere, as these procedures can't tell the difference)   */
/*  `endpoint1'.  The order in which the two endpoints are given matters--it */
/*  must match the desired orientation of the link ring.  It is possible to  */
/*  have a link ring with just two edges--say, 1-2 and 2-1.                  */
/*                                                                           */
/*  If both vertices of the new edge are already present, this procedure     */
/*  can glue two chains together, or close a single chain (making the link   */
/*  into a true ring), if it's appropriate to do so.                         */
/*                                                                           */
/*  This procedure will return 0 and leave the link ring unchanged if any of */
/*  the following is true.                                                   */
/*                                                                           */
/*  - There is already an edge immediately following `endpoint1'.  (That is, */
/*    `endpoint1' is in the list and is not followed by a ghost vertex.)     */
/*  - There is already an edge immediately preceding `endpoint2'.  (That is, */
/*    `endpoint2' is in the list and is not preceded by a ghost vertex.)     */
/*  - The link ring is truly a ring, with no space to insert a new edge      */
/*    (i.e. no ghost vertex).                                                */
/*  - The link ring currently consists of two or more chains, and the        */
/*    introduction of the new edge will glue one of them into a ring.  It's  */
/*    not topologically possible to have a link with a ring AND a chain.     */
/*                                                                           */
/*  WARNING:  This procedure will not usually work if there is more than one */
/*  copy of `endpoint1', or more than one copy of `endpoint2', in the ring.  */
/*                                                                           */
/*  pool:  The proxipool that the link ring was allocated from, and that new */
/*    molecules may be allocated from.                                       */
/*  linkring:  Tag for the first molecule in the link ring.                  */
/*  groundtag:  The ground tag relative to which the vertices are            */
/*    decompressed.                                                          */
/*  endpoint1:  The tag of the new edge's first vertex.                      */
/*  endpoint2:  The tag of the new edge's second vertex.                     */
/*                                                                           */
/*  Returns one of the following values.                                     */
/*    0:  If the edge cannot be inserted, and the link ring is unchanged.    */
/*    1:  If `endpoint1' was inserted, and `endpoint2' was already present.  */
/*    2:  If `endpoint2' was inserted, and `endpoint1' was already present.  */
/*    3:  If neither vertex was present, and both were inserted.  (In this   */
/*        case, a ghost vertex is inserted as well to prevent the new edge   */
/*        from being connected to any other vertex.)                         */
/*    4:  If both `endpoint1' and `endpoint2' were already present in the    */
/*        link, and merely needed to be connected by an edge (which is       */
/*        accomplished by deleting the ghost vertex between the two          */
/*        endpoints).                                                        */
/*                                                                           */
/*****************************************************************************/

int linkringinsertedge(struct proxipool *pool,
                       tag linkring,
                       tag groundtag,
                       tag endpoint1,
                       tag endpoint2)
{
  struct linkposition position;
  struct linkposition pt2position;
  struct linkposition ghostposition;
  struct linkposition ghostafter1position;
  struct linkposition afterghostafter2position;
  molecule cule;
  tag nextmoleculetag;
  tag moleculetag;
  int atomindex;
  int lasttextindex;
  int numatoms;
  char newvertexatoms[2 * COMPRESSEDTAGLENGTH + 1];
  char nowatom;

  /* There is no "previous" vertex we've visited yet. */
  tag firstvertextag = STOP;
  tag prevvertextag = STOP;
  tag prevprevvertextag = STOP;
  tag vertextag = 0;
  int vertexatoms = 0;
  /* Set to 1 during the first iteration of the "do" loop. */
  int firstflag = 1;
  /* The following flags indicate whether `endpoint1', `endpoint2', or */
  /*   a ghost vertex have been visited in the linked list yet.        */
  int oneflag = 0;
  int twoflag = 0;
  int ghostflag = 0;
  /* Set if `endpoint2' is one of the first two vertices in the linked list. */
  int early2flag = 0;
  /* Set if `endpoint2' has been visited, but not `endpoint1' after it. */
  int twonot1flag = 0;
  /* Set if a ghost vertex has been visited, but not `endpoint2' after it. */
  int ghostnot2flag = 0;
  /* Set if at least one ghost vertex comes after `endpoint2' but before */
  /*   `endpoint1' in the ring (taking into account that it loops).      */
  int ghostbetween21flag = 0;
  /* Set if `endpoint1' appears after `endpoint2' in the linked list. */
  int oneafter2flag = 0;
  /* Set if a ghost vertex appears after `endpoint2' in the linked list. */
  int ghostafter2flag = 0;

  if ((linkring >= STOP) || (endpoint1 >= STOP) || (endpoint2 >= STOP)) {
    /* Invalid parameters. */
    return 0;
  }

  /* Start at the first molecule. */
  nextmoleculetag = linkring;

  /* Loop through the linked list of molecules. */
  do {
    /* Convert the molecule's tag to a pointer. */
    cule = (molecule) proxipooltag2object(pool, nextmoleculetag);
    moleculetag = nextmoleculetag;
    /* Read the "next molecule" tag. */
    linkringreadtag(moleculetag, cule, lasttextindex, nextmoleculetag, nowatom,
                    "Internal error in linkringinsertedge():\n");
    if (firstflag) {
      /* Position to insert or delete atoms at the beginning of the list. */
      position.cule = cule;
      position.moleculetag = moleculetag;
      position.textindex = 0;
      position.lasttextindex = lasttextindex;
      position.nextmoleculetag = nextmoleculetag;
      firstflag = 0;
    }

    atomindex = 0;
    /* Loop through the atoms, stopping at the "next molecule" tag. */
    while (atomindex <= lasttextindex) {
      nowatom = cule[atomindex];
      /* Append the atom to the vertex tag. */
      vertextag = (vertextag << 7) + (nowatom & (char) 127);
      vertexatoms++;
      atomindex++;

      /* Is this the last atom in the compressed tag? */
      if (nowatom < (char) 0) {
        /* Yes; we have an uncompressed tag now.  What tag is it? */
        if ((nowatom == (char) STOP) && (vertexatoms == 1)) {
          /* STOP tag.  Exit the inner loop early; go on to next molecule. */
          break;
        } else if ((nowatom == (char) GHOSTVERTEX) && (vertexatoms == 1)) {
          /* Ghost vertex. */
          vertextag = GHOSTVERTEX;
          ghostflag = 1;
          if (twoflag) {
            /* The beginning/end of the linked list does not fall between */
            /*   `endpoint2' and the following ghost.                     */
            ghostafter2flag = 1;
          }
          if (twonot1flag) {
            /* Because `endpoint2' has been visited, and `endpoint1' didn't */
            /*   follow it, this ghost vertex appears after `endpoint2' and */
            /*   before `endpoint1' in the link ring...IF `endpoint1' is in */
            /*   the link at all.  (If not, this won't hurt.)               */
            ghostbetween21flag = 1;
          }
          /* Remember the ghost's position in case we need to insert a */
          /*   vertex here or delete the ghost.                        */
          ghostposition = position;
          if (prevvertextag == endpoint1) {
            ghostafter1position = position;
          }
          if (!ghostnot2flag) {
            /* Remember the position after the ghost, in case we need to */
            /*   rotate the atoms in the link ring.  Note that if        */
            /*   `endpoint2' isn't in the link ring, this branch might   */
            /*   execute, but it won't matter.                           */
            afterghostafter2position.cule = cule;
            afterghostafter2position.textindex = atomindex;
            afterghostafter2position.lasttextindex = lasttextindex;
            afterghostafter2position.moleculetag = moleculetag;
            afterghostafter2position.nextmoleculetag = nextmoleculetag;
          }
          ghostnot2flag = 1;
        } else {
          /* The tag is neither a STOP tag nor a ghost vertex. */
          if (prevvertextag == endpoint1) {
            /* `endpoint1' does not have a ghost vertex after it, so it is */
            /*   not possible to insert an (ordered) edge 1-2.             */
            return 0;
          }

          /* Use the ground tag to supply the high-order bits. */
          vertextag += (groundtag >> (7 * vertexatoms)) << (7 * vertexatoms);

          if (vertextag == endpoint1) {
            oneflag = 1;
            twonot1flag = 0;
            if (ghostnot2flag) {
              /* Because a ghost vertex has been visited, and `endpoint2' */
              /*   didn't follow it, that ghost vertex appears after      */
              /*   `endpoint2' and before `endpoint1' in the link ring... */
              /*   IF `endpoint2' is in the link at all.  (If not, this   */
              /*   won't hurt.)                                           */
              ghostbetween21flag = 1;
            }
            if (twoflag) {
              oneafter2flag = 1;
            }
          } else if (vertextag == endpoint2) {
            if ((prevvertextag != GHOSTVERTEX) && (prevvertextag != STOP)) {
              /* `endpoint2' does not have a ghost vertex before it, so it */
              /*   is not possible to insert an (ordered) edge 1-2.        */
              return 0;
            }
            if (prevprevvertextag == endpoint1) {
              /* `endpoint1' is immediately followed by a ghost vertex, then */
              /*   `endpoint2', so just delete the ghost from the middle.    */
              linkringdeleteatoms(pool, &ghostposition, 1);
              return 4;
            }

            if (prevprevvertextag == STOP) {
              /* `endpoint2' is one of the first two vertices in the list. */
              /*   Remember this in case `endpoint1' is the last.          */
              early2flag = 1;
            }
            twoflag = 1;
            twonot1flag = 1;
            ghostnot2flag = 0;
            /* Remember `endpoint2's position in case we need to insert */
            /*   `endpoint1' in front of it.                            */
            pt2position = position;
          }
        }

        if (prevvertextag == STOP) {
          /* This is the first vertex.  Remember it. */
          firstvertextag = vertextag;
        }

        /* Remember `vertextag' for the next two iterations. */
        prevprevvertextag = prevvertextag;
        prevvertextag = vertextag;
        /* Prepare to read another tag. */
        vertextag = 0;
        vertexatoms = 0;

        /* Store the position of the beginning of the next compressed tag,   */
        /*   in case we realize we need it once we reach the end of the tag. */
        position.cule = cule;
        position.textindex = atomindex;
        position.lasttextindex = lasttextindex;
        position.moleculetag = moleculetag;
        position.nextmoleculetag = nextmoleculetag;
      }
    }
  } while (nextmoleculetag != STOP);

  if (nowatom >= (char) 0) {
    /* Oops.  The end of the last vertex is missing from the last molecule. */
    printf("Internal error in linkringinsertedge():\n");
    printf("  Vertex at end of link ring not properly terminated.\n");
    internalerror();
  }

  if (prevvertextag == endpoint1) {
    /* The last vertex in the linked list is `endpoint1'. */
    if (firstvertextag != GHOSTVERTEX) {
      /* `endpoint1' does not have a ghost vertex after it, so it is */
      /*   not possible to insert an (ordered) edge 1-2.             */
      return 0;
    } else if (!twoflag) {
      /* `endpoint2' is not in the list, so insert it right after */
      /*   `endpoint1' (at the very end of the linked list).      */
      numatoms = linkringtagcompress(groundtag, endpoint2, newvertexatoms);
      linkringinsertatoms(pool, &position, numatoms, newvertexatoms,
                          proxipooltag2allocindex(pool, linkring));
      return 2;
    } else {
      /* The ghost following `endpoint1` is at the beginning of the list. */
      ghostafter1position.moleculetag = linkring;
      ghostafter1position.cule = (molecule) proxipooltag2object(pool,
                                                                linkring);
      ghostafter1position.textindex = 0;
      /* Read the "next molecule" tag. */
      linkringreadtag(linkring, ghostafter1position.cule,
                      ghostafter1position.lasttextindex,
                      ghostafter1position.nextmoleculetag,
                      nowatom, "Internal error in linkringinsertedge():\n");
      if (early2flag) {
        /* `endpoint2' must occur second in the linked list, with a ghost   */
        /*   vertex before it, and `endpoint1' at the end of the list (thus */
        /*   before the ghost in the link ring).  Delete the ghost vertex.  */
        linkringdeleteatoms(pool, &ghostafter1position, 1);
        return 4;
      }
      /* If execution reaches this point, we either need to rotate a portion */
      /*   of the link ring, or decide that inserting the edge is            */
      /*   impossible.  Code for doing these appears further down.           */
    }
  } else if (firstvertextag == endpoint2) {
    /* The first vertex in the linked list is `endpoint2'. */
    if (prevvertextag != GHOSTVERTEX) {
      /* `endpoint2' (at the beginning of the linked list) does not have a   */
      /*   ghost vertex before it in the link ring (at the end of the list), */
      /*   so it is not possible to insert an (ordered) edge 1-2.            */
      return 0;
    } else if (prevprevvertextag == endpoint1) {
      /* The linked list ends with `endpoint1' (second to last) and a ghost */
      /*   vertex (last), and begins with `endpoint2'.  Delete the ghost.   */
      linkringdeleteatoms(pool, &ghostposition, 1);
      return 4;
    } else if (!oneflag) {
      /* `endpoint1' is not in the list, so insert it at the very end of the */
      /*   linked list, putting it right before `endpoint2' in the ring.     */
      numatoms = linkringtagcompress(groundtag, endpoint1, newvertexatoms);
      linkringinsertatoms(pool, &position, numatoms, newvertexatoms,
                          proxipooltag2allocindex(pool, linkring));
      return 1;
    }
    /* If execution reaches this point, we either need to rotate a portion */
    /*   of the link ring, or decide that inserting the edge is            */
    /*   impossible.  Code for doing these appears soon.                   */
  }

  if (oneflag) {
    if (twoflag) {
      if (ghostbetween21flag) {
        /* Here is the yuckiest situation.  There is a ghost vertex after    */
        /*   `endpoint1', and a ghost vertex before `endpoint2', but they    */
        /*   are not the same ghost vertex.  There is a third ghost vertex   */
        /*   following `endpoint2' and preceding `endpoint1'.  Therefore,    */
        /*   the link ring is composed of three or more connected chains of  */
        /*   edges, and we need to reorder these chains to get `endpoint1'   */
        /*   and `endpoint2' connected to each other.  There's a procedure   */
        /*   written just for this purpose.                                  */
        /*                                                                   */
        /* Imagine dividing the link ring into three segments of vertices    */
        /*   (including ghost vertices).  Segment 2 begins with `endpoint2'  */
        /*   and ends with the first ghost vertex following it.  Segment 1   */
        /*   begins right after that ghost vertex, and ends at `endpoint1'.  */
        /*   Segment 0 begins with the next vertex, a ghost vertex, and ends */
        /*   with the vertex right before `endpoint2', also a ghost vertex.  */
        /*   We want to swap two of these segments, thereby creating the     */
        /*   edge 1-2.  The swap also puts two ghost vertices together, at   */
        /*   the start of Segment 0 and the end of Segment 2, so we then     */
        /*   delete one of those two ghost vertices.                         */
        /*                                                                   */
        /* However, it's dangerous to swap two arbitrary segments, because   */
        /*   the beginning of the link ring must coincide with the beginning */
        /*   of a vertex.  Therefore, we identify two segments that are safe */
        /*   to swap (don't have the beginning/end of the link ring inside   */
        /*   them), and swap those.                                          */
        if (oneafter2flag) {
          /* It is safe to swap Segments 2 and 1. */
          position = ghostafter1position;
          linkringrotateatoms(pool, linkring, &pt2position,
                              &afterghostafter2position, &ghostafter1position);
        } else if (ghostafter2flag) {
          /* `endpoint1' precedes `endpoint2' in the linked list, and a */
          /*    ghost vertex follows `endpoint2', so it is safe to swap */
          /*    Segments 0 and 2.                                       */
          linkringrotateatoms(pool, linkring, &ghostafter1position,
                              &pt2position, &afterghostafter2position);
          /* When the above procedure returns, `pt2position' is the new */
          /*   position of Segment 0.                                   */
          position = pt2position;
        } else {
          /* Segment 2 overlaps the beginning/end of the link ring, so it is */
          /*   safe to swap Segments 1 and 0.                                */
          position = afterghostafter2position;
          linkringrotateatoms(pool, linkring, &afterghostafter2position,
                              &ghostafter1position, &pt2position);
        }

        /*   The swap puts two ghost vertices together, at the end of      */
        /*   Segment 2 and the start of Segment 0.  Delete the second one. */
        linkringdeleteatoms(pool, &position, 1);
        return 4;
      } else {
        /* There is a ghost vertex after `endpoint1', and a ghost vertex     */
        /*   before `endpoint2', but they are not the same ghost vertex.     */
        /*   There is no ghost vertex following `endpoint2' and preceding    */
        /*   `endpoint1', so the two endpoints are ends of the same chain.   */
        /*   If we create an edge 1-2, it will close that chain into a ring. */
        /*   However, there's at least one other chain, and it's not         */
        /*   topologically possible to have a link with a ring AND a chain.  */
        return 0;
      }
    } else {
      /* `endpoint2' is not in the list, so insert it right after */
      /*   `endpoint1'.                                           */
      numatoms = linkringtagcompress(groundtag, endpoint2, newvertexatoms);
      linkringinsertatoms(pool, &ghostafter1position, numatoms, newvertexatoms,
                          proxipooltag2allocindex(pool, linkring));
      return 2;
    }
  } else {
    if (twoflag) {
      /* `endpoint1' is not in the list, so insert it right before */
      /*   `endpoint2'.                                            */
      numatoms = linkringtagcompress(groundtag, endpoint1, newvertexatoms);
      linkringinsertatoms(pool, &pt2position, numatoms, newvertexatoms,
                          proxipooltag2allocindex(pool, linkring));
      return 1;
    } else {
      /* Neither `endpoint1' nor `endpoint2' are in the list. */
      if (ghostflag || (prevvertextag == STOP)) {
        /* Insert a ghost vertex, followed by `endpoint1', followed by    */
        /*   `endpoint2'.  Because linkringtagcompress() writes the atoms */
        /*   backward, and linkringinsertatoms() reverses them back to    */
        /*   forward, write the sequence 2-1-ghost into the buffer.       */
        numatoms = linkringtagcompress(groundtag, endpoint2,
                                       newvertexatoms);
        numatoms += linkringtagcompress(groundtag, endpoint1,
                                        &newvertexatoms[numatoms]);
        newvertexatoms[numatoms] = (char) GHOSTVERTEX;
        if (ghostflag) {
          /* Insert them in front of a ghost vertex. */
          linkringinsertatoms(pool, &ghostposition, numatoms + 1,
                              newvertexatoms,
                              proxipooltag2allocindex(pool, linkring));
        } else {
          /* The link ring is empty.  Insert the three vertices. */
          linkringinsertatoms(pool, &position, numatoms + 1, newvertexatoms,
                              proxipooltag2allocindex(pool, linkring));
        }
        return 3;
      } else {
        /* The link ring is really a ring, so there's no gap to put a new */
        /*   edge into.                                                   */
        return 0;
      }
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*  linkringdeleteedge()   Delete an edge from a link ring.                  */
/*                                                                           */
/*  This procedure also deletes any endpoint of the edge that is not an      */
/*  endpoint of another edge.  Therefore, it cannot be used to create a      */
/*  single dangling vertex with no edge.  (You can accomplish that by using  */
/*  linkringdeletevertex() to whittle a chain down to one vertex, or by      */
/*  using linkringinsertvertex() to insert a ghost vertex.)                  */
/*                                                                           */
/*  The order in which the two endpoints are given matters--it must match    */
/*  the edge's orientation in the link ring.  It is possible to have a link  */
/*  ring with just two edges--say, 1-2 and 2-1.  In this case, the order of  */
/*  the endpoints determines which of the two edges is deleted.              */
/*                                                                           */
/*  WARNING:  This procedure will not usually work if there is more than one */
/*  copy of `endpoint1', or more than one copy of `endpoint2', in the ring.  */
/*                                                                           */
/*  pool:  The proxipool that the link ring was allocated from, and that new */
/*    molecules may be allocated from.                                       */
/*  linkring:  Tag for the first molecule in the link ring.                  */
/*  groundtag:  The ground tag relative to which the vertices are            */
/*    decompressed.                                                          */
/*  endpoint1:  The tag of the deleted edge's first vertex.                  */
/*  endpoint2:  The tag of the deleted edge's second vertex.                 */
/*                                                                           */
/*  Returns one of the following values.                                     */
/*    -1:  If the link ring is empty on entry (so it is not changed).        */
/*     0:  If the edge is not present, and the link ring is unchanged (but   */
/*         not empty).                                                       */
/*     1:  If `endpoint1' was deleted, and `endpoint2' survives because it   */
/*         is an endpoint of another edge.                                   */
/*     2:  If `endpoint2' was deleted, and `endpoint1' survives.             */
/*     3:  If both vertices are deleted, but some other vertex survives.     */
/*         (In this case, a ghost vertex is deleted as well, to prevent      */
/*         having two adjacent ghost vertices.)                              */
/*     7:  If both vertices are deleted, and the link ring is now empty.     */
/*         (A ghost vertex is deleted as well.)                              */
/*     8:  If the edge is deleted, but both vertices survive.  (In this      */
/*         case, a ghost vertex is inserted between them.)                   */
/*                                                                           */
/*    Hence, a positive return value implies that the edge was deleted, and  */
/*    the 4's bit signifies whether the link ring is empty on return.  If    */
/*    the result is not zero, the 1's bit signifies that `endpoint1' is no   */
/*    longer in the link ring (or never was), and the 2's bit signifies that */
/*    `endpoint2' is not in the link ring on return.  (If the result is      */
/*    zero, this procedure doesn't check whether the endpoints are present.) */
/*                                                                           */
/*****************************************************************************/

int linkringdeleteedge(struct proxipool *pool,
                       tag linkring,
                       tag groundtag,
                       tag endpoint1,
                       tag endpoint2)
{
  struct linkposition position;
  struct linkposition pt1position;
  struct linkposition pt2position;
  struct linkposition ghostposition;
  molecule cule;
  tag nextmoleculetag;
  tag moleculetag;
  int atomindex;
  int lasttextindex;
  char ghostatom[1];
  char nowatom;

  /* There is no "previous" vertex we've visited yet. */
  tag firstvertextag = STOP;
  tag prevvertextag = STOP;
  tag vertextag = 0;
  long vertexcount = 0;
  int vertexatoms = 0;
  int end1atoms = 0;
  int end2atoms = 0;
  /* Set to 1 during the first iteration of the "do" loop. */
  int firstflag = 1;
  /* Set if there is a ghost vertex immediately before `endpoint1'. */
  int ghostbefore1flag = 0;
  /* Set if there is a ghost vertex immediately after `endpoint2'. */
  int ghostafter2flag = 0;
  int earlystopflag = 0;

  if ((linkring >= STOP) || (endpoint1 >= STOP) || (endpoint2 >= STOP)) {
    /* Invalid parameters. */
    return 0;
  }

  /* Start at the first molecule. */
  nextmoleculetag = linkring;

  /* Loop through the linked list of molecules. */
  do {
    /* Convert the molecule's tag to a pointer. */
    cule = (molecule) proxipooltag2object(pool, nextmoleculetag);
    moleculetag = nextmoleculetag;
    /* Read the "next molecule" tag. */
    linkringreadtag(moleculetag, cule, lasttextindex, nextmoleculetag, nowatom,
                    "Internal error in linkringdeleteedge():\n");
    if (firstflag) {
      /* Position to insert or delete atoms at the beginning of the list. */
      position.cule = cule;
      position.moleculetag = moleculetag;
      position.textindex = 0;
      position.lasttextindex = lasttextindex;
      position.nextmoleculetag = nextmoleculetag;
      firstflag = 0;
    }

    atomindex = 0;
    /* Loop through the atoms, stopping at the "next molecule" tag. */
    while (atomindex <= lasttextindex) {
      nowatom = cule[atomindex];
      /* Append the atom to the vertex tag. */
      vertextag = (vertextag << 7) + (nowatom & (char) 127);
      vertexatoms++;
      atomindex++;

      /* Is this the last atom in the compressed tag? */
      if (nowatom < (char) 0) {
        /* Yes; we have an uncompressed tag now.  What tag is it? */
        if ((nowatom == (char) STOP) && (vertexatoms == 1)) {
          /* STOP tag.  Exit the inner loop early; go on to next molecule. */
          break;
        } else if ((nowatom == (char) GHOSTVERTEX) && (vertexatoms == 1)) {
          vertextag = GHOSTVERTEX;
          /* Remember this position in case we need to delete vertices here. */
          ghostposition = position;
          if (prevvertextag == endpoint2) {
            /* There's a ghost vertex immediately after `endpoint2'. */
            ghostafter2flag = 1;
          }
        } else {
          /* Use the ground tag to supply the high-order bits. */
          vertextag += (groundtag >> (7 * vertexatoms)) << (7 * vertexatoms);

          if (vertextag == endpoint2) {
            /* Remember this position in case we need to insert a ghost */
            /*   vertex here or delete `endpoint2'.                     */
            pt2position = position;
            end2atoms = vertexatoms;
          } else if (vertextag == endpoint1) {
            /* Remember this position in case we need to delete vertices */
            /*   here.                                                   */
            pt1position = position;
            end1atoms = vertexatoms;
            if (prevvertextag == GHOSTVERTEX) {
              /* There's a ghost vertex immediately before `endpoint1'. */
              ghostbefore1flag = 1;
            }
          }
        }

        vertexcount++;
        if (vertexcount == 1) {
          /* This is the first vertex.  Remember it. */
          firstvertextag = vertextag;
        } else if ((prevvertextag == endpoint1) ^ (vertextag == endpoint2)) {
          /* Either `endpoint1' occurs without `endpoint2' following it, or */
          /*   `endpoint2' occurs without `endpoint1' preceding it, so the  */
          /*   edge we seek to delete doesn't exist.                        */
          return 0;
        } else if ((prevvertextag == endpoint2) && (vertexcount > 3)) {
          /* If execution reaches here, the edge 1-2 does appear in the link */
          /*   ring, and we've collected enough information to determine     */
          /*   whether thre are ghost vertices immediately preceding and     */
          /*   following it.  Exit both loops early.                         */
          earlystopflag = 1;
          nextmoleculetag = STOP;
          break;
        }

        /* Remember this tag during the next iteration. */
        prevvertextag = vertextag;
        /* Prepare to read another tag. */
        vertextag = 0;
        vertexatoms = 0;

        /* Store the position of the beginning of the next compressed tag,   */
        /*   in case we realize we need it once we reach the end of the tag. */
        position.cule = cule;
        position.textindex = atomindex;
        position.lasttextindex = lasttextindex;
        position.moleculetag = moleculetag;
        position.nextmoleculetag = nextmoleculetag;
      }
    }
  } while (nextmoleculetag != STOP);

  if (nowatom >= (char) 0) {
    /* Oops.  The end of the last vertex is missing from the last molecule. */
    printf("Internal error in linkringdeleteedge():\n");
    printf("  Vertex at end of link ring not properly terminated.\n");
    internalerror();
  }

  if (!earlystopflag) {
    if (vertexcount < 2) {
      /* There are fewer than two vertices in the link ring, hence no edge. */
      return (vertexcount == 0) ? -1 : 0;
    } else if (firstvertextag == endpoint1) {
      /* Check if the last vertex in the linked list is a ghost vertex, as */
      /*   it immediately precedes `endpoint1' in the link ring.           */
      ghostbefore1flag = prevvertextag == GHOSTVERTEX;
    } else if (prevvertextag == endpoint2) {
      /* Check if the first vertex in the linked list is a ghost vertex, as */
      /*   it immediately follows `endpoint2' in the link ring.             */
      ghostafter2flag = firstvertextag == GHOSTVERTEX;
    } else if ((prevvertextag != endpoint1) || (firstvertextag != endpoint2)) {
      /* The edge 1-2 is not in the link ring. */
      return 0;
    }
  }

  /* If execution reaches this point, the edge 1-2 is in the link ring. */
  if (ghostafter2flag) {
    if (ghostbefore1flag) {
      /* There are ghost vertices immediately before and after the edge 1-2, */
      /*   and you can't have two ghosts in a row, so one of the ghosts must */
      /*   be deleted along with `endpoint1' and `endpoint2'.                */
      if (vertexcount <= 3) {
        /* Just edge 1-2 and a ghost in the ring.  Empty the link ring. */
        linkringrestart(pool, linkring);
        return 7;
      } else if (earlystopflag) {
        /* Delete the contiguous sequence 1-2-ghost. */
        linkringdeleteatoms(pool, &pt1position, end1atoms + end2atoms + 1);
      } else if (prevvertextag == endpoint1) {
        /* Delete the sequence ghost-1 from the end of the linked list. */
        linkringdeleteatoms(pool, &ghostposition, end1atoms + 1);
        /* The deletion above may have changed the "next molecule" tag */
        /*   stored in the molecule at `pt2position' to STOP.          */
        if (pt2position.cule[MOLECULESIZE - 1] == (char) STOP) {
          pt2position.nextmoleculetag = STOP;
          pt2position.lasttextindex = MOLECULESIZE - 2;
        }
        /* Delete `endpoint2' from the beginning of the list. */
        linkringdeleteatoms(pool, &pt2position, end2atoms);
      } else if (prevvertextag == endpoint2) {
        /* Delete the sequence ghost-1-2 from the end of the linked list. */
        linkringdeleteatoms(pool, &ghostposition, end1atoms + end2atoms + 1);
      } else {
        /* Delete a ghost vertex from the end of the linked list. */
        linkringdeleteatoms(pool, &ghostposition, 1);
        /* The deletion above may have changed the "next molecule" tag */
        /*   stored in the molecule at `pt1position' to STOP.          */
        if (pt1position.cule[MOLECULESIZE - 1] == (char) STOP) {
          pt1position.nextmoleculetag = STOP;
          pt1position.lasttextindex = MOLECULESIZE - 2;
        }
        /* Delete the sequence 1-2 from the beginning of the list. */
        linkringdeleteatoms(pool, &pt1position, end1atoms + end2atoms);
      }
      return 3;
    } else {
      /* There is a ghost vertex immediately after `endpoint2', but not   */
      /*   before `endpoint1', so `endpoint1' is shared with another edge */
      /*   and will survive.  Delete only `endpoint2'.                    */
      linkringdeleteatoms(pool, &pt2position, end2atoms);
      return 2;
    }
  } else {
    if (ghostbefore1flag) {
      /* There is a ghost vertex immediately before `endpoint1', but not */
      /*   after `endpoint2', so `endpoint2' is shared with another edge */
      /*   and will survive.  Delete only `endpoint1'.                   */
      linkringdeleteatoms(pool, &pt1position, end1atoms);
      return 1;
    } else {
      /* There is no ghost vertex immediately before or after edge 1-2, so   */
      /*   both vertices survive.  Insert a ghost vertex between `endpoint1' */
      /*   and `endpoint2'.                                                  */
      ghostatom[0] = (char) GHOSTVERTEX;
      if (prevvertextag == endpoint1) {
        /* Put the ghost vertex at the end of the linked list, immediately */
        /*   following `endpoint1'.                                        */
        linkringinsertatoms(pool, &position, 1, ghostatom,
                            proxipooltag2allocindex(pool, linkring));
      } else {
        /* Put the ghost vertex immediately before `endpoint2'. */
        linkringinsertatoms(pool, &pt2position, 1, ghostatom,
                            proxipooltag2allocindex(pool, linkring));
      }
      return 8;
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*  linkringinsertvertex()   Insert a vertex into a link ring, immediately   */
/*                           following a specified vertex.                   */
/*                                                                           */
/*  The main purpose of this procedure is to provide a way to swap one edge  */
/*  for two.  For instance, if the link ring contains an edge 1-3, and you   */
/*  request that the vertex 2 be inserted after the vertex 1, then the edge  */
/*  1-3 is replaced by the edges 1-2 and 2-3.  (You can accomplish the same  */
/*  thing by calling linkringdeleteedge() and linkringinsertedge(), but it's */
/*  much slower.)                                                            */
/*                                                                           */
/*  If you're feeling sneaky, you can insert GHOSTVERTEX after vertex 1,     */
/*  thereby deleting the edge 1-3.  (But it's safer to do that with          */
/*  linkringdeleteedge().  Or you can insert a specified vertex after        */
/*  GHOSTVERTEX.  However, if there's more than one ghost vertex in the link */
/*  ring, you cannot control which ghost vertex the new vertex is inserted   */
/*  after.                                                                   */
/*                                                                           */
/*  WARNING:  For speed, this procedure does little error checking.  You can */
/*  inadvertently corrupt a link ring by inserting two copies of the same    */
/*  (non-ghost) vertex, or inserting a ghost vertex next to another ghost    */
/*  vertex.  You should never do the latter.  Inserting two copies of the    */
/*  same non-ghost vertex is useful for implementing the Bowyer-Watson       */
/*  algorithm, but if you do so, you cannot use linkringinsertedge() or      */
/*  linkringdeleteedge() to affect the duplicated vertex, and you must use   */
/*  linkringdelete2vertices() carefully to finish digging the cavity, so     */
/*  that the duplicated vertex will merge back into one copy in the end.     */
/*                                                                           */
/*  pool:  The proxipool that the link ring was allocated from, and that new */
/*    molecules may be allocated from.                                       */
/*  linkring:  Tag for the first molecule in the link ring.                  */
/*  groundtag:  The ground tag relative to which the vertices are            */
/*    decompressed.                                                          */
/*  searchvertex:  The tag of the vertex to search for.                      */
/*  newvertex:  The tag to insert after `searchvertex'.                      */
/*                                                                           */
/*  Returns 1 if `searchvertex' is in the link ring; 0 otherwise.  In the    */
/*    latter case, the link ring is not changed.                             */
/*                                                                           */
/*****************************************************************************/

int linkringinsertvertex(struct proxipool *pool,
                         tag linkring,
                         tag groundtag,
                         tag searchvertex,
                         tag newvertex)
{
  struct linkposition position;
  tag vertextag;
  int vertexatoms;
  int numatoms;
  char newvertexatoms[COMPRESSEDTAGLENGTH];
  char nowatom;

  if ((linkring >= STOP) ||
      (newvertex == searchvertex) || (newvertex == STOP)) {
    /* Invalid parameters. */
    return 0;
  }

  vertextag = 0;
  vertexatoms = 0;
  /* Start at the first molecule. */
  position.nextmoleculetag = linkring;

  /* Loop through the linked list of molecules. */
  do {
    /* Convert the molecule's tag to a pointer. */
    position.cule = (molecule) proxipooltag2object(pool, 
                                                   position.nextmoleculetag);
    position.moleculetag = position.nextmoleculetag;
    /* Read the "next molecule" tag. */
    linkringreadtag(position.moleculetag, position.cule,
                    position.lasttextindex, position.nextmoleculetag, nowatom,
                    "Internal error in linkringinsertvertex():\n");

    position.textindex = 0;
    /* Loop through the atoms, stopping at the "next molecule" tag. */
    while (position.textindex <= position.lasttextindex) {
      nowatom = position.cule[position.textindex];
      /* Append the atom to the vertex tag. */
      vertextag = (vertextag << 7) + (nowatom & (char) 127);
      vertexatoms++;
      position.textindex++;

      /* Is this the last atom in the compressed tag? */
      if (nowatom < (char) 0) {
        /* Yes; we have an uncompressed tag now.  What tag is it? */
        if ((nowatom == (char) STOP) && (vertexatoms == 1)) {
          /* STOP tag.  Exit the inner loop early; go on to next molecule. */
          break;
        } else if ((nowatom == (char) GHOSTVERTEX) && (vertexatoms == 1)) {
          /* Ghost vertex. */
          vertextag = GHOSTVERTEX;
        } else {
          /* Use the ground tag to supply the high-order bits. */
          vertextag += (groundtag >> (7 * vertexatoms)) << (7 * vertexatoms);
        }

        /* Is this the vertex we're searching for? */
        if (vertextag == searchvertex) {
          /* Insert the new vertex here and return. */
          numatoms = linkringtagcompress(groundtag, newvertex, newvertexatoms);
          linkringinsertatoms(pool, &position, numatoms, newvertexatoms,
                              proxipooltag2allocindex(pool, linkring));
          return 1;
        }

        /* Prepare to read another tag. */
        vertextag = 0;
        vertexatoms = 0;
      }
    }
  } while (position.nextmoleculetag != STOP);

  if (nowatom >= (char) 0) {
    /* Oops.  The end of the last vertex is missing from the last molecule. */
    printf("Internal error in linkringinsertvertex():\n");
    printf("  Link ring not properly terminated.\n");
    internalerror();
  }

  /* `searchvertex' is not in the link ring. */
  return 0;
}

/*****************************************************************************/
/*                                                                           */
/*  linkringdeletevertex()   Delete a vertex from a link ring.               */
/*                                                                           */
/*  The main purpose of this procedure is to provide a way to swap two edges */
/*  for one.  For instance, if the link ring contains the edges 1-2 and 2-3, */
/*  and you delete the vertex 2, then the two edges are replaced by the edge */
/*  1-3.  (You can accomplish the same thing by calling linkringdeleteedge() */
/*  and linkringinsertedge(), but it's much slower.)                         */
/*                                                                           */
/*  You can also use it to delete an edge from an end of a chain.  For       */
/*  example, if the edge 1-2 is the beginning of a chain (i.e. there is a    */
/*  ghost vertex before the vertex 1), then deleting the vertex 1 has the    */
/*  effect of deleting the edge 1-2.  If the edge 1-2 was the entire chain,  */
/*  then the vertex 2 survives as a lone vertex in the link ring, with ghost */
/*  vertices on both sides of it.  If you subsequently delete the vertex 2,  */
/*  this procedure will do the right thing and delete one of the ghost       */
/*  vertices as well.                                                        */
/*                                                                           */
/*  If you're feeling sneaky, you can delete GHOSTVERTEX, thereby creating   */
/*  an edge.  For example, deleting a ghost vertex between vertices 1 and 2  */
/*  creates the edge 1-2.  However, if there's more than one ghost vertex in */
/*  the link ring, you cannot control which ghost vertex is deleted.         */
/*                                                                           */
/*  WARNING:  If there is are multiple copies of `deletevertex' in the link  */
/*  ring (be it a ghost vertex or not), you cannot control which one gets    */
/*  deleted.                                                                 */
/*                                                                           */
/*  pool:  The proxipool that the link ring was allocated from.              */
/*  linkring:  Tag for the first molecule in the link ring.                  */
/*  groundtag:  The ground tag relative to which the vertices are            */
/*    decompressed.                                                          */
/*  deletevertex:  The tag to delete.                                        */
/*                                                                           */
/*  Returns one of the following values.                                     */
/*    -1:  If the link ring is empty on entry (so it is not changed).        */
/*     0:  If `deletevertex' is not present, and the link ring is unchanged  */
/*         (but not empty).                                                  */
/*     1:  If `deletevertex' was in the link ring on entry, and thus was     */
/*         deleted; but some other vertex survives.                          */
/*     3:  If `deletevertex' was the sole (non-ghost) vertex on entry, and   */
/*         thus was deleted, leaving the link ring empty.                    */
/*                                                                           */
/*    Hence, a positive return value implies that the vertex was deleted,    */
/*    and the 2's bit signifies whether the link ring is empty on return.    */
/*                                                                           */
/*****************************************************************************/

int linkringdeletevertex(struct proxipool *pool,
                         tag linkring,
                         tag groundtag,
                         tag deletevertex)
{
  struct linkposition position;
  struct linkposition deleteposition;
  struct linkposition ghostposition;
  molecule cule;
  tag nextmoleculetag;
  tag moleculetag;
  int atomindex;
  int lasttextindex;
  char nowatom;

  /* There is no "previous" vertex we've visited yet. */
  tag firstvertextag = STOP;
  tag prevvertextag = STOP;
  tag vertextag = 0;
  long vertexcount = 0;
  int vertexatoms = 0;
  int deleteatoms = 0;
  /* Set to 1 during the first iteration of the "do" loop. */
  int firstflag = 1;

  if (linkring >= STOP) {
    /* Not a link ring. */
    return 0;
  }

  /* Start at the first molecule. */
  nextmoleculetag = linkring;

  /* Loop through the linked list of molecules. */
  do {
    /* Convert the molecule's tag to a pointer. */
    cule = (molecule) proxipooltag2object(pool, nextmoleculetag);
    moleculetag = nextmoleculetag;
    /* Read the "next molecule" tag. */
    linkringreadtag(moleculetag, cule, lasttextindex, nextmoleculetag, nowatom,
                    "Internal error in linkringdeletevertex():\n");
    if (firstflag) {
      /* Position to delete atoms from the beginning of the linked list. */
      position.cule = cule;
      position.textindex = 0;
      position.lasttextindex = lasttextindex;
      position.nextmoleculetag = nextmoleculetag;
      firstflag = 0;
    }

    atomindex = 0;
    /* Loop through the atoms, stopping at the "next molecule" tag. */
    while (atomindex <= lasttextindex) {
      nowatom = cule[atomindex];
      /* Append the atom to the vertex tag. */
      vertextag = (vertextag << 7) + (nowatom & (char) 127);
      vertexatoms++;
      atomindex++;

      /* Is this the last atom in the compressed tag? */
      if (nowatom < (char) 0) {
        /* Yes; we have an uncompressed tag now.  What tag is it? */
        if ((nowatom == (char) STOP) && (vertexatoms == 1)) {
          /* STOP tag.  Exit the inner loop early; go on to next molecule. */
          break;
        } else if ((nowatom == (char) GHOSTVERTEX) && (vertexatoms == 1)) {
          vertextag = GHOSTVERTEX;
          /* Remember this position in case we need to delete the ghost. */
          ghostposition = position;
        } else {
          /* Use the ground tag to supply the high-order bits. */
          vertextag += (groundtag >> (7 * vertexatoms)) << (7 * vertexatoms);
        }

        vertexcount++;
        if (vertexcount == 1) {
          /* This is the first vertex.  Remember it. */
          firstvertextag = vertextag;
        }

        /* Is this the vertex we're searching for? */
        if (vertextag == deletevertex) {
          /* Yes.  If there are ghost vertices immediately before and after */
          /*   this vertex, we must delete one of them as well.  If this    */
          /*   vertex is at the beginning or end of the linked list, it     */
          /*   takes a bit of effort to check for this.                     */

          if ((prevvertextag != STOP) && (prevvertextag != GHOSTVERTEX)) {
            /* No ghost vertex before.  Just delete this vertex and return. */
            linkringdeleteatoms(pool, &position, vertexatoms);
            return 1;
          }

          /* Look ahead one atom to see if a ghost vertex is next. */
          if (atomindex <= lasttextindex) {
            nowatom = cule[atomindex];
          } else if (nextmoleculetag == STOP) {
            nowatom = (char) STOP;
          } else {
            /* The next atom is in the next molecule. */
            nowatom = ((molecule)
                       proxipooltag2object(pool, nextmoleculetag))[0];
          }

          if ((nowatom != (char) GHOSTVERTEX) &&
              ((nowatom != (char) STOP) ||
               (firstvertextag != GHOSTVERTEX))) {
            /* No ghost vertex after.  Just delete this vertex and return. */
            if ((vertexcount == 1) && (nowatom == (char) STOP)) {
              /* Empty the link ring. */
              linkringrestart(pool, linkring);
              return 3;
            } else {
              linkringdeleteatoms(pool, &position, vertexatoms);
              return 1;
            }
          }

          if (prevvertextag == GHOSTVERTEX) {
            /* Ghost vertex before and after.  Delete this vertex and the */
            /*   ghost vertex preceding it.  (Note that this also handles */
            /*   the case where there are only two vertices in the link   */
            /*   ring, so the same ghost vertex is before and after.)     */
            if ((vertexcount == 2) && (nowatom == (char) STOP)) {
              /* Empty the link ring. */
              linkringrestart(pool, linkring);
              return 3;
            } else {
              linkringdeleteatoms(pool, &ghostposition, vertexatoms + 1);
              return 1;
            }
          }

          /* If execution reaches this point, there is a ghost vertex after */
          /*   the vertex we want to delete; but `deletevertex' is at the   */
          /*   beginning of the linked list, so we won't know if there's a  */
          /*   ghost vertex before until we reach the end of the list.      */
          /*   Save this vertex's position for later deletion.              */
          deleteposition = position;
          deleteatoms = vertexatoms;
        }

        /* Remember this tag during the next iteration. */
        prevvertextag = vertextag;
        /* Prepare to read another tag. */
        vertextag = 0;
        vertexatoms = 0;

        /* Store the position of the beginning of the next compressed tag,   */
        /*   in case we realize we need it once we reach the end of the tag. */
        position.cule = cule;
        position.textindex = atomindex;
        position.lasttextindex = lasttextindex;
        position.nextmoleculetag = nextmoleculetag;
      }
    }
  } while (nextmoleculetag != STOP);

  if (nowatom >= (char) 0) {
    /* Oops.  The end of the last vertex is missing from the last molecule. */
    printf("Internal error in linkringdeletevertex():\n");
    printf("  Vertex at end of link ring not properly terminated.\n");
    internalerror();
  }

  /* If execution reaches here, either `deletevertex' is absent from the */
  /*   link ring, or the linked list begins with `deletevertex' followed */
  /*   by a ghost vertex.                                                */

  if (firstvertextag == deletevertex) {
    /* The linked list begins with `deletevertex' and a ghost vertex */
    /*   immediately following it.                                   */
    if (vertexcount <= 2) {
      /* Empty the link ring. */
      linkringrestart(pool, linkring);
      return 3;
    } else if (prevvertextag == GHOSTVERTEX) {
      /* The linked list ends with a ghost vertex, which immediately      */
      /*   precedes `deletevertex' in the link ring.  Delete the ghost    */
      /*   vertex at the end.  (It's better to leave a ghost vertex at    */
      /*   the beginning, to reduce the frequency of having to go through */
      /*   the whole list just to see if there's a ghost at the end.)     */
      linkringdeleteatoms(pool, &ghostposition, 1);
      /* The deletion above may have changed the "next molecule" tag stored */
      /*   in the molecule at `deleteposition' to STOP.                     */
      if (deleteposition.cule[MOLECULESIZE - 1] == (char) STOP) {
        deleteposition.nextmoleculetag = STOP;
        deleteposition.lasttextindex = MOLECULESIZE - 2;
      }
    }
    /* Delete the target vertex. */
    linkringdeleteatoms(pool, &deleteposition, deleteatoms);
    return 1;
  }

  /* `deletevertex' is not in the link ring. */
  if (vertexcount == 0) {
    return -1;
  } else {
    return 0;
  }
}

/*****************************************************************************/
/*                                                                           */
/*  linkringdelete2vertices()   Delete a vertex and the vertex that follows  */
/*                              it from a link ring.                         */
/*                                                                           */
/*  This procedure differs from linkringdeletevertex() in that it does not   */
/*  check whether the deletion puts two ghost vertices next to each other;   */
/*  nor does it check whether the resulting link ring contains just one      */
/*  vertex (ghost or not).  It should not be used in circumstances where it  */
/*  will cause one of these problems.                                        */
/*                                                                           */
/*  The procedure's purpose is to help implement the Bowyer-Watson algorithm */
/*  for incremental insertion of a vertex into a Delaunay triangulation.     */
/*  It is used in the circumstance where the link ring contains two (or      */
/*  more) copies of the new vertex being inserted, with `deletevertex'       */
/*  sandwiched between them, which the Bowyer-Watson algorithm wants to      */
/*  delete.  Deleting just `deletevertex' would put two copies of the new    */
/*  vertex next to each other, but they should be merged into a single copy. */
/*  Therefore, this procedure deletes `deletevertex' and the vertex          */
/*  following it (which should be a copy of the new vertex) at once, leaving */
/*  behind one copy of the new vertex.  (That's why there are no checks for  */
/*  adjacent ghost vertices, nor for a one-vertex link ring.)  This          */
/*  procedure also works correctly in the case where there are only two      */
/*  vertices in the link ring, so the copies of the new vertex (before and   */
/*  after `deletevertex') are really the same copy.                          */
/*                                                                           */
/*  The procedure could be used for other purposes as well, so long as there */
/*  is no possibility of putting two ghost vertices together, or leaving     */
/*  behind just one ghost vertex and no other vertex.                        */
/*                                                                           */
/*  pool:  The proxipool that the link ring was allocated from.              */
/*  linkring:  Tag for the first molecule in the link ring.                  */
/*  groundtag:  The ground tag relative to which the vertices are            */
/*    decompressed.                                                          */
/*  deletevertex:  The tag to delete, along with its successor.              */
/*                                                                           */
/*  Returns one of the following values.                                     */
/*    -1:  If the link ring is empty on entry (so it is not changed).        */
/*     0:  If `deletevertex' is not present, and the link ring is unchanged  */
/*         (but not empty).                                                  */
/*     1:  If `deletevertex' was in the link ring on entry, and thus was     */
/*         deleted (with its successor); but some other vertex survives.     */
/*     3:  If `deletevertex' was deleted, and the link ring is now empty.    */
/*                                                                           */
/*    Hence, a positive return value implies that the vertex was deleted,    */
/*    and the 2's bit signifies whether the link ring is empty on return.    */
/*                                                                           */
/*****************************************************************************/

int linkringdelete2vertices(struct proxipool *pool,
                            tag linkring,
                            tag groundtag,
                            tag deletevertex)
{
  struct linkposition position;
  struct linkposition deleteposition;
  molecule cule;
  tag nextmoleculetag;
  tag moleculetag;
  int atomindex;
  int lasttextindex;
  char nowatom;

  /* There is no "previous" vertex we've visited yet. */
  tag prevvertextag = STOP;
  tag vertextag = 0;
  long vertexcount = 0;
  int vertexatoms = 0;
  int deleteatoms = 0;
  int firstatoms = 0;
  /* Set to 1 during the first iteration of the "do" loop. */
  int firstflag = 1;

  if (linkring >= STOP) {
    /* Not a link ring. */
    return 0;
  }

  /* Start at the first molecule. */
  nextmoleculetag = linkring;

  /* Loop through the linked list of molecules. */
  do {
    /* Convert the molecule's tag to a pointer. */
    cule = (molecule) proxipooltag2object(pool, nextmoleculetag);
    moleculetag = nextmoleculetag;
    /* Read the "next molecule" tag. */
    linkringreadtag(moleculetag, cule, lasttextindex, nextmoleculetag, nowatom,
                    "Internal error in linkringdelete2vertices():\n");
    if (firstflag) {
      /* Position to delete atoms from the beginning of the linked list. */
      position.cule = cule;
      position.textindex = 0;
      position.lasttextindex = lasttextindex;
      position.nextmoleculetag = nextmoleculetag;
      firstflag = 0;
    }

    atomindex = 0;
    /* Loop through the atoms, stopping at the "next molecule" tag. */
    while (atomindex <= lasttextindex) {
      nowatom = cule[atomindex];
      /* Append the atom to the vertex tag. */
      vertextag = (vertextag << 7) + (nowatom & (char) 127);
      vertexatoms++;
      atomindex++;

      /* Is this the last atom in the compressed tag? */
      if (nowatom < (char) 0) {
        /* Yes; we have an uncompressed tag now.  What tag is it? */
        if ((nowatom == (char) STOP) && (vertexatoms == 1)) {
          /* STOP tag.  Exit the inner loop early; go on to next molecule. */
          break;
        } else if ((nowatom == (char) GHOSTVERTEX) && (vertexatoms == 1)) {
          vertextag = GHOSTVERTEX;
        } else {
          /* Use the ground tag to supply the high-order bits. */
          vertextag += (groundtag >> (7 * vertexatoms)) << (7 * vertexatoms);
        }

        vertexcount++;
        if (vertexcount == 1) {
          /* This is the first vertex.  Remember its length. */
          firstatoms = vertexatoms;
        }

        /* Is the previous vertex the one we want to delete? */
        if (prevvertextag == deletevertex) {
          /* Yes; delete `deletevertex' and the following vertex. */
          linkringdeleteatoms(pool, &deleteposition,
                              deleteatoms + vertexatoms);
          /* Is the link ring empty now? */
          if ((vertexcount == 2) &&
              (((molecule) proxipooltag2object(pool, linkring))[0] ==
               (char) STOP)) {
            /* Yes. */
            return 3;
          } else {
            return 1;
          }
        } else if (vertextag == deletevertex) {
          /* This is the vertex we want to delete.  Remember its position. */
          deleteposition = position;
          deleteatoms = vertexatoms;
        }

        /* Remember this tag during the next iteration. */
        prevvertextag = vertextag;
        /* Prepare to read another tag. */
        vertextag = 0;
        vertexatoms = 0;

        /* Store the position of the beginning of the next compressed tag,   */
        /*   in case we realize we need it once we reach the end of the tag. */
        position.cule = cule;
        position.textindex = atomindex;
        position.lasttextindex = lasttextindex;
        position.nextmoleculetag = nextmoleculetag;
      }
    }
  } while (nextmoleculetag != STOP);

  if (nowatom >= (char) 0) {
    /* Oops.  The end of the last vertex is missing from the last molecule. */
    printf("Internal error in linkringdelete2vertices():\n");
    printf("  Vertex at end of link ring not properly terminated.\n");
    internalerror();
  }

  /* If execution reaches here, either `deletevertex' is absent from the */
  /*   link ring, or the linked list ends with `deletevertex'.           */

  if (prevvertextag == deletevertex) {
    if (vertexcount <= 2) {
      /* Empty the link ring. */
      linkringrestart(pool, linkring);
      return 3;
    } else {
      /* Delete `deletevertex' from the end first. */
      linkringdeleteatoms(pool, &deleteposition, deleteatoms);
      /* Delete one of the vertex copies from the beginning.  Note that the */
      /*   deletion above may have changed the first molecule in the list,  */
      /*   so we must re-read it.                                           */
      position.cule = (molecule) proxipooltag2object(pool, linkring);
      position.textindex = 0;
      linkringreadtag(linkring, position.cule, position.lasttextindex,
                      position.nextmoleculetag, nowatom,
                      "Internal error in linkringdelete2vertices():\n");
      linkringdeleteatoms(pool, &position, firstatoms);
      return 1;
    }
  }

  /* `deletevertex' is not in the link ring. */
  if (vertexcount == 0) {
    return -1;
  } else {
    return 0;
  }
}

/**                                                                         **/
/**                                                                         **/
/********* Link ring routines end here                               *********/


