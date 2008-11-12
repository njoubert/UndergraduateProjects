/** Represents an array of integers each in the range -8..7. 
 *  Such integers may be represented in 4 bits (called nybbles). */
public class Nybbles {
   /** An array of size N. */
   public Nybbles (int N) {
      data = new int[ (N / 8) + 1]; //We can store eight 4-bit vals per int
      this.N = N;
   }

   /** The size of THIS. */
   public int size () { return N; }

   /** Return the Kth integer in THIS array, numbering from 0. 
    *  Assumes 0 <= K < N. */
   public int get (int k) {
      if (k < 0 || k >= N)
	 throw new IndexOutOfBoundsException ();
      else 
    	  return data[k/8] << (32 - (4 * (k % 8)) - 4) >> 28;
	 //return (int) ((data[k / 8] >> ((k % 8) * 4)) & (0xf)); //My way, but it doesnt to minus right
   }

   /** Set the Kth integer in THIS array to VAL.  Assumes 
    *  0 <= K < N and -8 <= VAL < 8. */
   public void set (int k, int val) {
      if (k < 0 || k >= N)
	 throw new IndexOutOfBoundsException ();
      else if (val < -8 || val > 7)
	 throw new IllegalArgumentException ();
      else 
	  data[k / 8] =
	      data[k / 8] & (~ (0xf << (4 * (k % 8)))) | ((val & 0xf) << (4 * (k % 8)));
   }

   private int N;
   private int[] data;
}