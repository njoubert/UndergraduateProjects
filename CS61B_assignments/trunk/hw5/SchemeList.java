import java.util.Iterator;

public class SchemeList {

    // Refers to the first node in this list.
    // Contains null if this list is empty.
    private ConsNode myHead;

    // Refers to the last node in this list.
    // Contains null if this list is empty.
    private ConsNode myTail;
    
    // Node Class starts here ----------------------------------
    static private class ConsNode {

        public Object myCar;
        public ConsNode myCdr;

        public ConsNode (Object car, ConsNode cdr) {
            myCar = car;
            myCdr = cdr;
        }
            
        public ConsNode (Object car) {
            myCar = car;
            myCdr = null;
        }
    }
    // Node Class end here ----------------------------------
    
    private class SchemeIterator implements Iterator<Object> {
    	
    	private ConsNode tracker;
    	
    	public SchemeIterator() {
    		tracker = myHead;
    	}
    	
    	public boolean hasNext() {
    		if (tracker != null) return true;
    		else return false;
    	}
    	
    	public Object next() {
    		Object buffer = tracker.myCar;
    		tracker = tracker.myCdr;
    		return buffer;
    	}
    	
    	public void remove () { }
    	
    }
    
    
    // END OF NESTED CLASSES
    
    public SchemeList ( ) {
        myHead = null;
        myTail = null;
    }
    
    public SchemeList (ConsNode head) {
        myHead = head;
        
        while (head != null) {
        	if (head.myCdr == null) {
        		myTail = head;
        		break;
        	} else {
        		head = head.myCdr;
        	}
        }
        
    }
    
    public Object car ( ) {
        return myHead.myCar;
    }
    
    public SchemeList cdr ( ) {
        return new SchemeList (myHead.myCdr);
    }
    
    public boolean isEmpty ( ) {
        return myHead == null;
    }
    
    public void setCar (Object o) {
        myHead.myCar = o;
    }
    
    public void setCdr (SchemeList l) {
        myHead.myCdr = l.myHead;
        myTail = l.myTail;
    }

    public SchemeList cons (Object o) {
        return new SchemeList(new ConsNode (o, myHead));
        
        }

    public String toString ( ) {
        if (myHead == null) {
            return "( )";
        }
        String rtn = "( ";
        for (Iterator p = this.iterator(); p.hasNext();) {
            rtn = rtn + p.next() + " ";
        }
        return rtn + ")";
    }
    
	public Iterator iterator () {
		return new SchemeIterator();
	}
    
    public void insert (int pos, Object x) {
    	ConsNode roller = myHead;
    	
    	while (pos > 0) {
    		if (roller.myCdr != null) {
	    		roller = roller.myCdr;
	    		pos--;
    		} else {
    			break;
    		}
    	}
    	if (roller.myCdr == null) {
    		roller.myCdr = new ConsNode(x);
    		myTail = roller.myCdr;
    	} else {
	    	Object temp = roller.myCar;
	    	ConsNode rest = roller.myCdr;
	    	roller.myCar = x;
	    	roller.myCdr = new ConsNode(temp, rest);
    	}
    }
    
    public void dappend(SchemeList l2) {
    	myTail.myCdr = l2.myHead;
    	myTail = l2.myTail;
    }
    
    public SchemeList copy() {
    	Iterator original = this.iterator();
    	if (original.hasNext()) {
	    	SchemeList newlist = new SchemeList(new ConsNode(original.next()));
	    	ConsNode i = newlist.myHead;
	    	while (original.hasNext()) {
	    		i.myCdr = new ConsNode(original.next());
	    		i = i.myCdr;
	    	}
	    	newlist.myTail = i;
	    	return newlist;
    	} else {
    		return new SchemeList();
    	}
    }
}


