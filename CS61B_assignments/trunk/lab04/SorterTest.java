import junit.framework.TestCase;

public class SorterTest extends TestCase {
	

	private static boolean equals (int [ ] a1, int [ ] a2) {
		if (a1 == null && a2 != null) {
			return false;
		} else if (a1 != null && a2 == null) {
			return false;
		} else if (a1.length != a2.length) {
			return false;
		} else {
			for (int k=0; k<a1.length; k++) {
				if (a1[k] != a2[k]) {
					return false;
				}
			}
			return true;
		}
	}
	
	public void testInOrder ( ) {
		int [ ] values = {1, 2, 3, 4, 5};
		int [ ] result = {1, 2, 3, 4, 5};
		Sorter.sort(values);
		assertTrue (equals(result, values));
	}
	
	public void testInReverseOrder ( ) {
		int [ ] values = {9, 8, 3, -5};
		int [ ] result = {-5, 3, 8, 9};
		Sorter.sort(values);
		assertTrue (equals(result, values));
	}

	public void testSmall ( ) {
		int [ ] values = {14};
		int [ ] result = {14};
		Sorter.sort(values);
		assertTrue (equals(result, values));
	}

	public void testEmpty ( ) {
		int [ ] values = { };
		int [ ] result = { };
		Sorter.sort(values);
		assertTrue (equals(result, values));
	}
	
	public void testMixed ( ) {
		int [ ] values = {4, 1, 3, 5, 9, 8, 6};
		int [ ] result = {1, 3, 4, 5, 6, 8, 9};
		Sorter.sort(values);
		assertTrue (equals(result, values));
	}
	
	public void testIdentical ( ) {
		int [ ] values = {1, 1, 1, 1, 1, 1};
		int [ ] result = {1, 1, 1, 1, 1, 1};
		Sorter.sort(values);
		assertTrue (equals(result, values));
	}
}
