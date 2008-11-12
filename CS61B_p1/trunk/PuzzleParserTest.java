import junit.framework.TestCase;
import java.util.Scanner;
import java.io.*;

public class PuzzleParserTest extends TestCase {

	public void testCreate() {
		//Does it actually do anything?
		PuzzleParser woopsie = new PuzzleParser(new Scanner ("You bastard."));
		
		try { PuzzleParser blah = new PuzzleParser(new Scanner (new File ("test1.in"))); }
		catch (FileNotFoundException e) {
			
		}
	}
	
	public void testInputReading() {
		
		//Empy file input - what if i try to read anyway?
		String inputString = "";
		PuzzleParser reader = new PuzzleParser(new Scanner(inputString));
		assertFalse(reader.hasAssertion());
		assertFalse(reader.hasQuestion());
		int errorCount = 0;
		try {
			Assertion blah = reader.nextAssertion();
		} catch (IllegalStateException e) {
			errorCount += 1;
		}
		try {
			Question blah = reader.nextQuestion();
		} catch (IllegalStateException e) {
			errorCount += 1;
		}
		assertTrue(errorCount == 2);
		
		//Garbadge input shoudl give immediate error.
		inputString = "garbadgegarbadge\n\nyoyoyoyoy\n\n\tyes";
		reader = new PuzzleParser(new Scanner(inputString));
		assertTrue(reader.error());
		
		//Incorrect input should work until you try to extract actual info from it.
		inputString = "your mom is not my mom. what's going on?";
		reader = new PuzzleParser(new Scanner(inputString));
		assertEquals("your mom is not my mom.", reader.currentSentence());
		assertTrue(reader.hasAssertion());
		assertFalse(reader.hasQuestion());
		reader.nextAssertion();
		assertTrue(reader.error());
		
		//Incorrect input should work until you try to extract actual info from it.
		//If its in the wrong order we should set the error flag to true.
		inputString = "Who is the plumber? John lives in the yellow house.";
		reader = new PuzzleParser(new Scanner(inputString));
		assertEquals("Who is the plumber?", reader.currentSentence());
		assertFalse(reader.hasAssertion());
		assertTrue(reader.hasQuestion());
		reader.nextQuestion();
		assertTrue(reader.error());
		
		/** The following errors would happen if there is programming defect
		 * for example, you keep asking for assertions or questions when there are none.
		 * that's why these cases throw exceptions and not only set the error flag.
		 */
		
		//If you demand a question and there is none, we should be very very angry, 
		//and throw an exception.
		inputString = "your mom is not my mom. what's going on?";
		reader = new PuzzleParser(new Scanner(inputString));
		assertEquals("your mom is not my mom.", reader.currentSentence());
		assertTrue(reader.hasAssertion());
		assertFalse(reader.hasQuestion());
		try {
			reader.nextQuestion();
		} catch (IllegalStateException e){
			errorCount += 1;
		}
		assertTrue(errorCount == 3);

		//Asking in the wrong order is very very bad! Exception!
		inputString = "John lives in the blue house. Who is the plumber?";
		reader = new PuzzleParser(new Scanner(inputString));
		assertEquals("John lives in the blue house.", reader.currentSentence());
		assertTrue(reader.hasAssertion());
		assertFalse(reader.hasQuestion());
		try {
			reader.nextQuestion();
		} catch (IllegalStateException e){
			errorCount += 1;
		}
		assertTrue(errorCount == 4);
		
	}
	/** Here we will actually create some Questions and Assertions */
	public void testClassification() {
		String inputString = "The plumber lives in the blue house. Tom is not the carpenter.\n" +
				"John lives in the yellow house. John is not the carpenter. Mary does not \n" +
				"live in the blue house. The architect does not live in the blue house.\n" +
				"What do you know about John?\n" +
				"What do you know about Mary?\n" +
				"Who is the plumber?";
		PuzzleParser actualInput = new PuzzleParser(new Scanner(inputString));
		
		assertTrue(actualInput.hasAssertion());
		assertFalse(actualInput.hasQuestion());
		assertEquals("The plumber lives in the blue house.", actualInput.currentSentence());
		Assertion assert1 = actualInput.nextAssertion();
		if (assert1 == null) { throw new IllegalArgumentException(); }
		assertEquals("The plumber lives in the blue house.", assert1.toString());
		
		assertTrue(actualInput.hasAssertion());
		assertFalse(actualInput.hasQuestion());
		assertEquals("Tom is not the carpenter.", actualInput.currentSentence());
		Assertion assert2 = actualInput.nextAssertion();
		assertEquals("Tom is not the carpenter.", assert2.toString());
		
		assertTrue(actualInput.hasAssertion());
		assertFalse(actualInput.hasQuestion());
		assertEquals("John lives in the yellow house.", actualInput.currentSentence());
		Assertion assert3 = actualInput.nextAssertion();
		assertEquals("John lives in the yellow house.", assert3.toString());
		
		assertTrue(actualInput.hasAssertion());
		assertFalse(actualInput.hasQuestion());
		assertEquals("John is not the carpenter.", actualInput.currentSentence());
		Assertion assert4 = actualInput.nextAssertion();
		assertEquals("John is not the carpenter.", assert4.toString());
		
		assertTrue(actualInput.hasAssertion());
		assertFalse(actualInput.hasQuestion());
		assertEquals("Mary does not \nlive in the blue house.", actualInput.currentSentence());
		Assertion assert5 = actualInput.nextAssertion();
		assertEquals("Mary does not live in the blue house.", assert5.toString());
		
		assertTrue(actualInput.hasAssertion());
		assertFalse(actualInput.hasQuestion());
		assertEquals("The architect does not live in the blue house.", actualInput.currentSentence());
		Assertion assert6 = actualInput.nextAssertion();
		assertEquals("The architect does not live in the blue house.", assert6.toString());
		
		assertFalse(actualInput.hasAssertion());
		assertTrue(actualInput.hasQuestion());
		assertEquals("What do you know about John?", actualInput.currentSentence());
		Question question1 = actualInput.nextQuestion();
		assertEquals("What do you know about John?", question1.toString());
		
		assertFalse(actualInput.hasAssertion());
		assertTrue(actualInput.hasQuestion());
		assertEquals("What do you know about Mary?", actualInput.currentSentence());
		Question question2 = actualInput.nextQuestion();
		assertEquals("What do you know about Mary?", question2.toString());
		
		assertFalse(actualInput.hasAssertion());
		assertTrue(actualInput.hasQuestion());
		assertEquals("Who is the plumber?", actualInput.currentSentence());
		Question question3 = actualInput.nextQuestion();
		assertEquals("Who is the plumber?", question3.toString());
	}
}
