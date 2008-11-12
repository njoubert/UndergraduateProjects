import java.io.FileInputStream;
import java.io.FileNotFoundException;

import tracker.Main;

public class track {

  /** Run the tracking interpreter.  If ARGS is non-empty, use ARGS[0] as
   *  the name of the file from which to take input. */
  // NOTE: Taking input from a file is an OPTIONAL feature, added to make 
  // it easier for you to test your program using Eclipse, which apparently
  // does not provide for input redirection.
  public static void main(String[] args) {
	  if (args.length >= 1) {
		  try {
			  System.setIn (new FileInputStream (args[0]));
		  } catch (FileNotFoundException e) {
			  System.err.printf ("Error: could not open %s%n", args[0]);
			  System.exit (1);
		  }
	  }
	  (new Main ()).main ();
  }

}