package tracker;

import java.util.Scanner;

/**
 * Handles the command "quit"
 */
class CommandQuit extends Command {
	
	String parse (String commandText, Scanner inp, State theState) throws ProblemException, ParsedException {
		if (!commandText.equals("quit")) return null;
		
		return execute(theState);
	}
	
	private String execute(State theState) throws ProblemException {
		theState.getMyMain().theState = new State(theState.getMyMain());
		Main.exit(0);
		return null;
	}
	
}