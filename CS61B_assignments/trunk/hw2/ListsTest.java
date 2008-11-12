import junit.framework.TestCase;

public class ListsTest extends TestCase {
	public void testNaturalRuns() {
		IntList inp = new IntList(1, new IntList(3, new IntList(7, new IntList(5, new IntList(4, new IntList(6, new IntList(9, new IntList(10, null))))))));
		assertEquals("[ [ 1 3 7] [ 5] [ 4 6 9 10]]", Lists.naturalRuns(inp).toString());
	}
}
