/** An accumulator of ints.  This object is given a sequence of
 *  int values via its accum method.  It performs some processing on
 *  this sequence, and returns a result (or partial result) when
 *  its result() method is called. */
abstract class Accum {

  /** The result of the accumulation. */
  abstract int result ();

  /** Process and accumulate the value X. */
  abstract void accum (int x);

  /** The result of accumulating all of the values in vals. */
  int reduce (int[] vals) {
    for (int x : vals) 
      accum (x);
    return result ();
  }
}

public class Quiz1 {
  /** The sum VALS'[0] + VALS'[1] + ... where VALS'[i] is the 
   *  minimum of VALS[i] and MAX. */
  public static int clippedSum (int[] vals, int max) {
    Accum acc = new Seunabal(max);
    return acc.reduce (vals);
  }
  
  public static void main (String[] args) {
	  int [] yes = {1, 2, 3, 4, 5, 6};
	  System.out.println(clippedSum(yes, 4));
  }
}

// Add any other classes you need here:

class Seunabal extends Accum {
	
	private int accumBuffer = 0;
	private int max = Integer.MAX_VALUE;
	
	public Seunabal(int maxVal) {
		max = maxVal;
	}
	
	public int result() {
		return accumBuffer;
	}
	
	public void accum(int x) {
		if (x > max) {
			accumBuffer = accumBuffer + max;
		} else {
			accumBuffer = accumBuffer + x;
		}
		
	}
 
}
