package tracker;

import java.util.Scanner;

/**
 * Handles the command "rad"
 */
class CommandRad extends Command {
	
	String parse (String commandText, Scanner inp, State theState) throws ProblemException, ParsedException {
		if (!commandText.equals("rad")) return null;
		
		double[] inputRad = extractArray (inp, 1);
		
		return execute(theState, inputRad[0]);
	}
	
	private String execute(State theState, double r) throws ProblemException {
		if (theState.setRadius(r)) return "rad";
		else throw new ProblemException("The radius cannot be decreased, and the radius cannot be smaller than zero.");
	}
	
}
