/** A set of nonnegative ints from 0 to some fixed maximum
 *  size. */
public class SetFW {
	
	
  private boolean[] contains;
	
  /* An initially empty set of values in the range 0 .. MAXELEMENT-1. */
  public SetFW (int maxElement) {
	  contains = new boolean[maxElement];
  }
	
  /** Precondition: 0 <= K < maximum element.
   *  Postcondition: K is in this set. */
  public void insert (int k) {
		if (k < contains.length && k >= 0) {
			contains[k] = true;
		}
  }
	
  /** Precondition: 0 <= K < maximum element.
   *  Postcondition: K is not in this set. */
  public void remove (int k) {
	  if (k >= 0 && k < contains.length) {
		  contains[k] = false;
	  }
  }
	
  /** Precondition: 0 <= K < maximum element.
   *  Returns true iff K is in this set. */
  public boolean member (int k) {
	  if (k >= 0 && k < contains.length) {
		  return contains[k];
	  } else return false; 
  }
	
  /** Returns true iff this set is empty. */
  public boolean isEmpty ( ) {
	  boolean emptyTest = true;
	  for (int i = 0; i < contains.length; i += 1) {
			if (contains[i] == true) {
				emptyTest = false;
				break;
			}
		}
		return emptyTest;
  }
}
