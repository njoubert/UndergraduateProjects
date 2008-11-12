
public class TriangleDrawer {

	/**
	 * @param args
	 */
	/** This is the main method
	 * to draw a triangle of asterixes
	 * @author Niels Joubert cs61b-bo
	 * @return nothing
	 * @param none
	 */
	public static void main(String[] args) {
		int SIZE = 10;
		int row = 0;
		while (row < SIZE) {
			int col = 0;
			while (col <= row) {
				System.out.print ('*');
				col = col + 1;
			}
			System.out.println ( );
			row = row + 1;
		}

	}

}
