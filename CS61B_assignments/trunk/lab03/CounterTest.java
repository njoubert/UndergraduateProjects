import junit.framework.TestCase;

public class CounterTest extends TestCase {
	
	public void testConstructor() {
		Counter fobber = new Counter();
		assertEquals(0, fobber.value());
	}
	
	public void testIncrement() {
		Counter fobber = new Counter();
		fobber.increment();
		assertEquals(1, fobber.value());
		fobber.increment();
		assertEquals(2, fobber.value());
	}
	
	public void testReset() {
		Counter fobber = new Counter();
		fobber.increment();
		fobber.increment();
		assertEquals(2, fobber.value());
		fobber.reset();
		assertEquals(0, fobber.value());
	}
}
