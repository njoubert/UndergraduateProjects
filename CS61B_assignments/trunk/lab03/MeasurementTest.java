import junit.framework.TestCase;

public class MeasurementTest extends TestCase {
	public void testInstantiation() {
		Measurement trip = new Measurement();
		Measurement trippy = new Measurement(0);
		Measurement trippadacious = new Measurement(0, 0);
		assertEquals(true, trip.equals(trippy));
		assertEquals(true, trippy.equals(trippadacious));
	}
	
	public void testValueCoversion() {
		Measurement trip = new Measurement(0, 120);
		Measurement trippy = new Measurement(10);
		assertEquals(true, trip.equals(trippy));
		Measurement trippadacious = new Measurement(20, 20);
		Measurement trippadee = new Measurement(21, 8);
		assertEquals(true, trippadee.equals(trippadacious));
	}
	
	public void testPlus() {
		Measurement trip = new Measurement(10,10);
		Measurement trippy = new Measurement(10,10);
		assertEquals("21\'8\"", trip.plus(trippy).toString());
		Measurement trippadacious = new Measurement(5);
		Measurement trippadee = new Measurement(0, 5);
		assertEquals("5\'5\"", trippadee.plus(trippadacious).toString());
	}
	
	public void testMinus() {
		Measurement trip = new Measurement(10,10);
		Measurement trippy = new Measurement(10,10);
		assertEquals("0\'0\"", trip.minus(trippy).toString());
		Measurement trippadacious = new Measurement(5);
		Measurement trippadee = new Measurement(0, 5);
		assertEquals("4\'7\"", trippadacious.minus(trippadee).toString());
	}
	
	public void testMultiply() {
		Measurement trip = new Measurement(10,10);
		Measurement trippy = new Measurement(10,10);
		assertEquals("54\'2\"", trip.multiply(5).toString());
		assertEquals("10\'10\"", trippy.multiply(1).toString());
	}
	
	
}
