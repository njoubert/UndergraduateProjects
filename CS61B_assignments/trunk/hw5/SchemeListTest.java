import junit.framework.TestCase;

public class SchemeListTest extends TestCase {

	  public void testEmpty ( ) {
	    SchemeList l1 = new SchemeList ( );
	    assertTrue (l1.isEmpty());
	  }
	        
	  public void test1element ( ) {
	    SchemeList l1 = new SchemeList ( );
	    SchemeList l2 = l1.cons("x");
	    assertEquals ("( x )", l2.toString());
	    assertFalse (l2.isEmpty());
	    assertEquals ("x", l2.car());
	    assertTrue (l2.cdr().isEmpty());
	  }
	        
	  public void test2elements ( ) {
	    SchemeList l1 = new SchemeList ( );
	    SchemeList l2 = l1.cons("x");
	    SchemeList l3 = l2.cons("w");
	    assertEquals ("( w x )", l3.toString());
	    assertFalse (l3.isEmpty());
	    assertEquals ("w", l3.car());
	    assertEquals ("( x )", l3.cdr().toString());
	  }
	  
	  //Circular structure, infinite loop!
	  
	  public void test3elements ( ) {
		    SchemeList l1 = new SchemeList ( );
		    SchemeList l2 = l1.cons("x");
		    SchemeList l3 = l2.cons("w");
		    l3.setCar("a");
		    l3.setCdr(l1);
		    assertEquals ("( a )", l3.toString());
		    assertFalse (l3.isEmpty());
		    assertEquals ("a", l3.car());
		    assertEquals ("( )", l3.cdr().toString());
		  }
	  
	  public void testIntest() {
		    SchemeList l1 = new SchemeList ( );
		    SchemeList l2 = l1.cons("x");
		    SchemeList l3 = l2.cons("w");
		    SchemeList l4 = l3.cons("a");
		    SchemeList l5 = l4.cons("yes");
		    l5.insert(2, "s");
		    assertEquals ("( yes a s w x )", l5.toString());
		    l5.insert(5, "blah");
		    assertEquals ("( yes a s w x blah )", l5.toString());
		    l5.insert(0, "blah");
		    assertEquals("( blah yes a s w x blah )", l5.toString());
	  }
	  
	  public void testTail() {
		    SchemeList l1 = new SchemeList ( );
		    SchemeList l2 = l1.cons("x");
		    SchemeList l3 = l2.cons("w");
		    SchemeList l4 = l3.cons("a");
		    SchemeList l5 = l4.cons("yes");
		    
		    assertEquals("( yes a w x )", l5.toString());
		    
		    SchemeList b1 = new SchemeList();
		    SchemeList b2 = b1.cons("a");
		    SchemeList b3 = b2.cons("aa");
		    SchemeList b4 = b3.cons("aaa");
		    SchemeList b5 = b4.cons("aaaa");
		    	
		    assertEquals("( aaaa aaa aa a )", b5.toString());
		    
		    l5.dappend(b5);
		    
		    assertEquals("( yes a w x aaaa aaa aa a )", l5.toString());
	  }
	  
	  public void testCopy() {
		    SchemeList l1 = new SchemeList ( );
		    SchemeList l2 = l1.cons("x");
		    SchemeList l3 = l2.cons("w");
		    SchemeList l4 = l3.cons("a");
		    SchemeList l5 = l4.cons("yes");
		    
		    assertEquals("( yes a w x )", l5.toString());
		    
		    SchemeList copy5 = l5.copy();
		    
		    assertEquals("( yes a w x )", copy5.toString());
		    
		    SchemeList emptylist = new SchemeList() ;
		    SchemeList copyempty = emptylist.copy();
		    assertEquals("( )", copyempty.toString());
		    
		    SchemeList noElement = new SchemeList();
		    SchemeList oneElement = noElement.cons("one");
		    SchemeList oneCopy = oneElement.copy();
		    assertEquals("( one )", oneCopy.toString());
		    
		    SchemeList twoElement = oneElement.cons("two");
		    SchemeList twoCopy = twoElement.copy();
		    assertEquals("( two one )", twoCopy.toString());
	  }
	}