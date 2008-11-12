import java.math.BigInteger;

public class PermutationGenerator {

	  private int[] a;
	  private BigInteger numLeft;
	  private BigInteger total;

	  public PermutationGenerator (int n) {
	    a = new int[n];
	    total = getFactorial (n);
	    reset ();
	  }

	  /** resets this generator to its original state */
	  public void reset () {
	    for (int i = 0; i < a.length; i++) {
	      a[i] = i;
	    }
	    numLeft = new BigInteger (total.toString ());
	  }

	  /** returns the amout of ungenerated permutations */
	  public BigInteger getNumLeft () {
	    return numLeft;
	  }

	  /** total number of perms */
	  public BigInteger getTotal () {
	    return total;
	  }

	/** checks to see whether there's more perms left */
	  public boolean hasMore () {
	    return numLeft.compareTo (BigInteger.ZERO) == 1;
	  }

	  /** simple factorial procedure */
	  private static BigInteger getFactorial (int n) {
	    BigInteger fact = BigInteger.ONE;
	    for (int i = n; i > 1; i--) {
	      fact = fact.multiply (new BigInteger (Integer.toString (i)));
	    }
	    return fact;
	  }

	  /** This generates the next permutation. Algorithm from Rosen p. 284 */
	  public int[] getNext () {

	    if (numLeft.equals (total)) {
	      numLeft = numLeft.subtract (BigInteger.ONE);
	      return a;
	    }

	    int temp;

	    // Find largest index j with a[j] < a[j+1]

	    int j = a.length - 2;
	    while (a[j] > a[j+1]) {
	      j--;
	    }

	    // Find index k such that a[k] is smallest integer
	    // greater than a[j] to the right of a[j]

	    int k = a.length - 1;
	    while (a[j] > a[k]) {
	      k--;
	    }

	    // Interchange a[j] and a[k]

	    temp = a[k];
	    a[k] = a[j];
	    a[j] = temp;

	    // Put tail end of permutation after jth position in increasing order

	    int r = a.length - 1;
	    int s = j + 1;

	    while (r > s) {
	      temp = a[s];
	      a[s] = a[r];
	      a[r] = temp;
	      r--;
	      s++;
	    }

	    numLeft = numLeft.subtract (BigInteger.ONE);
	    return a;

	  }

	}

/** This class was made available to the public by Michael Gilleland, Merriam Park Software
* for use in the public domain. */


