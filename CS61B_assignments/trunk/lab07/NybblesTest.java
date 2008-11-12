import junit.framework.TestCase;

public class NybblesTest extends TestCase {

	public void testGetPut() {
		Nybbles blah = new Nybbles(16);
		blah.set(0, -8);
		blah.set(1, -7);
		blah.set(2, -6);
		blah.set(3, -5);
		blah.set(4, -4);
		blah.set(5, -3);
		blah.set(6, -2);
		blah.set(7, -1);
		blah.set(8, 0);
		blah.set(9, 1);
		blah.set(10, 2);
		blah.set(11, 3);
		blah.set(12, 4);
		blah.set(13, 5);
		blah.set(14, 6);
		blah.set(15, 7);
		
		assertEquals(-8, blah.get(0));
		assertEquals(-7, blah.get(1));
		assertEquals(-6, blah.get(2));
		assertEquals(-5, blah.get(3));
		assertEquals(-4, blah.get(4));
		assertEquals(-3, blah.get(5));
		assertEquals(-2, blah.get(6));
		assertEquals(-1, blah.get(7));
		assertEquals(0, blah.get(8));
		assertEquals(1, blah.get(9));
		assertEquals(2, blah.get(10));
		assertEquals(3, blah.get(11));
		assertEquals(4, blah.get(12));
		assertEquals(5, blah.get(13));
		assertEquals(6, blah.get(14));
		assertEquals(7, blah.get(15));
	}
}
