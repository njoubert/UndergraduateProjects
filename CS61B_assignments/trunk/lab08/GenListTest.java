import junit.framework.TestCase;

public class GenListTest extends TestCase {

	public void testUnZipOddList() {
		GenList blah = new GenList();
		blah.addLast("a");
		blah.addLast("b");
		blah.addLast("c");
		blah.addLast("d");
		blah.addLast("e");
		blah.addLast("f");
		blah.addLast("g");
		
		GenList into = new GenList();
		blah.unzip(into);
		
		assertEquals("a", into.get(0));
		assertEquals("c", into.get(1));
		assertEquals("e", into.get(2));
		assertEquals("g", into.get(3));
		
		assertEquals("b", blah.get(0));
		assertEquals("d", blah.get(1));
		assertEquals("f", blah.get(2));
	}
	public void testUnzipEvenList() {
		GenList blah = new GenList();
		blah.addLast("a");
		blah.addLast("b");
		blah.addLast("c");
		blah.addLast("d");
		blah.addLast("e");
		blah.addLast("f");
		
		GenList into = new GenList();
		blah.unzip(into);
		
		assertEquals("a", into.get(0));
		assertEquals("c", into.get(1));
		assertEquals("e", into.get(2));
		
		assertEquals("b", blah.get(0));
		assertEquals("d", blah.get(1));
		assertEquals("f", blah.get(2));
	}
	
	public void testZipEvenLists() {
		GenList odds = new GenList();
		GenList evens = new GenList();
		
		odds.addLast("b");
		odds.addLast("d");
		odds.addLast("f");
		
		evens.addLast("a");
		evens.addLast("c");
		evens.addLast("e");
		
		odds.zip(evens);
		
		assertEquals("a", odds.get(0));
		assertEquals("b", odds.get(1));
		assertEquals("c", odds.get(2));
		assertEquals("d", odds.get(3));
		assertEquals("e", odds.get(4));
		assertEquals("f", odds.get(5));
	}
	
	public void testZipDifferentLists() {
		GenList odds = new GenList();
		GenList evens = new GenList();
		
		odds.addLast("b");
		odds.addLast("d");
		odds.addLast("f");
		
		evens.addLast("a");
		evens.addLast("c");
		evens.addLast("e");
		evens.addLast("g");
		
		odds.zip(evens);
		
		assertEquals("a", odds.get(0));
		assertEquals("b", odds.get(1));
		assertEquals("c", odds.get(2));
		assertEquals("d", odds.get(3));
		assertEquals("e", odds.get(4));
		assertEquals("f", odds.get(5));
		assertEquals("g", odds.get(6));
	}
}
