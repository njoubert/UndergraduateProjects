import java.util.*;

//import BinaryTree.TreeNode;

public class AmoebaFamily {
	
  private Amoeba myRoot = null;
	
  public AmoebaFamily (String name) {
    myRoot = new Amoeba (name, null);
  }
	
  /** Add a new amoeba named CHILDNAME as the youngest child
   * of the amoeba named PARENTNAME. Precondition: the amoeba family 
   * contains an amoeba named PARENTNAME. */
  public void addChild (String parentName, String childName) {
	  if (myRoot.myName.equals(parentName)) {
		  myRoot.addChild(childName);
	  } else {
		  if (!myRoot.myChildren.isEmpty()) {
			  for (Amoeba e : myRoot.myChildren) {
				  addChild(parentName, childName, e);
			  }
		  }
	  }
  }
  
  private void addChild (String parentName, String childName, Amoeba currentRoot) {
	  if (currentRoot.myName.equals(parentName)) {
		  currentRoot.addChild(childName);
	  } else {
		  if (!currentRoot.myChildren.isEmpty()) {
			  for (Amoeba e : currentRoot.myChildren) {
				  addChild(parentName, childName, e);
			  }
		  }
	  }
  }
	
  /** Print the names of all amoebas in the family. Names appear in 
   *  preorder, with children's names printed oldest first. Members of 
   *  the family constructed with the main program below would be
   *  printed in the following sequence:
   *		Amos McCoy, mom/dad, me, Mike, Bart, Lisa, Homer, Marge,
   *		Bill, Hilary, Fred, Wilma */
  public void print () {
    System.out.println(myRoot.myName);
    for (Amoeba e : myRoot.myChildren) {
    	print(e, 4);
    }
  }
  
  private void print (Amoeba currentRoot, int indentation) {
	  for (int i = 0; i < indentation; i++) {
		  System.out.print(" ");
	  }
	  System.out.println(currentRoot.myName);
	  for (Amoeba e : currentRoot.myChildren) {
	  	print(e, indentation + 4);
	  }
  }
  
  /** The number of nodes in the tree rooted at A. */
  private static int count (Amoeba a) {
	  int total = 1;
	  for (Amoeba e : a.myChildren) total = total + count(e);
	  return total;
   
  }
  public int count () {
	  return count(myRoot);
   
  }
  
  /** Find the node with the most children. */
  public String busiest () {
	  Amoeba b = busiestAmoeba (myRoot);
	  return b.myName;
  }
  
  // Return the amoeba among a and its descendents
  // that has the most children.
  private Amoeba busiestAmoeba (Amoeba a) {
	  
	  Amoeba currentBusiest = a;
	  int currentMax = a.myChildren.size();
	  
	  for (Amoeba anE : a.myChildren) {
		  
		  if (busiestAmoeba(anE).myChildren.size() > currentMax) {
			  currentBusiest = busiestAmoeba(anE);
			  currentMax = anE.myChildren.size();
		  }
		  
	  }
	  
	  return currentBusiest;
  }
  
  /** Find the height of this tree of Amoeba */
  
  public int height() {
	  return height(myRoot);
  }
  
  private static int height (Amoeba x) {
	    if (x == null) {
	    	return -1;
	    } else {
	        int bestSoFar = 0;
	        for (Amoeba child : x.myChildren) {
	        	bestSoFar = Math.max(bestSoFar, height(child));
	        }
	        return bestSoFar + 1;
	    }
	}
  /** A preorder iterator yielding all Amoebas in THIS. */
  public Iterator<String> allAmoebas ( ) {
    return new AmoebaIterator ( );
  }
  
  public Iterator<String> allAmoebasByGeneration ( ) {
	  return new AmoebaBreadthIterator();
  }
  
  public static void main (String [ ] args) {
    AmoebaFamily family = new AmoebaFamily ("Amos McCoy");
    family.addChild ("Amos McCoy", "auntie");
    family.addChild ("Amos McCoy", "mom/dad");
    family.addChild ("mom/dad", "me");
    family.addChild ("mom/dad", "Fred");
    family.addChild ("mom/dad", "Wilma");
    family.addChild ("me", "Mike");
    family.addChild ("me", "Homer");
    family.addChild ("me", "Marge");
    family.addChild ("Mike", "Bart");
    family.addChild ("Mike", "Lisa");
    family.addChild ("Marge", "Bill");
    family.addChild ("Marge", "Hilary");
    System.out.println ("Here's the family:");
    family.print ( );
    System.out.println(family.count());
    System.out.println(family.busiest());
    System.out.println(family.height());
    
      System.out.println ("");
      System.out.println ("Here it is again!");
      Iterator<String> iter = family.allAmoebas ( );
      while (iter.hasNext ( )) {
      System.out.println (iter.next ( ));
      }
      
      System.out.println ("");
      System.out.println ("Here it is in breadth-first order!");
      Iterator<String> iter2 = family.allAmoebasByGeneration ( );
      while (iter2.hasNext ( )) {
    	  System.out.println (iter2.next ( ));
      }
    
  }
	
  public class AmoebaIterator implements Iterator<String> {
    /** Names of amoebas in the family are enumerated in preorder,
     *  with children enumerated oldest first.
     *  Members of the family constructed with the main program above
     *  will be enumerated in the following sequence:
     *	   Amos McCoy, mom/dad, me, Mike, Bart, Lisa, Homer, Marge,
     *	   Bill, Hilary, Fred, Wilma
     *  Complete enumeration of a family of N amoebas should take
     *  O(N) operations. */
	  
	  private Stack<Amoeba> fringe = new Stack<Amoeba> ( );
	
    public AmoebaIterator ( ) {
	      if (myRoot != null) fringe.push(myRoot);
	}
	
    public String next () {
    	Amoeba a = fringe.pop();
      if (a.myChildren.size() > 0) {
    	  for (int i = a.myChildren.size() - 1; i >= 0; i--) {
    		  fringe.add(a.myChildren.get(i));
    	  }
      }
      return a.myName;
    }

    public boolean hasNext () {
      return !fringe.isEmpty();
    }

    public void remove () {
      throw new UnsupportedOperationException ();  // We won't do this.
    }
		
  } 

  public class AmoebaBreadthIterator implements Iterator<String> {
	    /** Names of amoebas in the family are enumerated in preorder,
	     *  with children enumerated oldest first.
	     *  Members of the family constructed with the main program above
	     *  will be enumerated in the following sequence:
	     *	   Amos McCoy, mom/dad, me, Mike, Bart, Lisa, Homer, Marge,
	     *	   Bill, Hilary, Fred, Wilma
	     *  Complete enumeration of a family of N amoebas should take
	     *  O(N) operations. */
		  
		private ArrayList<Amoeba> queue = new ArrayList<Amoeba>();
		
	    public AmoebaBreadthIterator ( ) {
		      if (myRoot != null) queue.add(myRoot);
		}
		
	    public String next () {
	    	Amoeba a = queue.remove(0);
	      if (a.myChildren.size() > 0) {
	    	  queue.addAll(a.myChildren);
	      }
	      return a.myName;
	    }

	    public boolean hasNext () {
	      return !queue.isEmpty();
	    }

	    public void remove () {
	      throw new UnsupportedOperationException ();  // We won't do this.
	    }
			
	  } 

  private class Amoeba {

    String myName;		
    Amoeba myParent;		
    ArrayList<Amoeba> myChildren;
		
    public Amoeba (String name, Amoeba parent) {
      myName = name;
      myParent = parent;
      myChildren = new ArrayList<Amoeba> ( );
    }
		
    public String toString ( ) {
      return myName;
    }
		
    public Amoeba parent ( ) {
      return myParent;
    }
		
    /** Add an amoeba named CHILDNAME as this amoeba's youngest child. */
    public void addChild (String childName) {
      Amoeba child = new Amoeba (childName, this);
      myChildren.add (child);
    }
  } // end of Amoeba inner class
}
