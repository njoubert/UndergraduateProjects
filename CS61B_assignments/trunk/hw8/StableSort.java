import java.util.Random;

interface IntComparator {
  /** Defines an ordering on ints by returning an integer that is <, ==, 
   *  or > 0 depending on whether, X comes before, at the same place, or
   *  after Y in the ordering.  */
  int compare (int x, int y);
}

public class StableSort {

  /** Sort the array A according to the given ORDERING.  The sort is not
   *  stable. */
  static void sort (int[] A, IntComparator ordering) {
    sort (A, 0, A.length-1, ordering);
  }

  private static Random gen = new Random ();

  static void swap (int[] A, int i, int j) {
    int t = A[i]; A[i] = A[j]; A[j] = t;
  }

  static void sort (int[] A, int L, int U, IntComparator ordering) {
    if (L >= U)
      return;
    else {
      int m;
      swap (A, L, L+gen.nextInt (U - L + 1));
      int T = A[L];  /* Random element is pivot */
      m = L;
      for (int i = L+1; i <= U; i += 1) {
	if (ordering.compare (A[i], T) < 0) {
	  m += 1;
	  swap (A, i, m);
	}
      }
      swap (A, L, m);
      sort (A, L, m-1, ordering);
      sort (A, m+1, U, ordering);
    }
  }

  /** Given that D is an Nx2 array of Strings (that is, each D[i] is 
   *  a String[] with length 2), produce an array p such that
   *  D[p[0]], D[p[1]], D[p[2]], ... D[p[D.length-1]] is a stable sorting
   *  of D by the first strings.  D is not modified.   That is, 
   *  produce a p that sorts D[0], D[1], etc., by D[0][0], D[1][0], D[2][0],
   *  etc., and where two D[i] have the same first element, leaves them 
   *  in the same order they have in D.  */
  static int[] sortStably (String[][] D) {
    int[] p = new int[D.length];
    for (int i = 0; i < p.length; i += 1)
      p[i] = i;

    sort (p, null /* REPLACE WITH EXPRESSION */);
    return p;
  }

}
