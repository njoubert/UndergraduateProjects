/* NOTE: The file ListUtil.java contains some functions that may be useful
 * in testing your answers. */

/** HW #2, Problem #1. */

class Lists {
  /** The list of lists formed by breaking up L into "natural runs":
   *  that is, maximal ascending sublists, in the same order as
   *  the original.  For example, if L is (1, 3, 7, 5, 4, 6, 9, 10), 
   *  then result is the three-item list ((1, 3, 7), (5), (4, 6, 9, 10)).  
   *  Destructive: creates no new IntList items, and may modify the 
   *  original list pointed to by L. */
  static IntList2 naturalRuns (IntList L) {
	  
	  IntList2 returnList = null;
	  IntList tempPointer = null;
	  
	  while (L.tail != null) {
		  
		  returnList = new IntList2(L, returnList);
		  
		  while (L.tail != null) {
			  
			  if (L.head > L.tail.head) {
				  tempPointer = L.tail;
				  L.tail = null;
				  L = tempPointer;
				  break;
			  } else {
				  L = L.tail;
			  }  
		  }
	  }
	  return Utils.dreverse(returnList);
  }
	  
}
