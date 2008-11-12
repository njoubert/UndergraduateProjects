
public class IntSequence {
	
    /** myItems[0] .. myItems[myCount-1] contain the elements of the 
     *  sequence. */
	int[] myItems;
    
	/** Current number of items in the sequence. */
    int myCount;

    /** Construct a new, empty IntSequence with a maximum size of
     *  CAPACITY.  Assumes CAPACITY >= 0. */
    public IntSequence (int capacity) {
        myItems = new int[capacity];
        myCount = 0;
    }

    /** Maximum size of this sequence. */
    public int capacity () {
       return myItems.length;
    }
    
    /** Append NEWITEM to this sequence of items.
     *  will call expand if needed to double size of array used to store sequence */
    public void addToSequence (int newItem) {
    	if (myCount == myItems.length) this.expand();
        myItems[myCount] = newItem;        
        myCount += 1;
    }

    /** The current number of items in this sequence. */
    public int size () {
        return myCount;
    }
    
    public String toString() {
    	String out = new String();
    	for (int i = 0; i < myCount; i += 1) {
    		out = out + myItems[i] + " ";
    	}
    	return out;
    }
    
    /** Delete item DELETEPOS, where 0 <= DELETEPOS < size () */
    public void delete (int deletePos) {
    	for (int k = deletePos ; k < myCount - 1 ; k += 1 ) {
    		myItems[k] = myItems[k+1] ;	
    	}
    	myItems[myCount - 1] = 0;
        myCount -= 1;
    }
    
    /** Insert TOINSERT into the sequence as item number INSERTPOS (where 0
     *  is the first), shifting any later elements in the sequence over 
     *  to make room for the new element. 
     *  if inserting an element that will force the data sequence to exceed
     *  the current array's length, expand is called to create a new array */
    public void insert (int toInsert, int insertPos) {
    	if (myCount == myItems.length) this.expand();
           for (int i = myCount ; i >= insertPos ; i -= 1 ) {
               myItems[i] = myItems[i - 1] ;
           }
           myItems[insertPos] = toInsert;
           myCount += 1;
       } 
    
    /** doubles the size of the current array
     * whenever necessary to successfully add an element.
     */
    private void expand () {
    	int[] temp = new int [myItems.length * 2];

    	for (int i = 0; i < myCount; i += 1) {
    		temp[i] = myItems[i];
    	}
    	myItems = temp;
    }
    
    /** Remove all 0 entries from the sequence, preserving order of the rest */
    public void removeZeroes() {
    	int i, j;
    	for (i = 0, j = 0; i < myCount; i += 1) {
    	    if (myItems[i] != 0) {
    	       myItems[j] = myItems[i];
    	       j += 1;
    	    }
    	}
    	myCount = j;

}
}