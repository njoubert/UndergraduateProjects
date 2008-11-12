import java.util.ArrayList;
import java.util.*;

class HeapStuff {

  /** This main program is started like this:
   *
   *      java HeapStuff X V0 V1 V2 V3 ...
   *
   *  It puts the strings Vi into a heap, and then prints out all values
   *  that are >= X (String comparison). */

  public static void main (String[] args) {
    Heap<String> H = new Heap<String> ();
    for (int i = 0; i < args.length; i += 1)
      H.add (args[i]);
    H.printLarger (args[0]);
  }

}

/** A Heap ordered with largest element first */
class Heap<T extends Comparable<? super T>> {
  /** The heap data.  The children of element # K in data are at
   *  locations 2K+1 and 2K+2, when those locations are valid (K>=0).
   */
  private final ArrayList<T> data = new ArrayList<T> ();

  void add (T x) {
    data.add (x);
    reheapifyUp (data.size () - 1);
  }

  /** Print all values in THIS that are >= X on the 
   *  standard output, one per line. */
  void printLarger (T x) {
	  if (data.get(0).compareTo(x) > 0) {
		  System.out.println(data.get(0));
		  printLargerPart(1, 2, x);
	  }
  }

  private void printLargerPart (int leftIndex, int rightIndex, T x) {
	  if (leftIndex < data.size() && data.get(leftIndex).compareTo(x) >= 0) {
		  System.out.println(data.get(leftIndex));
		  printLargerPart(2*leftIndex + 1, 2*leftIndex + 2, x);
	  }
	  if (rightIndex < data.size() && data.get(rightIndex).compareTo(x) >= 0) {
		  System.out.println(data.get(rightIndex));
		  printLargerPart(2*rightIndex + 1, 2*rightIndex + 2, x);
	  }
  }
  
  
  /** Restore the heap property, assuming that it may be violated only
   *  at K (a position in the array 'data') and K's parent. */
  private void reheapifyUp (int k) {
    T v = data.get (k);
    for (int p = k; k > 0; k = p) {
      p = (k-1)/2;
      if (v.compareTo (data.get (p)) > 0)
	data.set (k, data.get (p));
      else
	break;
    }
    data.set (k, v);
  }

}
