import junit.framework.TestCase;

public class AssertionTest extends TestCase {
	
	public void testCreate() {
		Assertion type1a = Assertion.create("John lives   in the yellow house.");
		Assertion type1b = Assertion.create("John does    not live in the yellow house.");
		Assertion type2a = Assertion.create("The\n\nplumber lives in the purple house.");
		Assertion type2b = Assertion.create("The plumber  does not live in the purple house.");
		Assertion type3a = Assertion.create("John  is  the  plumber.");
		Assertion type3b = Assertion.create("John is\t\t not the plumber.");
		Assertion type4 = Assertion.create("Mary lives   around    here.");
		Assertion type5 = Assertion.create("There   is    a yellow   house.");
		
		assertEquals("John lives in the yellow house.", type1a.toString());
		assertEquals("John does not live in the yellow house.", type1b.toString());
		assertEquals("The plumber lives in the purple house.", type2a.toString());
		assertEquals("The plumber does not live in the purple house.", type2b.toString());
		assertEquals("John is the plumber.", type3a.toString());
		assertEquals("John is not the plumber.", type3b.toString());
		assertEquals("Mary lives around here.", type4.toString());
		assertEquals("There is a yellow house.", type5.toString());
		
		assertTrue(type1a.allows("John", "#u", "yellow"));
		assertFalse(type1a.allows("John", "#u", "purple"));
		assertFalse(type1a.allows("Mary", "#u", "yellow"));
		assertTrue(type1a.allows("Mary", "#u", "grey"));
		assertTrue(type1a.allows("#u", "green", "#u"));
		assertFalse(type1a.allows("#u", "#u", "yellow"));
		assertFalse(type1a.allows("John", "#u", "#u"));
		
		assertFalse(type1b.allows("John", "#u", "yellow"));
		assertTrue(type1b.allows("John", "#u", "purple"));
		assertTrue(type1b.allows("John", "#u", "#u"));
		assertTrue(type1b.allows("Mary", "#u", "yellow"));
		assertTrue(type1b.allows("#u", "#u", "yellow"));
		assertTrue(type1b.allows("Mary", "#u", "grey"));
		assertTrue(type1b.allows("#u", "green", "#u"));
		
		assertTrue(type2a.allows("#u", "plumber", "purple"));
		assertFalse(type2a.allows("#u", "carpenter", "purple"));
		assertFalse(type2a.allows("#u", "#u", "purple"));
		assertFalse(type2a.allows("#u", "plumber", "grey"));
		assertFalse(type2a.allows("#u", "plumber", "#u"));
		assertTrue(type2a.allows("#u", "carpenter", "yellow"));
		assertTrue(type2a.allows("#u", "#u", "#u"));
		
		assertFalse(type2b.allows("#u", "plumber", "purple"));
		assertTrue(type2b.allows("#u", "plumber", "yellow"));
		assertTrue(type2b.allows("#u", "plumber", "#u"));
		assertTrue(type2b.allows("#u", "#u", "purple"));
		assertTrue(type2b.allows("#u", "carpenter", "purple"));
		assertTrue(type2b.allows("#u", "#u", "#u"));
		assertTrue(type2b.allows("#u", "carpenter", "yellow"));
		
		assertTrue(type3a.allows("John", "plumber", "#u"));
		assertFalse(type3a.allows("#u", "plumber", "#u"));
		assertFalse(type3a.allows("Mary", "plumber", "#u"));
		assertFalse(type3a.allows("John", "#u", "#u"));
		assertFalse(type3a.allows("John", "carpenter", "#u"));
		assertTrue(type3a.allows("#u", "#u", "#u"));
		assertTrue(type3a.allows("Mary", "carpenter", "#u"));
		
		assertFalse(type3b.allows("John", "plumber", "#u"));
		assertTrue(type3b.allows("Mary", "plumber", "#u"));
		assertTrue(type3b.allows("#u", "plumber", "#u"));
		assertTrue(type3b.allows("John", "carpenter", "#u"));
		assertTrue(type3b.allows("John", "#u", "#u"));
		assertTrue(type3b.allows("#u", "#u", "#u"));
		assertTrue(type3b.allows("Mary", "carpenter", "#u"));
		
		assertTrue(type4.allows("John", "#u", "yellow"));
		assertTrue(type4.allows("John", "#u", "purple"));
		assertTrue(type4.allows("Mary", "#u", "yellow"));
		assertTrue(type4.allows("Mary", "#u", "grey"));
		assertTrue(type4.allows("#u", "green", "#u"));
		assertTrue(type4.allows("#u", "#u", "yellow"));
		assertTrue(type4.allows("John", "#u", "#u"));
		
		assertTrue(type5.allows("John", "#u", "yellow"));
		assertTrue(type5.allows("John", "#u", "purple"));
		assertTrue(type5.allows("Mary", "#u", "yellow"));
		assertTrue(type5.allows("Mary", "#u", "grey"));
		assertTrue(type5.allows("#u", "green", "#u"));
		assertTrue(type5.allows("#u", "#u", "yellow"));
		assertTrue(type5.allows("John", "#u", "#u"));
		
	}
}
