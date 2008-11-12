class Progs {

    // 1a.
    /** The sum of all integers, k, such that 1 <= k < N and 
     *  N is evenly divisible by k. */
    static int factorSum (int N) {
    	
    	int sum = 0;
    	for (int k = 1; k < N; k++ ) {
    		
    		if ((N % k) == 0)
    			sum = sum + k;
    		
    	}
    	return sum;
    }

    // 1b.
    /** Print the set of all sociable pairs whose members are all
     *  between 1 and N>=0 (inclusive) on the standard output (one pair per 
     *  line, smallest member of each pair first, with no repetitions). */
    static void printSociablePairs (int N) {
        int i = 1;
        int currFactorSum;
        int newFactorSum;
        
        while (i < N) {	
        	currFactorSum = Progs.factorSum(i);
        	newFactorSum = Progs.factorSum(currFactorSum);
        	
        	if (i != currFactorSum && currFactorSum < N && i < currFactorSum) 
        		//Prevents duplicates, indistinct integers and out-of-range integers
        		if (i == newFactorSum) 
        			System.out.println(i + " " + currFactorSum);
        	i++;
        }
    }

    // 2a.
    /** A list consisting of the elements of A followed by the
     *  the elements of B.  May modify items of A. 
     *  Don't use 'new'. */
    static IntList dcatenate(IntList A, IntList B) {
	
    	if (A.tail == null) {
    		A.tail = B;
    	} else {
    		Progs.dcatenate(A.tail, B);
    	}
    	
	return null;
    }

    // 2b.
    /** The sublist consisting of LEN items from list L,
     *  beginning with item #START (where the first item is #0). 
     *  Does not modify the original list elements. 
     *  It is an error if the desired items don't exist. */
    static IntList sublist (IntList L, int start, int len) {
    	
		for (int i = 0; i < start; i++) {
			L = L.tail;
		}
		
		if (len == 0) {
			return null;
		} else {
			return createListFromList (L, len);
		}
    }
    
    static IntList createListFromList (IntList in, int len) {
    	if (len == 0) {
    		return null;
    	} else {
    		return new IntList (in.head, createListFromList(in.tail, len - 1) );
    	}
    }

    // 2c.
    /** The sublist consisting of LEN items from list L,
     *  beginning with item #START (where the first item is #0). 
     *  May modify the original list elements. Don't use 'new'.
     *  It is an error if the desired items don't exist. */
    static IntList dsublist (IntList L, int start, int len) {
	
    	for (int i = 0; i < start; i++) {
    		L = L.tail;
    	}
    	
    	IntList startPointer = L;
    	
    	for (int i = 1; i <= len; i++) {
    		if (i == len) {
    			L.tail = null;
    			break;
    		}
    		L = L.tail;
    	}
    	
    	return startPointer;
    }

}
