/** This is a test clause **/

public class YearStuff {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		int year = 1900;
		if (year % 400 != 0) {
			if (year % 4 != 0) {
				System.out.printf ("%d is not a leap year.%n", year);
			} else {
				if (year % 100 != 0) {
					System.out.printf ("%d is a leap year.%n", year);
			} else {
				System.out.printf ("%d is not a leap year.%n", year);
			}
		}
		} else {
			System.out.printf ("%d is a leap year.%n", year);
		}
			
		
	} 

}
