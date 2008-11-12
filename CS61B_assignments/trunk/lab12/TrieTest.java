import junit.framework.TestCase;

public class TrieTest extends TestCase {

	
	public void testOneInsert() {
		TrieSet one = new TrieSet();
		one.add("a");
		assertTrue(one.contains("a"));
	}
	
	public void testOneLongInsert() {
		TrieSet one = new TrieSet();
		one.add("asseblief");
		assertTrue(one.contains("asseblief"));
	}
	
	public void testTwoInsert() {
		TrieSet two = new TrieSet();
		two.add("a");
		two.add("d");
		assertTrue(two.contains("a"));
		assertTrue(two.contains("d"));
	}
	
	public void testTwoLongInsert() {
		TrieSet two = new TrieSet();
		two.add("asseblief");
		two.add("dankie");
		assertTrue(two.contains("asseblief"));
		assertTrue(two.contains("dankie"));
	}
	
	public void testOneLevelTrie() {
		TrieSet blah = new TrieSet();
		blah.add("a");
		blah.add("b");
		blah.add("c");
		blah.add("dello");
		blah.add("effo");
		blah.add("gigalo");
		assertTrue(blah.contains("a"));
		assertTrue(blah.contains("b"));
		assertTrue(blah.contains("c"));
		assertTrue(blah.contains("dello"));
		assertTrue(blah.contains("effo"));
		assertTrue(blah.contains("gigalo"));
	}
	
	public void testMultipleCase() {
		TrieSet blah = new TrieSet();
		blah.add("Niels");
		blah.add("Ngo");
		blah.add("NJAM");
		assertTrue(blah.contains("Niels"));
		assertTrue(blah.contains("Ngo"));
		assertTrue(blah.contains("NJAM"));
	}
	
	public void testTwoLevelSimpleTree() {
		TrieSet blah = new TrieSet();
		blah.add("aa");
		blah.add("ab");
		assertTrue(blah.contains("aa"));
		assertTrue(blah.contains("ab"));
	}
	
	public void testThreeLevelComplexTrie() {
		TrieSet blah = new TrieSet();
		blah.add("alaml");
		blah.add("alamo");
		blah.add("abloe");
		blah.add("azzie");
		blah.add("azzor");
		blah.add("gigalo");
		assertTrue(blah.contains("alaml"));
		assertTrue(blah.contains("alamo"));
		assertTrue(blah.contains("abloe"));
		assertTrue(blah.contains("azzie"));
		assertTrue(blah.contains("azzor"));
		assertTrue(blah.contains("gigalo"));
		
	}
}
