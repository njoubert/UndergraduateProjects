/* Author: Niels Joubert cs61b-bo */

package tracker;

import java.io.ByteArrayOutputStream;
import java.io.PrintStream;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.BufferedInputStream;
import java.io.File;
import java.util.Scanner;
import java.util.regex.Pattern;
import java.util.regex.Matcher;

import junit.framework.TestCase;

public class TrackTest extends TestCase {

  public TrackTest (String name) {
    super (name);
  }

  public void testGreet () {
    //runMain ("/dev/null");
    //assertEquals (out, "Particle tracker, v2.0.\n> ");
  }

  /**
   * Here we will run all our test input files that 
   * does not cause any errors.
   */
  public void testAllTestingFiles() {
	  for (int i = 1; i < 18; i++) {
		    runMain ("testing/Test" + i + ".in");
		    assertEquals (normalize (fileContents ("testing/Test" + i + ".out")), 
		                  normalize (out));
		    assertEquals ("", err);
	  }
  }
  
  /**
   * Here we will run all our test input files that 
   * does cause errors.
   */
  public void testErrorFiles() {
	  for (int i = 1; i < 9; i++) {
		    runMain ("testing/ErrorTest" + i + ".in");
		    assertFalse (err.equals(""));
	  }
  }  
  
  /* Some useful utilities, usable from other JUnit files in this
   * package */

  private final static PrintStream 
    realStandardOut = System.out,
    realStandardErr = System.err;

  private static ByteArrayOutputStream capturedOut, capturedErr;

  static String out, err;

  /** Run the main program as if started with 
   *      java track < FILE
   *  That is, start the program and cause System.in to come from
   *  FILE.  
   *  Set out to everything the program writes to the standard output,
   *  and err to everything it writes to the standard error file.  
   *  In both cases, canonicalize "\r\n" to "\n" to avoid Windows/Unix
   *  differences. */
  static void runMain (String file) {
    try {
      BufferedInputStream inp 
        = new BufferedInputStream (new FileInputStream (file));
      System.setIn (inp);
    } catch (IOException e) {
      fail ("Could not open " + file);
      return;
    }
    out = err = "";
    captureOutput ();
    try {
      Main.setTesting (true);
      (new Main ()).main ();
    } finally { 
      /* Always restore System.out and System.err, even if main ()
       * causes an exception. */
      restoreOutput ();
      Main.setTesting (false);
      try {
        System.in.close ();
      } catch (IOException e) {
        fail ("Some problem closing " + file);
      }

      out = capturedOut.toString ().replace ("\r", "");
      err = capturedErr.toString ().replace ("\r", "");
    }
  }

  /** Divert output through System.out and System.err to newly created
   *  streams in capturedOut and capturedErr. */
  static void captureOutput () {
    capturedOut = new ByteArrayOutputStream ();
    capturedErr =  new ByteArrayOutputStream ();
    System.setOut (new PrintStream (capturedOut));
    System.setErr (new PrintStream (capturedErr));
  }
   
  /** Restore System.out and System.err to their initial values. */
  static void restoreOutput () {
    System.setOut (realStandardOut);
    System.setErr (realStandardErr);
  }

  static final Pattern NORM_PATN = 
    Pattern.compile ("(?m)(?: +$|> |( +))");

  /** The string S with all prompts ("> ") and trailing spaces removed, 
   *  and all floating-point numbers with more than 6 digits rounded to 
   *  8 significant digits. */
  String normalize (String s) {
    Matcher m = NORM_PATN.matcher (s);
    StringBuffer out = new StringBuffer ();
    while (m.find ()) {
      if (m.group (1) == null)
        m.appendReplacement (out, "");
      else
        m.appendReplacement (out, " ");
    }
    m.appendTail (out);
    return out.toString ().replaceAll ("\n\\s*(\n|$)","\n");
  }

  /** The contents of the file named FILE as a String.  Removes "\r" to
   *  avoid Unix/Windows differences. This method is useful for reading
   *  a file containing the expected output of your program. */
  String fileContents (String file) {
    try {
      Scanner inp = new Scanner (new File (file));
      String val = inp.findWithinHorizon ("(?s).*", 0);
      inp.close ();
      return val.replace ("\r", "");
    } catch (IOException e) {
      fail ("Problem reading " + file);
      return null;
    }
  }
}
