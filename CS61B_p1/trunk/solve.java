/* The Puzzle Solver
 * Author: NIELS JOUBERT CS61B-BO
 */

import java.io.*;
import java.util.Scanner;
import java.util.regex.*;
import java.util.LinkedList;
import java.util.Iterator;

public class solve {

	
  public static void main (String[] args) {
      if (args.length != 1) {
    	  printInstructions();
      } else {
    	  solvePuzzle(new File(args[0]));
      }
      solve.exit(0);
    }
    
  /* The stored assertions and questions after it is extracted by parseFile */
  private static LinkedList<Assertion> assertions;
  private static LinkedList<Question> questions;
  
  /** This is where the body of the work is happening.
   * We create a PuzzleParser to parse the specific input.
   * We generate a list of assertions and questions.
   * We pass this to the PuzzleSolver.
   *
   */
  public static void solvePuzzle(File inputFile) {
	  assertions = new LinkedList<Assertion>();
	  questions = new LinkedList<Question>();
	  try {
		  parseFile(inputFile);
	  } catch (FileNotFoundException e) {
		  solve.exit(1);
	  } catch (IllegalArgumentException e) {
		  solve.exit(1);
	  }
	  
	  //Create the PuzzleSolver that will compute everything we need to know.
	  try { 
		  PuzzleSolver computeAnswers = new PuzzleSolver(assertions);
		  
		  
		  //Print the known info
		  printAssertions();
	 
		  //It is possible??
		  if (computeAnswers.isPossible()) {
		  
			  
			  //Answer each question;
			  Iterator<Question> questionIterator = questions.iterator();
			  Question currentQuestion;
			  while (questionIterator.hasNext()) {
				  currentQuestion = questionIterator.next();
				  printQuestion(currentQuestion);
				  printAnswer(computeAnswers.answer(currentQuestion));
			  }
		  
		  } else {
			System.out.println("That's impossible.");  
		  }
		  solve.exit(0);
		  
	  } catch (IllegalArgumentException e) {
		  
		  solve.exit(1);
	  }
	  
	  
  }
  
  /** This method will parse the whole file and save the created objects in the linkedlists
   * declared earlier.
   * If the parser at any point runs into something illegal, this will thrown an exception.
   * @param inputFile - is the given inputFile passed from solvePuzzle();
   * @throws FileNotFoundException
   * @throws IllegalArgumentException
   */
  public static void parseFile(File inputFile) throws FileNotFoundException, IllegalArgumentException {
	  
	  PuzzleParser inputParser = new PuzzleParser(new Scanner (inputFile));
	  
	  if (inputParser.error()) throw new IllegalArgumentException(); 
	  
	  while (inputParser.hasAssertion()) {
		  Assertion nextAssertion = inputParser.nextAssertion();
		  if (nextAssertion == null) throw new IllegalArgumentException();
		  assertions.add(nextAssertion);
	  }
	  
	  if (inputParser.error()) throw new IllegalArgumentException(); 
	  
	  while (inputParser.hasQuestion()) { 
		  Question nextQuestion = inputParser.nextQuestion();
		  if (nextQuestion == null) throw new IllegalArgumentException();
		  questions.add(nextQuestion);
	  }
	  
	  if (inputParser.error()) throw new IllegalArgumentException(); 
	  
	  
  }

  /** this method prints the assertions given to us, each on its own line.  */
  public static void printAssertions() {
	  Iterator assertionsIterator = assertions.iterator();
	  int counter = 1;
	  while (assertionsIterator.hasNext()) {
		  System.out.println(counter + ". " + assertionsIterator.next().toString());
		  counter += 1;
	  }
	  
  }
  
  public static void printQuestion(Question currentQ) {
	  System.out.println("Q: " + currentQ.toString());
  }
  
  public static void printAnswer(String currentA) {
	  System.out.println("A: " + currentA);
  }

  /** Prints the instructions to use this program if the arguments are wrong, and exits with solve.exit */
  private static void printInstructions() {   
	  try {
		  Scanner infoOut = new Scanner(new File("Default-output.txt"));
		  infoOut.useDelimiter(Pattern.compile("\n"));
		  while (infoOut.hasNext()) {
			  System.out.println(infoOut.next());
		  }
	  } catch (FileNotFoundException e) {
		  System.out.println("You supplied incorrect arguments to the program.");
		  System.out.println("For some reason the regular help file is unavailable.");
		  System.out.println("You probably need to contact the programmer.");
		  System.out.println("Email him: njoubert@gmail.com");
	  }
	  solve.exit(0);
  }
  
  /** Prints the error message if the input is faulty. This is called from solve.exit() */
  private static void printError() {
		  System.out.println("error ERROR error ERROR error ERROR");
		  System.out.println("Your input is faulty!");
  }

  /* Special provisions for JUnit tests. */

  /* To allow JUnit to test the main program, we have to make sure that
   * the program does not really call System.exit when it finishes (doing
   * so would terminate the JUnit testing as well).  So we never call
   * System.exit directly except in the exit method below.  A JUnit
   * test procedure that wants to test the main program will first
   * call setTestingRun (true), which causes exit(C) to translate 
   * to do nothing if C is 0 (meaning "normal exit") and will
   * fail by throwing an Error if C is non-zero. */

  private static boolean testingRun = false;

  /** Iff ARETESTING, then subsequent calls to solve.exit(n) will 
   *  not call System.exit. */
  public static void setTestingRun (boolean areTesting) {
    testingRun = areTesting;
  }

  /** When not in testing mode (testingRun is false), this is
   *  identical to System.exit; otherwise, it either does nothing
   *  (CODE == 0) or throws an Error. */
  public static void exit (int code) {
    if (! testingRun) {
    	if (code != 0) printError();
      	System.exit (code);
    } else if (code != 0)
      throw new Error ("Exit program with code " + code);
  }

}

