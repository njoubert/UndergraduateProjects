/* NOTE: The file ArrayUtil.java contains some functions that may be useful
 * in testing your answers. */

/** HW #2, Problem #2. */

class Arrays {
  // a.
  /** A new array consisting of the elements of A followed by the
   *  the elements of B. */
  static int[] catenate(int[] A, int[] B) {
    /* *Replace this body with the solution. */
	  int[] returnInt = new int[(A.length + B.length)];
	  for (int i = 0; i < A.length; i += 1) {
		  returnInt[i] = A[i];
	  }
	  for (int i = A.length; i < returnInt.length; i += 1) {
		  returnInt[i] = B[i - A.length];
	  }
    return returnInt;
  }

  // b.
  /** The array formed by removing LEN items from A,
   *  beginning with item #START. */
  static int[] remove (int[] A, int start, int len) {
    /* *Replace this body with the solution. */
	  if (len + start > A.length) len = A.length - start;
	  int[] returnInt = new int[A.length - len];
	  for (int i = 0; i < start; i += 1) {
		  returnInt[i] = A[i];
	  }
	  for (int i = start + len; i < A.length; i += 1) {
		  returnInt[i - len] = A[i]; 
	  }
    return returnInt;
  }

  // c.
  /** The array of arrays formed by breaking up A into 
   *  maximal ascending lists, without reordering.  
   *  For example, if A is {1, 3, 7, 5, 4, 6, 9, 10}, then
   *  returns the three-element array
   *  {{1, 3, 7}, {5}, {4, 6, 9, 10}}. */
  
  static int[][] naturalRuns (int[] A) {
	  
	  int subArrays = numberOfSubArrays(A);
	  
	  int[][] returnInt = new int[subArrays][];
	  
	  for (int i = 0; i < subArrays; i += 1) {
		  returnInt[i] = remove(A, lengthOfFirstSubArray(A), A.length);
		  A = remove(A, 0, returnInt[i].length);
	  }
	  return returnInt;
  }
  
  static int numberOfSubArrays(int[] A) {
	  int rolloverTimes = 1;
	  for (int i = 0; i < A.length - 1; i += 1) {
		  if (A[i+1] < A[i]) rolloverTimes += 1;
	  }
	  return rolloverTimes;
  }

  static int lengthOfFirstSubArray(int[] A) {
	  for (int i = 0; i < A.length - 1; i += 1) {
		  if (A[i+1] < A[i]) return i + 1;
		  }
	  return A.length;
  }
}
  
