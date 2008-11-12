import java.util.Random;

class SortTesting {

  /** When run with java SortTesting N K, creates a pseudo-random 
   *  permutation of the integers 0 .. N-1, uses quicksort to 
   *  partially sort them, stopping the recursion at arrays of size <= K, 
   *  and then sorts using insertion sort.  It reports the time taken 
   *  to do this. For each value of N, the pseudo-random numbers used
   *  are the same each time for reproducible results. */
  public static void main (String[] args) {
    int N = Integer.parseInt (args[0]), K = Integer.parseInt (args[1]);
    
    int[] nums = createRandomPermutation (N);

    long start = System.currentTimeMillis ();
    quicksort (nums, 0, N-1, K);
    insertionSort (nums, 0, N-1);
    long end = System.currentTimeMillis ();
    check (nums);
    System.out.printf ("Sorted %d numbers in %.3f seconds with K=%d%n",
		       N, (end-start) * 0.001, K);
  }

  /** Sort A[L..U] via quicksort, stopping the recursion when A[L..U]
   *  has <= K items in it, where K > 0.  */
  static void quicksort (int[] A, int L, int U, int K) {
    if (U - L < Math.max (1, K))
      return;
    int M = pivot (A, L, U, choosePivot (A, L, U));
    quicksort (A, L, M-1, K);
    quicksort (A, M+1, U, K);
  }

  /** Assuming L <= U, choose i such that L <= i <= U, attempting
   *  to make A[i] a suitable pivot for A[L..U] in the quicksort
   *  algorithm. */
  private static int choosePivot (int[] A, int L, int U) {
    int M = (L+U)/2;
    if (A[L] < A[M]) {
      if (A[M] < A[U])
	return M;
      else if (A[L] < A[U])
	return U;
      else
	return L;
    } else if (A[L] < A[U])
      return L;
    else if (A[M] < A[U])
      return U;
    else
      return M;
  }

  /** Exchange A[I] and A[J]. */
  private static void swap (int[] A, int i, int j) {
    int t = A[i]; A[i] = A[j]; A[j] = t;
  }

  /** Assuming L <= kp <= U, find M, L <= M <= U, and permute A[L..U]
   *  so that all elements A[L..M-1] are < P, A[M..U] are >= P, 
   *  and A[M] == P, where P is the original value of A[kp].  Returns M. */
  private static int pivot (int[] A, int L, int U, int kp) {
    /* Method ascribed to Nico Lomuto. */
    int M;
    int P = A[kp];
    swap (A, kp, L);
    M = L;
    for (int i = L+1; i <= U; i += 1) {
      /* L <= M < i <= U and A[L+1..M] are all < P, 
	 A[M..i-1] are all >= P, and A[L] == P. */
      if (A[i] < P) {
	M += 1;
	swap (A, i, M);
      }
    }
    swap (A, L, M);
    return M;
  }


  /** Sort A[L..U]. */
  static void insertionSort (int A[], int L, int U) {
    for (int i = L+1; i <= U; i += 1) {
      int w = A[i];
      int j;
      for (j = i-1; j >= L && A[j] > w; j -= 1) {
	A[j+1] = A[j];
      }
      A[j+1] = w;
    }
  }

  /** A seed value that determines the pseudo-random number sequence. */
  private static long seed = 314159265L;

  /** An array containing a pseudo-random permutation of 0 .. N-1. */
  private static int[] createRandomPermutation (int N) {
    Random gen = new Random (seed);
    int[] result = new int[N];
    for (int i = 0; i < N; i += 1)
      result[i] = i;
    for (int k = N-1; k > 0; k -= 1)
      swap (result, k, gen.nextInt (k+1));
    return result;
  }

  /** Check that A is 0 .. N-1. */
  private static void check (int[] A) {
    for (int i = 0; i < A.length; i += 1)
      if (A[i] != i)
	throw new AssertionError ("Element " + i + " is out of order");
  }
}
      



