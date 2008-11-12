package tracker;

import java.util.Scanner;

/**
 * Handles the command "add"
 */
class CommandAdd extends Command {
	
	String parse (String commandText, Scanner inp, State theState) throws ProblemException, ParsedException {
		if (!commandText.equals("add")) return null;
		
		double[] inputPoint = extractArray (inp, 5);
		
		return execute(theState, (int) inputPoint[0], inputPoint[1], inputPoint[2], inputPoint[3], inputPoint[4]);
	}
	
	private String execute(State theState, int ID, double x, double y, double vx, double vy) throws ProblemException {
		if (!theState.add(ID, x, y, vx, vy)) {
			throw new ProblemException("The ID you are trying to add already exists.");
		}
		return "add";
	}
	
}