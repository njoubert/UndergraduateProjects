import junit.framework.TestCase;

public class PermutationGeneratorTest extends TestCase {
	
	public void testCreate() {
		PermutationGenerator blah = new PermutationGenerator(3);
		assertEquals("6", blah.getTotal().toString());
		int[] first = blah.getNext();
		assertEquals(0, first[0]);
	}
	
}
