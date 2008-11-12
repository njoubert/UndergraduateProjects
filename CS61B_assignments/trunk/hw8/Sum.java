// HW #8, Problem 5

import java.util.*;

public class Sum {

  /** True iff A[i]+B[j] = m for some i and j. */
  public static boolean sumsTo (int[] A, int[] B, int m) {
	  int countA = 0, countB = B.length - 1, a, b;
	  Arrays.sort(A);
	  Arrays.sort(B);
	  while (countA < A.length && countB >= 0) {
		  a = A[countA];
		  b = B[countB];
		  if (m - a == b) return true;
		  else {
			  countA = findJustSmaller(A, countA, m-b);
		  }
		  if (m - a == b) return true;
		  if (m - b == a) return true;
		  else {
			  countB = findJustBigger(B, countB, m-a);
		  }

		  if (a > b) return false;
	  }
    return false;  // REPLACE WITH YOUR ANSWER
  }
  
  /* Return POSITION IN ARRAY! */

  private static int findJustSmaller(int[] A, int count, int val) {
//	find next value (bigger) just below val
	  for (int i = count; i < A.length; i++) {
		  if (A[i] >= val) 
			  if (i == count) return i;
			  else return i + 1;
	  }
	  return A.length;
  }
  private static int findJustBigger(int[] B, int count, int val) {
	  for (int i = count; i > 0; i--) {
		  if (B[i] < val) return i + 1;
	  }
	  return 0;
//	find smaller value just biggen than val
  }
  
  public static void main (String[] args) {
	  int[] A = {1, 2, 5, 7, 12, 15, 21, 28};
	  int[] B = {1, 2, 3, 4, 10, 11, 12, 15};
	  System.out.println(sumsTo(A, B, 40));
  }

}
