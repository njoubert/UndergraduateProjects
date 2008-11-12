/* Author: NIELS JOUBERT CS61B-BO */

import junit.framework.TestCase;
import junit.framework.Test;
import junit.framework.TestSuite;

/** The suite of all JUnit tests for the Puzzle Solver. */
public class FullTest extends TestCase {

  public FullTest (String method) {
    super (method);
  }

  static public Test suite () {
    TestSuite suite = new TestSuite ();

    // Add other JUnit test classes to this suite, including...
    suite.addTestSuite (solveTest.class);
    suite.addTestSuite (AssertionTest.class);
    suite.addTestSuite (QuestionTest.class);
    suite.addTestSuite (PermutationGeneratorTest.class);
    suite.addTestSuite (PuzzleParserTest.class);
    suite.addTestSuite (PuzzleSolverTest.class);
    
    return suite;
  }

}

    

