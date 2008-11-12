// HW #9, Problem 2

/** A set of non-empty disjoint sets of integers whose union is a contiguous
 *  range of integers.  At any time, each set of integers is identified 
 *  with a single <dfn>representative member</dfn> of the set.  The class
 *  provides operations to map an integer to the representative member
 *  of its current set (find), and to merge two sets into one (union).  These 
 *  operations are implemented such that any sequence of M unions and finds
 *  requires time proportional to M times a very slowly growing function
 *  of M. */
public class UnionFind {

  /** A union-find structure of the integers 0 .. N-1.  Initially, each
   *  integer is the sole (and therefore representative) member of
   *  a distinct set. */
  public UnionFind (int N) {
    // FILL THIS IN
  }

  /** The representative member of the set K belongs to. */
  public int find (int k) {
    // REPLACE WITH SOLUTION
    return k;
  }

  /** Merge the set containing K1 with the set containing K2, returning
   *  the representative member of the resulting set. */
  public int union (int k1, int k2) {
    // REPLACE WITH SOLUTION
    return k1;
  }

  /** The number of distinct sets in THIS. */
  public int numSets () {
    // REPLACE WITH SOLUTION
    return 0;
  }

  /** The number of integers in the union of all sets in THIS (thus, 
   *  the value given to the constructor). */
  public int size () {
    // REPLACE WITH SOLUTION
    return 0;
  }
    
  // FILL IN WITH PRIVATE DATA

}
