package tracker;

import java.util.Scanner;

/**
 * Handles the command "add"
 */
class CommandGUI extends Command {
	
	String parse (String commandText, Scanner inp, State theState) throws ProblemException, ParsedException {
		if (!commandText.equals("gui")) return null;
		return execute(theState);
	}
	
	private String execute(State theState) throws ProblemException {
		theState.setGui(new TrackerFrame("Tracker v2.0", theState));
		return "add";
	}
	
}