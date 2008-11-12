
public class IntSequence {
	
    /** myValues[0] .. myValues[myCount-1] contain the elements of the 
     *  sequence. */
	int[] myValues;
    
	/** Current number of items in the sequence. */
    int myCount;

    /** Construct a new, empty IntSequence with a maximum size of
     *  CAPACITY.  Assumes CAPACITY >= 0. */
    public IntSequence (int capacity) {
        myValues = new int[capacity];
        myCount = 0;
    }

    /** Maximum size of this sequence. */
    public int capacity () {
       return myValues.length;
    }
    
    /** Append NEWITEM to this sequence of items.
     *  will call expand if needed to double size of array used to store sequence */
    public void addToSequence (int newItem) {
    	if (myCount == myValues.length) this.expand();
        myValues[myCount] = newItem;        
        myCount += 1;
    }

    /** The current number of items in this sequence. */
    public int size () {
        return myCount;
    }
    
    public String toString() {
    	String out = new String();
    	for (int i = 0; i < myCount; i += 1) {
    		out = out + myValues[i] + " ";
    	}
    	return out;
    }
    
    /** Delete item DELETEPOS, where 0 <= DELETEPOS < size () */
    public void delete (int deletePos) {
    	for (int k = deletePos ; k < myCount - 1 ; k += 1 ) {
    		myValues[k] = myValues[k+1] ;	
    	}
    	myValues[myCount - 1] = 0;
        myCount -= 1;
    }
    
    /** Insert TOINSERT into the sequence as item number INSERTPOS (where 0
     *  is the first), shifting any later elements in the sequence over 
     *  to make room for the new element. 
     *  if inserting an element that will force the data sequence to exceed
     *  the current array's length, expand is called to create a new array */
    public void insert (int toInsert, int insertPos) {
    	if (myCount == myValues.length) this.expand();
           for (int i = myCount ; i >= insertPos ; i -= 1 ) {
               myValues[i] = myValues[i - 1] ;
           }
           myValues[insertPos] = toInsert;
           myCount += 1;
       } 
    
    /** doubles the size of the current array
     * whenever necessary to successfully add an element.
     */
    private void expand () {
    	int[] temp = new int [myValues.length * 2];

    	for (int i = 0; i < myCount; i += 1) {
    		temp[i] = myValues[i];
    	}
    	myValues = temp;
    }
}