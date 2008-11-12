
public class mytests {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
			mytests.printSociablePairs(5000);
			 
	}
	
	 static void printSociablePairs (int N) {

	    	for ( ; N >= 0; N = N - 1) {

	    		int pair = Progs.factorSum(N);

	    		int numberN = Progs.factorSum(pair);

	    		if (pair == numberN) {

	    			if (pair < N) {

	        			System.out.println("(" + pair + ", " + N + ")");

	        		}

	    		}

	    	}

	    }


}
