import java.util.Iterator;
import java.util.LinkedList;
import java.util.*;
/** A kind of object that acts like a void function on some type
 *  T. */
interface Procedure<T> {
   /** Execute THIS on ARG. */
   void apply (T arg);
}

class Add implements Procedure<int[]> {
	
	public void apply (int[] arg) {
		for (int i = 0; i < arg.length; i += 1) {
			arg[i] = arg[i] + 1;
		}
	}
}

/** A generator of permutations of an interval of integers. */
public class Permuter implements Iterator<int[]> {
	
   /** A generator of permutations of 0..N-1. */
   public Permuter (int N) {
      // FILL IN
   }

   /** True if there is another permutation in this generator. */
   public boolean hasNext () {
      // REPLACE WITH SOLUTION
      return false;
   }

   /** The next permutation of 0..N-1. */
   public int[] next () {
      // REPLACE WITH SOLUTION
      return null;
   }

   /** This operation is not supported on Permuter. */
   public void remove () {
      throw new UnsupportedOperationException ();
   }

   /** Apply P to all permutations of 0..N-1, starting at
    *  next ().  When done, hasNext () will be false. */
   public void map (Procedure<int[]> P) {
      while (this.hasNext()) {
    	  P.apply(this.next());
      }
   }
}

class PermutationPairs {

  /** The command 'java PermutationPairs N' prints all pairs of
   *  permutations of 0..N-1 on the standard output in the format
   *     { 0, 1, 2 }, { 0, 2, 1 }
   *  one per line. */
  public static void main (String[] args) {
    int N = Integer.parseInt (args[0]);
    // FILL IN
  }

}
