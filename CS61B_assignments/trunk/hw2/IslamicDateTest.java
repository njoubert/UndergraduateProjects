import junit.framework.TestCase;

public class IslamicDateTest extends TestCase {

	public void testInit() {
		IslamicDate foobar = new IslamicDate(5, 3);
		IslamicDate frobar = new IslamicDate(65);
		IslamicDate frrbar = new IslamicDate("02/10");

		
	
	}
	
	public void testString() {
		IslamicDate foobar = new IslamicDate(5, 3);
		IslamicDate frobar = new IslamicDate(65);
		IslamicDate frrbar = new IslamicDate("02/10");
		assertEquals("5/3", foobar.toString());
		assertEquals("3/6", frobar.toString());
		assertEquals("2/10", frrbar.toString());
	}
	
	public void testTomorrow() {
		IslamicDate foobar = new IslamicDate(5, 3);
		IslamicDate frobar = new IslamicDate(65);
		IslamicDate frrbar = new IslamicDate("02/10");
		
		assertEquals("5/4", foobar.tomorrow().toString());
		assertEquals("3/7", frobar.tomorrow().toString());
		assertEquals("2/11", frrbar.tomorrow().toString());
	}
	
	public void testMakeTomorrow() {
		IslamicDate foobar = new IslamicDate(5, 3);
		IslamicDate frobar = new IslamicDate(65);
		IslamicDate frrbar = new IslamicDate("02/10");
		foobar.makeTomorrow();
		frobar.makeTomorrow();
		frrbar.makeTomorrow();
		
		assertEquals("5/4", foobar.toString());
		assertEquals("3/7", frobar.toString());
		assertEquals("2/11", frrbar.toString());
	}
	
	public void testDaySpan() {
		IslamicDate foobar = new IslamicDate(60);
		IslamicDate frobar = new IslamicDate(65);
		IslamicDate frrbar = new IslamicDate("02/10");
		IslamicDate frrbrr = new IslamicDate("06/10");
		
		assertEquals(5, foobar.daySpan(frobar));
		assertEquals(118, frrbar.daySpan(frrbrr));
	}
	
	
		
}
