import junit.framework.TestCase;

public class IntSequenceTest extends TestCase {
	
	public void testRemoveZeroes() {
		IntSequence test3 = new IntSequence(10);
		test3.addToSequence(0);
		test3.addToSequence(0);
		test3.addToSequence(5);
		test3.addToSequence(-14);
		test3.addToSequence(0);
		test3.addToSequence(3);
		test3.addToSequence(0);

		test3.removeZeroes();
		assertEquals("5 -14 3 ", test3.toString());
	}
}
