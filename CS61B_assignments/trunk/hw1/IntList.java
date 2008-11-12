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
  public IntList list (Integer ... args) {
    IntList result, p;

    if (args.length > 0)
      result = new IntList (args[0], null);
    else 
      return null;

    int k;
    for (k = 1, p = result; k < args.length; k += 1, p = p.tail)
      p.tail = new IntList (args[k], null);
    return result;
  }

  /** True iff X is an IntList containing the same sequence of ints
   *  as THIS. */
  public boolean equals (Object x) {
    if (! (x instanceof IntList))
      return false;
    IntList L = (IntList) x;
    IntList p;
    for (p = this; p != null && L != null; p = p.tail, L = L.tail) {
      if (p.head != L.head)
	return false;
    }
    if (p != null || L != null)
      return false;
    return true;
  }

}
