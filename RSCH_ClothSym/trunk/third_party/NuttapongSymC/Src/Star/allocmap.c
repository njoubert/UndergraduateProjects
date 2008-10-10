/********* Allocation map routines begin here                        *********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  Allocation maps (struct allocmap)                                        */
/*                                                                           */
/*  An allocation map is a data structure that maps each point in 3D space   */
/*  to an "allocation index" (of type proxipoolulong), which determines      */
/*  which pool of memory data should be used to allocate structures          */
/*  associated with the point.  The allocation indices are meant for use     */
/*  with proxipools (proximity-based memory pools), though other data        */
/*  structures could be designed to use them as well.                        */
/*                                                                           */
/*  An allocation map allows a client to insert or delete "sample points."   */
/*  Each sample point is assigned a unique allocation index.  A client can   */
/*  query the allocation map with a point (not usually a sample point), and  */
/*  learn the allocation index of a sample point that is "near" the query    */
/*  point.  The purpose is to allow a client to store data structures so     */
/*  they are spatially coherent:  structures that are near each other        */
/*  geometrically also tend to be near each other in memory (because they    */
/*  are likely to have the same allocation index).                           */
/*                                                                           */
/*  A good way to use an allocation map is to have it contain points that    */
/*  are a small random sample of the entire point set processed by the       */
/*  client.  The random sample ensures that the points in the allocation map */
/*  are roughly representative of the spatial distribution of the larger     */
/*  point set.  The sample size should be chosen so perhaps thousands or     */
/*  tens of thousands of point map to a single allocation index.             */
/*                                                                           */
/*  Ideally, an allocation map would always return the allocation index of   */
/*  the sample point nearest the query point, perhaps using a dynamic        */
/*  Voronoi diagram.  But not only would that be a hassle to implement; it   */
/*  would be slower than I'd like.  Instead, I order the points along a      */
/*  space-filling z-order curve and store them in a splay tree.  The space-  */
/*  filling curve reduces the search problem to one dimension, so a simple   */
/*  data structure suffices; and splay trees run like greased weasels.  The  */
/*  region of space that maps to a sample point isn't ideally shaped, but    */
/*  it's good enough.                                                        */
/*                                                                           */
/*  A sample point can be removed from the allocation map.  Part of the      */
/*  reason for this is to support dynamically changing triangulations, in    */
/*  which vertices might just disappear.  When a sample point is removed,    */
/*  its allocation index is placed in a "spare tree," from which it can be   */
/*  reallocated again.  The goal is to make sure that the memory pools (say, */
/*  in a proxipool) associated with an allocation index don't fall into      */
/*  disuse (which would be bad, as they're still taking up memory.  When a   */
/*  sample point is inserted into the allocation tree, the code finds the    */
/*  allocation index in the spare tree whose old sample point is closest to  */
/*  the new sample point, and allocates that index to the new point.  With   */
/*  luck, the new points that map to the index might partly overlap the old  */
/*  points, still in use, that used to map to the index.  When the spare     */
/*  tree is empty, the code allocates brand spankin' new indices.            */
/*                                                                           */
/*  If there are no sample points in the allocation map, every point maps to */
/*  a default allocation index of zero.  When sample points do exist, their  */
/*  allocation indices are numbered starting from one--no sample point ever  */
/*  get assigned the allocation index zero.                                  */
/*                                                                           */
/*  Public interface:                                                        */
/*  struct allocmap   Allocation map object.                                 */
/*  void allocmapinit(tree, verbose)   Initialize an empty allocation tree.  */
/*  void allocmaprestart(tree)   Deallocates all the tree nodes (not to OS). */
/*  void allocmapdeinit()   Free an allocation tree's memory to the OS.      */
/*  proxipoolulong allocmapindex(tree, x, y, z)   Query a point, returning   */
/*    an allocation index.                                                   */
/*  proxipoolulong allocmapnewpoint(tree, x, y, z)   Insert a point.         */
/*  void allocmapdeletepoint(tree, x, y, z)   Delete a point.                */
/*  arraypoolulong allocmapbytes(tree)   Returns memory taken by tree.       */
/*                                                                           */
/*  For internal use only:                                                   */
/*  struct allocmapnode *allocmapnewnode(tree)                               */
/*  int allocmapsplay(treeroot, x, y, z, nearnode)                           */
/*  void allocmapmax(treeroot)                                               */
/*  void allocmapinsertroot(treeroot, insertnode, splayside)                 */
/*  void allocmapremoveroot(treeroot)                                        */
/*                                                                           */
/*****************************************************************************/


/*  The logarithm of the number of allocation tree nodes allocated at once.  */

#define LOG2OCTREENODESPERBLOCK 8


/*  Each `allocmapnode' is a node in the allocation tree.  Its fields        */
/*  include its `left' and `right' children and its sample point             */
/*  (xsam, ysam, zsam).  Each node has an allocation index `index', which is */
/*  the node's index in the arraypool of nodes, and is also used by          */
/*  memorypools to decide which chunks of memory to allocate objects from.   */
/*                                                                           */
/*  The node with index zero is the "default" node.  This node can never be  */
/*  part of the splay tree.  It is the node returned when the splay tree is  */
/*  empty, so that every point can be assigned an allocation index even when */
/*  there are no points in the tree.  Nodes allocated to participate in the  */
/*  tree are numbered starting from one.                                     */

struct allocmapnode {
  struct allocmapnode *left, *right;          /* My left and right children. */
  starreal xsam, ysam, zsam;     /* Coordinates of this node's sample point. */
  proxipoolulong index;                      /* My index (in the arraypool). */
};

/*  Each `allocmap' is an allocation tree, used to map a point in 3D space   */
/*  to an index that specifies which minipool (in a proxipool) an object     */
/*  should be allocated from.  The idea is that points that are spatially    */
/*  close together should be close together in memory too.  The allocation   */
/*  tree data structure is a splay tree whose points are ordered along a     */
/*  z-order space-filling curve.                                             */
/*                                                                           */
/*  The nodes are allocated from the memory pool `nodearray'.  The root of   */
/*  the allocation tree is `allocroot'.  When a node is removed from the     */
/*  allocation tree, it is inserted into the spare tree, whose root is       */
/*  `spareroot'.  Nodes in the spare tree can be reused, whereupon they are  */
/*  inserted back into the allocation tree.  The purpose of the spare tree   */
/*  is to allow reuse of allocation indices in a spatially coherent way--    */
/*  ideally, a new point in the allocation tree will receive an index that   */
/*  was previously used by a point close to it.                              */
/*                                                                           */
/*  The number "verbosity" indicates how much debugging information to       */
/*  print, from none (0) to lots (4+).                                       */

struct allocmap {
  struct arraypool nodearray;      /* Tiered array of allocation tree nodes. */
  struct allocmapnode *allocroot;                /* Root of allocation tree. */
  struct allocmapnode *spareroot;                /* Root of spare node tree. */
  int verbosity;                /* Amount of debugging information to print. */
};


/*****************************************************************************/
/*                                                                           */
/*  allocmaprestart()   Deallocate all the tree nodes in an allocmap.        */
/*                                                                           */
/*  The tree returns to its starting state, except that no memory (allocated */
/*  by the arraypool of tree nodes) is freed to the operating system.        */
/*                                                                           */
/*  tree:  The allocmap to restart.                                          */
/*                                                                           */
/*****************************************************************************/

void allocmaprestart(struct allocmap *tree)
{
  struct allocmapnode *defaultnode;
  arraypoolulong defaultindex;

  /* Empty the trees. */
  tree->allocroot = (struct allocmapnode *) NULL;
  tree->spareroot = (struct allocmapnode *) NULL;

  /* Restart the pool of tree nodes. */
  arraypoolrestart(&tree->nodearray);

  /* Allocate a "default" node (which is never in either tree) having index */
  /*   zero.                                                                */
  defaultindex = arraypoolnewindex(&tree->nodearray, (void **) &defaultnode);
  if (defaultindex != 0) {
    printf("Internal error in allocmaprestart():\n");
    printf("  First index allocated from restarted arraypool is not zero.\n");
    internalerror();
  }

  defaultnode->index = 0;
  /* Give the default node a bogus sample point. */
  defaultnode->xsam = 0.0;
  defaultnode->ysam = 0.0;
  defaultnode->zsam = 0.0;
}

/*****************************************************************************/
/*                                                                           */
/*  allocmapinit()   Initialize an empty allocation tree.                    */
/*                                                                           */
/*  tree:  The allocmap to initialize.                                       */
/*  verbose:  How much debugging information allocmap procedures should      */
/*    print, from none (0) to lots (4+).                                     */
/*                                                                           */
/*****************************************************************************/

void allocmapinit(struct allocmap *tree,
                  int verbose)
{
  /* Initialize an arraypool to allocate nodes from.  Note that this is an */
  /*   array of actual nodes, not an array of pointers to nodes.           */
  arraypoolinit(&tree->nodearray,
                (arraypoolulong) sizeof(struct allocmapnode),
                (arraypoolulong) LOG2OCTREENODESPERBLOCK, 1);
  tree->verbosity = verbose;
  /* Empty out the tree and set up the default node. */
  allocmaprestart(tree);
}

/*****************************************************************************/
/*                                                                           */
/*  allocmapdeinit()   Free to the operating system all memory taken by an   */
/*                     allocation tree.                                      */
/*                                                                           */
/*  tree:  The allocmap to free.                                             */
/*                                                                           */
/*****************************************************************************/

void allocmapdeinit(struct allocmap *tree)
{
  arraypooldeinit(&tree->nodearray);
}

/*****************************************************************************/
/*                                                                           */
/*  allocmapnewnode()   Allocate a new node in an allocation tree.           */
/*                                                                           */
/*  This routine may allocate a node that has been previously freed.  If so, */
/*  the node has the same index it had before.  (Node indices never change.) */
/*                                                                           */
/*  tree:  The allocmap to allocate a new node from.                         */
/*                                                                           */
/*  Returns a pointer to the new node.                                       */
/*                                                                           */
/*****************************************************************************/

struct allocmapnode *allocmapnewnode(struct allocmap *tree)
{
  struct allocmapnode *newnode;
  arraypoolulong newindex;

  /* Allocate a node from the arraypool. */
  newindex = arraypoolnewindex(&tree->nodearray, (void **) &newnode);
  /* Teach it its own index. */
  newnode->index = (proxipoolulong) newindex;

  return newnode;
}

/*****************************************************************************/
/*                                                                           */
/*  allocmapsplay()   This top-down splay tree code searches the tree for a  */
/*                    node with coordinates (x, y, z), and splays it (if it  */
/*                    is found) or the last node encountered to the root.    */
/*                    Nodes in the tree are ordered by a z-order space-      */
/*                    filling curve.                                         */
/*                                                                           */
/*  If the search does not find a node whose sample point is exactly         */
/*  (x, y, z), consider two nodes:  the latest node that precedes (x, y, z), */
/*  and the earliest node that follows (x, y, z).  One of these nodes,       */
/*  chosen arbitrarily, is splayed to the root (if the tree is not empty).   */
/*  If both nodes exist (i.e. (x, y, z) does not precede every node in the   */
/*  tree, nor follow every node), the other node is stored in `*nearnode' on */
/*  return.                                                                  */
/*                                                                           */
/*  If the search finds an exact match, or if (x, y, z) precedes or follows  */
/*  every node in the tree, then `*nearnode' is NULL on return.              */
/*                                                                           */
/*  treeroot:  The root of the tree in which to search, twice indirected.    */
/*    On return, `*treeroot' will point to the new root.                     */
/*  x, y, z:  The coordinates of the point whose near neighbor is sought.    */
/*  nearnode:  On return, `*nearnode' may point to a node containing an      */
/*    alternative point near (x, y, z).  Output only; `*nearnode' need not   */
/*    be initialized.                                                        */
/*                                                                           */
/*  Returns 0 if a node with value (x, y, z) is found and splayed to the     */
/*    root; -1 if (x, y, z) precedes the node splayed to the root; or 1 if   */
/*    (x, y, z) follows the node splayed to the root.  Returns 0 if the tree */
/*    is empty.                                                              */
/*                                                                           */
/*****************************************************************************/

int allocmapsplay(struct allocmapnode **treeroot,
                  starreal x,
                  starreal y,
                  starreal z,
                  struct allocmapnode **nearnode)
{
  struct allocmapnode catcher;
  struct allocmapnode *root;
  struct allocmapnode *child;
  struct allocmapnode *leftcatcher, *rightcatcher;
  int compare;

  *nearnode = (struct allocmapnode *) NULL;

  root = *treeroot;
  if (root == (struct allocmapnode *) NULL) {
    /* The tree is empty. */
    return 0;
  }

  /* `catcher' is an allocmapnode which will not be part of the final tree,  */
  /*   but is used temporarily to hold subtrees.  The subtree that will be   */
  /*   the left child of the root is stored as the right child of `catcher', */
  /*   and vice versa.  (This backward connection simplifies the algorithm.) */
  /*   `leftcatcher' is a pointer used up to build up the left subtree of    */
  /*   the root, from top to bottom right; it points to the rightmost node   */
  /*   in the subtree.  `rightcatcher' serves a symmetric purpose.           */
  leftcatcher = &catcher;
  rightcatcher = &catcher;

  /* Loop of splaying operations.  During each iteration, `root' is the node */
  /*   currently being visited, as well as the root of the subtree currently */
  /*   being searched.                                                       */
  while (1) {
    /* Compare (x, y, z) with this node's sample point.  The result is */
    /*   0 if equal; -1 if (x, y, z) precedes; 1 if (x, y, z) follows. */
    compare = ((x == root->xsam) && (y == root->ysam) && (z == root->zsam)) ?
              0 : zorderbefore(x, y, z, root->xsam, root->ysam, root->zsam) ?
              -1 : 1;
    /* Is `root' the node that will be the root in the end? */
    if ((compare == 0) ||
        ((compare < 0) && (root->left == (struct allocmapnode *) NULL)) ||
        ((compare > 0) && (root->right == (struct allocmapnode *) NULL))) {
      /* Yes, the search is over.  If `root' has children, attach them to */
      /*   the new left and right subtrees.                               */
      leftcatcher->right = root->left;
      rightcatcher->left = root->right;
      /* Attach the left and right subtrees to the new root. */
      root->left = catcher.right;
      root->right = catcher.left;
      /* The new root is `root'. */
      *treeroot = root;
      /* Find the node `nearnode' such that (x, y, z) is between `root' */
      /*   and `nearnode'.                                              */
      if ((compare < 0) && (leftcatcher != &catcher)) {
        *nearnode = leftcatcher;
      } else if ((compare > 0) && (rightcatcher != &catcher)) {
        *nearnode = rightcatcher;
      }
      return compare;
    } else if (compare < 0) {
      /* Search the left subtree of `root'. */
      child = root->left;
      /* Compare (x, y, z) with the left child's sample point. */
      compare = ((x == child->xsam) && (y == child->ysam) &&
                 (z == child->zsam)) ? 0 :
                zorderbefore(x, y, z, child->xsam, child->ysam, child->zsam) ?
                -1 : 1;
      /* Is `child' the node that will be the root in the end? */
      if ((compare == 0) ||
          ((compare < 0) && (child->left == (struct allocmapnode *) NULL)) ||
          ((compare > 0) && (child->right == (struct allocmapnode *) NULL))) {
        /* Yes, the search is over.  Perform a "zig" rotation. */
        root->left = child->right;
        /* If `child' has a left child, attach it to the new left subtree. */
        /*   Attach `root' to the new right subtree.                       */
        leftcatcher->right = child->left;
        rightcatcher->left = root;
        /* Attach the left and right subtrees to the new root. */
        child->left = catcher.right;
        child->right = catcher.left;
        /* The new root is `child'. */
        *treeroot = child;
        /* Find the node such that (x, y, z) is between `child' and */
        /*   `nearnode'.                                            */
        if (compare > 0) {
          *nearnode = root;
        } else if ((compare < 0) && (leftcatcher != &catcher)) {
          *nearnode = leftcatcher;
        }
        return compare;
      } else if (compare < 0) {
        /* The search continues to the left.  Perform a "zig-zig" rotation. */
        root->left = child->right;
        child->right = root;
        /* Attach the child to the new right subtree. */
        rightcatcher->left = child;
        rightcatcher = child;
        /* On the next iteration, examine the left-left grandchild of */
        /*   `root'.                                                  */
        root = child->left;
      } else {
        /* The search continues to the right.  Perform a "zig-zag" rotation  */
        /*   by attaching the left child to the new left subtree, and `root' */
        /*   to the new right subtree.                                       */
        leftcatcher->right = child;
        leftcatcher = child;
        rightcatcher->left = root;
        rightcatcher = root;
        /* On the next iteration, examine the left-right grandchild of */
        /*   `root'.                                                   */
        root = child->right;
      }
    } else {
      /* Search the right subtree of `root'. */
      child = root->right;
      /* Compare (x, y, z) with the right child's sample point. */
      compare = ((x == child->xsam) && (y == child->ysam) &&
                 (z == child->zsam)) ? 0 :
                zorderbefore(x, y, z, child->xsam, child->ysam, child->zsam) ?
                -1 : 1;
      /* Is `child' the node that will be the root in the end? */
      if ((compare == 0) ||
          ((compare < 0) && (child->left == (struct allocmapnode *) NULL)) ||
          ((compare > 0) && (child->right == (struct allocmapnode *) NULL))) {
        /* Yes, the search is over.  Perform a "zig" rotation. */
        root->right = child->left;
        /* If `child' has a right child, attach it to the new right subtree. */
        /*   Attach `root' to the new left subtree.                          */
        leftcatcher->right = root;
        rightcatcher->left = child->right;
        /* Attach the left and right subtrees to the new root. */
        child->left = catcher.right;
        child->right = catcher.left;
        /* The new root is `child'. */
        *treeroot = child;
        /* Find the node `nearnode' such that (x, y, z) is between `child' */
        /*   and `nearnode'.                                               */
        if (compare < 0) {
          *nearnode = root;
        } else if ((compare > 0) && (rightcatcher != &catcher)) {
          *nearnode = rightcatcher;
        }
        return compare;
      } else if (compare > 0) {
        /* The search continues to the right.  Perform a "zig-zig" rotation. */
        root->right = child->left;
        child->left = root;
        /* Attach the child to the new left subtree. */
        leftcatcher->right = child;
        leftcatcher = child;
        /* On the next iteration, examine the right-right grandchild of */
        /*   `root'.                                                    */
        root = child->right;
      } else {
        /* The search continues to the left.  Perform a "zig-zag" rotation */
        /*   by attaching `root' to the new left subtree, and the right    */
        /*   child to the new left subtree.                                */
        leftcatcher->right = root;
        leftcatcher = root;
        rightcatcher->left = child;
        rightcatcher = child;
        /* On the next iteration, examine the right-left grandchild of */
        /*   `root'.                                                   */
        root = child->left;
      }
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*  allocmapmax()   Splays the rightmost node of a tree to the root.         */
/*                                                                           */
/*  treeroot:  The root of the tree to splay, twice indirected.  On return,  */
/*    `*treeroot' will point to the new root.                                */
/*                                                                           */
/*****************************************************************************/

void allocmapmax(struct allocmapnode **treeroot)
{
  struct allocmapnode catcher;
  struct allocmapnode *root;
  struct allocmapnode *rightchild;
  struct allocmapnode *leftcatcher;

  root = *treeroot;
  if (root == (struct allocmapnode *) NULL) {
    /* The tree is empty. */
    return;
  }

  /* `catcher' is an allocmapnode which will not be part of the final tree,  */
  /*   but is used temporarily to hold a subtree.  The subtree that will be  */
  /*   the left child of the root is stored as the right child of `catcher'. */
  /*   (This backward connection simplifies the algorithm.)  `leftcatcher'   */
  /*   is a pointer used up to build up the left subtree of the root, from   */
  /*   top to bottom right; it points to the rightmost node in the subtree.  */
  leftcatcher = &catcher;

  /* Search to the far right of the tree, splaying as we go. */
  while (1) {
    rightchild = root->right;
    if (rightchild == (struct allocmapnode *) NULL) {
      /* If `root' has a left child, attach it to the new left subtree. */
      leftcatcher->right = root->left;
      /* Attach the left subtree to the new root. */
      root->left = catcher.right;
      /* The new root is `root'. */
      *treeroot = root;
      return;
    } else if (rightchild->right == (struct allocmapnode *) NULL) {
      /* The right child of `root' will be the new root.  Perform a "zig" */
      /*   rotation.                                                      */
      root->right = rightchild->left;
      /* Attach `root' to the bottom right of the new left subtree. */
      leftcatcher->right = root;
      /* Attach the left subtree to the new root. */
      rightchild->left = catcher.right;
      /* The new root is `rightchild'. */
      *treeroot = rightchild;
      return;
    } else {
      /* The search continues to the right.  Perform a "zig-zig" rotation. */
      root->right = rightchild->left;
      rightchild->left = root;
      /* Attach the child to the new left subtree. */
      leftcatcher->right = rightchild;
      leftcatcher = rightchild;
      /* On the next iteration, examine the right-right grandchild of */
      /*   `root'.                                                    */
      root = rightchild->right;
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*  allocmapinsertroot()   Finish inserting a node into a splay tree.        */
/*                                                                           */
/*  After a splay tree is splayed with allocmapsplay() on the coordinates    */
/*  (x, y, z), this procedure is used to finish inserting a node with point  */
/*  (x, y, z) into the splay tree.  The new node becomes the root of the     */
/*  tree.                                                                    */
/*                                                                           */
/*  treeroot:  The root of the tree in which to insert the new node, twice   */
/*    indirected.  On return, `*treeroot' will point to the new root.        */
/*  insertnode:  The node to insert into the tree.                           */
/*  splayside:  The value returned by allocmapsplay().  -1 if (x, y, z)      */
/*    precedes the node splayed to the root; 1 if (x, y, z) follows the node */
/*    splayed to the root; irrelevant if the tree is empty or the node       */
/*    splayed to the root is also (x, y, z).                                 */
/*                                                                           */
/*****************************************************************************/

void allocmapinsertroot(struct allocmapnode **treeroot,
                        struct allocmapnode *insertnode,
                        int splayside)
{
  /* Is the tree empty? */
  if (*treeroot == (struct allocmapnode *) NULL) {
    /* Empty tree; the new node has no children. */
    insertnode->left = (struct allocmapnode *) NULL;
    insertnode->right = (struct allocmapnode *) NULL;
  } else if (splayside < 0) {
    /* The new sample point precedes the root's sample point.  The root   */
    /*   becomes the new node's right child, and the root's (former) left */
    /*   child becomes the new node's left child.                         */
    insertnode->left = (*treeroot)->left;
    insertnode->right = *treeroot;
    (*treeroot)->left = (struct allocmapnode *) NULL;
  } else {
    /* The new sample point follows the root's sample point.  The root    */
    /*   becomes the new node's left child, and the root's (former) right */
    /*   child becomes the new node's right child.                        */
    insertnode->left = *treeroot;
    insertnode->right = (*treeroot)->right;
    (*treeroot)->right = (struct allocmapnode *) NULL;
  }
  /* The new node becomes the root. */
  *treeroot = insertnode;
}

/*****************************************************************************/
/*                                                                           */
/*  allocmapremoveroot()   Removes the root from a splay tree.               */
/*                                                                           */
/*  Does nothing if the tree is empty.                                       */
/*                                                                           */
/*  treeroot:  The root of the tree in which to insert the new node, twice   */
/*    indirected.  On return, `*treeroot' will point to the new root.        */
/*                                                                           */
/*****************************************************************************/

void allocmapremoveroot(struct allocmapnode **treeroot)
{
  struct allocmapnode *root;
  struct allocmapnode *left;
  struct allocmapnode *right;

  /* Do nothing if the tree is empty. */
  if (*treeroot != (struct allocmapnode *) NULL) {
    /* Remove the root from the tree. */
    root = *treeroot;
    left = root->left;
    right = root->right;
    if (left == (struct allocmapnode *) NULL) {
      /* Root has no left child, so its right child becomes the new root. */
      *treeroot = right;
    } else {
      *treeroot = left;
      /* Did the old root have a right child? */
      if (right != (struct allocmapnode *) NULL) {
        /* The largest entry in the left subtree becomes the new root. */
        allocmapmax(treeroot);
        /* Attach the former right subtree. */
        (*treeroot)->right = right;
      }
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*  allocmapindex()   Find the node in an allocation tree whose sample point */
/*                    is "closest" to (x, y, z), and return its index.       */
/*                                                                           */
/*  If the tree contains a node whose sample point is (x, y, z), then that   */
/*  node's index is returned.  Otherwise, returns the node whose sample      */
/*  point is immediately before or after (x, y, z) in the z-order.  If there */
/*  are two choices (the node immediately before vs. the node immediately    */
/*  after), selects the nearest one by Euclidean distance.                   */
/*                                                                           */
/*  tree:  The allocmap in which to search.                                  */
/*  x, y, z:  The coordinates of the point whose near neighbor is sought.    */
/*                                                                           */
/*  Returns the index of the found node.  If the tree is empty, returns      */
/*    zero, which acts as a default index.                                   */
/*                                                                           */
/*****************************************************************************/

proxipoolulong allocmapindex(struct allocmap *tree,
                             starreal x,
                             starreal y,
                             starreal z)
{
  struct allocmapnode *foundnode;
  struct allocmapnode *othernode;
  int splayside;

  /* If the tree is empty, the default index is zero. */
  if (tree->allocroot == (struct allocmapnode *) NULL) {
    return 0;
  }

  /* Search for (x, y, z) and splay some node to the top. */
  splayside = allocmapsplay(&tree->allocroot, x, y, z, &othernode);
  foundnode = tree->allocroot;

  if ((splayside != 0) && (othernode != (struct allocmapnode *) NULL)) {
    /* Decide which node--`foundnode' or `othernode'--is closest to */
    /*   (x, y, z) by Euclidean distance.                           */
    if ((x - othernode->xsam) * (x - othernode->xsam) +
        (y - othernode->ysam) * (y - othernode->ysam) +
        (z - othernode->zsam) * (z - othernode->zsam) <
        (x - foundnode->xsam) * (x - foundnode->xsam) +
        (y - foundnode->ysam) * (y - foundnode->ysam) +
        (z - foundnode->zsam) * (z - foundnode->zsam)) {
      return othernode->index;
    }
  }

  return foundnode->index;
}

/*****************************************************************************/
/*                                                                           */
/*  allocmapnewpoint()   Add a new sample point (x, y, z) to the allocation  */
/*                       tree.                                               */
/*                                                                           */
/*  If the point (x, y, z) is already in the tree, this procedure does not   */
/*  insert a new one.                                                        */
/*                                                                           */
/*  tree:  The allocmap in which to insert the new point.                    */
/*  x, y, z:  The coordinates of the sample point to insert.                 */
/*                                                                           */
/*  Returns the index of the new node, or the index of the preexisting node  */
/*    if (x, y, z) is already in the tree.                                   */
/*                                                                           */
/*****************************************************************************/

proxipoolulong allocmapnewpoint(struct allocmap *tree,
                                starreal x,
                                starreal y,
                                starreal z)
{
  struct allocmapnode *newnode;
  struct allocmapnode *othernode;
  int splayside = 0;

  /* If the tree is not empty, splay the tree to find a good spot to insert */
  /*   the new node.                                                        */
  if (tree->allocroot != (struct allocmapnode *) NULL) {
    splayside = allocmapsplay(&tree->allocroot, x, y, z, &othernode);
    if (splayside == 0) {
      /* The point is already in the tree. */
      return tree->allocroot->index;
    }
  }

  /* Allocate a node to hold the new sample point. */
  if (tree->spareroot == (struct allocmapnode *) NULL) {
    /* No freed nodes left.  Allocate a brand new one. */
    newnode = allocmapnewnode(tree);
  } else {
    /* Find a freed node whose sample point was near (x, y, z) and */
    /*   remove it from the spare tree.                            */
    splayside = allocmapsplay(&tree->spareroot, x, y, z, &othernode);
    newnode = tree->spareroot;
    if ((splayside != 0) && (othernode != (struct allocmapnode *) NULL)) {
      /* Decide which node--`newnode' or `othernode'--is closest to */
      /*   (x, y, z) by Euclidean distance.                         */
      if ((x - othernode->xsam) * (x - othernode->xsam) +
          (y - othernode->ysam) * (y - othernode->ysam) +
          (z - othernode->zsam) * (z - othernode->zsam) <
          (x - newnode->xsam) * (x - newnode->xsam) +
          (y - newnode->ysam) * (y - newnode->ysam) +
          (z - newnode->zsam) * (z - newnode->zsam)) {
        allocmapsplay(&tree->spareroot, othernode->xsam, othernode->ysam,
                      othernode->zsam, &othernode);
        newnode = tree->spareroot;
      }
    }

    allocmapremoveroot(&tree->spareroot);
  }
  newnode->xsam = x;
  newnode->ysam = y;
  newnode->zsam = z;

  /* The new node becomes the root of the allocation tree. */
  allocmapinsertroot(&tree->allocroot, newnode, splayside);

  return newnode->index;
}

/*****************************************************************************/
/*                                                                           */
/*  allocmapdeletepoint()   Deletes a sample point (x, y, z) from the        */
/*                          allocation tree.                                 */
/*                                                                           */
/*  Does nothing if the point (x, y, z) is not actually in the tree.         */
/*                                                                           */
/*  The allocation index associated with (x, y, z) is placed in a "spare     */
/*  tree," so it can be reused--hopefully associated with a new point that   */
/*  is close to (x, y, z).                                                   */
/*                                                                           */
/*  tree:  The allocmap from which to delete the point.                      */
/*  x, y, z:  The coordinates of the sample point to delete.                 */
/*                                                                           */
/*****************************************************************************/

void allocmapdeletepoint(struct allocmap *tree,
                         starreal x,
                         starreal y,
                         starreal z)
{
  struct allocmapnode *deletenode;
  struct allocmapnode *dummynode;
  int splayside;

  /* Do nothing if the tree is empty. */
  if (tree->allocroot != (struct allocmapnode *) NULL) {
    /* Try to splay the point (x, y, z) to the root. */
    splayside = allocmapsplay(&tree->allocroot, x, y, z, &dummynode);

    /* Do nothing if the node is not found. */
    if (splayside == 0) {
      /* Remember the deleted node. */
      deletenode = tree->allocroot;
      /* Remove it from the tree. */
      allocmapremoveroot(&tree->allocroot);

      /* If the spare tree is not empty, splay it to find a good spot to */
      /*   insert the deleted node.                                      */
      if (tree->spareroot != (struct allocmapnode *) NULL) {
        /* Find where to insert (x, y, z) in the spare tree. */
        splayside = allocmapsplay(&tree->spareroot, x, y, z, &dummynode);
      }
      /* The deleted node becomes the root of the spare tree, ready to be */
      /*   reused.                                                        */
      allocmapinsertroot(&tree->spareroot, deletenode, splayside);
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*  allocmapbytes()   Returns the size (in bytes) of the allocation tree.    */
/*                                                                           */
/*  Returns the dynamically allocated memory used by the arraypool used to   */
/*  store the nodes of the tree.  Does not include the size of the `struct   */
/*  allocmap', which is presumably part of some other object and accounted   */
/*  for there.  Note that allocmaps only release memory to the operating     */
/*  system when allocmapdeinit() is called, and the return value includes    */
/*  the memory occupied by all the spare nodes.                              */
/*                                                                           */
/*  tree:  The allocmap in question.                                         */
/*                                                                           */
/*  Returns the number of dynamically allocated bytes in `tree'.             */
/*                                                                           */
/*****************************************************************************/

arraypoolulong allocmapbytes(struct allocmap *tree)
{
  return arraypoolbytes(&tree->nodearray);
}

/**                                                                         **/
/**                                                                         **/
/********* Allocation map routines end here                          *********/


