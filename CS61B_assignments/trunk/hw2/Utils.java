import java.io.*;
import java.util.*;
import java.util.Arrays;

/* NOTE: This doesn't get turned in. */

/** Various functions for dealing with arrays:
 *  * int[] B; ... B = Utils.readIntArray();
 *       will, for example, read the input
 *          [ 1, 2, 3 ]
 *       from System.in and set B to point to the 3-element array 
 *       whose elements are 1, 2, and 3.
 *
 *  * int[][] B; ... B = Utils.readIntArray2();
 *       reads a 2-D array (array of array of ints) from System.in,
 *       in the form
 *           [ [ 1, 2, 3 ], [ 0, 12, 14 ], [ 42 ] ]
 *       Each inner [] segment is one row.  Rows need not be the same 
 *       length.
 *  * Utils.print(A)
 *       writes a representation of A to System.out. A may be an int[]
 *       or int[][].
 *  * Utils.equals (A, B), where A and B are 1- or 2-D arrays of ints
 *       is true iff both represent the same sequences of values.
 *  * Utils.toString (A), converts to String with printed form.

 *  * IntList B; ... B = Utils.readIntList();
 *       will, for example, read the input
 *          [ 1, 2, 3 ]
 *       from System.in and set B to point to the 3-element list 
 *       whose elements are 1, 2, and 3.
 *
 *  * IntList2 B; ... B = Utils.readIntList2();
 *       reads a 2-D list (list of list of ints) from System.in,
 *       in the form
 *           [ [ 1, 2, 3 ], [ 0, 12, 14 ], [ 42 ] ]
 *       Each inner [] segment is one row.  Rows need not be the same 
 *       length.
 *
 *  * Utils.reverse (A), 
 *  * Utils.dreverse (A) (destructive).
 *  * Utils.toList (V) (convert int[] to IntList, int[][] to IntList2).
 *  * Utils.equals (L1, L2) equality test (L2 can also be an array).
 */

public class Utils {

  /* ARRAYS */

  /** Read an array of integers from System.in (the standard input).
   *  The input must consist of a left curly brace ("["), followed by a 
   *  sequence of integers separated by commas or blanks, followed by a 
   *  closing right curly brace ("]").  Returns null if an end-of-file
   *  or input format error occurs. */
  public static int[] readIntArray () {
    return readIntArray (new Scanner (System.in));
  }

  /** As for readIntArray(), but reads from IN. */
  public static int[] readIntArray (Scanner in) {
    String data = 
      in.findWithinHorizon ("\\[\\s*((\\d+\\s*,?\\s*)*)\\]|(\\S)", 0);
    
    if (data == null || in.match ().group (1) == null)
      return null;

    StringTokenizer ints = 
      new StringTokenizer (in.match ().group (1), ", \t\n");
    
    int[] A = new int[ints.countTokens ()];
    for (int i = 0; ints.hasMoreTokens (); i += 1)
      A[i] = Integer.parseInt (ints.nextToken ());
    return A;
  }

  /** Read an array of arrays of ints from System.in.  The input
   *  has the form of a sequence of arrays of double (in the format 
   *  required by readDoubleArray, above) separated by commas or whitespace,
   *  and surrounded in braces ("[", "]").  For example
   *     [ [ 0, 1 ], [ ], [ 3, 1, 4 ] ]
   *  is an array of three rows, the first of length 2, the second of length
   *  0 and the third of length 3. */
  public static int[][] readIntArray2 () {
    return readIntArray2 (new Scanner (System.in));
  }
    
  public static int[][] readIntArray2 (Scanner in) {
    if (in.findWithinHorizon ("\\S", 0) == null
	|| ! in.match ().group ().equals ("["))
      return null;

    List<int[]> result = new ArrayList<int[]> ();

    while (true) {
      int[] item = readIntArray (in);
      if (item == null)
	break;
      result.add (item);
    }

    return result.toArray (new int[result.size ()][]);
  }

  /** A Java array initializer that gives A. */
  public static String toString (int[] A) {
    return java.util.Arrays.toString (A);
  }    

  /** Write A to the standard output. */
  public static void print (int[] A) {
    System.out.print (toString (A));
  }

  /** A Java array initializer that gives A. */
  public static String toString (int[][] A) {
    return Arrays.deepToString (A);
  }

  /** Write A to the standard output. */
  public static void print (int[][] A) {
    System.out.print (toString (A));
  }

  /** The subarray of A consisting of the LEN items starting at index K. */
  public static int[] subarray (int[] A, int k, int len) {
    int[] result = new int[len];
    System.arraycopy (A, k, result, 0, len);
    return result;
  }

  /** True iff the sequence of items in A0 of length LEN>=0 starting at #K0 is 
   *  the same as the sequence in A1 starting at #K1.   Returns false if
   *  the items to be compared are missing on either array. */
  public static boolean equals (int[] A0, int k0, int[] A1, int k1, int len) {
    if (k0 < 0 || k1 < 0 || k0 + len > A0.length || k1 + len > A1.length)
      return false;
    else
      return equals (subarray (A0, k0, len), subarray (A1, k1, len));
  }

  /** True iff the sequences of items in A and B are equal. */
  public static boolean equals (int[] A, int[] B) {
    return Arrays.equals (A, B);
  }

  /** True iff the sequences of arrays in A and B are equal. */
  public static boolean equals (int[][] A, int[][] B) {
    return Arrays.deepEquals (A, B);
  }

  /* INTLISTS */

  static IntList IntList_EOF = new IntList (0, null);
  static IntList2 IntList2_EOF = new IntList2 (null, null);

  /** Read an list of integers from System.in (the standard input).
   *  The input must consist of a left curly brace ("["), followed by a 
   *  sequence of integers separated by commas or blanks, followed by a 
   *  closing right curly brace ("]").  Returns IntList_EOF if an 
   *  end-of-file or input format error occurs. */
  public static IntList readIntList () {
    return readIntList (new Scanner (System.in));
  }

  /** As for readIntList(), but reads from IN. */
  public static IntList readIntList (Scanner in) {
    int[] A = readIntArray (in);
    if (A == null)
      return IntList_EOF;
    else
      return toList (readIntArray (in));
  }

  /** The IntList whose sequence of items is the same as that in A. */
  public static IntList toList (int[] A) {
    return toList (A, 0, A.length-1);
  }

  /** The IntList whose sequence of items is the same as that in
   *  items L through U of array A. */
  public static IntList toList (int[] A, int L, int U) {
    IntList result;
    result = null;
    for (int i = U; i >= L; i -= 1)
      result = new IntList (A[i], result);
    return result;
  }

  /** Read an list of integers from System.in (the standard input).
   *  The input must consist of a left curly brace ("["), followed by a 
   *  sequence of integers separated by commas or blanks, followed by a 
   *  closing right curly brace ("]").  Returns IntList_EOF if an 
   *  end-of-file or input format error occurs. */
  public static IntList2 readIntList2 () {
    return readIntList2 (new Scanner (System.in));
  }

  /** As for readIntList2(), but reads from IN. */
  public static IntList2 readIntList2 (Scanner in) {
    int[][] A = readIntArray2 (in);
    if (A == null)
      return IntList2_EOF;
    else
      return toList (readIntArray2 (in));
  }

  /** The IntList2 whose sequence of items is the same as that in A. */
  public static IntList2 toList (int[][] A) {
    return toList (A, 0, A.length-1);
  }

  /** The IntList2 whose sequence of items is the same as that in
   *  items L through U of array A. */
  public static IntList2 toList (int[][] A, int L, int U) {
    IntList2 result;
    result = null;
    for (int i = U; i >= L; i -= 1)
      result = new IntList2 (toList (A[i]), result);
    return result;
  }

  public static String toString (IntList A) {
    StringBuffer result = new StringBuffer ();
    result.append("{ ");
    for (IntList L = A; L != null; L = L.tail) {
      if (L != A)
	result.append(", ");
      result.append(L.head);
    }
    result.append(" }");
    return result.toString ();
  }    

  /** The number of elements in L. */
  public static int length (IntList L) {
    int n;
    IntList p;
    for (p = L, n = 0; p != null; p = p.tail, n += 1)
      ;
    return n;
  }

  /** The number of elements in L. */
  public static int length (IntList2 L) {
    int n;
    IntList2 p;
    for (p = L, n = 0; p != null; p = p.tail, n += 1)
      ;
    return n;
  }

  /** The reverse of L. */
  public static IntList reverse (IntList L) {
    IntList R;
    for (R = null; L != null; L = L.tail)
      R = new IntList (L.head, R);
    return R;
  }

  /** The destructive reverse of L; original L is destroyed. */
  public static IntList dreverse (IntList L) {
    IntList R, Lt;
    for (R = null; L != null; L = Lt)
      {
	Lt = L.tail;
	L.tail = R;
	R = L;
      }
    return R;
  }

  /** The reverse of L. */
  public static IntList2 reverse (IntList2 L) {
    IntList2 R;
    for (R = null; L != null; L = L.tail)
      R = new IntList2 (L.head, R);
    return R;
  }

  /** The destructive reverse of L; original L is destroyed. */
  public static IntList2 dreverse (IntList2 L) {
    IntList2 R, Lt;
    for (R = null; L != null; L = Lt)
      {
	Lt = L.tail;
	L.tail = R;
	R = L;
      }
    return R;
  }

  /** True iff the sequence of items in A is the same as the sequence in 
   *  B. */
  public static boolean equals (IntList A, int[] B) {
    if (A == null)
      return B.length == 0;
    else
      return A.equals (B);
  }

  public static boolean equals (IntList2 A, int[][] B) {
    if (A == null)
      return B.length == 0;
    else
      return A.equals (B);
  }

  /** True iff the sequence of items in A is the same as the sequence in 
   *  B. */
  public static boolean equals (IntList A, IntList B) {
    if (A == null)
      return B == null;
    else
      return A.equals (B);
  }

  public static boolean equals (IntList2 A, IntList2 B) {
    if (A == null)
      return B == null;
    else
      return A.equals (B);
  }

}
