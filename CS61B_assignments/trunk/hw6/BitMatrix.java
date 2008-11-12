// DO NOT CHANGE THIS FILE!

/** A BitMatrix represents an NxN matrix of 1's and 0's in which all
 *  1's occur before all 0's in each row.  */
public class BitMatrix {

  /** An NxN BitMatrix, initially all 0s. */
  public BitMatrix (int n) {
    firstZero = new int[n];
  }

  /** Returns n, where THIS is an nxn BitMatrix. */
  public int size () {
    return firstZero.length;
  }

  /** The contents of (ROW, COL) in THIS BitMatrix. */
  public int get (int row, int col) {
    if (col >= firstZero[row]) 
      return 0;
    else
      return 1;
  }

  /** Set the contents of (ROW, COL) in THIS to VAL, treating all 
   *  non-zero VALs as if they were 1.  Modify other values in the
   *  same as little as possible to guarantee that all 1's still come
   *  before all 0's. */
  /** NOTE: Your solution should not use this method.  It's here so
   *  that you (and we) can build BitMatrices for testing purposes. */
  public void set (int row, int col, int val) {
    if (val != 0)
      val = 1;

    if (val == 0)
      firstZero[row] = Math.min (firstZero[row], col);
    else
      firstZero[row] = Math.max (firstZero[row], col+1);
  }

  private int[] firstZero;
}

    
    
  
