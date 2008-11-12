package tracker;

import java.util.Scanner;

public class Main {
	
	public void main () {
		Scanner inp = new Scanner (System.in);
		System.out.println ("Particle tracker, v2.0.");
		System.out.print("> ");
		driverLoop(inp);
		
	}
	
	/**
	 * Executes the input the Scanner contains.
	 * @param inp
	 */
	void driverLoop(Scanner inp) {
		inp.useDelimiter("(\\s*#.*\\s+)|(\\s+)");	//Getting rid of comments
		
		while (inp.hasNext ()) {
			
			try {
				
				Command.create(inp.next(), inp, theState); 
				
				
			} catch (ProblemException e) {
				System.err.printf ("Error: %s%n", e.getMessage ());
				//recoverFromError (inp);
			} catch (ParsedException e) {
				System.err.printf ("Error: \"%s\" The unrecognized and ignored token was: %s%n", e.getMessage (), e.getIllegalToken());
				//recoverFromError (inp);
			} catch (ExitException e) {
				if (e.code != 0)
					throw e;
			}
			System.out.print("> ");
			
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
			throw new ExitException (code);
		else
			
			System.exit (code);
	}
	
	/** A convenience function for generating exceptions.  Example:
	 *     throw ERR ("unknown command: %s", token);
	 */
	static ProblemException ERR (String msgTemplate, Object ... args) {
		return new ProblemException (String.format (msgTemplate, args));
	}

	State theState = new State(this);
	
}

/* Note: See the on-line documentation for the class ucb.proj2.Physics 
 * for methods to compute the times at which objects collide and their
 * new velocities after collision. */
