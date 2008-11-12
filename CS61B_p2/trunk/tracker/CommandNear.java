package tracker;

import java.util.Scanner;
import java.util.regex.*;
import java.util.InputMismatchException;
import util.Set2DIterator;
;

/**
 * Handles the command "near".
 * DOES NOT WORK YET!!!
 */
class CommandNear extends Command {
	
	String parse (String commandText, Scanner inp, State theState) throws ProblemException, ParsedException {
		if (!commandText.equals("near")) return null;
		try {
			if (inp.hasNext("\\*")) {
				return execute(theState, inp.next(), inp.nextDouble(), inp.nextDouble());
			} else {
				double x = inp.nextDouble();
				if (inp.hasNext("\\*")) {
					return execute(theState, x, inp.next(), inp.nextDouble());
				} else {
					return execute(theState, x, inp.nextDouble(), inp.nextDouble());
				}
			}
		} catch (InputMismatchException e) {
			throw new ProblemException ("We expected a double as argument, but did not get it!");
		}
	}
	/**
	 * The execute method for near when you know the x,y position of the center of your region.
	 */
	private String execute(State theState, double x, double y, double d) throws ProblemException {
		int linecounter = 0;
		for (Integer id : theState.nearCircle(theState.iterator(x - d, y - d, x + d, y + d), x, y, d)) {
			if (linecounter == 4) {
				linecounter = 1;
				System.out.printf("%n%s:(%.4g,%.4g) ", id, theState.get(id)[0], theState.get(id)[1]);
			} else {
				linecounter += 1;
				System.out.printf("%s:(%.4g,%.4g) ", id, theState.get(id)[0],  theState.get(id)[1]);
			}
		}
		System.out.println();
		return "near";
	}
	
	/**
	 * The execute method for near when you know the y position of the center of your region.
	 */
	private String execute(State theState, String x, double y, double d) throws ProblemException {
		int linecounter = 0;
		for (Integer id : theState.nearBox(theState.iterator(theState.llx(), y - d, theState.urx(), y + d))) {
			if (linecounter == 4) {
				linecounter = 1;
				System.out.printf("%n%s:(%.4g, %.4g) ", id, theState.getX(id), theState.getY(id));
				
			} else {
				linecounter += 1;
				System.out.printf("%s:(%.4g, %.4g) ", id, theState.getX(id), theState.getY(id));
			}
		}
		System.out.println();
		return "near";
	}
	/**
	 * The execute method for near when you know the x position of the center of your region.
	 */
	private String execute(State theState, double x, String y, double d) throws ProblemException {
		int linecounter = 0;
		for (Integer id : theState.nearBox(theState.iterator(x - d, theState.lly(), x + d, theState.ury()))) {
			if (linecounter == 4) {
				linecounter = 1;
				System.out.printf("%n%s:(%.4g, %.4g) ", id, theState.getX(id), theState.getY(id));
				
			} else {
				linecounter += 1;
				System.out.printf("%s:(%.4g, %.4g) ", id, theState.getX(id), theState.getY(id));
			}
		}
		System.out.println();
		return "near";
	}

}
