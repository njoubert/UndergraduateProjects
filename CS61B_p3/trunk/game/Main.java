package game;

import ucb.util.CommandArgs;

import java.io.FileInputStream;
import java.io.FileNotFoundException;

public class Main {
	
	private static void Usage () {
		System.out.println ("Usage: java jump61b [ --display ] [ FILE ]");
		throw new ExitException ("command-line error");
	}
	
	public static void main (String[] rawArgs) {
		Game theGame;
		CommandArgs args = 
			new CommandArgs ("--display --={0,1}", rawArgs);
		
		theGame = null;
		try {
			if (! args.ok ()) {
				Usage ();
				Main.exit (1);
			}
			
			if (args.containsKey ("--")) {
				try {
					System.setIn (new FileInputStream (args.getLast ("--")));
				} catch (FileNotFoundException e) {
					System.err.printf ("Error: could not open %s%n", args.getLast ("--"));
					Main.exit (1);
				}
			}
			
			// Uncomment for the optional GUI part.
			// 
			if (args.containsKey ("--display"))
			   theGame = new Game (new GUIPlayer ());
			 else
			theGame = new Game (new HumanPlayer ());
			System.out.println ("Jump 16B, v2.0.");
			theGame.play ();
			
		} catch (ExitException e) {
			if (theGame != null) {
				theGame.getPlayer (1).close ();
				theGame.getPlayer (2).close ();
			}
			
		}
		
	}
	
	/** True iff we are performing a testing run and should not do
	 *  a System.exit upon exit. */
	private static boolean testingRun = false;
	
	/** Set the testing state to ON.  This is a convenience method for
	 *  JUnit testing purposes.  When the testing state is true, calls 
	 *  to Main.exit will throw an ExitException with the exit code
	 *  as the parameter to the constructor, caught by Main.main.
	 *  Otherwise, if the testing state is false, calls to Main.exit
	 *  call System.exit, as usual.   JUnit tests should therefore
	 *  call setTesting(true) before proceeding.  By default, the testing
	 *  state is false. */
	static void setTesting (boolean on) {
		testingRun = on;
	}
	
	/** Exit from the program.  When the testing state is true, throws 
	 *  ExitException with the given CODE as parameter.  Otherwise,
	 *  behaves just like System.exit.  For this all to work, other
	 *  parts of the program should not call System.exit directly. */
	static void exit (int code) {
		if (testingRun)
			throw new ExitException ();
		else
			
			System.exit (code);
	}
	
}
