
public class MMjar {

	/**
	 * This is the M&M Jar Simulator Class
	 * @author Niels Joubert cs61b-bo
	 */
	private int amountOfMMs;
	
	MMjar (int startMMs) {
		/** 
		 * This method initializes the object with the correct number of M&Ms
		 * @startMMs amount of MMs to initialize method with
		 */
		amountOfMMs = startMMs;
	}
	
	void add (int amount) {
		/**
		 * adds M&Ms to the jar the object is simulating
		 * @return void
		 * @amount specifies the amount of M&Ms to add to the jar
		 */
		amountOfMMs = amountOfMMs + amount;
	}
	
	void eat (int amount) {
		/**
		 * removes M&Ms from the jar the object is simulating.
		 * @amount specifies the amount of M&Ms to remove.
		 */
		amountOfMMs = amountOfMMs - amount;
		if (amountOfMMs < 100) System.out.println("OH NO! There is LESS THAN 100 M&Ms left!");
	}


}
