package tracker;

import java.util.Scanner;

/**
 * Handles the command "simulate"
 */
class CommandSimulate extends Command {
	
	String parse (String commandText, Scanner inp, State theState) throws ProblemException, ParsedException {
		if (!commandText.equals("simulate")) return null;
		
		double[] inputTime = extractArray (inp, 1);
		
		return execute(theState, inputTime[0]);
	}
	
	private String execute(State theState, double t) throws ProblemException {
		theState.simulate(t);
		return "simulate";
	}
}