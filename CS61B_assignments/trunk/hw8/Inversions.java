// HW #8, Problem 4.

import java.util.*;

public class Inversions {

  /** A main program for testing purposes.  Prints the number of inversions
   *  in the sequence ARGS. */
  public static void main (String[] args) {
    System.out.println (inversions (Arrays.asList (args)));
  }

  /** The number of inversions in ARGS. */
  public static <T extends Comparable<? super T>> int inversions (List<T> args)
  {
	  int count = 0;
	  Iterator argIter = args.iterator();
	  while (argIter.hasNext()) {
		  T one = argIter.next();
		  T two = argIter.next();
		  if (one.compareTo(two) > -1) count += 1;
	  }
  }

}
