package tracker;

import java.util.Scanner;
import java.io.PrintWriter;
import java.io.File;
import java.io.IOException;

import util.Set2DIterator;

/**
 * Handles the command "write"
 */
class CommandWrite extends Command {
	
	String parse (String commandText, Scanner inp, State theState) throws ProblemException, ParsedException {
		if (!commandText.equals("write")) return null;
		return execute(theState, inp.next());
	}
	
	private String execute(State theState, String filename) throws ProblemException {
		PrintWriter out;
		Set2DIterator iterator = theState.iterator();
		int id;
		try {
			out = new PrintWriter(new File(filename));
			out.format("bounds %g %g %g %g%n", theState.llx(), theState.lly(), theState.urx(), theState.ury() );
			out.format("rad %s%n", theState.getRadius());
			
			while (iterator.hasNext()) {
				id = iterator.next();
				double[] p = theState.get(id);
				out.format("add %s %g %g %g %g%n", id, p[0], p[1], p[2], p[3]);
			}
			out.close();
		} catch (IOException e) {
			throw new ProblemException("The file could not be written to.");
		}
		
		
		
		return "write";
	}
	
}
