/** HW 5, Problem 7 */

class BigMat {

  /** The number, r, of the row of A that contains the most 1's.  When
   *  there is more than one such row, the smallest index. */
  public static int mostOnes (BitMatrix A) {
    int currentMax = -1; //The index of the first column in a row that contains a zero
    int currentIndex = -1; //The index os the row with the most ones.
    // run through all the rows, each time checking the current biggest column.
    for (int rowi = 0; rowi < A.size(); rowi++) { 
    	if (A.get(rowi, currentMax) > 0) {
    		
    		currentIndex = rowi;
    		for (int coli = currentMax + 1; coli < A.size(); coli++) {
    			if (A.get(rowi, coli) == 0) {
    				currentMax = coli;
    				break;
    			}
    		}
    		
    	}
    } 
    return currentIndex;
  }
}

