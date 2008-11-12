import junit.framework.TestCase;

public class BigMatTest extends TestCase {
	public void testGeneral() {
		BitMatrix blah = new BitMatrix(5000);
		assertEquals(0, BigMat.mostOnes(blah));
		
		blah.set(0, 0, 1);
		assertEquals(0, BigMat.mostOnes(blah));
		
		blah.set(5, 1, 1);
		assertEquals(5, BigMat.mostOnes(blah));
		
		blah.set(7, 455, 1);
		assertEquals(7, BigMat.mostOnes(blah));
		
		blah.set(466, 460, 1);
		assertEquals(466, BigMat.mostOnes(blah));
		
		blah.set(468, 460, 1);
		assertEquals(466, BigMat.mostOnes(blah));
		
		blah.set(470, 460, 1);
		assertEquals(466, BigMat.mostOnes(blah));
	}
}



