/********* Tetrahedral complex routines begin here                   *********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  Space-efficient tetrahedral complexes (struct tetcomplex)                */
/*                                                                           */
/*  Roughly speaking, a tetrahedral complex (as it is implemented here) is   */
/*  a set of tetrahedra that follow the rules of simplicial complexes.  It   */
/*  cannot represent a three-dimensional simplicial complex with full        */
/*  generality, though, because it does not (fully) support the presence of  */
/*  edges or triangles that are not faces of any tetrahedron.                */
/*                                                                           */
/*  The tetcomplex procedures are purely topological, which means that none  */
/*  of them ever looks at the coordinates of a vertex (or even knows how     */
/*  vertices are represented).  A tetrahedron is denoted by the labels of    */
/*  its four vertices, which are integer tags (that is, the `tag' type       */
/*  defined for proxipools).  The tetcomplex procedures take and return      */
/*  vertex tags (not data structures that represent vertices).               */
/*                                                                           */
/*  Every tetrahedron has an orientation, implied by the order of its        */
/*  vertices.  A tetrahedron can be oriented two ways.  All even             */
/*  permutations of a tetrahedron's vertices are considered equivalent       */
/*  (having the same orientation), and all odd permutations of a             */
/*  tetrahedron's vertices have the opposite orientation (but are equivalent */
/*  to each other).  So (1, 2, 3, 4) = (2, 1, 4, 3) = (1, 4, 2, 3), but      */
/*  (1, 2, 3, 4) is opposite to (1, 2, 4, 3).  Typically, topological        */
/*  orientation echoes geometrical orientation according to a right-hand     */
/*  rule:  the orientation (1, 2, 3, 4) implies that the vertices 2, 3, 4    */
/*  occur in counterclockwise order as seen from vertex 1.  If you curl the  */
/*  fingers of your right hand to follow the vertices 2, 3, 4, then your     */
/*  thumb points toward vertex 1.                                            */
/*                                                                           */
/*  You could use the left-hand rule (a mirror image) instead, and this data */
/*  structure won't know the difference.  However, all the tetrahedron       */
/*  orientations must agree!  This data structure does not support complexes */
/*  in which the tetrahedron orientations are mixed.                         */
/*                                                                           */
/*  The data structure is designed to support tetrahedral complexes in       */
/*  three-dimensional space.  One rule is that each triangle may be a face   */
/*  of at most two tetrahedra.  In other words, for any triple of vertices,  */
/*  only two tetrahedra can have all three of those vertices.  (This rules   */
/*  out general tetrahedral complexes in four-dimensional space.)            */
/*                                                                           */
/*  Another rule is that if two tetrahedra share a triangular face, their    */
/*  orientations must be such that the tetrahedra are on opposite sides of   */
/*  the face.  For example, the tetrahedra (1, 2, 3, 4) and (1, 3, 2, 5) can */
/*  coexist in a tetcomplex, but (1, 2, 3, 4) and (1, 2, 3, 5) cannot.       */
/*                                                                           */
/*  The data structure supports the insertion of tetrahedra, so long as they */
/*  obey these rules.  It supports the deletion of any tetrahedra in the     */
/*  complex.  There are also procedures for performing common bistellar      */
/*  flips.                                                                   */
/*                                                                           */
/*  The main query is a "triangle query", which takes three vertices         */
/*  (representing a triangle) as input, and returns the two tetrahedra       */
/*  adjoining the triangle, represented by the fourth vertex of each         */
/*  tetrahedron.  The absence of one (or both) of these tetrahedra is        */
/*  indicated by a value of GHOSTVERTEX as the fourth vertex.  If the        */
/*  triangle itself is missing from the complex, the return value indicates  */
/*  that.                                                                    */
/*                                                                           */
/*  There are also specialized procedures designed to help the Bowyer-Watson */
/*  algorithm for incremental insertion in Delaunay triangulations run fast. */
/*                                                                           */
/*  Public interface:                                                        */
/*  struct tetcomplex   Represents a tetrahedral complex.                    */
/*  struct tetcomplexstar   Holds information about one vertex's 2D link.    */
/*  struct tetcomplexposition   Represents a position for iterating through  */
/*    the tetrahedra in a complex.                                           */
/*  tetcomplexinit(plex, vertexpool, verbose)   Initialize a tetcomplex.     */
/*  tetcomplexrestart(plex)   Erase all tetrahedra.                          */
/*  tetcomplexdeinit(plex)   Free a tetcomplex's memory to the OS.           */
/*  tetcomplextag2vertex(plex, searchtag)   Map a tag to a pointer that      */
/*    points at a vertex data structure.  Unsafe; fast macro.                */
/*  tetcomplextag2attributes(plex, searchtag)   Map a tag to a pointer at    */
/*    the attributes associated with a vertex.  Unsafe; fast macro.          */
/*  tetcomplexadjacencies(plex, vtx1, vtx2, vtx3, adjacencies[2])   Find the */
/*    tetrahedra adjoining the triangle (vtx1, vtx2, vtx3).                  */
/*  tetcomplexiteratorinit(plex, pos)   Initialize an iterator over the      */
/*    tetrahedra of the complex.                                             */
/*  tetcomplexiterate(pos, nexttet[4])   Return the next tetrahedron,        */
/*    excluding duplicates.  Certain ghosts are included.                    */
/*  tetcomplexiteratenoghosts(pos, nexttet[4])   Return the next             */
/*    tetrahedron, excluding duplicates and ghosts.                          */
/*  tetcomplexprint(plex)   Print the tetrahedra in a tetcomplex.            */
/*  tetcomplexprintstars(plex)   Print the stars in a tetcomplex.            */
/*  tetcomplexconsistency(plex)   Check internal consistency of tetcomplex.  */
/*  tetcomplexdeletetet(plex, vtx1, vtx2, vtx3, vtx4)   Delete a             */
/*    tetrahedron.                                                           */
/*  tetcomplexinserttet(plex, vtx1, vtx2, vtx3, vtx4)   Insert a             */
/*    tetrahedron.                                                           */
/*  tetcomplex14flip(plex, vtx1, vtx2, vtx3, vtx4, newvertex)   Replace one  */
/*    tetrahedron with four by lazily inserting `newvertex'.                 */
/*  tetcomplex23flip(plex, vtxtop, vtx1, vtx2, vtx3, vtxbot)   Replace two   */
/*    tetrahedra with three (bistellar flip).                                */
/*  tetcomplex32flip(plex, vtxtop, vtx1, vtx2, vtx3, vtxbot)   Replace three */
/*    tetrahedra with two (bistellar flip).  Inverse of tetcomplex23flip.    */
/*  tetcomplex41flip(plex, vtx1, vtx2, vtx3, vtx4, deletevertex)   Replace   */
/*    four tetrahedra with one.  Inverse of tetcomplex14flip.                */
/*  tetcomplexremoveghosttets(plex)   Remove all the ghost tetrahedra.       */
/*  tetcomplextetcount(plex)   Return number of tetrahedra (no ghosts).      */
/*  tetcomplexghosttetcount(plex)   Return number of explicity inserted      */
/*    ghost tetrahedra.                                                      */
/*  tetcomplexbytes(plex)   Return bytes used by the complex.                */
/*                                                                           */
/*  For algorithms that use inconsistent stars (e.g. star splaying):         */
/*  tetcomplexringadjacencies(plex, vtx1, vtx2, vtx3, adjacencies[2])        */
/*    Search the star of (vtx1, vtx2) to try to find the tetrahedra          */
/*    adjoining the triangle (vtx1, vtx2, vtx3).                             */
/*  tetcomplexiterateall(pos, nexttet[4])   Return the next tetrahedron,     */
/*    with duplicate copies included.  Certain ghosts are included.          */
/*  tetcomplexdeleteorderedtet(plex, vtx1, vtx2, vtx3, vtx4)   Delete an     */
/*    ordered tetrahedron.                                                   */
/*  tetcomplexinsertorderedtet(plex, vtx1, vtx2, vtx3, vtx4)   Insert an     */
/*    ordered tetrahedron.                                                   */
/*  tetcomplex12fliponedge(plex, vtx1, vtx2, vtx3, vtx4, newvertex)          */
/*    Replace (vtx1, vtx2, vtx3, vtx4) with (vtx1, vtx2, vtx3, newvertex)    */
/*    and (vtx1, vtx2, newvertex, vtx4) in the star of (vtx1, vtx2).         */
/*  tetcomplex21fliponedge(plex, vtx1, vtx2, vtx3)   Replace (vtx1,          */
/*    vtx2, ?, vtx3) and (vtx1, vtx2, vtx3, ??) with (vtx1, vtx2, ?, ??) in  */
/*    the star of (vtx1, vtx2).                                              */
/*                                                                           */
/*  Specialized for the Bowyer-Watson algorithm:                             */
/*  tetcomplexinserttetontripod(plex, vtx1, vtx2, vtx3, vtx4)   Insert a     */
/*    tetrahedron into the stars of 3 edges that meet at `vtx1'.             */
/*  tetcomplexdeletetriangle(plex, vtx1, vtx2, vtx3)   Delete a triangle,    */
/*    gluing two tetrahedra into a polyhedron.                               */
/*  tetcomplexsqueezetriangle(plex, vtx1, vtx2, vtx3)   Delete a triangle    */
/*    that has the new vertex on both sides of it.                           */
/*  tetcomplexbuild3dstar(plex, newvertex, trianglelist, firstindex,         */
/*    trianglecount)   Convert a 2D link triangulation made up of `struct    */
/*    tetcomplexlinktriangle' triangles to the native data structure.        */
/*                                                                           */
/*  For internal use only:                                                   */
/*  tetcomplexlookup3dstar(plex, vtx)                                        */
/*  tetcomplexmissingtet(plex, vtx1, vtx2, vtx3, vtx4)                       */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/*  Ghost tetrahedra                                                         */
/*                                                                           */
/*  A ghost tetrahedron is a tetrahedron that has the GHOSTVERTEX for one of */
/*  its vertices.  A solid tetrahedron is a tetrahedron whose vertices are   */
/*  all not GHOSTVERTEX.  Except in one special case, tetrahedra with two    */
/*  ghost vertices are prohibited.                                           */
/*                                                                           */
/*  Ghost tetrahedra are implicitly returned by the face adjacency query.    */
/*  For example, if you query the identity of the two tetrahedra adjoining   */
/*  the triangle (1, 2, 3), and the tetrahedron (1, 2, 3, 4) is the sole     */
/*  tetrahedron in the entire complex, the query will return GHOSTVERTEX and */
/*  4, indicating that the triangle adjoins the tetrahedra (1, 2, 3, 4) and  */
/*  (1, 3, 2, GHOSTVERTEX).  However, this does not mean that the latter     */
/*  tetrahedron was ever inserted into the tetcomplex.                       */
/*                                                                           */
/*  However, it is permissible to insert ghost tetrahedra into a tetcomplex, */
/*  and they will (usually) be stored therein, if they obey the same rules   */
/*  that solid tetrahedra obey about sharing faces.  Why would you want to   */
/*  do this?  Because they make it easier to navigate along the boundary of  */
/*  a complex.  This takes a bit of explanation...                           */
/*                                                                           */
/*  A ghost triangle is a triangle that has the GHOSTVERTEX for one of its   */
/*  vertices.  Suppose you know that the edge (1, 2) is on the boundary of   */
/*  the mesh, and you want to find the identity of the boundary triangles    */
/*  that have it for an edge.  Suppose these triangles are (1, 2, 3) and     */
/*  (2, 1, 4).  Because they are boundary triangles, you might think that    */
/*  you could query the identity of the tetrahedra that adjoin the ghost     */
/*  triangle (1, 2, GHOSTVERTEX).  Ideally, the query would return the       */
/*  tetrahedra (1, 2, GHOSTVERTEX, 3) and (1, GHOSTVERTEX, 2, 4), thereby    */
/*  answering the question of which boundary triangles adjoin (1, 2).        */
/*                                                                           */
/*  One of the disadvantages of the space-efficient data structure is that   */
/*  this query doesn't always find the desired ghost tetrahedra.  (Sometimes */
/*  it does; sometimes it doesn't.)  However, if the ghost tetrahedra are    */
/*  _explicitly_ inserted into the tetcomplex (at the cost of taking up      */
/*  extra memory), adjacency queries on ghost triangles are answered         */
/*  reliably.  For this to work, you must insert one ghost tetrahedron for   */
/*  every triangular face on the boundary of the tetrahedralization.  This   */
/*  makes it possible, for example, to efficiently "walk" along the boundary */
/*  of the tetrahedralization.                                               */
/*                                                                           */
/*  There are some oddities to explicitly stored ghost tetrahedra, though.   */
/*  First, they are independent of the solid tetrahedra stored in the        */
/*  tetcomplex.  The tetcomplex won't notice if the solid tetrahedra and     */
/*  ghost tetrahedra are completely inconsistent with each other.  It's the  */
/*  caller's responsibility to keep them consistent, because the tetcomplex  */
/*  doesn't check.                                                           */
/*                                                                           */
/*  All adjacency queries on solid triangles are answered using the solid    */
/*  tetrahedra; stored ghost tetrahedra have no influence on these queries.  */
/*  Adjacency queries on ghost triangles are answered using a combination    */
/*  of ghost and solid tetrahedra.  If the ghost tetrahedra are not          */
/*  consistent with the solid tetrahedra, these queries may be answered      */
/*  inconsistently.                                                          */
/*                                                                           */
/*  Second, imagine that two solid tetrahedra (1, 2, 3, 4) and (1, 2, 5, 6)  */
/*  share a boundary edges (1, 2), but no other tetrahedra do.  So there are */
/*  actually four boundary triangles sharing the edge.  (The boundary of the */
/*  tetrahedral complex is nonmanifold.)  If you try to explicitly store a   */
/*  ghost tetrahedron for each of these boundary faces, then you have four   */
/*  ghost tetrahedra sharing the face (1, 2, GHOSTVERTEX).  This is not      */
/*  allowed; only two ghost tetrahedra may share a ghost triangle.  So       */
/*  explicitly stored ghost tetrahedra only really make sense for a          */
/*  tetrahedralization covering a (nondegenerate) convex polyhedron.  They   */
/*  don't make sense for a general-purpose mesh.                             */
/*                                                                           */
/*  Third, it isn't really possible to determine whether a tetcomplex        */
/*  contains a particular ghost tetrahedron or not.  For example, if a solid */
/*  tetrahedron (1, 2, 3, 4) is present, then queries on the faces (1, 2, 3) */
/*  or (1, 2, GHOSTVERTEX) might both indicate the presence of a tetrahedron */
/*  (1, 3, 2, GHOSTVERTEX), but that doesn't tell you whether that           */
/*  tetrahedron is explicitly stored or not.                                 */
/*                                                                           */
/*  Fourth, sometimes the insertion (or deletion) of a ghost tetrahedron     */
/*  doesn't actually change the data structure at all--except for the count  */
/*  of ghost tetrahedra.  This is because of the unusual nature of the       */
/*  space-efficient tetrahedral complex data structure:  it might not store  */
/*  the link of any of the tetrahedron's edges adjoining the GHOSTVERTEX.    */
/*  With these tetrahedra, an application might be able to corrupt the count */
/*  of ghost tetrahedra by inserting (or deleting) the same ghost            */
/*  tetrahedron twice.                                                       */
/*                                                                           */
/*  Despite all these quirks, explicitly stored ghost tetrahedra can be very */
/*  useful.  My implementation of the Bowyer-Watson algorithm for            */
/*  incremental vertex insertion in Delaunay triangulations uses them        */
/*  because it simplifies the algorithm in several ways:  by making it       */
/*  possible to treat inserting a vertex outside the convex hull much the    */
/*  same as inserting a vertex inside the tetrahedralization, and by making  */
/*  it easy to walk along the boundary of the tetrahedralization (which is   */
/*  important when a vertex is inserted outside the convex hull).            */
/*  Nevertheless, it's simpler to use the tetcomplex data structure if you   */
/*  don't explicitly store ghost tetrahedra.                                 */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/*  The memory-efficient data structure                                      */
/*  (Blandford, Blelloch, Cardoze, and Kadow)                                */
/*                                                                           */
/*  The tetcomplex structure represents a tetrahedral complex as a set of    */
/*  _stars_.  The star of a vertex is the set of simplices that adjoin the   */
/*  vertex.  The star of an edge is the set of simplices that have the edge  */
/*  for a face (i.e. simplices that share both of the edge's vertices).      */
/*                                                                           */
/*  The _link_ of a vertex is the set of simplices that don't adjoin the     */
/*  vertex, but are faces of simplices in the vertex's star.  For example,   */
/*  if vertex 1 adjoins just two tetrahedra, (1, 2, 3, 4) and (1, 3, 2, 5),  */
/*  then its link is the set { (2), (3), (4), (5), (2, 3), (2, 4), (3, 4),   */
/*  (2, 5), (3, 5), (2, 3, 4), (3, 2, 5) }.  Because the tetcomplex data     */
/*  structure does not support edges or triangles that aren't faces of       */
/*  tetrahedra, you can specify the link of a vertex simply by specifying    */
/*  the triangles--in this case, (2, 3, 4) and (3, 2, 5).                    */
/*                                                                           */
/*  The link of an edge is the set of simplices that don't adjoin either of  */
/*  the edge's vertices, but are faces of simplices in the edge's star.  In  */
/*  the previous example, the link of (1, 2) is { (3), (4), (5), (3, 4),     */
/*  (3, 5) }.  You can specify the link of an edge simply by specifying the  */
/*  edges--in this case, (3, 4) and (3, 5).  The link of an edge is either a */
/*  ring of edges, or a (possibly empty) sequence of chains of edges.        */
/*                                                                           */
/*  A data structure that represents the link of a vertex implicitly         */
/*  represents the star of the vertex as well, and vice versa.  (The star is */
/*  three-dimensional, because it includes tetrahedra, and the link is two-  */
/*  dimensional, because its highest-dimensional simplices are triangles.)   */
/*  Likewise, an edge's link implies its star, and vice versa.  (The star is */
/*  three-dimensional, but the link is only one-dimensional, because its     */
/*  highest-dimensional simplices are edges.)                                */
/*                                                                           */
/*  The tetcomplex data structure uses a `tetcomplexstar' to represent the   */
/*  star of a vertex.  The tetcomplexstar references (using a tag) a link2d  */
/*  data structure, which simultaneously represents the 2D link and the 3D   */
/*  star.  Each link2d contains a list of edges and their stars, represented */
/*  by the linkring data structure, which simultaneously represents the 1D   */
/*  link and the 3D star.  (Both the link2d and linkring data structures are */
/*  described in separate headers like this one.)  The link2d and linkring   */
/*  data structures store tags in a compressed form, helping to make the     */
/*  tetcomplex data structure occupy less space.                             */
/*                                                                           */
/*  However, that's not the only trick Blandford et al. use to save space.   */
/*  At first sight, the star representation of a tetrahedral complex might   */
/*  seem to store a lot of redundant information, because each tetrahedron   */
/*  is in the star of four different vertices; and within one vertex's star, */
/*  each tetrahedron is in the star of three different edges.  So is each    */
/*  tetrahedron stored twelve times?                                         */
/*                                                                           */
/*  No.  On average, each tetrahedron is stored just three times, because    */
/*  of the following two optimizations of Blandford et al.                   */
/*                                                                           */
/*  (1)  If a tetcomplex contains an edge (a, b), the link ring for that     */
/*       edge is stored only if a and b have the same parity--in other       */
/*       words, if both are odd or both are even.  (If a and b have opposite */
/*       parity, the edge's link must be deduced by looking at other edges'  */
/*       links.)  This cuts the memory occupied by link rings in half.       */
/*       Note that the GHOSTVERTEX has odd parity.                           */
/*                                                                           */
/*  (2)  If a tetcomplex contains an edge (a, b), where a and b have the     */
/*       same parity, the link ring for edge (a, b) is stored in one         */
/*       vertex's star--either the star for a or the star for b, but not     */
/*       both.  If one of the vertices is the GHOSTVERTEX, then the edge's   */
/*       link ring is stored in the other vertex's star.  (The GHOSTVERTEX   */
/*       does not get a star.)  Otherwise, if both vertices have the same    */
/*       two's bit, the edge's link ring is stored in the star of the vertex */
/*       with the greater index.  If the vertices have opposite two's bits,  */
/*       the edge's link is stored with the vertex with the lesser index.    */
/*       This cuts the memory occupied by link rings in half.                */
/*                                                                           */
/*       Why such a complicated rule?  I could have simply stored every      */
/*       edge's link with the vertex with the lesser index (which is what    */
/*       Blandford et al. do).  But then vertices with small indices would   */
/*       tend to have a lot of link rings stored in their stars, and         */
/*       vertices with large indices would tend to have few or none.  But    */
/*       the overall amount of time a program spends searching through a     */
/*       vertex's star is probably quadratic in the number of link rings it  */
/*       stores, so it's better to distribute link rings among vertices in a */
/*       pseudorandom way.  Moreover, fewer vertices will have empty stars,  */
/*       so the operation of finding some tetrahedron that adjoins a vertex  */
/*       is usually faster.                                                  */
/*                                                                           */
/*  Even with these optimizations, the tetcomplex data structure supports    */
/*  fast triangle queries.  Suppose you query the identity of the two        */
/*  tetrahedra adjoining the triangle (a, b, c).  At least two of those      */
/*  three vertices must have the same parity.  Suppose a and c have the same */
/*  parity.  The query is answered by a search through the link ring of the  */
/*  edge (a, c), which is stored either in a's star or in c's star.          */
/*                                                                           */
/*  The disadvantage of these optimizations is that it is not always         */
/*  efficient to answer the queries "find a tetrahedron that adjoins vertex  */
/*  a" or "find a tetrahedron that has the edge (a, b)".  In the worst case, */
/*  these queries might require an exhaustive search through the entire      */
/*  tetcomplex.  Therefore, these queries should be used as little as        */
/*  possible.  Fortunately, incremental Delaunay triangulation and many      */
/*  other algorithms can be implemented without these queries.               */
/*                                                                           */
/*  Ghost tetrahedra inserted into a tetcomplex are stored in the link rings */
/*  of ghost edges.  Inserting or deleting a ghost tetrahedron does not      */
/*  modify any solid edges' link rings.  However, triangle queries on ghost  */
/*  triangles sometimes use ghost edges' link rings, and sometimes solid     */
/*  edges' link rings, to answer those queries.  For example, a query on the */
/*  triangle (2, 4, GHOSTVERTEX) will consult the edge (2, 4), because the   */
/*  GHOSTVERTEX has odd parity, and no link ring is stored for               */
/*  (2, GHOSTVERTEX) or (4, GHOSTVERTEX).  That's why these queries may be   */
/*  answered incorrectly if the ghost tetrahedra are not consistent with the */
/*  solid tetrahedra.                                                        */
/*                                                                           */
/*  Solid tetrahedra inserted into a tetcomplex are stored in the link rings */
/*  of solid edges.  Inserting or deleting a solid tetrahedron does not      */
/*  modify any ghost edges' link rings.  Triangle queries on solid triangles */
/*  never consult ghost edges' link rings.                                   */
/*                                                                           */
/*****************************************************************************/


/*  A tetcomplex is a tetrahedral complex data structure.  It is composed of */
/*  a set of 3D stars, one for each vertex.  These stars have type `struct   */
/*  tetcomplexstar', and are stored (referenced by vertex tag) through the   */
/*  array `stars'.  The molecules that the stars are composed of are stored  */
/*  in the memory pool `moleculepool'.  All of the stars in `stars' having   */
/*  index less than `nextinitindex' have been initialized, and all the stars */
/*  having index `nextinitindex' or greater have not.                        */
/*                                                                           */
/*  Each tetcomplex has a pointer `vertexpool' to the memory pool in which   */
/*  the complex's vertices are stored.  One set of vertices can be shared by */
/*  any number of tetcomplexes.  Recall that no tetcomplex procedure ever    */
/*  looks at the coordinates of a vertex.  So what is `vertexpool' for?      */
/*  First, it is used to look up allocation indices; the molecules that make */
/*  up a vertex's star are allocated using the same allocation index that    */
/*  was used to allocate the vertex itself.  Second, the procedures          */
/*  `tetcomplextag2vertex' and `tetcomplextag2attributes' are provided to    */
/*  clients, so that clients can look up vertices without having to remember */
/*  which vertices go with which tetcomplex (and perhaps getting it wrong!). */
/*                                                                           */
/*  `tetcount' is the number of solid tetrahedra in the complex, and         */
/*  `ghosttetcount' is the number of ghost tetrahedra.  These counts are not */
/*  guaranteed to be preserved correctly by all operations.  In particular,  */
/*  if the stars become mutually inconsistent during star splaying, these    */
/*  counts will become wrong.  (When the stars are inconsistent with each    */
/*  other, you can't meaningfully say how many tetrahedra there are.)  Also, */
/*  sometimes inserting or removing a ghost tetrahedron doesn't change the   */
/*  data structure at all (except the count of ghost tetrahedra), so         */
/*  `ghosttetcount' is maintained primarily by counting the tetrahedra       */
/*  inserted or removed by the calling application, and can be fooled if the */
/*  application adds or removes a ghost tetrahedron whose absence or         */
/*  presence the tetcomplex cannot verify.                                   */
/*                                                                           */
/*  `consistentflag' indicates whether the stars are (believed to be)        */
/*  mutually consistent.                                                     */
/*                                                                           */
/*  The number `verbosity' indicates how much debugging information to       */
/*  print, from none (0) to lots (4+).                                       */

struct tetcomplex {
  struct proxipool moleculepool;     /* Pool of molecules storing the links. */
  struct arraypool stars;    /* `tetcomplexstar' array addressing the links. */
  struct proxipool *vertexpool;                    /* The array of vertices. */
  link2dcache cache;                  /* Cache for fast lookups in 2D links. */
  tag nextinitindex;      /* First uninitialized index in the `stars' array. */
  arraypoolulong tetcount;           /* Number of tetrahedra in the complex. */
  arraypoolulong ghosttetcount;          /* Ghost tetrahedra in the complex. */
  int consistentflag;           /* Are the stars consistent with each other? */
  int verbosity;                /* Amount of debugging information to print. */
};

/*  A tetcomplexstar addresses a vertex's 3D star, represented by a 2D link, */
/*  stored as a link2d data structure.  `linkhead' and `linktail' are tags   */
/*  referencing the first and last molecules, respectively, of the link2d    */
/*  linked list.  The tail is stored to make `link2dinsertvertex' fast.      */

struct tetcomplexstar {
  tag linkhead;                 /* First molecule of the link2d linked list. */
  tag linktail;                  /* Last molecule of the link2d linked list. */
};

/*  A tetcomplexposition represents a position in a tetrahedral complex, and */
/*  is used to iterate over all the tetrahedra in the complex.               */

struct tetcomplexposition {
  struct tetcomplex *mycomplex;          /* The complex this position is in. */
  tag starvertex;     /* The vertex whose star is currently being traversed. */
  struct link2dposition link2dposition;            /* That vertex's 2D link. */
  tag link2dvertex;                  /* Vertex in 2D link, denoting an edge. */
  struct linkposition link1dposition;           /* That edge's 1D link ring. */
  tag link1dfirstvertex;    /* First vertex in ring (needed at end of ring). */
  tag link1dprevvertex;                 /* Vertex visited on last iteration. */
};

/*  The tetcomplexlinktriangle struct is part of the interface of            */
/*  tetcomplexbuild3dstar().  The procedure takes as input a complete 2D     */
/*  link made up of tetcomplexlinktriangles linked together, and converts    */
/*  them into link rings and a 2D link.                                      */

struct tetcomplexlinktriangle {
  tag vtx[3];
  arraypoolulong neighbor[3];
};


/*****************************************************************************/
/*                                                                           */
/*  tetcomplexinit()   Initialize an empty tetrahedral complex.              */
/*                                                                           */
/*  Before a tetcomplex may be used, it must be initialized by this          */
/*  procedure.                                                               */
/*                                                                           */
/*  The optional `vertexpool' parameter has two purposes.  First, it allows  */
/*  the tetcomplex to look up the allocation indices of the vertices, and    */
/*  use the same allocation indices to allocate the data structures used to  */
/*  store the vertices' links.  Thus, if vertices that are geometrically     */
/*  close to each other tend to have the same allocation indices, then the   */
/*  same will be true for the link data structures.  Second, it enables the  */
/*  fast macros tetcomplextag2vertex() and tetcomplextag2attributes(), which */
/*  directly map a vertex tag to a vertex data structure.  These save the    */
/*  programmer from having to remember which vertexpool goes with the        */
/*  tetcomplex.                                                              */
/*                                                                           */
/*  If you are not storing vertex data structures in a proxipool (or if you  */
/*  don't have vertex data structures at all), set `vertexpool' to NULL.     */
/*  This will cause all link data structures to be allocated from a common   */
/*  pool, thereby sacrificing some of the memory locality and compression    */
/*  of the data structure.  It will also disable the macros (as they will    */
/*  dereference a NULL pointer).                                             */
/*                                                                           */
/*  Don't call this procedure on a tetcomplex that has already been          */
/*  initialized (by this procedure), unless you call tetcomplexdeinit() on   */
/*  it first.  You will leak memory.  Also see tetcomplexrestart().          */
/*                                                                           */
/*  plex:  The tetcomplex to initialize.                                     */
/*  vertexpool:  The proxipool of vertices associated with this tetcomplex.  */
/*    The tags stored in the tetcomplex reference these vertices.  This      */
/*    parameter may be set to NULL, but if it is, you cannot subsequently    */
/*    use tetcomplextag2vertex() or tetcomplextag2attributes().              */
/*  verbose:  How much debugging information tetcomplex procedures should    */
/*    print, from none (0) to lots (4+).                                     */
/*                                                                           */
/*****************************************************************************/

void tetcomplexinit(struct tetcomplex *plex,
                    struct proxipool *vertexpool,
                    int verbose)
{
  /* Initialize a pool to store the molecules in the 1D and 2D links. */
  proxipoolinit(&plex->moleculepool, MOLECULESIZE, 0, verbose);
  /* Initialize a cache for fast 2D link lookups. */
  link2dcacheinit(plex->cache);
  /* Initialize a pool to store the 3D stars. */
  arraypoolinit(&plex->stars, sizeof(struct tetcomplexstar), 10, 0);
  /* Remember the vertices in the tetcomplex. */
  plex->vertexpool = vertexpool;
  /* No tag has a star yet. */
  plex->nextinitindex = 0;
  /* No tetrahedra yet. */
  plex->tetcount = 0;
  plex->ghosttetcount = 0;
  plex->consistentflag = 1;
  plex->verbosity = verbose;
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexrestart()   Empty a tetrahedral complex.                       */
/*                                                                           */
/*  The complex is returned to its starting state, except that no memory is  */
/*  freed to the operating system.  Rather, the previously allocated         */
/*  proxipool (of molecules) and arraypool (of stars) are ready to be        */
/*  reused.                                                                  */
/*                                                                           */
/*  plex:  The tetcomplex to restart.                                        */
/*                                                                           */
/*****************************************************************************/

void tetcomplexrestart(struct tetcomplex *plex)
{
  /* Restart the memory pools. */
  proxipoolrestart(&plex->moleculepool);
  arraypoolrestart(&plex->stars);
  /* Empty the cache for fast 2D link lookups. */
  link2dcacheinit(plex->cache);
  /* No tag has a star yet. */
  plex->nextinitindex = 0;
  /* No tetrahedra yet. */
  plex->tetcount = 0;
  plex->ghosttetcount = 0;
  plex->consistentflag = 1;
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexdeinit()   Free to the operating system all memory taken by a  */
/*                       tetcomplex.                                         */
/*                                                                           */
/*  plex:  The tetcomplex to free.                                           */
/*                                                                           */
/*****************************************************************************/

void tetcomplexdeinit(struct tetcomplex *plex)
{
  proxipooldeinit(&plex->moleculepool);
  arraypooldeinit(&plex->stars);
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplextag2vertex()   Map a tag to the vertex it indexes.             */
/*                                                                           */
/*  This procedure is much like proxipooltag2object(), but it saves callers  */
/*  from needing to remember which proxipool holds the vertices associated   */
/*  with a tetrahedral complex (and possibly getting it wrong).              */
/*                                                                           */
/*  WARNING:  fails catastrophically, probably with a segmentation fault, if */
/*  the tag's minipool doesn't yet exist.                                    */
/*                                                                           */
/*  Implemented as a macro to meet your need for speed.                      */
/*                                                                           */
/*  plex:  The tetcomplex in which to find a vertex.                         */
/*         Type (struct tetcomplex *).                                       */
/*  searchtag:  The tag whose vertex is sought.                              */
/*              Type (tag).                                                  */
/*                                                                           */
/*  Returns a pointer to the vertex.  Might crash if the tag has not been    */
/*    allocated yet.                                                         */
/*    Type (void *).                                                         */
/*                                                                           */
/*****************************************************************************/

#define tetcomplextag2vertex(plex, searchtag)  \
  proxipooltag2object(plex->vertexpool, searchtag)

/*****************************************************************************/
/*                                                                           */
/*  tetcomplextag2attributes()   Map a tag to the vertex attributes (stored  */
/*                               in a supplementary object) it indexes.      */
/*                                                                           */
/*  This procedure is much like proxipooltag2object2(), but it saves callers */
/*  from needing to remember which proxipool holds the vertices associated   */
/*  with a tetrahedral complex (and possibly getting it wrong).              */
/*                                                                           */
/*  WARNING:  fails catastrophically, probably with a segmentation fault, if */
/*  the tag's minipool doesn't yet exist.                                    */
/*                                                                           */
/*  Implemented as a macro to meet your need for speed.                      */
/*                                                                           */
/*  plex:  The tetcomplex in which to find a vertex's attributes.            */
/*         Type (struct tetcomplex *).                                       */
/*  searchtag:  The tag whose attributes (supplementary object) are sought.  */
/*              Type (tag).                                                  */
/*                                                                           */
/*  Returns a pointer to the vertex attributes.  Might crash if the tag has  */
/*    not been allocated yet.                                                */
/*    Type (void *).                                                         */
/*                                                                           */
/*****************************************************************************/

#define tetcomplextag2attributes(plex, searchtag)  \
  proxipooltag2object2(plex->vertexpool, searchtag)

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexringadjacencies()   Search the link of a specified edge for    */
/*                                the apex vertices of the two tetrahedra    */
/*                                having a specified triangle for a face.    */
/*                                                                           */
/*  This procedure is a helper procedure for tetcomplexadjacencies().  It    */
/*  searches for the tetrahedra adjoining triangle (vtx1, vtx2, vtx3), but   */
/*  it only looks in the star of the (undirected) edge (vtx1, vtx2), which   */
/*  is appropriate if `vtx1' and `vtx2' have the same parity.                */
/*                                                                           */
/*  Do not use this procedure if the stars of `vtx1' and `vtx2' might be     */
/*  inconsistent with each other and disagree about the star/link of the     */
/*  edge (vtx1, vtx2).                                                       */
/*                                                                           */
/*  plex:  The tetcomplex that contains the triangle.                        */
/*  vtx1, vtx2, vtx3:  The vertices of the query triangle.  (vtx1, vtx2) is  */
/*    the (only) edge whose link ring will be searched.                      */
/*  adjacencies[2]:  Array in which the procedure returns the tags of the    */
/*    apex vertices of the two tetrahedra adjoining the query triangle.      */
/*    The tetrahedra have orientations (vtx1, vtx2, vtx3, adjacencies[1])    */
/*    and (vtx1, vtx2, adjacencies[0], vtx3).  One or both elements of this  */
/*    arrary may be GHOSTVERTEX, if there are no such adjoining tetrahedra.  */
/*    The contents of this array do not need to be initialized prior to      */
/*    calling this procedure.                                                */
/*                                                                           */
/*  Returns 1 if the triangle is in the tetcomplex; 0 otherwise.             */
/*                                                                           */
/*****************************************************************************/

int tetcomplexringadjacencies(struct tetcomplex *plex,
                              tag vtx1,
                              tag vtx2,
                              tag vtx3,
                              tag adjacencies[2])
{
  struct tetcomplexstar *star;
  tag ring;
  tag swap;

  /* The link ring for the edge (vtx1, vtx2) is stored in the star for one   */
  /*   of those two vertices.  If one of the vertices is GHOSTVERTEX, it is  */
  /*   stored with the other vertex (GHOSTVERTEX never stores its star).     */
  /*   Otherwise, if both vertices have the same two's bit, the edge's link  */
  /*   ring is stored in the star of the vertex with the greater index.  If  */
  /*   the vertices have opposite two's bits, the edge's link is stored with */
  /*   the vertex with the lesser index.  See the tetcomplex header for an   */
  /*   explanation.                                                          */
  if ((vtx2 == GHOSTVERTEX) ||
      ((vtx1 != GHOSTVERTEX) &&
       ((vtx1 < vtx2) ^ ((vtx1 & 2) == (vtx2 & 2))))) {
    /* Does vertex `vtx1' even have a star yet? */
    if (vtx1 >= plex->nextinitindex) {
      /* No. */
      adjacencies[0] = STOP;
      adjacencies[1] = STOP;
      return 0;
    }
    /* Look up the star of `vtx1'. */
    star = (struct tetcomplexstar *)
           arraypoolfastlookup(&plex->stars, (arraypoolulong) vtx1);
    /* Search it for the link ring of (vtx1, vtx2). */
    ring = link2dfindring(&plex->moleculepool, plex->cache, star->linkhead,
                          vtx1, vtx2);
    /* Find the neighbors of `vtx3' in the link ring.  Note that if the edge */
    /*   (vtx1, vtx2) is not present, `ring' is STOP, but the following line */
    /*   will do the right thing (return 0) anyway.                          */
    return linkringadjacencies(&plex->moleculepool, ring, vtx1, vtx3,
                               adjacencies);
  } else {
    /* Does vertex `vtx2' even have a star yet? */
    if (vtx2 >= plex->nextinitindex) {
      /* No. */
      adjacencies[0] = STOP;
      adjacencies[1] = STOP;
      return 0;
    }
    /* Look up the star of `vtx2'. */
    star = (struct tetcomplexstar *)
           arraypoolfastlookup(&plex->stars, (arraypoolulong) vtx2);
    /* Search it for the link ring of (vtx2, vtx1). */
    ring = link2dfindring(&plex->moleculepool, plex->cache, star->linkhead,
                          vtx2, vtx1);
    /* Find the neighbors of `vtx3' in the link ring. */
    if (linkringadjacencies(&plex->moleculepool, ring, vtx2, vtx3,
                            adjacencies)) {
      /* The orientation of the edge and the link ring is opposite to the */
      /*   orientation of the query, so swap the tetrahedron apices to    */
      /*   compensate.                                                    */
      swap = adjacencies[0];
      adjacencies[0] = adjacencies[1];
      adjacencies[1] = swap;
      return 1;
    }
    return 0;
  }
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexadjacencies()   Find the apex vertices of the two tetrahedra   */
/*                            having a specified triangle for a face.  Known */
/*                            as a "triangle query" for short.               */
/*                                                                           */
/*  This procedure only works correctly if all the vertex and edge stars     */
/*  (i.e. link2d and linkring data structures) in the tetcomplex are         */
/*  consistent with each other.                                              */
/*                                                                           */
/*  plex:  The tetcomplex that contains the triangle.                        */
/*  vtx1, vtx2, vtx3:  The vertices of the query triangle.                   */
/*  adjacencies[2]:  Array in which the procedure returns the tags of the    */
/*    apex vertices of the two tetrahedra adjoining the query triangle.      */
/*    The tetrahedra have orientations (vtx1, vtx2, vtx3, adjacencies[1])    */
/*    and (vtx1, vtx2, adjacencies[0], vtx3).  One or both elements of this  */
/*    arrary may be GHOSTVERTEX, if there are no such adjoining tetrahedra.  */
/*    The contents of this array do not need to be initialized prior to      */
/*    calling this procedure.                                                */
/*                                                                           */
/*  Returns 1 if the triangle is in the tetcomplex; 0 otherwise.             */
/*                                                                           */
/*****************************************************************************/

int tetcomplexadjacencies(struct tetcomplex *plex,
                          tag vtx1,
                          tag vtx2,
                          tag vtx3,
                          tag adjacencies[2])
{
  /* Reject a query triangle with two identical vertices. */
  if ((vtx1 == vtx2) || (vtx2 == vtx3) || (vtx3 == vtx1)) {
    return 0;
  }

  /* Do `vtx1' and `vtx2' have the same parity? */
  if ((vtx1 & 1) == (vtx2 & 1)) {
    /* Yes; check the link ring of edge (vtx1, vtx2). */
    return tetcomplexringadjacencies(plex, vtx1, vtx2, vtx3, adjacencies);
  } else if ((vtx2 & 1) == (vtx3 & 1)) {
    /* Check the link ring of edge (vtx2, vtx3). */
    return tetcomplexringadjacencies(plex, vtx2, vtx3, vtx1, adjacencies);
  } else {
    /* `vtx3' and `vtx1' must have the same parity. */
    return tetcomplexringadjacencies(plex, vtx3, vtx1, vtx2, adjacencies);
  }
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexiteratorinit()   Initialize an iterator that traverses all the */
/*                             tetrahedra in a tetcomplex one by one.        */
/*                                                                           */
/*  The iterator is a variable `pos', whose internals should not be          */
/*  examined or modified by the client.  The iterator's job is to keep       */
/*  track of where it is in the lists of tetrahedra.  This procedure sets    */
/*  the iterator so that the first call to tetcomplexiterateall() will find  */
/*  the first tetrahedron in the first link ring in the first nonempty       */
/*  link2d.                                                                  */
/*                                                                           */
/*  When a tetcomplex is modified, any iterators on that tetcomplex may be   */
/*  corrupted and should not be used without being initialized (by this      */
/*  procedure) again.                                                        */
/*                                                                           */
/*  plex:  The tetcomplex whose tetrahedra you want to iterate over.         */
/*  pos:  The iterator.                                                      */
/*                                                                           */
/*****************************************************************************/

void tetcomplexiteratorinit(struct tetcomplex *plex,
                            struct tetcomplexposition *pos)
{
  pos->mycomplex = plex;
  /* Start with the star of the vertex having tag zero. */
  pos->starvertex = 0;
  /* The iterator is not in the middle of a link. */
  pos->link2dvertex = STOP;
  pos->link1dprevvertex = STOP;
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexiterateall()   Advance a tetcomplexposition iterator to the    */
/*                           next tetrahedron and return it (with            */
/*                           duplicates).                                    */
/*                                                                           */
/*  This procedure returns every _copy_ of every tetrahedron in the data     */
/*  structure, so it exposes the underlying implementation, and the fact     */
/*  that each solid tetrahedron is stored in the link rings of two, three,   */
/*  or six edges (depending on the parities of the tetrahedron's vertices).  */
/*  In most cases, it's better to use tetcomplexiterate() or                 */
/*  tetcomplexiteratenoghosts() instead of this procedure, because they      */
/*  return each tetrahedron only once.  (They call this procedure and filter */
/*  out duplicate tetrahedra.)                                               */
/*                                                                           */
/*  However, when the stars are not consistent with each other (as in the    */
/*  star splaying algorithm), this procedure can be useful because it        */
/*  returns "ordered" tetrahedra.  Each tetrahedron returned is in the star  */
/*  of the vertex nexttet[0], and in the star of the edge (nexttet[0],       */
/*  nexttet[1]).                                                             */
/*                                                                           */
/*  This procedure also returns ghost tetrahedra.  Even if no ghost          */
/*  tetrahedron has been explicitly inserted into the complex, there will    */
/*  still be ghost tetrahedra returned for triangles that are faces of       */
/*  exactly one solid tetrahedron.  If ghost tetrahedra have been inserted,  */
/*  more copies of them will be returned.  Any vertex of the returned        */
/*  tetrahedron can be GHOSTVERTEX except the first one (nexttet[0]).        */
/*                                                                           */
/*  The iterator is a variable `pos', whose internals should not be          */
/*  examined or modified by the client.                                      */
/*                                                                           */
/*  When a tetcomplex is modified, any iterators on that tetcomplex may be   */
/*  corrupted and should not be passed to this procedure again until they    */
/*  are re-initialized.                                                      */
/*                                                                           */
/*  pos:  The iterator.                                                      */
/*  nexttet:  An array used to return the tags for the vertices of the       */
/*    next tetrahedron.  The tetrahedron is always returned with positive    */
/*    orientation.  If the iterator has enumerated all the tetrahedra in the */
/*    complex, all four vertices will be STOP on return.  Does not need to   */
/*    be initialized before the call.                                        */
/*                                                                           */
/*****************************************************************************/

void tetcomplexiterateall(struct tetcomplexposition *pos,
                          tag nexttet[4])
{
  struct tetcomplexstar *star;
  tag vertexandlink[2];
  int iteratorflag;

  if (pos->link1dprevvertex != STOP) {
    /* We're in the middle of traversing a link ring, so find the next edge */
    /*   in the ring.                                                       */
    nexttet[0] = pos->starvertex;
    nexttet[1] = pos->link2dvertex;
    /* The previous vertex is one endpoint of the edge. */
    nexttet[2] = pos->link1dprevvertex;
    /* The next vertex in the link ring is the other endpoint. */
    nexttet[3] = linkringiterate(&pos->mycomplex->moleculepool,
                                 pos->starvertex, &pos->link1dposition);
    pos->link1dprevvertex = nexttet[3];
    if (nexttet[3] == STOP) {
      /* We've reach the end of the list, so "close" the link ring by using */
      /*   the first vertex in the list as the last endpoint.               */
      nexttet[3] = pos->link1dfirstvertex;
    }
    return;
  }

  /* We need to find the next link ring to traverse.  Are there any vertex */
  /*   stars left to traverse?                                             */
  while (pos->starvertex < pos->mycomplex->nextinitindex) {
    /* Presume we'll find a vertex's 2D link until proven otherwise. */
    iteratorflag = 1;
    /* Are we in the middle of traversing a vertex's 2D link? */
    if (pos->link2dvertex == STOP) {
      /* No.  Find the next vertex star to traverse. */
      star = (struct tetcomplexstar *)
             arraypoolfastlookup(&pos->mycomplex->stars,
                                 (arraypoolulong) pos->starvertex);
      if (star->linkhead == STOP) {
        /* Vertex `pos->starvertex' doesn't have a star.  Try the */
        /*   next vertex.                                         */
        iteratorflag = 0;
        pos->starvertex++;
      } else {
        /* Prepare to iterate through the vertex's 2D link. */
        link2diteratorinit(&pos->mycomplex->moleculepool, star->linkhead,
                           pos->starvertex, &pos->link2dposition);
      }
    }

    /* Are we traversing a 2D link? */
    if (iteratorflag) {
      /* Yes.  Find the next vertex and link ring in the 2D link. */
      link2diterate(&pos->mycomplex->moleculepool, &pos->link2dposition,
                    vertexandlink);
      pos->link2dvertex = vertexandlink[0];
      /* Have we reached the end of the list? */
      if (vertexandlink[0] != STOP) {
        /* No.  Return a tetrahedron adjoining this edge. */
        nexttet[0] = pos->starvertex;
        nexttet[1] = vertexandlink[0];
        /* Find the first edge in the new link ring. */
        linkringiteratorinit(&pos->mycomplex->moleculepool, vertexandlink[1],
                             &pos->link1dposition);
        nexttet[2] = linkringiterate(&pos->mycomplex->moleculepool,
                                     pos->starvertex, &pos->link1dposition);
        pos->link1dfirstvertex = nexttet[2];
        nexttet[3] = linkringiterate(&pos->mycomplex->moleculepool,
                                     pos->starvertex, &pos->link1dposition);
        pos->link1dprevvertex = nexttet[3];
        return;
      }
      pos->starvertex++;
    }
  }

  /* There are no more vertex stars to traverse or tetrahedra to return. */
  nexttet[0] = STOP;
  nexttet[1] = STOP;
  nexttet[2] = STOP;
  nexttet[3] = STOP;
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexiterate()   Advance a tetcomplexposition iterator to a new     */
/*                        tetrahedron and return it (skipping duplicates).   */
/*                                                                           */
/*  This procedure returns every tetrahedron in the data structure once.     */
/*  It returns ghost tetrahedra as well as solid tetrahedra (even if no      */
/*  ghost tetrahedron has been explicitly inserted into the complex):  for   */
/*  each triangle that is a face of exactly one solid tetrahedron, a ghost   */
/*  tetrahedron adjoining that face is returned.                             */
/*                                                                           */
/*  Ghost tetrahedra explicitly inserted into the tetcomplex have no effect  */
/*  on the tetrahedra this procedure returns!  The ghost tetrahedra returned */
/*  depend solely on the solid tetrahedra.  (You will notice this only if    */
/*  the inserted ghost tetrahedra are inconsistent with the solid            */
/*  tetrahedra.)                                                             */
/*                                                                           */
/*  The first two vertices (nexttet[0] and nexttet[1]) of the returned       */
/*  tetrahedron are never GHOSTVERTEX.  A ghost tetrahedron has its ghost    */
/*  vertex in one of the last two places (nexttet[2] or nexttet[3]).         */
/*                                                                           */
/*  The iterator is a variable `pos', whose internals should not be          */
/*  examined or modified by the client.                                      */
/*                                                                           */
/*  When a tetcomplex is modified, any iterators on that tetcomplex may be   */
/*  corrupted and should not be passed to this procedure again until they    */
/*  are re-initialized.                                                      */
/*                                                                           */
/*  pos:  The iterator.                                                      */
/*  nexttet:  An array used to return the tags for the vertices of the       */
/*    next tetrahedron.  The tetrahedron is always returned with positive    */
/*    orientation.  If the iterator has enumerated all the tetrahedra in the */
/*    complex, all four vertices will be STOP on return.  Does not need to   */
/*    be initialized before the call.                                        */
/*                                                                           */
/*****************************************************************************/

void tetcomplexiterate(struct tetcomplexposition *pos,
                       tag nexttet[4])
{
  /* To ensure that no tetrahedron is returned twice (with the vertices in   */
  /*   a different order, we use the following rules.  (It's helpful to      */
  /*   remember that nexttet[0] and nexttet[1] always have the same parity.) */
  /*                                                                         */
  /* (1)  Do not return the tetrahedron if nexttet[1] is GHOSTVERTEX.  This  */
  /*      ensures that no tetrahedron in the star of a ghost edge is ever    */
  /*      returned; so ghost tetrahedra explicitly inserted into the         */
  /*      tetcomplex have no effect on the tetrahedra this procedure         */
  /*      returns.                                                           */
  /* (2)  Do not return the tetrahedron if nexttet[2] has the same parity as */
  /*      nexttet[0] and nexttet[1], and a smaller index than at least one   */
  /*      of those two.  (The tetrahedron is in at least three link rings,   */
  /*      one for each of the edges of the triangle nexttet[0, 1, 2], so     */
  /*      return it only when nexttet[2] has the largest index.)             */
  /* (3)  Same thing for nexttet[3] (replacing nexttet[2]).                  */
  /* (4)  Do not return the tetrahedron if nexttet[2] has the same parity as */
  /*      nexttet[3], and either nexttet[0] or nexttet[1] has the largest    */
  /*      index of all four tetrahedron vertices.  (The tetrahedron is in    */
  /*      the link rings of two edges, nexttet[0, 1] and nexttet[2, 3], and  */
  /*      the edge containing the vertex with the largest index defers to    */
  /*      the other edge.)                                                   */
  /*                                                                         */
  /* These rules ensure that only one copy of each tetrahedron is returned.  */
  do {
    tetcomplexiterateall(pos, nexttet);
  } while ((nexttet[1] == GHOSTVERTEX) ||
           (((nexttet[2] & 1) == (nexttet[0] & 1)) &&
            ((nexttet[2] < nexttet[0]) || (nexttet[2] < nexttet[1]))) ||
           (((nexttet[3] & 1) == (nexttet[0] & 1)) &&
            ((nexttet[3] < nexttet[0]) || (nexttet[3] < nexttet[1]))) ||
           (((nexttet[3] & 1) == (nexttet[2] & 1)) &&
            (((nexttet[2] < nexttet[0]) && (nexttet[3] < nexttet[0])) ||
             ((nexttet[2] < nexttet[1]) && (nexttet[3] < nexttet[1])))));
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexiteratenoghosts()   Advance a tetcomplexposition iterator to a */
/*                                new solid tetrahedron and return it        */
/*                                (skipping duplicates).                     */
/*                                                                           */
/*  This procedure returns every solid tetrahedron in the data structure     */
/*  once.  Ghost tetrahedra are skipped.                                     */
/*                                                                           */
/*  The iterator is a variable `pos', whose internals should not be          */
/*  examined or modified by the client.                                      */
/*                                                                           */
/*  When a tetcomplex is modified, any iterators on that tetcomplex may be   */
/*  corrupted and should not be passed to this procedure again until they    */
/*  are re-initialized.                                                      */
/*                                                                           */
/*  pos:  The iterator.                                                      */
/*  nexttet:  An array used to return the tags for the vertices of the       */
/*    next tetrahedron.  The tetrahedron is always returned with positive    */
/*    orientation.  If the iterator has enumerated all the tetrahedra in the */
/*    complex, all four vertices will be STOP on return.  Does not need to   */
/*    be initialized before the call.                                        */
/*                                                                           */
/*****************************************************************************/

void tetcomplexiteratenoghosts(struct tetcomplexposition *pos,
                               tag nexttet[4])
{
  /* This loop ensures that no ghost tetrahedron is returned (note that   */
  /*   nexttet[0] is never a GHOSTVERTEX), and no tetrahedron is returned */
  /*   twice.  For an explanation of how the latter is accomplished, see  */
  /*   the comments in tetcomplexiterate().  The first copy of each       */
  /*   tetrahedron is returned, and later copies are not.                 */
  do {
    tetcomplexiterateall(pos, nexttet);
  } while ((nexttet[1] == GHOSTVERTEX) || (nexttet[2] == GHOSTVERTEX) ||
           (nexttet[3] == GHOSTVERTEX) ||
           (((nexttet[2] & 1) == (nexttet[0] & 1)) &&
            ((nexttet[2] < nexttet[0]) || (nexttet[2] < nexttet[1]))) ||
           (((nexttet[3] & 1) == (nexttet[0] & 1)) &&
            ((nexttet[3] < nexttet[0]) || (nexttet[3] < nexttet[1]))) ||
           (((nexttet[3] & 1) == (nexttet[2] & 1)) &&
            (((nexttet[2] < nexttet[0]) && (nexttet[3] < nexttet[0])) ||
             ((nexttet[2] < nexttet[1]) && (nexttet[3] < nexttet[1])))));
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexprint()   Print the contents of a tetcomplex.                  */
/*                                                                           */
/*  This procedure prints every tetrahedron in a tetcomplex.  It only works  */
/*  correctly if the stars are consistent with each other; for instance, it  */
/*  will not print meaningful information half way through star splaying.    */
/*                                                                           */
/*  plex:  The tetcomplex to print.                                          */
/*                                                                           */
/*****************************************************************************/

void tetcomplexprint(struct tetcomplex *plex) {
  struct tetcomplexposition pos;
  tag nexttet[4];
  int i;

  printf("List of all tetrahedra in the tetcomplex:\n");
  tetcomplexiteratorinit(plex, &pos);
  tetcomplexiterate(&pos, nexttet);
  while (nexttet[0] != STOP) {
    printf("  Vertex tags");
    for (i = 0; i < 4; i++) {
      if (nexttet[i] == GHOSTVERTEX) {
        printf(" GHOST");
      } else {
        printf(" %lu", (unsigned long) nexttet[i]);
      }
    }
    printf("\n");
    tetcomplexiterate(&pos, nexttet);
  }
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexprintstars()   Print the stars in a tetcomplex.                */
/*                                                                           */
/*  This procedure prints every _copy_ of every tetrahedron in the data      */
/*  structure, so it exposes the underlying implementation, and the fact     */
/*  that each solid tetrahedron is stored in the link rings of two, three,   */
/*  or six edges (depending on the parities of the tetrahedron's vertices).  */
/*                                                                           */
/*  plex:  The tetcomplex to print.                                          */
/*                                                                           */
/*****************************************************************************/

void tetcomplexprintstars(struct tetcomplex *plex) {
  struct tetcomplexstar *star;
  tag tagloop;

  printf("List of all stars in the tetcomplex:\n");
  for (tagloop = 0; tagloop < plex->nextinitindex; tagloop++) {
    star = (struct tetcomplexstar *)
           arraypoolfastlookup(&plex->stars, (arraypoolulong) tagloop);
    if (star->linkhead != STOP) {
      printf("Vertex tag %lu, ", (unsigned long) tagloop);
      link2dprint(&plex->moleculepool, star->linkhead, tagloop);
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexmissingtet()   Check whether a tetrahedron is missing from a   */
/*                           (particular) link ring where it should be.      */
/*                                                                           */
/*  This procedure checks whether the edge (vtx1, vtx2) stores a link ring,  */
/*  and if so, whether the star of (vtx1, vtx2) includes the tetrahedron     */
/*  (vtx1, vtx2, vtx3, vtx4).  If not, it prints an error message and        */
/*  returns 1.                                                               */
/*                                                                           */
/*  Ghost edges are not guaranteed to have explicitly stored link rings,     */
/*  so if one of `vtx1' or `vtx2' is a ghost vertex, this procedure does     */
/*  nothing but return 0.                                                    */
/*                                                                           */
/*  plex:  The tetcomplex to check for the presence of the tetrahedron.      */
/*  vtx1, vtx2, vtx3, vtx4:  The tags for the vertices of the tetrahedron.   */
/*    (vtx1, vtx2) is the edge whose link ring should include (vtx3, vtx4).  */
/*                                                                           */
/*  Returns 1 if the tetrahedron should be present but is missing.  Returns  */
/*  0 if the tetrahedron is present, if one of `vtx1' or `vtx2' is a ghost   */
/*  vertex, or if (vtx1, vtx2) does not store a link ring (because the two   */
/*  vertices have opposite parity).                                          */
/*                                                                           */
/*****************************************************************************/

unsigned int tetcomplexmissingtet(struct tetcomplex *plex,
                                  tag vtx1,
                                  tag vtx2,
                                  tag vtx3,
                                  tag vtx4) {
  tag adjacencies[2];
  int existflag;

  /* Are `vtx1' and `vtx2' non-ghost vertices with the same parity? */
  if (((vtx1 & 1) == (vtx2 & 1)) &&
      (vtx1 != GHOSTVERTEX) && (vtx2 != GHOSTVERTEX) &&
      ((vtx3 != GHOSTVERTEX) || (vtx4 != GHOSTVERTEX))) {
    /* Yes.  The tetrahedron (vtx1, vtx2, vtx3, vtx4) should exist in the */
    /*   link ring of (vtx1, vtx2).                                       */
    if (vtx3 != GHOSTVERTEX) {
      existflag = tetcomplexringadjacencies(plex, vtx1, vtx2, vtx3,
                                            adjacencies);
      existflag = existflag && (adjacencies[1] == vtx4);
    } else {
      existflag = tetcomplexringadjacencies(plex, vtx1, vtx2, vtx4,
                                            adjacencies);
      existflag = existflag && (adjacencies[0] == vtx3);
    }
    if (!existflag) {
      printf("  !! !! Tet x=%lu y=%lu %lu %lu is in star of edge (x, y),\n",
             (unsigned long) vtx1, (unsigned long) vtx4,
             (unsigned long) vtx2, (unsigned long) vtx3);
      printf("    but tet v=%lu w=%lu %lu %lu is not in star of (v, w).\n",
             (unsigned long) vtx1, (unsigned long) vtx2,
             (unsigned long) vtx3, (unsigned long) vtx4);
      return 1;
    }
  }

  return 0;
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexconsistency()   Check whether a tetcomplex is consistent, in   */
/*                            the sense that any tetrahedron stored in the   */
/*                            complex is duplicated in the stars of all its  */
/*                            represented edges.                             */
/*                                                                           */
/*  Prints error messages if inconsistencies are found, or a reassuring      */
/*  message otherwise.                                                       */
/*                                                                           */
/*  plex:  The tetcomplex to check for internal consistency.                 */
/*                                                                           */
/*****************************************************************************/

void tetcomplexconsistency(struct tetcomplex *plex) {
  struct tetcomplexposition pos;
  tag nexttet[4];
  arraypoolulong horrors;

  printf("Checking consistency of tetrahedral complex...\n");

  horrors = 0;
  tetcomplexiteratorinit(plex, &pos);
  tetcomplexiterateall(&pos, nexttet);
  while (nexttet[0] != STOP) {
    horrors += tetcomplexmissingtet(plex, nexttet[0], nexttet[2],
                                    nexttet[3], nexttet[1]);
    horrors += tetcomplexmissingtet(plex, nexttet[0], nexttet[3],
                                    nexttet[1], nexttet[2]);
    horrors += tetcomplexmissingtet(plex, nexttet[1], nexttet[2],
                                    nexttet[0], nexttet[3]);
    horrors += tetcomplexmissingtet(plex, nexttet[1], nexttet[3],
                                    nexttet[2], nexttet[0]);
    horrors += tetcomplexmissingtet(plex, nexttet[2], nexttet[3],
                                    nexttet[0], nexttet[1]);
    tetcomplexiterateall(&pos, nexttet);
  }

  if (horrors == 0) {
      printf("  Tremble before my vast wisdom, with which "
             "I find the mesh to be consistent.\n");
  } else if (horrors == 1) {
    printf("  !! !! !! !! Precisely one oozing cyst sighted.\n");
  } else {
    printf("  !! !! !! !! %lu monstrosities witnessed.\n",
           (unsigned long) horrors);
  }
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexdeleteorderedtet()   Delete an ordered tetrahedron from a      */
/*                                 tetcomplex.                               */
/*                                                                           */
/*  "Ordered tetrahedron" implies that the tetrahedron is deleted only from  */
/*  the star of vertex `vtx1', and therein, only from the star of edge       */
/*  (vtx1, vtx2).  This is a helper function for other tetrahedron deletion  */
/*  procedures, and is directly useful for algorithms like star splaying     */
/*  where the stars are not always mutually consistent.  If you want to keep */
/*  your tetcomplex internally consistent, you probably should not call this */
/*  procedure directly.                                                      */
/*                                                                           */
/*  Any face of the tetrahedron that is not shared by another tetrahedron    */
/*  in the star of (vtx1, vtx2) also vanishes from that star.                */
/*                                                                           */
/*  The orientation of the tetrahedron matters.  (The orientation can be     */
/*  reversed by swapping `vtx3' and `vtx4'.)  You cannot delete (1, 2, 3, 4) */
/*  by asking to delete (1, 2, 4, 3).                                        */
/*                                                                           */
/*  WARNING:  This procedure will not usually work if there is more than one */
/*  copy of `vtx3', or more than one copy of `vtx4', in the link of (vtx1,   */
/*  vtx2).  (This circumstance is not easy to create, but it's possible.)    */
/*                                                                           */
/*  plex:  The tetcomplex to delete a tetrahedron from.                      */
/*  vtx1, vtx2, vtx3, vtx4:  The tags for the vertices of the deleted        */
/*    tetrahedron, ordered so that only the star of `vtx1' changes, and      */
/*    within that star, only the star of edge (vtx1, vtx2) changes.          */
/*                                                                           */
/*  Returns one of the following values.                                     */
/*     0:  If the tetrahedron is not present in the star of the ordered edge */
/*         (vtx1, vtx2), and the tetcomplex is unchanged.                    */
/*     1:  If the triangle (vtx1, vtx2, vtx3) was deleted, and (vtx1, vtx2,  */
/*         vtx4) survives because it is a face of another tetrahedron in the */
/*         star.                                                             */
/*     2:  If (vtx1, vtx2, vtx4) was deleted, and (vtx1, vtx2, vtx3)         */
/*         survives.                                                         */
/*     3:  If both faces are deleted, but some other tetrahedron survives in */
/*         the edge's star.                                                  */
/*     7:  If both faces are deleted, leaving the ordered edge's star empty, */
/*         so the ordered edge is deleted as well.                           */
/*     8:  If the tetrahedron is deleted, but both faces survive.            */
/*                                                                           */
/*    Hence, a positive return value implies that the tetrahedron was        */
/*    deleted.  If the result is not zero, the 1's bit signifies that (vtx1, */
/*    vtx2, vtx3) is no longer in the edge's star, and the 2's bit signifies */
/*    that (vtx1, vtx2, vtx4) is no longer in the edge's star.  (If the      */
/*    result is zero, this procedure doesn't check whether the faces are     */
/*    present.)                                                              */
/*                                                                           */
/*****************************************************************************/

int tetcomplexdeleteorderedtet(struct tetcomplex *plex,
                               tag vtx1,
                               tag vtx2,
                               tag vtx3,
                               tag vtx4)
{
  struct tetcomplexstar *star;
  tag ring;
  int result;

  if (vtx1 >= plex->nextinitindex) {
    /* `vtx1' has never had a star, so the tetrahedron can't exist. */
    return 0;
  }
  /* Look up the star of vertex `vtx1'. */
  star = (struct tetcomplexstar *)
         arraypoolfastlookup(&plex->stars, (arraypoolulong) vtx1);

  /* Find the link ring for edge (vtx1, vtx2) in the star of `vtx1'. */
  ring = link2dfindring(&plex->moleculepool, plex->cache, star->linkhead,
                        vtx1, vtx2);
  if (ring == STOP) {
    /* `vtx2' is not in the link of `vtx1', so the tetrahedron can't exist. */
    return 0;
  }

  /* Attempt to delete the tetrahedron from the star of (vtx1, vtx2). */
  result = linkringdeleteedge(&plex->moleculepool, ring, vtx1, vtx3, vtx4);
  if ((result & 4) != 0) {
    /* The link ring is now empty, so deallocate it and remove `vtx2' from */
    /*   the 2D link of `vtx1'.                                            */
    linkringdelete(&plex->moleculepool, ring);
    link2ddeletevertex(&plex->moleculepool, plex->cache, star->linkhead,
                       &star->linktail, vtx1, vtx2);
  }

  /* If linkringdeleteedge() returned -1, change it to 0. */
  if (result <= 0) {
    return 0;
  }
  return result;
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexdeletetet()   Delete a tetrahedron from a tetcomplex.          */
/*                                                                           */
/*  Any face or edge of the tetrahedron that is not shared by another        */
/*  tetrahedron also vanishes.                                               */
/*                                                                           */
/*  The orientation of the tetrahedron matters.  (The orientation can be     */
/*  reversed by swapping any two vertices.)  You cannot delete (1, 2, 3, 4)  */
/*  by asking to delete (1, 2, 4, 3).  However, all even permutations of the */
/*  vertices are equivalent; deleting (1, 2, 3, 4) is the same as deleting   */
/*  (2, 1, 4, 3).                                                            */
/*                                                                           */
/*  If the tetrahedral complex is not internally consistent, this procedure  */
/*  will delete whatever copies of the tetrahedron it happens to find (which */
/*  might not be all of them).  It it finds some but not all of the copies   */
/*  it expects to find, it returns -1.  This differs from                    */
/*  tetcomplexinserttet(), which rolls back the changes when it finds an     */
/*  inconsistency.                                                           */
/*                                                                           */
/*  plex:  The tetcomplex to delete a tetrahedron from.                      */
/*  vtx1, vtx2, vtx3, vtx4:  The tags for the vertices of the deleted        */
/*    tetrahedron.                                                           */
/*                                                                           */
/*  Returns one of the following values.                                     */
/*   -1:  If the tetrahedron was deleted from some, but not all, of the edge */
/*        stars in which it should have resided.  This usually means that    */
/*        the tetcomplex is (or was) not internally consistent.              */
/*    0:  If the tetrahedron cannot be deleted, and the tetcomplex is        */
/*        unchanged.                                                         */
/*    1:  If the tetrahedron was deleted successfully.                       */
/*                                                                           */
/*****************************************************************************/

int tetcomplexdeletetet(struct tetcomplex *plex,
                        tag vtx1,
                        tag vtx2,
                        tag vtx3,
                        tag vtx4)
{
  tag swaptag;
  int result;
  int deletionflag;
  int failflag;
  int i;

  if (plex->verbosity > 3) {
    printf("        Deleting tet w/tags %lu %lu %lu %lu.\n",
           (unsigned long) vtx1, (unsigned long) vtx2,
           (unsigned long) vtx3, (unsigned long) vtx4);
  }

#ifdef SELF_CHECK
  if ((vtx1 == vtx2) || (vtx1 == vtx3) || (vtx1 == vtx4) ||
      (vtx2 == vtx3) || (vtx2 == vtx4) || (vtx3 == vtx4)) {
    printf("Internal error in tetcomplexdeletetet():\n");
    printf("  Asked to delete tetrahedron with two vertices alike.\n");
    internalerror();
  }
#endif /* SELF_CHECK */

  /* No changes have been made to the tetcomplex yet. */
  deletionflag = 0;
  /* Assume we won't fail, until proven otherwise. */
  failflag = 0;

  if ((vtx1 == GHOSTVERTEX) || (vtx2 == GHOSTVERTEX) ||
      (vtx3 == GHOSTVERTEX) || (vtx4 == GHOSTVERTEX)) {
    /* Make `vtx1', `vtx2', and `vtx3' the non-ghost vertices (preserving */
    /*   the tetrahedron's orientation).                                  */
    if (vtx1 == GHOSTVERTEX) {
      vtx1 = vtx2;
      vtx2 = vtx4;
    } else if (vtx2 == GHOSTVERTEX) {
      vtx2 = vtx3;
      vtx3 = vtx4;
    } else if (vtx3 == GHOSTVERTEX) {
      vtx3 = vtx1;
      vtx1 = vtx4;
    }

    /* For each vertex with the same parity as the ghost vertex, there is  */
    /*   a ghost edge from which the tetrahedron should be deleted.  Ghost */
    /*   tetrahedra are not explicitly stored in the link rings of solid   */
    /*   edges, so ignore the solid edges.                                 */
    if ((vtx1 & 1) == (GHOSTVERTEX & 1)) {
      if (tetcomplexdeleteorderedtet(plex, vtx1, GHOSTVERTEX, vtx2, vtx3) >
          0) {
        deletionflag = 1;
      } else {
        failflag = 1;
      }
    }
    if ((vtx2 & 1) == (GHOSTVERTEX & 1)) {
      if (tetcomplexdeleteorderedtet(plex, vtx2, GHOSTVERTEX, vtx3, vtx1) >
          0) {
        deletionflag = 1;
      } else {
        failflag = 1;
      }
    }
    if ((vtx3 & 1) == (GHOSTVERTEX & 1)) {
      if (tetcomplexdeleteorderedtet(plex, vtx3, GHOSTVERTEX, vtx1, vtx2) >
          0) {
        deletionflag = 1;
      } else {
        failflag = 1;
      }
    }

    if (!failflag) {
      /* Note that this counter may be decremented even if no change was     */
      /*   made to the tetcomplex at all, simply because the GHOSTVERTEX has */
      /*   a parity opposite to that of the other three vertices.            */
      plex->ghosttetcount--;

      /*^^^ Assertion:  The tetrahedral complex is now consistent. */
#ifdef PARANOID
      tetcomplexconsistency(plex);
#endif /* PARANOID */

      return 1;
    }
  } else {
    /* Loop over all six edges of the tetrahedron. */
    for (i = 0; i < 6; i++) {
      /* If the endpoints of the edge (vtx1, vtx2) have the same parity, the */
      /*   edge's link ring is stored, so delete the tetrahedron from the    */
      /*   edge's star.                                                      */
      if ((vtx1 & 1) == (vtx2 & 1)) {
        /* Determine which vertex star stores the link for (vtx1, vtx2). */
        if ((vtx1 < vtx2) ^ ((vtx1 & 2) == (vtx2 & 2))) {
          result = tetcomplexdeleteorderedtet(plex, vtx1, vtx2, vtx3, vtx4);
        } else {
          result = tetcomplexdeleteorderedtet(plex, vtx2, vtx1, vtx4, vtx3);
        }
        if (result > 0) {
          deletionflag = 1;
        } else {
          failflag = 1;
        }
      }

      /* The following shift cycles (vtx1, vtx2) through all the edges while */
      /*   maintaining the tetrahedron's orientation.  The schedule is       */
      /*   i = 0:  1 2 3 4 => 2 3 1 4                                        */
      /*   i = 1:  2 3 1 4 => 2 4 3 1                                        */
      /*   i = 2:  2 4 3 1 => 4 3 2 1                                        */
      /*   i = 3:  4 3 2 1 => 4 1 3 2                                        */
      /*   i = 4:  4 1 3 2 => 1 3 4 2                                        */
      /*   i = 5:  1 3 4 2 => 1 2 3 4 (which isn't used).                    */
      if ((i & 1) == 0) {
        swaptag = vtx1;
        vtx1 = vtx2;
        vtx2 = vtx3;
        vtx3 = swaptag;
      } else {
        swaptag = vtx4;
        vtx4 = vtx3;
        vtx3 = vtx2;
        vtx2 = swaptag;
      }
    }

    if (!failflag) {
      /* The tetrahedron has been successfully deleted. */
      plex->tetcount--;

      /*^^^ Assertion:  The tetrahedral complex is now consistent. */
#ifdef PARANOID
      tetcomplexconsistency(plex);
#endif /* PARANOID */

      return 1;
    }
  }

  /* The tetrahedron was missing from some edge's star where it should have */
  /*   appeared, so the deletion has failed.                                */
  if (deletionflag) {
    /* Nevertheless, a partial deletion occurred and the tetcomplex changed. */
    return -1;
  }

  /* The tetcomplex didn't change. */
  return 0;
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexlookup3dstar()   Return the `tetcomplexstar' record for a      */
/*                             vertex, creating one if necessary.            */
/*                                                                           */
/*  This procedure allocates memory for, and initializes, `tetcomplexstar'   */
/*  records for all uninitialized vertex stars up to and including the tag   */
/*  `vtx'.  It also allocates a 2D link for `vtx' if it doesn't already have */
/*  one; but it doesn't allocate 2D links for tags prior to `vtx'.           */
/*                                                                           */
/*  plex:  The tetcomplex in which the vertex needs a `tetcomplexstar'.      */
/*  vtx:  The tag for the vertex whose `tetcomplexstar' record is returned   */
/*        (and possibly created).                                            */
/*                                                                           */
/*  Returns a pointer to the `tetcomplexstar' record for `vtx'.              */
/*                                                                           */
/*****************************************************************************/

struct tetcomplexstar *tetcomplexlookup3dstar(struct tetcomplex *plex,
                                              tag vtx)
{
  struct tetcomplexstar *star;
  proxipoolulong allocindex;
  tag i;

  for (i = plex->nextinitindex; i <= vtx; i++) {
    /* Make sure memory is allocated for star `i'. */
    star = (struct tetcomplexstar *)
           arraypoolforcelookup(&plex->stars, (arraypoolulong) i);
    /* Initially, the 2D link is empty. */
    star->linkhead = STOP;
    star->linktail = STOP;
  }
  /* Remember where to continue next time. */
  plex->nextinitindex = i;

  /* Look up the star of vertex `vtx'. */
  star = (struct tetcomplexstar *)
         arraypoolfastlookup(&plex->stars, (arraypoolulong) vtx);
  if (star->linkhead == STOP) {
    /* Vertex `vtx' doesn't have a 2D link yet.  Allocate one, using the */
    /*   same allocation index that was used for the vertex `vtx'.       */
    if (plex->vertexpool == (struct proxipool *) NULL) {
      allocindex = 0;
    } else {
      allocindex = proxipooltag2allocindex(plex->vertexpool, vtx);
    }
    star->linkhead = link2dnew(&plex->moleculepool, allocindex);
    star->linktail = star->linkhead;
  }

  return star;
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexinsertorderedtet()   Insert an ordered tetrahedron into a      */
/*                                 tetcomplex.                               */
/*                                                                           */
/*  "Ordered tetrahedron" implies that the tetrahedron is inserted only into */
/*  the star of vertex `vtx1', and therein, it is inserted only into the     */
/*  star of edge (vtx1, vtx2).  Therefore, even permutations of the vertices */
/*  are NOT equivalent in this procedure.  This is a helper procedure for    */
/*  other tetrahedron insertion procedures, and is also directly useful for  */
/*  algorithms like star splaying where the stars are not always mutually    */
/*  consistent.  If you want to keep your tetcomplex internally consistent,  */
/*  you should not call this procedure directly.                             */
/*                                                                           */
/*  The orientation of the tetrahedron matters.  (The orientation can be     */
/*  reversed by swapping `vtx3' and `vtx4'.)  Inserting (1, 2, 3, 4) is not  */
/*  the same as inserting (1, 2, 4, 3).                                      */
/*                                                                           */
/*  This procedure will return 0 and leave the tetcomplex unchanged if any   */
/*  of the following is true.                                                */
/*                                                                           */
/*  - There is already a tetrahedron (vtx1, vtx2, vtx3, X) for some X in the */
/*    star of the ordered edge (vtx1, vtx2).                                 */
/*  - There is already a tetrahedron (vtx1, vtx2, X, vtx4) for some X in the */
/*    star of the ordered edge (vtx1, vtx2).                                 */
/*  - The link of the ordered edge (vtx1, vtx2) is truly a ring, with no     */
/*    place to insert a new tetrahedron.                                     */
/*  - The link of the ordered edge (vtx1, vtx2) currently consists of two or */
/*    more chains, and the introduction of edge (vtx3, vtx4) into the link   */
/*    will glue one of the chains into a ring.  It's not topologically       */
/*    possible (in a tetrahedral complex embeddable in 3D space) for an edge */
/*    to have a link with a ring AND a chain.                                */
/*                                                                           */
/*  WARNING:  This procedure will not usually work if there is more than one */
/*  copy of `vtx3', or more than one copy of `vtx4', in the link of (vtx1,   */
/*  vtx2).  (This circumstance is not easy to create, but it's possible.)    */
/*                                                                           */
/*  plex:  The tetcomplex to insert a tetrahedron into.                      */
/*  vtx1, vtx2, vtx3, vtx4:  The tags for the vertices of the new            */
/*    tetrahedron, ordered so that only the star of `vtx1' changes, and      */
/*    within that star, only the star of edge (vtx1, vtx2) changes.          */
/*                                                                           */
/*  Returns one of the following values.                                     */
/*    0:  If the tetrahedron cannot be inserted, and the tetcomplex is       */
/*        unchanged.                                                         */
/*    1:  If the triangle (vtx1, vtx2, vtx3) is new in the star of the       */
/*        ordered edge (vtx1, vtx2), and (vtx1, vtx2, vtx4) was already      */
/*        present.                                                           */
/*    2:  If the triangle (vtx1, vtx2, vtx4) is new in the star of the       */
/*        ordered edge (vtx1, vtx2), and (vtx1, vtx2, vtx3) was already      */
/*        present.                                                           */
/*    3:  If neither triangle was present, and both are new.                 */
/*    4:  If both triangles (vtx1, vtx2, vtx3) and (vtx1, vtx2, vtx4) were   */
/*        already present in the star of (vtx1, vtx2), and the new           */
/*        tetrahedron slid nicely between them.                              */
/*                                                                           */
/*  Hence, the 1's bit signifies that (vtx1, vtx2, vtx3) was created anew,   */
/*  and the 2's bit signifies that (vtx1, vtx2, vtx4) was created anew.      */
/*                                                                           */
/*****************************************************************************/

int tetcomplexinsertorderedtet(struct tetcomplex *plex,
                               tag vtx1,
                               tag vtx2,
                               tag vtx3,
                               tag vtx4)
{
  struct tetcomplexstar *star;
  tag ring;

  /* Make sure `vtx1' has a 2D star, and find it. */
  star = tetcomplexlookup3dstar(plex, vtx1);
  /* Find the link ring for edge (vtx1, vtx2) in the star of `vtx1'. */
  ring = link2dfindinsert(&plex->moleculepool, plex->cache, star->linkhead,
                          &star->linktail, vtx1, vtx2);
  /* Insert the tetrahedron into the star of the ordered edge (vtx1, vtx2). */
  return linkringinsertedge(&plex->moleculepool, ring, vtx1, vtx3, vtx4);
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexinserttet()   Insert a tetrahedron into a tetcomplex.          */
/*                                                                           */
/*  The orientation of the tetrahedron matters.  (The orientation can be     */
/*  reversed by swapping any two vertices.)  Inserting (1, 2, 3, 4) is not   */
/*  the same as inserting (1, 2, 4, 3).  However, all even permutations of   */
/*  the vertices are equivalent; (1, 2, 3, 4) is the same as (2, 1, 4, 3).   */
/*                                                                           */
/*  This procedure will return 0 and leave the link ring unchanged if any of */
/*  the following is true.                                                   */
/*                                                                           */
/*  - Some tetrahedron already in the complex has a face in common with the  */
/*    new tetrahedron, and both tetrahedra would be on the same side of that */
/*    face.  For example, there is already a tetrahedron (vtx1, vtx2, X,     */
/*    vtx4) for some vertex X.                                               */
/*  - One of the edges of the new tetrahedron has a full complement of       */
/*    tetrahedra encircling it.  In other words, the edge's link is a ring.  */
/*  - One of the edges of the new tetrahedron has a link consisting of two   */
/*    or more chains, and the introduction of the new tetrahedron will glue  */
/*    one of the chains into a ring.  It's not topologically possible to     */
/*    have a link with a ring AND a chain.                                   */
/*                                                                           */
/*  plex:  The tetcomplex to insert a tetrahedron into.                      */
/*  vtx1, vtx2, vtx3, vtx4:  The tags for the vertices of the new            */
/*    tetrahedron.                                                           */
/*                                                                           */
/*  Returns one of the following values.                                     */
/*    0:   If the tetrahedron cannot be inserted, and the tetcomplex is      */
/*         unchanged.                                                        */
/*    1:   If a ghost tetrahedron was inserted successfully.                 */
/*    2:   If a solid tetrahedron was inserted successfully, and the number  */
/*         of boundary triangles decreased by 4.                             */
/*    4:   If a solid tetrahedron was inserted successfully, and the number  */
/*         of boundary triangles decreased by 2.                             */
/*    6:   If a solid tetrahedron was inserted successfully, and the number  */
/*         of boundary triangles remained constant.                          */
/*    8:   If a solid tetrahedron was inserted successfully, and the number  */
/*         of boundary triangles increased by 2.                             */
/*    10:  If a solid tetrahedron was inserted successfully, and the number  */
/*         of boundary triangles increased by 4.                             */
/*                                                                           */
/*****************************************************************************/

int tetcomplexinserttet(struct tetcomplex *plex,
                        tag vtx1,
                        tag vtx2,
                        tag vtx3,
                        tag vtx4)
{
  tag lasthead;
  tag lasttail;
  tag swaptag;
  int result;
  int linkringchanges;
  int deltafaces;
  int i;

  if (plex->verbosity > 3) {
    printf("        Inserting tet w/tags %lu %lu %lu %lu.\n",
           (unsigned long) vtx1, (unsigned long) vtx2,
           (unsigned long) vtx3, (unsigned long) vtx4);
  }

#ifdef SELF_CHECK
  if ((vtx1 == vtx2) || (vtx1 == vtx3) || (vtx1 == vtx4) ||
      (vtx2 == vtx3) || (vtx2 == vtx4) || (vtx3 == vtx4)) {
    printf("Internal error in tetcomplexinserttet():\n");
    printf("  Asked to insert tetrahedron with two vertices alike.\n");
    internalerror();
  }
#endif /* SELF_CHECK */

  /* No changes have been made to the tetcomplex yet. */
  linkringchanges = 0;

  if ((vtx1 == GHOSTVERTEX) || (vtx2 == GHOSTVERTEX) ||
      (vtx3 == GHOSTVERTEX) || (vtx4 == GHOSTVERTEX)) {
    /* Make `vtx1', `vtx2', and `vtx3' the non-ghost vertices (preserving */
    /*   the tetrahedron's orientation).                                  */
    if (vtx1 == GHOSTVERTEX) {
      vtx1 = vtx2;
      vtx2 = vtx4;
    } else if (vtx2 == GHOSTVERTEX) {
      vtx2 = vtx3;
      vtx3 = vtx4;
    } else if (vtx3 == GHOSTVERTEX) {
      vtx3 = vtx1;
      vtx1 = vtx4;
    }

    /* For each vertex with the same parity as the ghost vertex, there is */
    /*   (or will be) a ghost edge.  Store the ghost tetrahedron in the   */
    /*   star of the ghost edge.  Note that ghost tetrahedra are not      */
    /*   explicitly stored in the link rings of solid edges, but they are */
    /*   implicitly already there if the ghost tetrahedra are consistent  */
    /*   with the solid tetrahedra.                                       */
    if ((vtx1 & 1) == (GHOSTVERTEX & 1)) {
      if (tetcomplexinsertorderedtet(plex, vtx1, GHOSTVERTEX, vtx2, vtx3) <=
          0) {
        return 0;
      }
      linkringchanges = 1;
    }
    if ((vtx2 & 1) == (GHOSTVERTEX & 1)) {
      if (tetcomplexinsertorderedtet(plex, vtx2, GHOSTVERTEX, vtx3, vtx1) <=
          0) {
        if (linkringchanges) {
          /* Roll back the change that has been made. */
          tetcomplexdeleteorderedtet(plex, vtx1, GHOSTVERTEX, vtx2, vtx3);
        }
        return 0;
      }
      linkringchanges = linkringchanges | 2;
    }
    if ((vtx3 & 1) == (GHOSTVERTEX & 1)) {
      if (tetcomplexinsertorderedtet(plex, vtx3, GHOSTVERTEX, vtx1, vtx2) <=
          0) {
        if ((linkringchanges & 1) != 0) {
          /* Roll back the change. */
          tetcomplexdeleteorderedtet(plex, vtx1, GHOSTVERTEX, vtx2, vtx3);
        }
        if ((linkringchanges & 2) != 0) {
          /* Roll back the change. */
          tetcomplexdeleteorderedtet(plex, vtx2, GHOSTVERTEX, vtx3, vtx1);
        }
        return 0;
      }
    }

    /* Note that this counter may be incremented even if no change was made */
    /*   to the tetcomplex at all, simply because the GHOSTVERTEX has a     */
    /*   parity opposite to that of the other three vertices.               */
    plex->ghosttetcount++;

    /*^^^ Assertion:  The tetrahedral complex is now consistent. */
#ifdef PARANOID
    tetcomplexconsistency(plex);
#endif /* PARANOID */

    return 1;
  }

  /* No triangular boundary faces have been created/covered yet. */
  deltafaces = 0;

  /* Loop over all six edges of the tetrahedron. */
  for (i = 0; i < 6; i++) {
    /* If the endpoints of the edge (vtx1, vtx2) have the same parity, the */
    /*   edge's link ring is stored, so insert the new tetrahedron into    */
    /*   the edge's star.                                                  */
    if ((vtx1 & 1) == (vtx2 & 1)) {
      /* Determine which vertex star stores the link for (vtx1, vtx2). */
      if ((vtx1 < vtx2) ^ ((vtx1 & 2) == (vtx2 & 2))) {
        result = tetcomplexinsertorderedtet(plex, vtx1, vtx2, vtx3, vtx4);
        lasthead = vtx1;
        lasttail = vtx4;
      } else {
        result = tetcomplexinsertorderedtet(plex, vtx2, vtx1, vtx4, vtx3);
        lasthead = vtx2;
        lasttail = vtx3;
      }
      if (result == 3) {
        /* Two new triangular faces are on the boundary. */
        deltafaces += 2;
      } else if (result == 4) {
        /* Two triangular faces are covered, and no longer on the boundary. */
        deltafaces -= 2;
      } else if (result <= 0) {
        /* The insertion failed.  If any previous insertions were */
        /*   successful, roll back the changes that were made.    */
        if (linkringchanges != 0) {
          tetcomplexdeletetet(plex, vtx1, vtx2, vtx3, vtx4);
        }
        return 0;
      }
      /* Count the number of edges whose link rings have changed. */
      linkringchanges++;
    }

    /* The following rotation cycles (vtx1, vtx2) through all the edges    */
    /*   while maintaining the tetrahedron's orientation.  The schedule is */
    /*   i = 0:  1 2 3 4 => 2 3 1 4                                        */
    /*   i = 1:  2 3 1 4 => 2 4 3 1                                        */
    /*   i = 2:  2 4 3 1 => 4 3 2 1                                        */
    /*   i = 3:  4 3 2 1 => 4 1 3 2                                        */
    /*   i = 4:  4 1 3 2 => 1 3 4 2                                        */
    /*   i = 5:  1 3 4 2 => 1 2 3 4 (which isn't used).                    */
    if ((i & 1) == 0) {
      swaptag = vtx1;
      vtx1 = vtx2;
      vtx2 = vtx3;
      vtx3 = swaptag;
    } else {
      swaptag = vtx4;
      vtx4 = vtx3;
      vtx3 = vtx2;
      vtx2 = swaptag;
    }
  }

  /* The tetrahedron has been successfully inserted. */
  plex->tetcount++;

  /*^^^ Assertion:  The tetrahedral complex is now consistent. */
#ifdef PARANOID
  tetcomplexconsistency(plex);
#endif /* PARANOID */

  /* `linkringchanges' can be 2, 3, or 6, depending on the parities of       */
  /*   the vertices.  If it's 2, then only two (opposing) edges of the       */
  /*   tetrahedron have link rings, so the change `deltafaces' in the number */
  /*   of boundary faces was counted correctly.  Otherwise, it needs to be   */
  /*   adjusted.                                                             */
  if (linkringchanges == 3) {
    /* One of the face's changes was counted three times (that face being  */
    /*   the one opposite the vertex whose parity disagrees with the other */
    /*   three vertices), whereas the other three faces' changes were each */
    /*   counted once.                                                     */
    if (deltafaces > 0) {
      /* A newly created boundary face was counted thrice. */
      deltafaces -= 2;
    } else if (deltafaces < 0) {
      /* A covered boundary face was counted thrice. */
      deltafaces += 2;
    } else {
      /* The face that was counted three times cancels out the other three  */
      /*   faces.  Therefore, `deltafaces' is zero, and `result' is either  */
      /*   1 or 2.  Was a new boundary face counted three times, or was a   */
      /*   covered boundary face counted thrice?  If `lasttail' has parity  */
      /*   opposite to `lasthead', then `lasttail' is the parity-mismatched */
      /*   vertex.                                                          */
      if (((lasttail & 1) != (lasthead & 1)) ^ (result == 1)) {
        /* Either `lasttail' is the parity-mismatched vertex and             */
        /*   result == 2, in which case a boundary face opposite `lasttail'  */
        /*   got covered (is no longer on the boundary) and counted thrice;  */
        /*   or `lasttail' matches two other vertices' parity and            */
        /*   result == 1, in which case a boundary face adjoining `lasttail' */
        /*   got covered and counted thrice.                                 */
        deltafaces += 2;
      } else {
        /* Reversing the reasoning above, a new boundary face got counted */
        /*   thrice.                                                      */
        deltafaces -= 2;
      }
    }
  } else if (linkringchanges == 6) {
    /* All four vertices have the same parity, so all six edges of the */
    /*   tetrahedron have link rings, so every face change was counted */
    /*   thrice.                                                       */
    deltafaces /= 3;
  }
  return 6 + deltafaces;
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplex12fliponedge()   Replace a tetrahedron with two in an edge's   */
/*                             star.                                         */
/*                                                                           */
/*  Replaces the tetrahedron (vtx1, vtx2, vtx3, vtx4) with (vtx1, vtx2,      */
/*  vtx3, newvertex) and (vtx1, vtx2, newvertex, vtx4) in the star of the    */
/*  edge (vtx1, vtx2).  If `vtx1' and `vtx2' have opposite parities, this    */
/*  procedure assumes that the edge is not stored, and does nothing.         */
/*                                                                           */
/*  Equivalently, this procedure replace one edge with two in the link       */
/*  ring of the edge (vtx1, vtx2).  This is accomplished by inserting a      */
/*  vertex `newvertex' between `vxt3' and `vtx4' in the link ring.           */
/*                                                                           */
/*  This is a helper procedure for other procedures.  Used in isolation, it  */
/*  changes the complex so the stars are not mutually consistent.  If you    */
/*  want to keep your tetcomplex internally consistent, you probably should  */
/*  not call this procedure directly.                                        */
/*                                                                           */
/*  WARNING:  This procedure does not check whether the tetrahedron (vtx1,   */
/*  vtx2, vtx3, vtx4) is in the vertex's star.  If the tetrahedron is not    */
/*  there, `newvertex' may be inserted in the link ring, next to `vtx3' or   */
/*  `vtx4', or the program may halt with an internal error.  Don't call this */
/*  procedure unless you're sure the tetrahedron is in the edge's star (if   */
/*  the edge is stored at all).                                              */
/*                                                                           */
/*  plex:  The tetcomplex in which the flip occurs.                          */
/*  vtx1, vtx2, vtx3, vtx4:  The tags for the vertices of the tetrahedron    */
/*    being replaced.  (vtx1, vtx2) is the edge whose star is changed.       */
/*  newvertex:  Tag for the vertex to be inserted between `vtx3' and `vtx4'  */
/*    in the link ring.                                                      */
/*                                                                           */
/*****************************************************************************/

void tetcomplex12fliponedge(struct tetcomplex *plex,
                            tag vtx1,
                            tag vtx2,
                            tag vtx3,
                            tag vtx4,
                            tag newvertex)
{
  struct tetcomplexstar *star;
  tag ring;
  tag swaptag;
  int result;

  /* The edge (vtx1, vtx2) is stored only if vtx1 and vtx2 have the */
  /*   same parity.                                                 */
  if ((vtx1 & 1) != (vtx2 & 1)) {
    return;
  }

  /* Determine which vertex's star stores the edge's link ring. */
  if (!((vtx2 == GHOSTVERTEX) ||
        ((vtx1 != GHOSTVERTEX) &&
         ((vtx1 < vtx2) ^ ((vtx1 & 2) == (vtx2 & 2)))))) {
    /* `vtx2' stores the link ring, so swap it with `vtx1'.  To preserve the */
    /*   tetrahedron orientation, `vtx4' replaces `vtx3'.                    */
    swaptag = vtx1;
    vtx1 = vtx2;
    vtx2 = swaptag;
    vtx3 = vtx4;
  }

  /* Has a star ever been allocated for vtx1? */
  if (vtx1 < plex->nextinitindex) {
    /* Insert `newvertex' after `vtx3' in the link ring for (vtx1, vtx2). */
    star = (struct tetcomplexstar *)
           arraypoolfastlookup(&plex->stars, (arraypoolulong) vtx1);
    ring = link2dfindring(&plex->moleculepool, plex->cache, star->linkhead,
                          vtx1, vtx2);
    result = linkringinsertvertex(&plex->moleculepool, ring,
                                  vtx1, vtx3, newvertex);
    if (result <= 0) {
      printf("Internal error in tetcomplex12fliponedge():\n");
      printf("  Tetrahedron is missing from the edge's star.\n");
      internalerror();
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexinserttetontripod()   Insert a tetrahedron into the link rings */
/*                                  of three edges that meet at `vtx1',      */
/*                                  forming a "tripod".                      */
/*                                                                           */
/*  This is a special-purpose procedure designed to help ease and speed up   */
/*  the Bowyer-Watson algorithm.  It should not normally be used by other    */
/*  algorithms.                                                              */
/*                                                                           */
/*  This procedure is similar to tetcomplexinserttet(), but it only modifies */
/*  the link rings of three of the tetrahedron's six edges--the three edges  */
/*  that meet at `vtx1'.  (Note that this is NOT the same as changing only   */
/*  the star of `vtx1'; the three edges might be stored with the other       */
/*  vertices, and both endpoints of each edge are presumed to have had       */
/*  their stars changed.)  This procedure can introduce (or repair)          */
/*  inconsistencies _within_ the stars of `vtx2', `vtx3', and `vtx4', which  */
/*  the Bowyer-Watson implementation eventually repairs.                     */
/*                                                                           */
/*  This procedure will return 0 and leave the link ring under roughly the   */
/*  same circumstances as tetcomplexinserttet(), although this procedure     */
/*  does not check the edges it doesn't modify.                              */
/*                                                                           */
/*  plex:  The tetcomplex to insert a tetrahedron into.                      */
/*  vtx1, vtx2, vtx3, vtx4:  The tags for the vertices of the new            */
/*    tetrahedron.  Only edges adjoining `vtx1' have their stars modified.   */
/*                                                                           */
/*  Returns one of the following values.                                     */
/*    0:  If the tetrahedron cannot be inserted, and the tetcomplex is       */
/*        unchanged.                                                         */
/*    1:  If the tetrahedron was inserted successfully.                      */
/*                                                                           */
/*****************************************************************************/

int tetcomplexinserttetontripod(struct tetcomplex *plex,
                                tag vtx1,
                                tag vtx2,
                                tag vtx3,
                                tag vtx4)
{
  tag swaptag;
  int result;
  int insertionflag;
  int i;

  if (plex->verbosity > 3) {
    printf("        Inserting tet w/tags %lu %lu %lu %lu\n"
           "          in stars of edges adjoining vertex tag %lu.\n",
           (unsigned long) vtx1, (unsigned long) vtx2,
           (unsigned long) vtx3, (unsigned long) vtx4, (unsigned long) vtx1);
  }

#ifdef SELF_CHECK
  if (vtx1 >= STOP) {
    printf("Internal error in tetcomplexinserttetontripod():\n");
    printf("  First parameter is not a valid vertex tag.\n");
    internalerror();
  }
  if ((vtx1 == vtx2) || (vtx1 == vtx3) || (vtx1 == vtx4) ||
      (vtx2 == vtx3) || (vtx2 == vtx4) || (vtx3 == vtx4)) {
    printf("Internal error in tetcomplexinserttetontripod():\n");
    printf("  Asked to insert tetrahedron with two vertices alike.\n");
    internalerror();
  }
#endif /* SELF_CHECK */

  /* No changes have been made to the tetcomplex yet. */
  insertionflag = 0;

  result = 1;
  if ((vtx2 == GHOSTVERTEX) || (vtx3 == GHOSTVERTEX) ||
      (vtx4 == GHOSTVERTEX)) {
    /* A ghost tetrahedron changes only a ghost edge's link ring.  `vtx1'   */
    /*   stores a ghost edge only if it has the same parity as GHOSTVERTEX. */
    if ((vtx1 & 1) == (GHOSTVERTEX & 1)) {
      /* Insert the ghost tetrahedron into a ghost edge's star. */
      if (vtx2 == GHOSTVERTEX) {
        if (tetcomplexinsertorderedtet(plex,
                                       vtx1, GHOSTVERTEX, vtx3, vtx4) == 0) {
          return 0;
        }
      } else if (vtx3 == GHOSTVERTEX) {
        if (tetcomplexinsertorderedtet(plex,
                                       vtx1, GHOSTVERTEX, vtx4, vtx2) == 0) {
          return 0;
        }
      } else {
        /* vtx4 == GHOSTVERTEX */
        if (tetcomplexinsertorderedtet(plex,
                                       vtx1, GHOSTVERTEX, vtx2, vtx3) == 0) {
          return 0;
        }
      }
    }

    /* Note that this counter may be incremented even if no change was */
    /*   made to the tetcomplex at all, simply because `vtx1' and the  */
    /*   GHOSTVERTEX have opposite parities.                           */
    plex->ghosttetcount++;
    return 1;
  } else {
    /* Loop over the three edges of the tetrahedron adjoining `vtx1'. */
    for (i = 0; i < 3; i++) {
      /* If the endpoints of the edge (vtx1, vtx2) have the same parity, the */
      /*   edge's link ring is stored, so insert the new tetrahedron into    */
      /*   the edge's star.                                                  */
      if ((vtx1 & 1) == (vtx2 & 1)) {
        /* Determine which vertex star stores the link for (vtx1, vtx2). */
        if ((vtx1 < vtx2) ^ ((vtx1 & 2) == (vtx2 & 2))) {
          result = tetcomplexinsertorderedtet(plex, vtx1, vtx2, vtx3, vtx4);
        } else {
          result = tetcomplexinsertorderedtet(plex, vtx2, vtx1, vtx4, vtx3);
        }
        if (result <= 0) {
          /* The insertion failed.  If any previous insertions were */
          /*   successful, roll back the changes that were made.    */
          if (insertionflag) {
            tetcomplexdeleteorderedtet(plex, vtx1, vtx2, vtx3, vtx4);
            tetcomplexdeleteorderedtet(plex, vtx2, vtx1, vtx4, vtx3);
            tetcomplexdeleteorderedtet(plex, vtx1, vtx3, vtx4, vtx2);
            tetcomplexdeleteorderedtet(plex, vtx3, vtx1, vtx2, vtx4);
          }
          return 0;
        }
        /* Remember this change in case it needs to be rolled back later. */
        insertionflag = 1;
      }

      /* The following shift cycles (vtx1, vtx2) through the edges adjoining */
      /*   `vtx1' while maintaining the tetrahedron's orientation.           */
      swaptag = vtx2;
      vtx2 = vtx3;
      vtx3 = vtx4;
      vtx4 = swaptag;
    }

    /* Note that this counter may be incremented even if no change was */
    /*   made to the tetcomplex at all, simply because `vtx1' has a    */
    /*   parity opposite to that of the other three vertices.          */
    plex->tetcount++;
    return 1;
  }
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplex21fliponedge()   Replace two adjacent tetrahedra with one in   */
/*                             an edge's star.                               */
/*                                                                           */
/*  Replaces the tetrahedra (vtx1, vtx2, X, vtx3) and (vtx1, vtx2, vtx3, Y)  */
/*  with (vtx1, vtx2, X, Y) in the star of the edge (vtx1, vtx2).  If `vtx1' */
/*  and `vtx2' have opposite parities, this procedure assumes that the edge  */
/*  is not stored, and does nothing.                                         */
/*                                                                           */
/*  Equivalently, this procedure replace two edges with one in the link      */
/*  ring of the edge (vtx1, vtx2).  This is accomplished by deleting a       */
/*  vertex `vtx3' from the link ring.                                        */
/*                                                                           */
/*  This is a helper procedure for other procedures.  Used in isolation, it  */
/*  changes the complex so the stars are not mutually consistent.  If you    */
/*  want to keep your tetcomplex internally consistent, you probably should  */
/*  not call this procedure directly.                                        */
/*                                                                           */
/*  WARNING:  don't call this procedure unless you're sure `vtx3' is in the  */
/*  edge's link ring (if the edge is stored at all).                         */
/*                                                                           */
/*  plex:  The tetcomplex in which the flip occurs.                          */
/*  vtx1, vtx2, vtx3:  The tags for the vertices of the triangle being       */
/*    deleted.  (vtx1, vtx2) is the edge whose star is changed.              */
/*                                                                           */
/*****************************************************************************/

void tetcomplex21fliponedge(struct tetcomplex *plex,
                            tag vtx1,
                            tag vtx2,
                            tag vtx3)
{
  struct tetcomplexstar *star;
  tag ring;
  tag swaptag;
  int result;

  /* The edge (vtx1, vtx2) is stored only if vtx1 and vtx2 have the */
  /*   same parity.                                                 */
  if ((vtx1 & 1) != (vtx2 & 1)) {
    return;
  }

  /* Determine which vertex's star stores the edge's link ring. */
  if (!((vtx2 == GHOSTVERTEX) ||
        ((vtx1 != GHOSTVERTEX) &&
         ((vtx1 < vtx2) ^ ((vtx1 & 2) == (vtx2 & 2)))))) {
    /* `vtx2' stores the link ring, so swap it with `vtx1'. */
    swaptag = vtx1;
    vtx1 = vtx2;
    vtx2 = swaptag;
  }

  /* Has a star ever been allocated for vtx1? */
  if (vtx1 < plex->nextinitindex) {
    /* Delete `vtx3' from the link ring for (vtx1, vtx2). */
    star = (struct tetcomplexstar *)
           arraypoolfastlookup(&plex->stars, (arraypoolulong) vtx1);
    ring = link2dfindring(&plex->moleculepool, plex->cache, star->linkhead,
                          vtx1, vtx2);
    result = linkringdeletevertex(&plex->moleculepool, ring, vtx1, vtx3);
    if ((result & 2) != 0) {
      /* Link ring now empty; free it and remove vtx2 from vtx1's 2D link. */
      linkringdelete(&plex->moleculepool, ring);
      link2ddeletevertex(&plex->moleculepool, plex->cache, star->linkhead,
                         &star->linktail, vtx1, vtx2);
    }
    if (result <= 0) {
      printf("Internal error in tetcomplex21fliponedge():\n");
      printf("  Triangle is missing from the edge's star.\n");
      internalerror();
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexdeletetriangle()   Delete a triangle from a tetcomplex, gluing */
/*                               the two adjoining tetrahedra (or one        */
/*                               tetrahedron and one polyhedron) together    */
/*                               into a single polyhedron.                   */
/*                                                                           */
/*  This is a special-purpose procedure designed to help ease and speed up   */
/*  the Bowyer-Watson algorithm.  It should not normally be used by other    */
/*  algorithms.                                                              */
/*                                                                           */
/*  To understand this procedure, it's useful to think of each edge's link   */
/*  ring not as a bunch of tetrahedra encircling an edge, but as a bunch of  */
/*  triangles encircling an edge, acting as boundaries in a polyhedral       */
/*  complex.  The Blandford et al. data structure can represent arbitrary    */
/*  polyhedral complexes (with all faces triangulated)--although to support  */
/*  efficient adjacency queries, you would need to store all the edges (not  */
/*  just half the edges, as Blandford et al. do).                            */
/*                                                                           */
/*  This procedure deletes a triangle (vtx1, vtx2, vtx3) from the complex,   */
/*  thereby gluing a tetrahedron (vtx1, vtx2, vtx3, X) onto the polyhedron   */
/*  on the other side of the triangle.  It is used by the Bowyer-Watson      */
/*  implementation to glue all the tetrahedra that are no longer Delaunay    */
/*  into one polyhedral cavity, which is subsequently filled with new        */
/*  tetrahedra.                                                              */
/*                                                                           */
/*  plex:  The tetcomplex to delete a triangle from.                         */
/*  vtx1, vtx2, vtx3:  The tags for the vertices of the triangle to delete.  */
/*                                                                           */
/*****************************************************************************/

void tetcomplexdeletetriangle(struct tetcomplex *plex,
                              tag vtx1,
                              tag vtx2,
                              tag vtx3)
{
  if (plex->verbosity > 3) {
    printf("        Deleting triangle w/tags %lu %lu %lu.\n",
           (unsigned long) vtx1, (unsigned long) vtx2, (unsigned long) vtx3);
  }

  tetcomplex21fliponedge(plex, vtx1, vtx2, vtx3);
  tetcomplex21fliponedge(plex, vtx2, vtx3, vtx1);
  tetcomplex21fliponedge(plex, vtx3, vtx1, vtx2);
}

#define tetcomplexsqueezeonhalfedge(plex, v1, v2, v3)  \
  if (v1 < plex->nextinitindex) {  \
    star = (struct tetcomplexstar *)  \
           arraypoolfastlookup(&plex->stars, (arraypoolulong) v1);  \
    ring = link2dfindring(&plex->moleculepool, plex->cache, star->linkhead,  \
                          v1, v2);  \
    result = linkringdelete2vertices(&plex->moleculepool, ring, v1, v3);  \
    if ((result & 2) != 0) {  \
      linkringdelete(&plex->moleculepool, ring);  \
      link2ddeletevertex(&plex->moleculepool, plex->cache, star->linkhead,  \
                         &star->linktail, v1, v2);  \
    }  \
    if (result < 1) {  \
      /* Internal error. */  \
    }  \
  }

#define tetcomplexsqueezeonedge(plex, v1, v2, v3)  \
  if ((v1 & 1) == (v2 & 1)) {  \
    if ((v2 == GHOSTVERTEX) || ((v1 != GHOSTVERTEX) &&  \
                                ((v1 < v2) ^ ((v1 & 2) == (v2 & 2))))) {  \
      tetcomplexsqueezeonhalfedge(plex, v1, v2, v3);  \
    } else {  \
      tetcomplexsqueezeonhalfedge(plex, v2, v1, v3);  \
    }  \
  }

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexsqueezetriangle()   Delete a triangle from a tetcomplex, in    */
/*                                the case where both tetrahedra adjoining   */
/*                                the triangle have the same apex.           */
/*                                                                           */
/*  This is a special-purpose procedure designed to help ease and speed up   */
/*  the Bowyer-Watson algorithm.  It should not normally be used by other    */
/*  algorithms.                                                              */
/*                                                                           */
/*  To understand this procedure, it's useful to think of each edge's link   */
/*  ring not as a bunch of tetrahedra encircling an edge, but as a bunch of  */
/*  triangles encircling an edge, acting as boundaries in a polyhedral       */
/*  complex.  The Blandford et al. data structure can represent arbitrary    */
/*  polyhedral complexes (with all faces triangulated)--although to support  */
/*  efficient adjacency queries, you would need to store all the edges (not  */
/*  just half the edges, as Blandford et al. do).                            */
/*                                                                           */
/*  This procedure deletes a triangle (vtx1, vtx2, vtx3) from the complex,   */
/*  in the special case where both tetrahedra, on each side of it, have      */
/*  already been "carved out" from the Bowyer-Watson cavity.  In this case,  */
/*  the triangle itself is not Delaunay and must be deleted also.  However,  */
/*  it is in a transient state where the newly inserted vertex appears on    */
/*  both sides of the triangle.  So when the triangle is deleted, one of     */
/*  the duplicate copies of the new vertex must also be deleted.             */
/*                                                                           */
/*  One or more edges of the triangle may be deleted (with their link        */
/*  rings).  This happens when the new vertex is the only vertex that would  */
/*  be left in the link ring.                                                */
/*                                                                           */
/*  plex:  The tetcomplex to delete a triangle from.                         */
/*  vtx1, vtx2, vtx3:  The tags for the vertices of the triangle to delete.  */
/*                                                                           */
/*****************************************************************************/

void tetcomplexsqueezetriangle(struct tetcomplex *plex,
                               tag vtx1,
                               tag vtx2,
                               tag vtx3)
{
  struct tetcomplexstar *star;
  tag ring;
  int result;

  if (plex->verbosity > 3) {
    printf("        Squeezing triangle w/tags %lu %lu %lu.\n",
           (unsigned long) vtx1, (unsigned long) vtx2, (unsigned long) vtx3);
  }

  /*
  tetcomplexsqueezeonedge(plex, vtx1, vtx2, vtx3);
  tetcomplexsqueezeonedge(plex, vtx2, vtx3, vtx1);
  tetcomplexsqueezeonedge(plex, vtx3, vtx1, vtx2);
  */

  if ((vtx1 & 1) == (vtx2 & 1)) {
    if ((vtx2 == GHOSTVERTEX) ||
        ((vtx1 != GHOSTVERTEX) &&
         ((vtx1 < vtx2) ^ ((vtx1 & 2) == (vtx2 & 2))))) {
      tetcomplexsqueezeonhalfedge(plex, vtx1, vtx2, vtx3);
    } else {
      if (vtx2 < plex->nextinitindex) {
        star = (struct tetcomplexstar *)
          arraypoolfastlookup(&plex->stars, (arraypoolulong) vtx2);
        ring = link2dfindring(&plex->moleculepool, plex->cache, star->linkhead,
                              vtx2, vtx1);
        result = linkringdelete2vertices(&plex->moleculepool, ring,
                                         vtx2, vtx3);
        if ((result & 2) != 0) {
          linkringdelete(&plex->moleculepool, ring);
          link2ddeletevertex(&plex->moleculepool, plex->cache, star->linkhead,
                             &star->linktail, vtx2, vtx1);
        }
        if (result < 1) {
          printf("Oops.\n");
        }
      }
    }
  }
  if ((vtx2 & 1) == (vtx3 & 1)) {
    if ((vtx3 == GHOSTVERTEX) ||
        ((vtx2 != GHOSTVERTEX) &&
         ((vtx2 < vtx3) ^ ((vtx2 & 2) == (vtx3 & 2))))) {
      tetcomplexsqueezeonhalfedge(plex, vtx2, vtx3, vtx1);
    } else {
      tetcomplexsqueezeonhalfedge(plex, vtx3, vtx2, vtx1);
    }
  }
  if ((vtx3 & 1) == (vtx1 & 1)) {
    if ((vtx1 == GHOSTVERTEX) ||
        ((vtx3 != GHOSTVERTEX) &&
         ((vtx3 < vtx1) ^ ((vtx3 & 2) == (vtx1 & 2))))) {
      tetcomplexsqueezeonhalfedge(plex, vtx3, vtx1, vtx2);
    } else {
      tetcomplexsqueezeonhalfedge(plex, vtx1, vtx3, vtx2);
    }
  }

}

void tetcomplex12flipon3edges(struct tetcomplex *plex,
                              tag vtx1,
                              tag vtx2,
                              tag vtx3,
                              tag vtx4,
                              tag newvertex)
{
  if (plex->verbosity > 3) {
    printf("        Inserting vertex into stars of 3 edges:\n");
    printf("          Tags %lu %lu %lu %lu; new vertex %lu.\n",
           (unsigned long) vtx1, (unsigned long) vtx2, (unsigned long) vtx3,
           (unsigned long) vtx4, (unsigned long) newvertex);
  }

  tetcomplex12fliponedge(plex, vtx1, vtx2, vtx3, vtx4, newvertex);
  tetcomplex12fliponedge(plex, vtx1, vtx3, vtx4, vtx2, newvertex);
  tetcomplex12fliponedge(plex, vtx1, vtx4, vtx2, vtx3, newvertex);
  if ((vtx1 == GHOSTVERTEX) || (vtx2 == GHOSTVERTEX) ||
      (vtx3 == GHOSTVERTEX) || (vtx4 == GHOSTVERTEX)) {
    /* A ghost tetrahedron was absorbed into the cavity. */
    plex->ghosttetcount--;
  } else {
    /* A solid tetrahedron was absorbed into the cavity. */
    plex->tetcount--;
  }
}

void tetcomplex12flipon6edges(struct tetcomplex *plex,
                              tag vtx1,
                              tag vtx2,
                              tag vtx3,
                              tag vtx4,
                              tag newvertex)
{
  if (plex->verbosity > 3) {
    printf("        Inserting vertex into stars of 6 edges (tetrahedron):\n");
    printf("          Tags %lu %lu %lu %lu; new vertex %lu.\n",
           (unsigned long) vtx1, (unsigned long) vtx2, (unsigned long) vtx3,
           (unsigned long) vtx4, (unsigned long) newvertex);
  }

  tetcomplex12fliponedge(plex, vtx1, vtx2, vtx3, vtx4, newvertex);
  tetcomplex12fliponedge(plex, vtx1, vtx3, vtx4, vtx2, newvertex);
  tetcomplex12fliponedge(plex, vtx1, vtx4, vtx2, vtx3, newvertex);
  tetcomplex12fliponedge(plex, vtx2, vtx3, vtx1, vtx4, newvertex);
  tetcomplex12fliponedge(plex, vtx2, vtx4, vtx3, vtx1, newvertex);
  tetcomplex12fliponedge(plex, vtx3, vtx4, vtx1, vtx2, newvertex);
  if ((vtx4 == GHOSTVERTEX) || (vtx3 == GHOSTVERTEX) ||
      (vtx2 == GHOSTVERTEX) || (vtx1 == GHOSTVERTEX)) {
    /* The cavity start with the deletion of a ghost tetrahedron. */
    plex->ghosttetcount--;
  } else {
    /* The cavity start with the deletion of a solid tetrahedron. */
    plex->tetcount--;
  }
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplex14flip()   Replace one tetrahedron with four (bistellar flip). */
/*                                                                           */
/*  The tetrahedron (vtx1, vtx2, vtx3, vtx4) is replaced by the tetrahedra   */
/*  (newvertex, vtx2, vtx3, vtx4), (vtx1, newvertex, vtx3, vtx4),            */
/*  (vtx1, vtx2, newvertex, vtx4), and (vtx1, vtx2, vtx3, newvertex).        */
/*  Equivalently, `newvertex' is lazily inserted in the tetrahedron.         */
/*                                                                           */
/*  This procedure is equivalent to                                          */
/*                                                                           */
/*    tetcomplexdeletetet(plex, vtx1, vtx2, vtx3, vtx4);                     */
/*    tetcomplexinserttet(plex, newvertex, vtx2, vtx3, vtx4);                */
/*    tetcomplexinserttet(plex, vtx1, newvertex, vtx3, vtx4);                */
/*    tetcomplexinserttet(plex, vtx1, vtx2, newvertex, vtx4);                */
/*    tetcomplexinserttet(plex, vtx1, vtx2, vtx3, newvertex);                */
/*                                                                           */
/*  only faster, and with less error checking.                               */
/*                                                                           */
/*  This procedure is the inverse of tetcomplex41flip().                     */
/*                                                                           */
/*  WARNING:  The tetcomplex `plex' must not contain any tetrahedra that     */
/*  have `newvertex' for a vertex.  (In other words, the star of `newvertex' */
/*  must be empty before the call.)  This procedure does not check whether   */
/*  the complex actually contains the tetrahedron (vtx1, vtx2, vtx3, vtx4),  */
/*  and may fail catastrophically if it does not.                            */
/*                                                                           */
/*  plex:  The tetcomplex containing the original tetrahedron.               */
/*  vtx1, vtx2, vtx3, vtx4:  The tags for the vertices of the tetrahedron to */
/*    flip.                                                                  */
/*  newvertex:  The tag for the new vertex to insert.                        */
/*                                                                           */
/*****************************************************************************/

void tetcomplex14flip(struct tetcomplex *plex,
                      tag vtx1,
                      tag vtx2,
                      tag vtx3,
                      tag vtx4,
                      tag newvertex)
{
  tag vtx[4];
  tag tagarray[3];
  struct tetcomplexstar *star;
  proxipoolulong allocindex;
  tag vtxowner, vtxowned;
  tag ring;
  int i;

  if (plex->verbosity > 2) {
    printf("      Transforming one tetrahedron to four (vertex insertion).\n");
    if (plex->verbosity > 3) {
      printf("        Tags %lu %lu %lu %lu; new vertex tag %lu.\n",
             (unsigned long) vtx1, (unsigned long) vtx2, (unsigned long) vtx3,
             (unsigned long) vtx4, (unsigned long) newvertex);
    }
  }

#ifdef SELF_CHECK
  if ((newvertex == vtx1) || (newvertex == vtx2) ||
      (newvertex == vtx3) || (newvertex == vtx4) ||
      (vtx1 == vtx2) || (vtx1 == vtx3) || (vtx1 == vtx4) ||
      (vtx2 == vtx3) || (vtx2 == vtx4) || (vtx3 == vtx4)) {
    printf("Internal error in tetcomplex14flip():\n");
    printf("  Asked to perform a flip with two identical vertices.\n");
    internalerror();
  }

  if (newvertex < plex->nextinitindex) {
    /* Look up the star of vertex `newvertex'. */
    star = tetcomplexlookup3dstar(plex, newvertex);
    if (!link2disempty(&plex->moleculepool, star->linkhead)) {
      printf("Internal error in tetcomplex14flip():\n");
      printf("  New vertex's star is not empty before the flip.\n");
      internalerror();
    }
  }
#endif /* SELF_CHECK */

  /* Insert six new triangles in the stars of the six edges of the original */
  /*   tetrahedron.                                                         */
  tetcomplex12fliponedge(plex, vtx1, vtx2, vtx3, vtx4, newvertex);
  tetcomplex12fliponedge(plex, vtx1, vtx3, vtx4, vtx2, newvertex);
  tetcomplex12fliponedge(plex, vtx1, vtx4, vtx2, vtx3, newvertex);
  tetcomplex12fliponedge(plex, vtx2, vtx3, vtx1, vtx4, newvertex);
  tetcomplex12fliponedge(plex, vtx2, vtx4, vtx3, vtx1, newvertex);
  tetcomplex12fliponedge(plex, vtx3, vtx4, vtx1, vtx2, newvertex);

  /* Create the four new edges and their link rings. */
  vtx[0] = vtx1;
  vtx[1] = vtx2;
  vtx[2] = vtx3;
  vtx[3] = vtx4;
  for (i = 0; i < 4; i++) {
    /* Does the edge (newvertex, vtx[i]) store a link ring?  (Parity check.) */
    if ((newvertex & 1) == (vtx[i] & 1)) {
      /* Yes.  Which of the two vertices is responsible for storing it? */
      if ((vtx[i] == GHOSTVERTEX) ||
          ((newvertex != GHOSTVERTEX) &&
           ((newvertex < vtx[i]) ^ ((newvertex & 2) == (vtx[i] & 2))))) {
        /* `newvertex' stores a new link ring containing three tetrahedra. */
        vtxowner = newvertex;
        vtxowned = vtx[i];
        tagarray[0] = vtx[i ^ 3];
        tagarray[1] = vtx[i ^ 2];
        tagarray[2] = vtx[i ^ 1];
      } else {
        /* `vtx[i]' stores a new link ring containing three tetrahedra.     */
        vtxowner = vtx[i];
        vtxowned = newvertex;
        tagarray[0] = vtx[i ^ 1];
        tagarray[1] = vtx[i ^ 2];
        tagarray[2] = vtx[i ^ 3];
      }

      /* Create the new link ring for edge (vtxowner, vtxowned), using the */
      /*   same allocation index that was used to allocate `vtxowner'.     */
      if (plex->vertexpool == (struct proxipool *) NULL) {
        allocindex = 0;
      } else {
        allocindex = proxipooltag2allocindex(plex->vertexpool, vtxowner);
      }
      ring = linkringnewfill(&plex->moleculepool, vtxowner, tagarray,
                             (proxipoolulong) 3, allocindex);
      /* Make sure `vtxowner' has a 2D link, and find it. */
      star = tetcomplexlookup3dstar(plex, vtxowner);
      /* Store the link ring in 'vtxowner's 2D link. */
      link2dinsertvertex(&plex->moleculepool, plex->cache, star->linkhead,
                         &star->linktail, vtxowner, vtxowned, ring);
    }
  }

  if (newvertex == GHOSTVERTEX) {
    /* The flip replaces one solid tetrahedron with four ghost tetrahedra. */
    plex->tetcount--;
    plex->ghosttetcount += 4;
  } else if ((vtx1 == GHOSTVERTEX) || (vtx2 == GHOSTVERTEX) ||
             (vtx3 == GHOSTVERTEX) || (vtx4 == GHOSTVERTEX)) {
    /* The flip replaces one ghost tetrahedron with one solid tetrahedron */
    /*   and three ghost tetrahedra.                                      */
    plex->tetcount++;
    plex->ghosttetcount += 2;
  } else {
    /* The flip replaces one solid tetrahedron with four. */
    plex->tetcount += 3;
  }

  /*^^^ Assertion:  The tetrahedral complex is now consistent. */
#ifdef PARANOID
  tetcomplexconsistency(plex);
#endif /* PARANOID */
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplex23flip()   Replace two tetrahedra with three (bistellar flip). */
/*                                                                           */
/*  The tetrahedra (vtxtop, vtx1, vtx2, vtx3) and (vtx1, vtx2, vtx3, vtxbot) */
/*  are replaced by the tetrahedra (vtx1, vtx2, vtxtop, vtxbot),             */
/*  (vtx2, vtx3, vtxtop, vtxbot), and (vtx3, vtx1, vtxtop, vtxbot).  This    */
/*  creates one new edge, (vtxtop, vtxbot).  It also deletes one triangle,   */
/*  (vtx1, vtx2, vtx3), and replaces it with three new triangles, all        */
/*  adjoining the new edge.                                                  */
/*                                                                           */
/*  This procedure is equivalent to                                          */
/*                                                                           */
/*    tetcomplexdeletetet(plex, vtxtop, vtx1, vtx2, vtx3);                   */
/*    tetcomplexdeletetet(plex, vtx1, vtx2, vtx3, vtxbot);                   */
/*    tetcomplexinserttet(plex, vtxtop, vtx1, vtx2, vtxbot);                 */
/*    tetcomplexinserttet(plex, vtxtop, vtx2, vtx3, vtxbot);                 */
/*    tetcomplexinserttet(plex, vtxtop, vtx3, vtx1, vtxbot);                 */
/*                                                                           */
/*  only faster, and with less error checking.                               */
/*                                                                           */
/*  This procedure is the inverse of tetcomplex32flip().                     */
/*                                                                           */
/*  WARNING:  This procedure does not check whether the complex actually     */
/*  contains either of the tetrahedra to be deleted, and may fail            */
/*  catastrophically if it does not.                                         */
/*                                                                           */
/*  plex:  The tetcomplex containing the original tetrahedra.                */
/*  vtx1, vtx2, vtx3:  The tags for the vertices of the deleted triangle.    */
/*  vtxbot, vtxtop:  The tags for the vertices of the newly created edge.    */
/*                                                                           */
/*****************************************************************************/

void tetcomplex23flip(struct tetcomplex *plex,
                      tag vtxtop,
                      tag vtx1,
                      tag vtx2,
                      tag vtx3,
                      tag vtxbot)
{
  tag tagarray[3];
  struct tetcomplexstar *star;
  proxipoolulong allocindex;
  tag vtxowner, vtxowned;
  tag ring;

  if (plex->verbosity > 2) {
    printf("      Transforming two tetrahedra to three.\n");
    if (plex->verbosity > 3) {
      printf("        Tags %lu %lu %lu / %lu %lu.\n",
             (unsigned long) vtx1, (unsigned long) vtx2, (unsigned long) vtx3,
             (unsigned long) vtxbot, (unsigned long) vtxtop);
    }
  }

#ifdef SELF_CHECK
  if ((vtx1 == vtx2) || (vtx2 == vtx3) || (vtx3 == vtx1) ||
      (vtx1 == vtxbot) || (vtx2 == vtxbot) || (vtx3 == vtxbot) ||
      (vtx1 == vtxtop) || (vtx2 == vtxtop) || (vtx3 == vtxtop) ||
      (vtxbot == vtxtop)) {
    printf("Internal error in tetcomplex23flip():\n");
    printf("  Asked to perform a flip with two identical vertices.\n");
    internalerror();
  }
#endif /* SELF_CHECK */

  /* Delete the triangle (vtx1, vtx2, vtx3) from the stars of all three */
  /*   of its edges.                                                    */
  tetcomplex21fliponedge(plex, vtx1, vtx2, vtx3);
  tetcomplex21fliponedge(plex, vtx2, vtx3, vtx1);
  tetcomplex21fliponedge(plex, vtx3, vtx1, vtx2);

  /* Insert the triangles (vtxtop, vtx1, vtxbot), (vtxtop, vtx2, vtxbot),  */
  /*   (vtxtop, vtx3, vtxbot) into the stars of their edges, excepting the */
  /*   edge (vtxtop, vtxbot), which we must create anew.                   */
  tetcomplex12fliponedge(plex, vtxtop, vtx1, vtx2, vtx3, vtxbot);
  tetcomplex12fliponedge(plex, vtxtop, vtx2, vtx3, vtx1, vtxbot);
  tetcomplex12fliponedge(plex, vtxtop, vtx3, vtx1, vtx2, vtxbot);
  tetcomplex12fliponedge(plex, vtxbot, vtx1, vtx3, vtx2, vtxtop);
  tetcomplex12fliponedge(plex, vtxbot, vtx2, vtx1, vtx3, vtxtop);
  tetcomplex12fliponedge(plex, vtxbot, vtx3, vtx2, vtx1, vtxtop);

  /* Does the edge (vtxtop, vtxbot) store a link ring?  (Parity check.) */
  if ((vtxtop & 1) == (vtxbot & 1)) {
    /* Yes.  Which of the two vertices is responsible for storing it? */
    if ((vtxbot == GHOSTVERTEX) ||
        ((vtxtop != GHOSTVERTEX) &&
         ((vtxtop < vtxbot) ^ ((vtxtop & 2) == (vtxbot & 2))))) {
      /* `vtxtop' stores a new link ring containing three new tetrahedra. */
      vtxowner = vtxtop;
      vtxowned = vtxbot;
      tagarray[0] = vtx1;
      tagarray[1] = vtx2;
      tagarray[2] = vtx3;
    } else {
      /* `vtxbot' stores a new link ring containing three new tetrahedra. */
      vtxowner = vtxbot;
      vtxowned = vtxtop;
      tagarray[0] = vtx3;
      tagarray[1] = vtx2;
      tagarray[2] = vtx1;
    }

    /* Create the new link ring for edge (vtxowner, vtxowned), using the */
    /*   same allocation index that was used to allocate `vtxowner'.     */
    if (plex->vertexpool == (struct proxipool *) NULL) {
      allocindex = 0;
    } else {
      allocindex = proxipooltag2allocindex(plex->vertexpool, vtxowner);
    }
    ring = linkringnewfill(&plex->moleculepool, vtxowner, tagarray,
                           (proxipoolulong) 3, allocindex);
    /* Make sure `vtxowner' has a 2D link, and find it. */
    star = tetcomplexlookup3dstar(plex, vtxowner);
    /* Store the link ring in 'vtxowner's 2D link. */
    link2dinsertvertex(&plex->moleculepool, plex->cache, star->linkhead,
                       &star->linktail, vtxowner, vtxowned, ring);
  }

  if ((vtxtop == GHOSTVERTEX) || (vtxbot == GHOSTVERTEX)) {
    /* The flip replaces one solid tetrahedron and one ghost tetrahedron */
    /*   with three ghost tetrahedra.                                    */
    plex->tetcount--;
    plex->ghosttetcount += 2;
  } else {
    /* Either the flip replaces two solid tetrahedra with three, or it     */
    /*   replaces two ghost tetrahedra with two different ghost tetrahedra */
    /*   and one solid tetrahedron.                                        */
    plex->tetcount++;
  }

  /*^^^ Assertion:  The tetrahedral complex is now consistent. */
#ifdef PARANOID
  tetcomplexconsistency(plex);
#endif /* PARANOID */
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplex32flip()   Replace three tetrahedra with two (bistellar flip). */
/*                                                                           */
/*  The tetrahedra (vtx1, vtx2, vtxtop, vtxbot), (vtx2, vtx3, vtxtop,        */
/*  vtxbot), and (vtx3, vtx1, vtxtop, vtxbot) are replaced by the tetrahedra */
/*  (vtxtop, vtx1, vtx2, vtx3) and (vtx1, vtx2, vtx3, vtxbot).  This deletes */
/*  one edge, (vtxtop, vtxbot).  It also deletes three triangles adjoining   */
/*  that edge, and replaces them with a new triangle, (vtx1, vtx2, vtx3).    */
/*                                                                           */
/*  This procedure is equivalent to                                          */
/*                                                                           */
/*    tetcomplexdeletetet(plex, vtxtop, vtx1, vtx2, vtxbot);                 */
/*    tetcomplexdeletetet(plex, vtxtop, vtx2, vtx3, vtxbot);                 */
/*    tetcomplexdeletetet(plex, vtxtop, vtx3, vtx1, vtxbot);                 */
/*    tetcomplexinserttet(plex, vtxtop, vtx1, vtx2, vtx3);                   */
/*    tetcomplexinserttet(plex, vtx1, vtx2, vtx3, vtxbot);                   */
/*                                                                           */
/*  only faster, and with less error checking.                               */
/*                                                                           */
/*  This procedure is the inverse of tetcomplex23flip().                     */
/*                                                                           */
/*  WARNING:  This procedure does not check whether the complex actually     */
/*  contains any of the tetrahedra to be deleted, and may fail               */
/*  catastrophically if it does not.                                         */
/*                                                                           */
/*  plex:  The tetcomplex containing the original tetrahedra.                */
/*  vtx1, vtx2, vtx3:  The tags for the vertices of the new triangle.        */
/*  vtxbot, vtxtop:  The tags for the vertices of the deleted edge.          */
/*                                                                           */
/*****************************************************************************/

void tetcomplex32flip(struct tetcomplex *plex,
                      tag vtxtop,
                      tag vtx1,
                      tag vtx2,
                      tag vtx3,
                      tag vtxbot)
{
  struct tetcomplexstar *star;
  tag ring;

  if (plex->verbosity > 2) {
    printf("      Transforming three tetrahedra to two.\n");
    if (plex->verbosity > 3) {
      printf("        Tags %lu %lu %lu / %lu %lu.\n",
             (unsigned long) vtx1, (unsigned long) vtx2, (unsigned long) vtx3,
             (unsigned long) vtxbot, (unsigned long) vtxtop);
    }
  }

#ifdef SELF_CHECK
  if ((vtx1 == vtx2) || (vtx2 == vtx3) || (vtx3 == vtx1) ||
      (vtx1 == vtxbot) || (vtx2 == vtxbot) || (vtx3 == vtxbot) ||
      (vtx1 == vtxtop) || (vtx2 == vtxtop) || (vtx3 == vtxtop) ||
      (vtxbot == vtxtop)) {
    printf("Internal error in tetcomplex32flip():\n");
    printf("  Asked to perform a flip with two identical vertices.\n");
    internalerror();
  }
#endif /* SELF_CHECK */

  /* Delete the triangles (vtxtop, vtx1, vtxbot), (vtxtop, vtx2, vtxbot),  */
  /*   (vtxtop, vtx3, vtxbot) from the stars of their edges, excepting the */
  /*   edge (vtxtop, vtxbot), which we shall delete in one blow.           */
  tetcomplex21fliponedge(plex, vtxtop, vtx1, vtxbot);
  tetcomplex21fliponedge(plex, vtxtop, vtx2, vtxbot);
  tetcomplex21fliponedge(plex, vtxtop, vtx3, vtxbot);
  tetcomplex21fliponedge(plex, vtxbot, vtx1, vtxtop);
  tetcomplex21fliponedge(plex, vtxbot, vtx2, vtxtop);
  tetcomplex21fliponedge(plex, vtxbot, vtx3, vtxtop);

  /* Insert the triangle (vtx1, vtx2, vtx3) into the stars of all three */
  /*   of its edges.                                                    */
  tetcomplex12fliponedge(plex, vtx1, vtx2, vtxtop, vtxbot, vtx3);
  tetcomplex12fliponedge(plex, vtx2, vtx3, vtxtop, vtxbot, vtx1);
  tetcomplex12fliponedge(plex, vtx3, vtx1, vtxtop, vtxbot, vtx2);

  /* Does the edge (vtxtop, vtxbot) store a link ring?  (Parity check.) */
  if ((vtxtop & 1) == (vtxbot & 1)) {
    /* Yes.  Which of the two vertices is responsible for storing it? */
    if ((vtxbot == GHOSTVERTEX) ||
        ((vtxtop != GHOSTVERTEX) &&
         ((vtxtop < vtxbot) ^ ((vtxtop & 2) == (vtxbot & 2))))) {
      if (vtxtop >= plex->nextinitindex) {
        printf("Internal error in tetcomplex32flip():\n");
        printf("  Central edge (to be deleted by a 3-2 flip) is missing.\n");
        internalerror();
      }

      /* Delete edge (vtxtop, vtxbot) and its link ring from `vtxtop's star. */
      star = (struct tetcomplexstar *)
             arraypoolfastlookup(&plex->stars, (arraypoolulong) vtxtop);
      ring = link2ddeletevertex(&plex->moleculepool, plex->cache,
                                star->linkhead, &star->linktail,
                                vtxtop, vtxbot);
    } else {
      if (vtxbot >= plex->nextinitindex) {
        printf("Internal error in tetcomplex32flip():\n");
        printf("  Central edge (to be deleted by a 3-2 flip) is missing.\n");
        internalerror();
      }

      /* Delete edge (vtxtop, vtxbot) and its link ring from `vtxbot's star. */
      star = (struct tetcomplexstar *)
             arraypoolfastlookup(&plex->stars, (arraypoolulong) vtxbot);
      ring = link2ddeletevertex(&plex->moleculepool, plex->cache,
                                star->linkhead, &star->linktail,
                                vtxbot, vtxtop);
    }
    if (ring == STOP) {
      printf("Internal error in tetcomplex32flip():\n");
      printf("  Central edge (to be deleted by a 3-2 flip) is missing.\n");
      internalerror();
    } else {
      linkringdelete(&plex->moleculepool, ring);
    }
  }

  if ((vtxtop == GHOSTVERTEX) || (vtxbot == GHOSTVERTEX)) {
    /* The flip replaces three ghost tetrahedra with one solid tetrahedron */
    /*   and one ghost tetrahedron.                                        */
    plex->tetcount++;
    plex->ghosttetcount -= 2;
  } else {
    /* Either the flip replaces three solid tetrahedra with two, or it    */
    /*   replaces two ghost tetrahedra and one solid tetrahedron with two */
    /*   different ghost tetrahedra.                                      */
    plex->tetcount--;
  }

  /*^^^ Assertion:  The tetrahedral complex is now consistent. */
#ifdef PARANOID
  tetcomplexconsistency(plex);
#endif /* PARANOID */
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplex41flip()   Replace four tetrahedra with one (bistellar flip).  */
/*                                                                           */
/*  The tetrahedra (newvertex, vtx2, vtx3, vtx4), (vtx1, newvertex, vtx3,    */
/*  vtx4), (vtx1, vtx2, newvertex, vtx4), and (vtx1, vtx2, vtx3, newvertex)  */
/*  are replaced by the tetrahedron (vtx1, vtx2, vtx3, vtx4).  Equivalently, */
/*  `deletevertex' is deleted from inside a tetrahedron.                     */
/*                                                                           */
/*  This procedure is equivalent to                                          */
/*                                                                           */
/*    tetcomplexinserttet(plex, deletevertex, vtx2, vtx3, vtx4);             */
/*    tetcomplexinserttet(plex, vtx1, deletevertex, vtx3, vtx4);             */
/*    tetcomplexinserttet(plex, vtx1, vtx2, deletevertex, vtx4);             */
/*    tetcomplexinserttet(plex, vtx1, vtx2, vtx3, deletevertex);             */
/*    tetcomplexdeletetet(plex, vtx1, vtx2, vtx3, vtx4);                     */
/*                                                                           */
/*  only faster, and with less error checking.                               */
/*                                                                           */
/*  This procedure is the inverse of tetcomplex14flip().                     */
/*                                                                           */
/*  WARNING:  This procedure does not check whether the complex actually     */
/*  contains any of the tetrahedra to be deleted, and may fail               */
/*  catastrophically if it does not.                                         */
/*                                                                           */
/*  plex:  The tetcomplex containing the original tetrahedra.                */
/*  vtx1, vtx2, vtx3, vtx4:  The tags for the vertices of the tetrahedron    */
/*    created by the flip.                                                   */
/*  deletevertex:  The tag for the vertex to delete.                         */
/*                                                                           */
/*****************************************************************************/

void tetcomplex41flip(struct tetcomplex *plex,
                      tag vtx1,
                      tag vtx2,
                      tag vtx3,
                      tag vtx4,
                      tag deletevertex)
{
  struct tetcomplexstar *star;
  tag ring;
  tag vtxi;
  int i;

  if (plex->verbosity > 2) {
    printf("      Transforming four tetrahedra to one (vertex deletion).\n");
    if (plex->verbosity > 3) {
      printf("        Tags %lu %lu %lu %lu; deleted vertex tag %lu.\n",
             (unsigned long) vtx1, (unsigned long) vtx2, (unsigned long) vtx3,
             (unsigned long) vtx4, (unsigned long) deletevertex);
    }
  }

#ifdef SELF_CHECK
  if ((deletevertex == vtx1) || (deletevertex == vtx2) ||
      (deletevertex == vtx3) || (deletevertex == vtx4) ||
      (vtx1 == vtx2) || (vtx1 == vtx3) || (vtx1 == vtx4) ||
      (vtx2 == vtx3) || (vtx2 == vtx4) || (vtx3 == vtx4)) {
    printf("Internal error in tetcomplex41flip():\n");
    printf("  Asked to perform a flip with two identical vertices.\n");
    internalerror();
  }
#endif /* SELF_CHECK */

  /* Delete the triangles adjoining `deletevertex' from the stars of the */
  /*   surviving edges.                                                  */
  tetcomplex21fliponedge(plex, vtx1, vtx2, deletevertex);
  tetcomplex21fliponedge(plex, vtx1, vtx3, deletevertex);
  tetcomplex21fliponedge(plex, vtx1, vtx4, deletevertex);
  tetcomplex21fliponedge(plex, vtx2, vtx3, deletevertex);
  tetcomplex21fliponedge(plex, vtx2, vtx4, deletevertex);
  tetcomplex21fliponedge(plex, vtx3, vtx4, deletevertex);

  /* Delete the four edges adjoining `deletevertex' and their link rings. */
  for (i = 1; i <= 4; i++) {
    vtxi = (i <= 2) ? ((i == 1) ? vtx1 : vtx2) : ((i == 3) ? vtx3 : vtx4);
    /* Does the edge (newvertex, vtxi) store a link ring?  (Parity check.) */
    if ((deletevertex & 1) == (vtxi & 1)) {
      /* Yes.  Which of the two vertices is responsible for storing it? */
      ring = STOP;
      if ((vtxi == GHOSTVERTEX) ||
          ((deletevertex != GHOSTVERTEX) &&
           ((deletevertex < vtxi) ^ ((deletevertex & 2) == (vtxi & 2))))) {
        /* `deletevertex' stores the link ring.  Delete the ring. */
        if (deletevertex < plex->nextinitindex) {
          star = (struct tetcomplexstar *)
                 arraypoolfastlookup(&plex->stars,
                                     (arraypoolulong) deletevertex);
          ring = link2ddeletevertex(&plex->moleculepool, plex->cache,
                                    star->linkhead, &star->linktail,
                                    deletevertex, vtxi);
        }
      } else {
        /* `vtxi' stores the link ring.  Delete the ring. */
        if (vtxi < plex->nextinitindex) {
          star = (struct tetcomplexstar *)
                 arraypoolfastlookup(&plex->stars, (arraypoolulong) vtxi);
          ring = link2ddeletevertex(&plex->moleculepool, plex->cache,
                                    star->linkhead, &star->linktail,
                                    vtxi, deletevertex);
        }
      }

      if (ring == STOP) {
        printf("Internal error in tetcomplex41flip():\n");
        printf("  An edge to be deleted by a 4-1 flip is missing.\n");
        internalerror();
      } else {
        linkringdelete(&plex->moleculepool, ring);
      }
    }
  }

  if (deletevertex == GHOSTVERTEX) {
    /* The flip replaces four ghost tetrahedra with one solid tetrahedron. */
    plex->tetcount++;
    plex->ghosttetcount -= 4;
  } else if ((vtx1 == GHOSTVERTEX) || (vtx2 == GHOSTVERTEX) ||
             (vtx3 == GHOSTVERTEX) || (vtx4 == GHOSTVERTEX)) {
    /* The flip replaces one solid tetrahedron and three ghost tetrahedra */
    /*   with one ghost tetrahedron.                                      */
    plex->tetcount--;
    plex->ghosttetcount -= 2;
  } else {
    /* The flip replaces four solid tetrahedra with one. */
    plex->tetcount -= 3;
  }

  /*^^^ Assertion:  The tetrahedral complex is now consistent. */
#ifdef PARANOID
  tetcomplexconsistency(plex);
#endif /* PARANOID */
}

void tetcomplexbuild3dstar(struct tetcomplex *plex,
                           tag newvertex,
                           struct arraypool *trianglelist,
                           arraypoolulong firstindex,
                           arraypoolulong trianglecount)
{
  struct tetcomplexstar *star;
  struct tetcomplexlinktriangle *triangle, *traveltri;
  tag tagarray[512];
  tag origin;
  tag vtx1, vtx2;
  tag ring;
  proxipoolulong allocindex;
  proxipoolulong tagindex;
  proxipoolulong i;
  arraypoolulong triindex;
  arraypoolulong travelindex;
  arraypoolulong orgneighbor;
  int edge;
  int traveledge;

  for (triindex = firstindex; triindex < firstindex + trianglecount;
       triindex++) {
    triangle = (struct tetcomplexlinktriangle *)
               arraypoolfastlookup(trianglelist, triindex);
    for (edge = 0; edge < 3; edge++) {
      origin = triangle->vtx[(edge == 2) ? 0 : edge + 1];
      if ((origin & 1) == (newvertex & 1)) {
        orgneighbor = triangle->neighbor[edge];
        if (orgneighbor < ~orgneighbor) {
          if ((origin == GHOSTVERTEX) ||
              ((newvertex != GHOSTVERTEX) &&
               ((newvertex < origin) ^ ((newvertex & 2) == (origin & 2))))) {
            vtx1 = newvertex;
            vtx2 = origin;
          } else {
            vtx1 = origin;
            vtx2 = newvertex;
          }

          tagindex = 0;
          traveledge = edge;
          traveltri = triangle;
          do {
            tagarray[tagindex] = traveltri->vtx[traveledge];
            tagindex++;
            if (vtx1 == newvertex) {
              traveltri->neighbor[traveledge] =
                ~traveltri->neighbor[traveledge];
              travelindex = traveltri->neighbor[(traveledge == 0) ?
                                                2 : traveledge - 1];
              travelindex = (travelindex < ~travelindex) ?
                            travelindex : ~travelindex;
              traveledge = travelindex & 3;
            } else {
              travelindex = traveltri->neighbor[traveledge];
              traveltri->neighbor[traveledge] = ~travelindex;
              traveledge = travelindex & 3;
              traveledge = (traveledge == 2) ? 0 : traveledge + 1;
            }
            traveltri = (struct tetcomplexlinktriangle *)
                        arraypoolfastlookup(trianglelist, travelindex >> 2);
          } while (traveltri != triangle);

          if (plex->verbosity > 3) {
            printf("        Creating link ring for edge w/tags %lu %lu:\n"
                   "         ",
                   (unsigned long) vtx1, (unsigned long) vtx2);
            for (i = 0; i < tagindex; i++) {
              printf(" %lu", (unsigned long) tagarray[i]);
            }
            printf("\n");
          }

          /* Create the new link ring for edge (vtx1, vtx2), using the */
          /*   same allocation index that was used to allocate `vtx1'. */
          if (plex->vertexpool == (struct proxipool *) NULL) {
            allocindex = 0;
          } else {
            allocindex = proxipooltag2allocindex(plex->vertexpool, vtx1);
          }
          ring = linkringnewfill(&plex->moleculepool, vtx1, tagarray,
                                 tagindex, allocindex);
          /* Make sure `vtx1' has a 2D star, and find it. */
          star = tetcomplexlookup3dstar(plex, vtx1);
          /* Store the link ring in 'vtx1's 2D link. */
          link2dinsertvertex(&plex->moleculepool, plex->cache, star->linkhead,
                             &star->linktail, vtx1, vtx2, ring);
        }

        triangle->neighbor[edge] = ~orgneighbor;
      }
    }

    if ((triangle->vtx[0] == GHOSTVERTEX) ||
        (triangle->vtx[1] == GHOSTVERTEX) ||
        (triangle->vtx[2] == GHOSTVERTEX)) {
      plex->ghosttetcount++;
    } else {
      plex->tetcount++;
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexremoveghosttets()   Deletes all ghost edges, and their stars,  */
/*                                from a tetcomplex, thereby deleting all    */
/*                                the explicitly inserted ghost tetrahedra.  */
/*                                                                           */
/*  This procedure searches the star of every vertex, rather than trying to  */
/*  do something more clever like finding one ghost tetrahedron by           */
/*  exhaustive search, then the others by walking through all the ghost      */
/*  triangles.  The advantage is that it will work correctly no matter how   */
/*  inconsistent the stars are with each other.  The disadvantage is that    */
/*  it's not as fast as it would be if implemented the clever way.           */
/*                                                                           */
/*  plex:  The tetcomplex in question.                                       */
/*                                                                           */
/*****************************************************************************/

void tetcomplexremoveghosttets(struct tetcomplex *plex)
{
  struct tetcomplexstar *newstar;
  tag starvertex;
  tag ghostring;

  if (plex->verbosity) {
    printf("  Removing ghost tetrahedra.\n");
  }

  /* Loop through all the vertices that might exist. */
  for (starvertex = 0; starvertex < plex->nextinitindex; starvertex++) {
    /* Is there a star for a vertex with tag `starvertex'? */
    newstar = (struct tetcomplexstar *)
      arraypoolfastlookup(&plex->stars, (arraypoolulong) starvertex);
    if (newstar->linkhead != STOP) {
      /* Remove the GHOSTVERTEX (and its link ring), if it is present, */
      /*   from the star of `starvertex'.                              */
      ghostring = link2ddeletevertex(&plex->moleculepool, plex->cache, 
                                     newstar->linkhead, &newstar->linktail,
                                     starvertex, GHOSTVERTEX);
      if (ghostring != STOP) {
        /* Deallocate the link ring. */
        linkringdelete(&plex->moleculepool, ghostring);
      }
    }
  }

  plex->ghosttetcount = 0;
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplextetcount()   Returns the number of solid tetrahedra a          */
/*                         tetcomplex believes it contains.                  */
/*                                                                           */
/*  Ghost tetrahedra have no effect on the return value.                     */
/*                                                                           */
/*  The return value is meaningless if the tetcomplex is not internally      */
/*  consistent (as during a run of the star splaying algorithm).             */
/*                                                                           */
/*  The return value may be wrong if the tetcomplex passed through a state   */
/*  where it was not internally consistent, and the count has not been       */
/*  refreshed since the tetcomplex became consistent again.                  */
/*                                                                           */
/*  plex:  The tetcomplex in question.                                       */
/*                                                                           */
/*  Returns the number of solid tetrahedra believed to be in the complex.    */
/*                                                                           */
/*****************************************************************************/

arraypoolulong tetcomplextetcount(struct tetcomplex *plex)
{
  return plex->tetcount;
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexghosttetcount()   Returns the number of _explicitly_inserted_  */
/*                              ghost tetrahedra a tetcomplex believes it    */
/*                              "contains".                                  */
/*                                                                           */
/*  Returns the value of an internal counter that is incremented with every  */
/*  successful explicit insertion of a ghost tetrahedron, and decremented    */
/*  with every successful explicit deletion of a ghost tetrahedron.  The     */
/*  explicit insertion or deletion of a ghost tetrahedron does not always    */
/*  change the tetcomplex data structure at all (because the GHOSTVERTEX     */
/*  might have a parity opposite to that of the other three vertices), so    */
/*  this number doesn't reflect any measurable property of the data          */
/*  structure.  The count is easily fooled, because you can repeatedly and   */
/*  "successfully" insert or delete a ghost tetrahedron that doesn't change  */
/*  the data structure.  Nevertheless, if ghost tetrahedra are inserted and  */
/*  deleted in a disciplined way, this count is the number of triangles on   */
/*  the boundary of the triangulation, and that is how the Bowyer-Watson     */
/*  implementation uses it.                                                  */
/*                                                                           */
/*  The return value is meaningless if the tetcomplex is not internally      */
/*  consistent (as it is not during a run of the star splaying algorithm).   */
/*                                                                           */
/*  The return value may be wrong if the tetcomplex passed through a state   */
/*  where it was not internally consistent, and the count has not been       */
/*  refreshed since the tetcomplex became consistent again.                  */
/*                                                                           */
/*  plex:  The tetcomplex in question.                                       */
/*                                                                           */
/*  Returns the number of ghost tetrahedra believed to be in the complex.    */
/*                                                                           */
/*****************************************************************************/

arraypoolulong tetcomplexghosttetcount(struct tetcomplex *plex)
{
  return plex->ghosttetcount;
}

/*****************************************************************************/
/*                                                                           */
/*  tetcomplexbytes()   Returns the number of bytes of dynamic memory used   */
/*                      by the tetrahedral complex.                          */
/*                                                                           */
/*  Does not include the memory for the `struct tetcomplex' record itself.   */
/*                                                                           */
/*  plex:  The tetcomplex in question.                                       */
/*                                                                           */
/*  Returns the number of dynamically allocated bytes in `plex'.             */
/*                                                                           */
/*****************************************************************************/

arraypoolulong tetcomplexbytes(struct tetcomplex *plex)
{
  return proxipoolbytes(&plex->moleculepool) +
         arraypoolbytes(&plex->stars);
}

/**                                                                         **/
/**                                                                         **/
/********* Tetrahedral complex routines end here                     *********/


