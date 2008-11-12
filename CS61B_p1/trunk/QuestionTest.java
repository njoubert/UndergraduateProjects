/* Author: NIELS JOUBERT CS61B-BO */

import junit.framework.TestCase;
import java.util.List;
import java.util.LinkedList;
import java.util.Iterator;

public class QuestionTest extends TestCase {
	
	public void testCreate() {
		
		//Incorrect inputs
		int errorCount = 0;
		
		try { Question gibberish = Question.create("blahlbhablalalala \n \n yes amadoda \t oops?"); }
		catch (IllegalArgumentException e) {errorCount += 1; }
		
		try { Question wrongCapitalization1 = Question.create("What does smalljohhny do?"); }
		catch (IllegalArgumentException e) {errorCount += 1; }
		
		try { Question wrongCapitalization2 = Question.create("What does the Architect do?"); }
		catch (IllegalArgumentException e) {errorCount += 1; }
			
		assertEquals(3, errorCount);
		
	
		//Creating normal questions
		Question type1 = Question.create("What  do  \n\t you  know about John?");
		Question type1b = Question.create("What do you know about the architect?");
		Question type2 = Question.create("What do you know about the blue house?");
		Question type3 = Question.create("Who      \n\n\n   is the carpenter?");
		Question type4 = Question.create("Who lives in the blue house?");
		Question type5 = Question.create("What does the occupant \n\nof the pink house do?");
		Question type6 = Question.create("What does Mary do?");
		Question type7a = Question.create("Where does\nJohn\tlive?");
		Question type7b = Question.create("Where\ndoes the architect live?");
		
		//Checking string representation
		assertEquals("What do you know about John?", type1.toString());
		assertEquals("What do you know about the architect?", type1b.toString());
		assertEquals("What do you know about the blue house?", type2.toString());
		assertEquals("Who is the carpenter?", type3.toString());
		assertEquals("Who lives in the blue house?", type4.toString());
		assertEquals("What does the occupant of the pink house do?", type5.toString());
		assertEquals("What does Mary do?", type6.toString());
		assertEquals("Where does John live?", type7a.toString());
		assertEquals("Where does the architect live?", type7b.toString());
		
		//Checking stored info
		assertEquals("John", type1.getName()); assertEquals(null, type1.getColor()); assertEquals(null, type1.getOccupation());
		assertEquals(null, type1b.getName()); assertEquals(null, type1b.getColor()); assertEquals("architect", type1b.getOccupation());
		assertEquals(null, type2.getName()); assertEquals("blue", type2.getColor()); assertEquals(null, type2.getOccupation());
		assertEquals(null, type3.getName()); assertEquals(null, type3.getColor()); assertEquals("carpenter", type3.getOccupation());	
		assertEquals(null, type4.getName()); assertEquals("blue", type4.getColor()); assertEquals(null, type4.getOccupation());
		assertEquals(null, type5.getName()); assertEquals("pink", type5.getColor()); assertEquals(null, type5.getOccupation());
		assertEquals("Mary", type6.getName()); assertEquals(null, type6.getColor()); assertEquals(null, type6.getOccupation());
		assertEquals("John", type7a.getName()); assertEquals(null, type7a.getColor()); assertEquals(null, type7a.getOccupation());
		assertEquals(null, type7b.getName()); assertEquals(null, type7b.getColor()); assertEquals("architect", type7b.getOccupation());
		

	}
	
	public void testExtractTriple() {
		String[][] testSet = { {"John", "carpenter", "#u"}, 
					{"Mary", "plumber", "#u"},
					{"Greg", "@u", "blue"},
					{"#u", "scientist", "pink"},
					{"Hilfinger", "genius", "yellow"},
					{"#u", "overlord", "supercool"}};
		
		//Lets try to find the genius:
		String[] theGenius = Question.extractTriple(1, "genius", testSet);
		assertEquals("Hilfinger", theGenius[0]);
		assertEquals("genius", theGenius[1]);
		assertEquals("yellow", theGenius[2]);
		
		//Lets try to find Mary
		String[] theMary = Question.extractTriple(0, "Mary", testSet);
		assertEquals("Mary", theMary[0]);
		assertEquals("plumber", theMary[1]);
		assertEquals("#u", theMary[2]);
		
		//Lets try to find something that isnt there
		int errorcount = 0;
		try {
			String[] woops = Question.extractTriple(0, "Nielsietjie", testSet);
		} catch (ArrayIndexOutOfBoundsException e) {
			errorcount += 1;
		}
		
		//Lets try to find a field that doesnt exist
		try {
			String[] woops = Question.extractTriple(5, "Nielsietjie", testSet);
		} catch (IllegalArgumentException e) {
			errorcount += 1;
		}
		
		assertTrue(errorcount == 2);
		
		}
	
	public void testFindAnswer() {
		LinkedList<String[][]> possibleSets = new LinkedList();
		
		//Generate some possibile sets:
		String[][] set1 = { 
				{"Jack" , "mechanic" , "blue" } , 
				{"Mary" , "architect" , "red" } , 
				{"#u" , "sailor" , "#u" }};
		
		//Test with one set
		possibleSets.add(set1);
		
		//We want to know about Jack.
		String[] weHaveJack = Question.findAnswer(0, "Jack", possibleSets);
		
		assertEquals("Jack", weHaveJack[0]);
		assertEquals("mechanic", weHaveJack[1]);
		assertEquals("blue", weHaveJack[2]);
		
		//Lets have lots of possibilities
		String[][] set2 = { 
				{"Jack" , "mechanic" , "blue" } , 
				{"Mary" , "architect" , "red" } , 
				{"#u" , "sailor" , "#u" }};
	
		
		String[][] set6 = { 
				{"Jack" , "architect" , "blue" } , 
				{"Mary" , "sailor" , "red" } , 
				{"#u" , "mechanic" , "#u" }}; 
		
		possibleSets.add(set2);
		possibleSets.add(set6);
		
		String[] weHaveLessOfJack = Question.findAnswer(0, "Jack", possibleSets);
		
		assertEquals("Jack", weHaveLessOfJack[0]);
		assertEquals("#u", weHaveLessOfJack[1]);
		assertEquals("blue", weHaveLessOfJack[2]);
		
		String[] haveSomeMary = Question.findAnswer(0, "Mary", possibleSets);
		
		assertEquals("Mary", haveSomeMary[0]);
		assertEquals("#u", haveSomeMary[1]);
		assertEquals("red", haveSomeMary[2]);
		
		String[] theMechanic = Question.findAnswer(1,  "mechanic", possibleSets);
		assertEquals("#u", theMechanic[0]);
		assertEquals("mechanic", theMechanic[1]);
		assertEquals("#u", theMechanic[2]);
		
	}
	
}

	
	

