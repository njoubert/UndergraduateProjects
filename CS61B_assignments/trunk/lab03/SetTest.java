import junit.framework.TestCase;

public class SetTest extends TestCase {
	
	public void testConstructor ( ) {
		SetFW s = new SetFW (2);
		assertTrue (s.isEmpty());
		assertFalse (s.member(0));
		assertFalse (s.member(1));
	}
	
	public void testInsert ( ) {
		SetFW s = new SetFW (2);
		s.insert(0);
		assertFalse (s.isEmpty());
		assertTrue (s.member(0));
		assertFalse (s.member(1));
		s.insert(0); // should have no effect
		assertFalse (s.isEmpty());
		assertTrue (s.member(0));
		assertFalse (s.member(1));
	}
	
	public void testRemove ( ) {
		SetFW s = new SetFW (2);
		s.insert(0);
		s.remove(1);
		assertFalse (s.isEmpty());
		assertTrue (s.member(0));
		assertFalse (s.member(1));
		s.remove(0);
		assertTrue (s.isEmpty());
		assertFalse (s.member(0));
		assertFalse (s.member(1));
		s.remove(0); // should have no effect
		assertTrue (s.isEmpty());
		assertFalse (s.member(0));
		assertFalse (s.member(1));
	}
	
	public void testTwoInsertsAndRemoves ( ) {
		SetFW s = new SetFW (2);
		s.insert(0);
		s.insert(1);
		assertFalse (s.isEmpty());
		assertTrue (s.member(0));
		assertTrue (s.member(1));
		s.remove(1);
		assertFalse (s.isEmpty());
		assertTrue (s.member(0));
		assertFalse (s.member(1));
		s.remove(1); // should have no effect
		assertFalse (s.isEmpty());
		assertTrue (s.member(0));
		assertFalse (s.member(1));
		s.remove(0);
		assertTrue (s.isEmpty());
		assertFalse (s.member(0));
		assertFalse (s.member(1));
		s.remove(0); // should have no effect
		assertTrue (s.isEmpty());
		assertFalse (s.member(0));
		assertFalse (s.member(1));
	}

}
