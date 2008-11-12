package tracker;

import java.util.Scanner;

/**
 * Handles the command "bounds"
 */
class CommandBounds extends Command {
	
	String parse (String commandText, Scanner inp, State theState) throws ProblemException, ParsedException {
		if (!commandText.equals("bounds")) return null;
		
		double[] inputBounds = extractArray (inp, 4);
		
		return execute(theState, inputBounds[0], inputBounds[1], inputBounds[2], inputBounds[3]);
	}
	
	private String execute(State theState, double xlow, double ylow, double xhigh, double yhigh) throws ProblemException {
		
		if (theState.moveBoundaries(xlow, ylow, xhigh, yhigh))
			return "bounds";
		else 
			throw new ProblemException("The boundaries cannot be moved inward.");
	}
	
}