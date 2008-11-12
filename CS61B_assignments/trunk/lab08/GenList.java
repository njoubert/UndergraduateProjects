public class GenList {
   /** Points to a permanent dummy node whose next pointer 
    *  is the first item in the list and whose prev pointer
    *  is the last. When empty, next and prev point to sentinel. */
   private ListNode sentinel;

   /** An empty list */
   public GenList () {
      sentinel = new ListNode (null, null, null);
      sentinel.next = sentinel.prev = sentinel;
   }

   /** True iff THIS is an empty list. */
   public boolean isEmpty () { return sentinel == sentinel.next; }

   /** Add an item to the front of the list */
   public void addFirst (Object x) {
       sentinel.next = sentinel.next.prev = 
	   new ListNode (x, sentinel, sentinel.next);
   }

   /** Add an item to the end of the list */
   public void addLast (Object x) {
       sentinel.prev = sentinel.prev.next =
	   new ListNode (x, sentinel.prev, sentinel);
   }

   /** The Kth item in THIS list, or -Kth from the end if K<0. */
   public Object get (int k) {
       ListNode p;
       if (k < 0) {
          p = sentinel.prev;
	  while (k < -1 && p != sentinel) {
	     k += 1;
	     p = p.prev;
	  }
       } else {
	  p = sentinel.next;
	  while (k > 0 && p != sentinel) {
	     k -= 1;
	     p = p.next;
	  }
       }
       if (p == sentinel)
 	   new IndexOutOfBoundsException ();
       return p.val;
   }

   /** Destructively move all even-numbered items in THIS to
    *  EVENS, deleting the previous contents of EVENS. */
   void unzip (GenList evens) {
	   
     evens.sentinel.next = evens.sentinel;
     evens.sentinel.prev = evens.sentinel;
     
     ListNode currentNode = sentinel;
     ListNode toMoveNode;
     
     while (currentNode.next != sentinel) { //Here we check that we dont move the sentinal thinking its even
    	 toMoveNode = currentNode.next;
    	 //change currentNode and its surrounding node to exclude toMoveNode
    	 currentNode.next.next.prev = currentNode;
    	 currentNode.next = currentNode.next.next;
    	 
    	 toMoveNode.prev = evens.sentinel.prev;
    	 toMoveNode.next = evens.sentinel;
    	 evens.sentinel.prev = evens.sentinel.prev.next = toMoveNode;
    	 //Change pointers in evens list to have this one at the end.
    	 
    	 if (currentNode.next == sentinel) break; //Here we check that we don't circle throug
    	 currentNode = currentNode.next;
     }
   }

   
   /** Destructively move the items in EVENS into the even-numbered
    *  positions of THIS, leaving EVENS empty. Assumes that the 
    *  the length of EVENS is either the same as THIS or one greater. */
   public void zip (GenList evens) {
	   ListNode currentMe = sentinel;
	   ListNode toMoveNode;
	   
	   while (evens.sentinel.next != evens.sentinel) { //Is there an item left?
		   
		   toMoveNode = evens.sentinel.next;
		   evens.sentinel.next.next.prev = evens.sentinel;
		   evens.sentinel.next = evens.sentinel.next.next;
		   
		   toMoveNode.prev = currentMe;
		   toMoveNode.next = currentMe.next;
		   currentMe.next = currentMe.next.prev = toMoveNode;
		   
		   if (currentMe.next.next == sentinel) break; //Can we move forward?
		   currentMe = currentMe.next.next;
	   }
      
   }
   
   
   private static class ListNode {
      Object val;
      ListNode next, prev;
      ListNode (Object x, ListNode prev, ListNode next) {
	 this.next = next; this.prev = prev; val = x;
      }
   }
}
