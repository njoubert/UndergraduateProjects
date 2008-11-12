/** Scheme-like pairs that can be used to form a list of 
 *  integers. */
public class IntList {
  public int head;
  public IntList tail;

  /** A List with head and tail. */
  public IntList (int head, IntList tail) {
    this.head = head; this.tail = tail;
  }

  /** A List with null tail, and head = 0. */
  public IntList () { this (0, null); }
  // NOTE: public IntList () { }  would also work.

  /** A new IntList containing the ints in ARGS. */
  public static IntList list (Integer ... args) {
    IntList sentinel = new IntList (0, null);

    IntList p;
    p = sentinel;
    for (Integer x : args)
      p = p.tail = new IntList (x, null);
    return sentinel.tail;
  }

  /** A new IntList containing the ints A. */
  public static IntList list (int[] A) {
    IntList sentinel = new IntList (0, null);

    IntList p;
    p = sentinel;
    for (int x : A)
      p = p.tail = new IntList (x, null);
    return sentinel.tail;
  }


  /** True iff X is an IntList or int[] containing the same sequence of ints
   *  as THIS. */
  public boolean equals (Object x) {
    if (x instanceof IntList) {
      IntList L = (IntList) x;
      IntList p;
      for (p = this; p != null && L != null; p = p.tail, L = L.tail) {
	if (p.head != L.head)
	  return false;
      }
      if (p == null && L == null)
	return true;
    } else if (x instanceof int[]) {
      int[] A = (int[]) x;
      IntList p;
      int i;
      for (i = 0, p = this; i < A.length && p != null; i += 1) {
	if (A[i] != p.head)
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
    for (IntList L = this; L != null; L = L.tail) 
      b.append (" " + L.head); 
    b.append ("]");
    return b.toString ();
  }

}
