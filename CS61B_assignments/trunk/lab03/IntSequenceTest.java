import junit.framework.TestCase;

public class IntSequenceTest extends TestCase {
	
	public void testInit() {
		IntSequence test1 = new IntSequence(10);
		assertEquals(10, test1.capacity());
		assertEquals("", test1.toString());
	}
	
	public void testInsert() {
		IntSequence test2 = new IntSequence(1);
		test2.addToSequence(1);
		test2.addToSequence(-5);
		test2.addToSequence(17);
		assertEquals(3, test2.size());
		assertEquals("1 -5 17 ", test2.toString());
	}
	
	public void testDeletePos() {
		IntSequence test3 = new IntSequence(7);
		test3.addToSequence(3);
		test3.addToSequence(-7);
		test3.addToSequence(42);
		test3.addToSequence(-11);
		test3.addToSequence(0);
		test3.addToSequence(6);
		test3.addToSequence(9);
		test3.delete(2);
		assertEquals(6, test3.size());
		assertEquals("3 -7 -11 0 6 9 ", test3.toString());
		
	}
	
	public void testInsertPos() {
		IntSequence test3 = new IntSequence(2);
		test3.addToSequence(3);
		test3.addToSequence(-7);
		test3.addToSequence(42);
		test3.addToSequence(-11);
		test3.addToSequence(0);
		test3.addToSequence(6);
		test3.addToSequence(9);
		test3.insert(5, 2);
		test3.insert(10, 5);
		assertEquals(9, test3.size());
		assertEquals("3 -7 5 42 -11 10 0 6 9 ", test3.toString());
	}
}
