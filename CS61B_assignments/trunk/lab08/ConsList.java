
import java.util.List;
import java.util.AbstractList;

public class ConsList extends AbstractList<Object> {

  /** An empty ConsList. */
  public ConsList () {
    myHead = new ConsNode(null, null); //Make a sentinal
  }

  /** The size of THIS. */
  public int size () {
	  ConsNode temp = myHead;
	  int count = 0;
	  while (temp.myCdr != null) {	//Taking the sentinal into account;
		  temp = temp.myCdr;
		  count += 1;
	  }
	  return count;
}

  /** Item #K of THIS.  Throws IndexOutOfBoundsException if K < 0 or
   *  K >= size (). */
  public Object get (int k) {
    if (k < 0 || k >= this.size()) {
    	throw new IndexOutOfBoundsException();
    } else {
    	ConsNode temp = myHead;
    	while (k >= 0) {
    		temp = temp.myCdr;
    		k -= 1;
    	}
    	return temp.myCar;
    }
  }

  /** Make X the new value of get (K), renumbering any existing elements 
   *  #j >= K to #j+1.  Throws IndexOutOfBoundsException if K < 0 or
   *  K > size(). */
  public void add (int k, Object x) {
    if (k < 0 || k > size()) {
    	throw new IndexOutOfBoundsException();
    } else {
    	ConsNode temp = myHead;
    	while (k > 0) {
    		temp = temp.myCdr;
    		k -= 1;
    	}
    	temp.myCdr = new ConsNode (x, temp.myCdr);
    }
  }

  /** Remove item #K, returning its value.   Throws IndexOutOfBoundsException
   *  if K < 0 or K >= size (). */
  public Object remove (int k) {
    if (k < 0 || k >= size()) {
    	throw new IndexOutOfBoundsException();
    } else {
    	ConsNode temp = myHead;
    	while (k > 0) {
    		temp = temp.myCdr;
    		k -= 1;
    	}
    	Object currentCar = temp.myCdr.myCar;
    	temp.myCdr = temp.myCdr.myCdr;
    	return currentCar;
    }
  }

  /** Set item #K of THIS to X, returning the value previously at that
   *  index.  Throws IndexOutOfBoundsException if K < 0 or
   *  K >= size (). */
  public Object set (int k, Object x) {
    if (k < 0 || k >= size()) {
    	throw new IndexOutOfBoundsException();
    } else {
    	ConsNode temp = myHead;
    	while (k >= 0) {
    		temp = temp.myCdr;
    		k -= 1;
    	}
    	Object currentCar = temp.myCar;
    	temp.myCar = x;
    	return currentCar;
    }
  }
  
  public String toString() {
	  String returnVal = "[";
	  ConsNode temp = myHead;
	  while (temp.myCdr.myCdr != null) {
		  temp = temp.myCdr;
		  returnVal = returnVal + temp.myCar.toString() + ", ";
	  }
	  temp = temp.myCdr;
	  return returnVal = returnVal + temp.myCar.toString() + "]";
		  
  }

  /* Only private members beyond this point. */

  private ConsNode myHead;

  // Other instance variables and private methods might go here.

  static private class ConsNode {

    // These are public to be consistent with Scheme.
    public Object myCar;
    public ConsNode myCdr;

    public ConsNode (Object car, ConsNode cdr) {
        myCar = car;
        myCdr = cdr;
    }

    public ConsNode (Object car) {
        this (car, null);
    }
  }

}

