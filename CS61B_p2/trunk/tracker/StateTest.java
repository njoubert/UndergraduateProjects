package tracker;

import util.Set2DIterator;
import junit.framework.TestCase;

public class StateTest extends TestCase {
	
	private static State theState;
	
	private State getRandomState() {
		State theState = new State(null, -10.0, -10.0, 10.0, 10.0, 0.25);
		try {
			theState.add(1, 0, 0, -4, -6);
			theState.add(2, 1, 1, 0, 0);
			theState.add(3, 1, 2, 0, 0);
			theState.add(4, 1, 3, 1, 1);
			theState.add(5, 2, 2, 12, 0);
			theState.add(6, 3, 3, 2, 2);
			theState.add(7, 5, 5, 0, 0);
			theState.add(8, 6, 6, 0, 0);
			theState.add(9, 7, 7, 3, 3);
			theState.add(10, 8, 8, 0, 0);
		} catch (ProblemException e) {
			assertTrue(false);
		}
		return theState;
	}
	
	public void testFaultyAdd() {
		State theState = getRandomState();
		int errorCount = 0;
		try {theState.add(1, -5, -5, 0, 0);	//Duplicate ID
		} catch (ProblemException e) {errorCount += 1;}
		
		try {theState.add(14, -0.2, 0, 0, 0);	//Overlapping radius
		} catch (ProblemException e) {errorCount += 1;}
		
		try {theState.add(14, -20, 0, 0, 0);	//Outside Walls
		} catch (ProblemException e) {errorCount += 1;}
		
		assertEquals(3, errorCount);
	}
	
	public void testDataStructureCorrectness() {
		
		State theState = getRandomState();
		
		//Adding
		try {
			theState.add(12, -2.0, -2.0, 20.0, 0.0);
			assertEquals(-2.0, theState.getX(12));
			assertEquals(-2.0, theState.getY(12));
		} catch (ProblemException e) {
			assertTrue(false);
		}
		//Setting
		assertTrue(theState.set(1, -4.0, -4.0, 20.0, 0.0));
		assertEquals(-4.0, theState.getX(1));
		assertEquals(-4.0, theState.getY(1));
		assertEquals(20.0, theState.getVX(1));
		assertEquals(0.0, theState.getVY(1));
		
		//Checking
		assertTrue(theState.contains(12));
		assertFalse(theState.contains(11));
		
		//Getting
		assertEquals(-2.0, theState.getX(12));
		assertEquals(-2.0, theState.getY(12));
		assertEquals(20.0, theState.getVX(12));
		assertEquals(0.0, theState.getVY(12));
		
	}
	
	public void testCollisionRelatedStuff() {
		State theState = getRandomState();
		assertEquals(12.0, theState.findMaxVelocity());
		assertTrue(0.3 > theState.findTimeToCollision());
	}
	
	public void testMath() {
		assertEquals(5.0, State.pyth(3, 4));
	}
	
	public void testPhysics() {
		double[] p0 = {0, 0, 1, 0};
		double[] p1 = {4, 0, -1, 0};
		Physics.rebound(p0, p1);
		assertEquals(-1.0, p0[2]);
		assertEquals(1.0, p1[2]);
		assertEquals(Double.POSITIVE_INFINITY, 1.0 / 0.0);
	}
}
