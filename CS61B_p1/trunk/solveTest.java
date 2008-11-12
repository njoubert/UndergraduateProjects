// Remove all comments that begin with //, and replace appropriately.
// Feel free to modify ANYTHING in this file, or to not use 
// it at all.

/* Author: NIELS JOUBERT CS61B-BO */

import junit.framework.TestCase;
import java.io.ByteArrayOutputStream;
import java.io.PrintStream;
import java.io.*;
import java.util.Scanner;
import java.util.regex.*;

/** The suite of all JUnit tests for the Puzzle Solver. 
 * FOR AN EXPLANATION OF THE TEST EACH FILE CONTAINTS, SEE THE INTERNALS FILE!
 * 	*/
public class solveTest extends TestCase {

  public solveTest (String method) {
    super (method);
  }


  public void testCorrectInputs() {
	  
	  String expectedOutput;	  
	  
	  /* Testing different files with actual answers */
	  for (int testNr = 8; testNr < 13; ++testNr) {
		  expectedOutput = "";
		  try {
			   Scanner expectedOutputScanner = new Scanner(new File("test" + testNr + ".out"));
			   expectedOutputScanner.useDelimiter(Pattern.compile("\\n"));
			   while (expectedOutputScanner.hasNext()) {
				   expectedOutput = expectedOutput + expectedOutputScanner.next() + "\n";
			   }
			  
			  runMain(new String[] {"test" + testNr + ".in"});
			  
			  assertEquals(expectedOutput, out); 
			  out = "";
		  } catch (FileNotFoundException e) {
		  
		  }
	  }
	  
  }
	  
  public void testErrorInputs() {
	  
	  /* Testing files with faulty input */
	  int errorCount = 0;
	  for (int testNr = 1; testNr < 5; ++testNr) {
  		try {
  			runMain(new String[] {"test" + testNr + ".in"});
  		} catch (Error e) {
  			errorCount++;
  		}
	  }
	  assertEquals(4, errorCount);
  }
  
  // Tests of solve's main program (and perhaps other methods). For example:
  /** Test help message ('java solve' with no arguments). */
  public void testHelpMessage () {
    runMain (new String[] { });
    assertTrue ("Exactly one output stream should contain a message",
		!err.equals ("") ^ !out.equals (""));
  }

  private final static PrintStream 
    realStandardOut = System.out,
    realStandardErr = System.err;

  private ByteArrayOutputStream capturedOut, capturedErr;

  private String out, err;

  /** Run the main program as if started from the command line with
   *      java solve ARGS
   *  Set out to everything the program writes to the standard output,
   *  and err to everything it writes to the standard error file. */
  private void runMain (String[] args) {
    out = err = "";
    try {
      captureOutput ();
      solve.setTestingRun (true);
      solve.main (args);
      out = capturedOut.toString ();
      err = capturedErr.toString ();
    } finally { 
      /* Always restore System.out and System.err, even if solve.main
       * causes an exception. */
      restoreOutput ();
      solve.setTestingRun (false);
    }
  }


  /** Divert output through System.out and System.err to newly created
   *  streams in captureOut and capturedErr. */
  private void captureOutput () {
    capturedOut = new ByteArrayOutputStream ();
    capturedErr =  new ByteArrayOutputStream ();
    System.setOut (new PrintStream (capturedOut));
    System.setErr (new PrintStream (capturedErr));
  }
   
  /** Restore System.out and System.err to their initial values. */
  private void restoreOutput () {
    System.setOut (realStandardOut);
    System.setErr (realStandardErr);
  }

}

    

