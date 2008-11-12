package game;

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

public class GameTest extends TestCase {
	
	public GameTest (String name) {
		super (name);
	}
	
	void doTest (String testName) {
		runMain ("testing/" + testName + ".in");
		assertEquals (maxFilter (fileContents ("testing/" + testName + ".out")),
				maxFilter (out));
		assertEquals (err, "");
	}
	
	
	/* A sample of testing a single session, looking only at required
	 * output: i.e., output from 'dump' and the "Red wins.", "Blue wins."
	 * messages. */
	
	public void testInput1 () {
		doTest ("test1");
	}
	public void testInput2 () {
		doTest ("test2");
	}
	public void testStartWins () {
		doTest ("test3");
	}
	public void testMultipleLoads() {
		doTest ("test4");
		doTest ("test12");
	}
	
	/* Some useful utilities, usable from other JUnit files in this
	 * package */
	
	private final static PrintStream 
	realStandardOut = System.out,
	realStandardErr = System.err;
	
	private static ByteArrayOutputStream capturedOut, capturedErr;
	
	static String out, err;
	
	/** Run the main program as if started with 
	 *      java jump61b FILE
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
			Main.main (new String[] { file });
		} finally { 
			/* Always restore System.out and System.err, even if main ()
			 * causes an exception. */
			restoreOutput ();
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
	
	static final Pattern MAX_FILTER_PATN = 
		Pattern.compile ("(?sm)(===[^=].*?^===|Red wins\\.|Blue wins\\.)");
	
	/** The String S with all output removed except dumps and announcements
	 *  of the winner.  Also deletes trailing blanks. */
	static String maxFilter (String s) {
		Matcher m = MAX_FILTER_PATN.matcher (s);
		StringBuilder out = new StringBuilder ();
		while (m.find ()) {
			out.append (m.group (0));
			out.append ("\\n");
		}
		return out.toString ().replaceAll ("(?m)[ \t]+$", "");
	}
	
	/** The contents of the file named FILE as a String.  Removes "\r" to
	 *  avoid Unix/Windows differences. This method is useful for reading
	 *  a file containing the expected output of your program. */
	static String fileContents (String file) {
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

