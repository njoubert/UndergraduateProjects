import junit.framework.TestCase;

public class ArraysTest extends TestCase {
	
	public void testCatenate() {
		int[] A = {1, 3, 5, 7, 9, 10, 11, 12};
		int[] B = {100, 200, 300, 500, 900};
		assertEquals("[1, 3, 5, 7, 9, 10, 11, 12, 100, 200, 300, 500, 900]", Utils.toString(Arrays.catenate(A, B)));
	}
	
	public void testRemove() {
		int[] A = {1, 3, 5, 7, 9, 11, 13, 15, 17};
		assertEquals("[1, 3, 5]", Utils.toString(Arrays.remove(A, 3, 6)));
		assertEquals("[1, 3, 5, 9, 11, 13, 15, 17]", Utils.toString(Arrays.remove(A, 3, 1)));
		assertEquals("[1, 3]", Utils.toString(Arrays.remove(A, 2, 50)));
	}
	
	public void testNaturalRuns() {
		int[] A = {1, 3, 7, 5, 4, 6, 9, 10};
		assertEquals(3, Arrays.numberOfSubArrays(A));
		assertEquals(3, Arrays.lengthOfFirstSubArray(A));
		assertEquals("[5, 4, 6, 9, 10]", Utils.toString(Arrays.remove(A, 0, Arrays.lengthOfFirstSubArray(A))));
		assertEquals("[1, 3, 7]", Utils.toString(Arrays.remove(A, Arrays.lengthOfFirstSubArray(A), A.length)));
		assertEquals("[[1, 3, 7], [5], [4, 6, 9, 10]]", Utils.toString(Arrays.naturalRuns(A)));
	}
}
