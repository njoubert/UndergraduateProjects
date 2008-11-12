/** A list of IntLists. */
public class IntList2 {
  /** A List with head and tail. */
  public IntList2 (IntList head, IntList2 tail) {
    this.head = head; this.tail = tail;
  }

  /** A List with null head and tail. */
  public IntList2 () { this (null, null); }

  public IntList head;
  public IntList2 tail;

  /** A new IntList2 containing the ints in ARGS. */
  public static IntList2 list (IntList ... args) {
    IntList2 sentinel = new IntList2 (null, null);

    IntList2 p;
    p = sentinel;
    for (IntList x : args)
      p = p.tail = new IntList2 (x, null);
    return sentinel.tail;
  }

  /** A new IntList2 containing the lists of ints corresponding
   *  to the arrays in A. */
  public static IntList2 list (int[][] A) {
    IntList2 sentinel = new IntList2 (null, null);

    IntList2 p;
    p = sentinel;
    for (int[] x : A)
      p = p.tail = new IntList2 (IntList.list (x), null);
    return sentinel.tail;
  }

  /** True iff X is an IntList2 or int[][] containing the same sequence of ints
   *  as THIS. */
  public boolean equals (Object x) {
    if (x instanceof IntList2) {
      IntList2 L = (IntList2) x;
      IntList2 p;
      for (p = this; p != null && L != null; p = p.tail, L = L.tail) {
	if ((p.head == null && L.head != null)
	    || (p.head != null && ! p.head.equals (L.head)))
	  return false;
      }
      if (p == null && L == null)
	return true;
    } else if (x instanceof int[][]) {
      int[][] A = (int[][]) x;
      IntList2 p;
      int i;
      for (i = 0, p = this; i < A.length && p != null; i += 1) {
	if ((p.head == null && A[i].length != 0)
	    || ! p.head.equals (A[i]))
	  return false;
      }
      if (i == A.length && p == null)
	return true;
    }
    return false;
  }

  /** Convert THIS into a readable String. */
  public String toString () {
    StringBuffer b = new StringBuffer ();
    b.append ("[");
    for (IntList2 L = this; L != null; L = L.tail) 
      b.append (" " + L.head); 
    b.append ("]");
    return b.toString ();
  }


}
