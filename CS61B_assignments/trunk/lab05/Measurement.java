import java.util.*;

public class Measurement implements Comparable {

	private int totalinches;
	
	public int compareTo(Object o) {
		if ((((Measurement) o).getFeet() * 12 + ((Measurement) o).getInches()) < totalinches) {
			return 1;
		} else if ((((Measurement) o).getFeet() * 12 + ((Measurement) o).getInches()) == totalinches) {
			return 0;
		} else {
			return -1;
		}
			
	}
	/** Initialize Measurement with zero length  */
	public Measurement() {
		totalinches = 0;
	}
	/** Initialize Measurement with the given amount of feet */
	public Measurement(int feet) {
		totalinches = 12 * feet;
	}
	
	/** Initialize Measurement with the given amount of feet and inches */
	public Measurement(int ft, int inches) {
		totalinches = (12 * ft) + inches;
	}
	
	/** Adds the measurement of the argument object to the current object
	 * non-destructively
	 * @param Mobj
	 * @return Measurement object
	 */
	public Measurement plus(Measurement Mobj) {
		return new Measurement(0, Mobj.totalinches + this.totalinches);
		
	}
	/** Subtracts the value of the parameter measurement object from the current object
	 * non-destructively
	 * @param Mobj
	 * @return Measurement object
	 */
	public Measurement minus(Measurement Mobj){
		return new Measurement(0, this.totalinches - Mobj.totalinches);
	}
	/** non-destructively multiplies the given Measurement object's 
	 * measurement value with the argument integer.
	 * @param amount
	 * @return Measurement object
	 */
	public Measurement multiply(int amount) {
		return new Measurement(0, this.totalinches * amount);
	}
	
	public String toString() {
		return new String(this.getFeet() + "\'" + this.getInches() + "\"");
	}

	/** return the amount of inches of the stored feet and inches value */
	public int getFeet() {
		return totalinches / 12;
	}
	/** return the amount of feet of the stored feet and inches value*/
	public int getInches() {
		return totalinches % 12;
	}
	public boolean equals(Measurement MObj) {
		if (MObj.totalinches == this.totalinches) return true;
		else return false;
	}
	
	public static void main (String [ ] args) {
	    Measurement [ ] marray = new Measurement [4];
	    marray[0] = new Measurement (5, 2); // 5' 2"
	    marray[1] = new Measurement (2, 9); // 2' 9"
	    marray[2] = new Measurement (6, 3); // 6' 3"
	    marray[3] = new Measurement (0, 11); // 11"
	    Arrays.sort (marray);
	    for (int k=0; k<marray.length; k++) {
	        System.out.println (marray[k]);
	    }
	    // should print the measurements in ascending order:
	    // 0' 11", 2' 9", 5' 2", and 6' 3".
	}
}
