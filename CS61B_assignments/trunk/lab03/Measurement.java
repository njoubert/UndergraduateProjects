
public class Measurement {

	private int totalinches;
	
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
}
