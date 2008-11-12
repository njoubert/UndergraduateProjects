import junit.framework.TestCase;
import java.util.List;
import java.util.LinkedList;
import java.util.Iterator;

public class PuzzleSolverTest extends TestCase {

	public void testCreate() {
		
		LinkedList<Assertion> asserts = new LinkedList();
		asserts.add(Assertion.create("John lives around here."));
		asserts.add(Assertion.create("Mary lives around here."));
		asserts.add(Assertion.create("Simon lives around here."));
		asserts.add(Assertion.create("Mary lives in the blue house."));
		asserts.add(Assertion.create("The plumber lives in the yellow house."));
		asserts.add(Assertion.create("The carpenter lives around here."));
		
		PuzzleSolver testS = new PuzzleSolver(asserts);
		LinkedList<String> names = testS.getNames();
		LinkedList<String> occupations = testS.getOccupations();
		LinkedList<String> colors = testS.getColors();
		
		assertEquals("[John, Mary, Simon]", names.toString());
		assertEquals("[plumber, carpenter, #u]", occupations.toString());
		assertEquals("[blue, yellow, #u]", colors.toString());
		

	}
	
	public void testOne() {
		LinkedList<Assertion> asserts = new LinkedList();
		asserts.add(Assertion.create("John lives around here."));
		
		PuzzleSolver testS = new PuzzleSolver(asserts);
		LinkedList<String> names = testS.getNames();
		LinkedList<String> occupations = testS.getOccupations();
		LinkedList<String> colors = testS.getColors();
		
		assertEquals("[John]", names.toString());
		assertEquals("[#u]", occupations.toString());
		assertEquals("[#u]", colors.toString());
	}
	
	public void testSpecial() {
		LinkedList<Assertion> asserts = new LinkedList();
		asserts.add(Assertion.create("John lives around here."));
		asserts.add(Assertion.create("Mary lives around here."));
		asserts.add(Assertion.create("The plumber lives in the yellow house."));
		
		PuzzleSolver testS = new PuzzleSolver(asserts);
		LinkedList<String> names = testS.getNames();
		LinkedList<String> occupations = testS.getOccupations();
		LinkedList<String> colors = testS.getColors();
		
		assertEquals("[John, Mary]", names.toString());
		assertEquals("[plumber, #u]", occupations.toString());
		assertEquals("[yellow, #u]", colors.toString());
	}
	
	public void testAssociation() {
		
		LinkedList<Assertion> asserts = new LinkedList();
		asserts.add(Assertion.create("John lives around here."));
		asserts.add(Assertion.create("Mary lives around here."));
		asserts.add(Assertion.create("Simon lives around here."));
		asserts.add(Assertion.create("Mary lives in the blue house."));
		asserts.add(Assertion.create("The plumber lives in the yellow house."));
		asserts.add(Assertion.create("The carpenter lives around here."));
		
		PuzzleSolver testS = new PuzzleSolver(asserts);
		
		PermutationPairs testP = new PermutationPairs(3);

		String[][] possibility = testS.associatePermsWithStrings(testP.nextPair());
		
		assertEquals("John", possibility[0][0]);
		assertEquals("plumber", possibility[0][1]);
		assertEquals("blue", possibility[0][2]);
		
		assertEquals("Mary", possibility[1][0]);
		assertEquals("carpenter", possibility[1][1]);
		assertEquals("yellow", possibility[1][2]);
		
		assertEquals("Simon", possibility[2][0]);
		assertEquals("#u", possibility[2][1]);
		assertEquals("#u", possibility[2][2]);
	}
}
